#include "Managers/DownloadManager.h"
#include "UI/MainFrame.h"
#include "Common/EventIDs.h"
#include "Common/CurlCallbacks.h"
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/datetime.h>
#include <wx/regex.h>
#include <wx/textfile.h>
#include <wx/file.h>
#include <wx/utils.h>
#include <wx/thread.h>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

// Define custom event for download operations
wxDEFINE_EVENT(wxEVT_DOWNLOAD_OPERATION, wxCommandEvent);

// Global mutex for thread safety
std::mutex g_downloadMutex;

// Constructor
DownloadManager::DownloadManager()
    : m_mainFrame(nullptr), m_nextId(1), m_isRunning(false), m_speedLimit(0)
{
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Initialize database manager
    m_databaseManager = new DatabaseManager("downloads.db");
    
    wxLogMessage("DownloadManager initialized");
}


// Constructor with main frame
DownloadManager::DownloadManager(MainFrame* mainFrame, const AppSettings& settings)
    : m_mainFrame(mainFrame), m_settings(settings), m_nextId(1), m_isRunning(false), m_speedLimit(0)
{
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Initialize database manager
    m_databaseManager = new DatabaseManager("downloads.db");
    
    // Load downloads from database
    LoadDownloads();
    
    // Connect event handler for download operations
    if (m_mainFrame) {
        m_mainFrame->Connect(wxEVT_DOWNLOAD_OPERATION, wxCommandEventHandler(MainFrame::OnDownloadOperation));
    }
    
    wxLogMessage("DownloadManager initialized with main frame");
}

// Destructor
DownloadManager::~DownloadManager()
{
    // Stop download thread
    Stop();
    
    // Disconnect event handler
    if (m_mainFrame) {
        m_mainFrame->Disconnect(wxEVT_DOWNLOAD_OPERATION, wxCommandEventHandler(MainFrame::OnDownloadOperation));
    }
    
    // Cleanup curl
    curl_global_cleanup();
    
    // Delete database manager
    if (m_databaseManager) {
        delete m_databaseManager;
        m_databaseManager = nullptr;
    }
    
    wxLogMessage("DownloadManager destroyed");
}

// Add download

int DownloadManager::AddDownload(const wxString& url, const wxString& savePath)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Create download item
    DownloadItem item;
    item.id = m_nextId++;
    item.url = url;
    item.savePath = savePath;
    item.status = DownloadStatus::PENDING;
    item.progress = 0;
    item.size = 0;
    item.downloadedSize = 0;
    item.speed = 0;
    item.dateAdded = wxDateTime::Now().Format("%Y-%m-%d %H:%M:%S");
    
    // Extract filename from URL
    wxString filename = url.AfterLast('/');
    if (filename.IsEmpty()) {
        filename = wxString::Format("download_%d", item.id);
    }
    
    // Clean up filename - remove query parameters
    if (filename.Contains("?")) {
        filename = filename.BeforeFirst('?');
    }
    
    // Ensure filename is valid
    filename.Replace(":", "_");
    filename.Replace("/", "_");
    filename.Replace("\\", "_");
    filename.Replace("*", "_");
    filename.Replace("?", "_");
    filename.Replace("\"", "_");
    filename.Replace("<", "_");
    filename.Replace(">", "_");
    filename.Replace("|", "_");
    
    item.name = filename;

  
    // Add to list
    m_downloads.push_back(item);
    
    // Save to database
    m_databaseManager->AddDownload(item);
    
    wxLogMessage("Download added, id: %d, url: %s, filename: %s", item.id, item.url, item.name);
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    return item.id;
}

// Add YouTube download
int DownloadManager::AddYouTubeDownload(const wxString& url, const wxString& savePath, const wxString& title, const wxString& format)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Create download item
    DownloadItem item;
    item.id = m_nextId++;
    item.url = url;
    item.savePath = savePath;
    item.status = DownloadStatus::PENDING;
    item.progress = 0;
    item.size = 0;
    item.downloadedSize = 0;
    item.speed = 0;
    item.dateAdded = wxDateTime::Now().Format("%Y-%m-%d %H:%M:%S");
    
    // Set name
    wxString cleanTitle = title;
    if (cleanTitle.IsEmpty()) {
        cleanTitle = wxString::Format("youtube_%d", item.id);
    } else {
        // Clean up title for filename
        cleanTitle.Replace(":", "_");
        cleanTitle.Replace("/", "_");
        cleanTitle.Replace("\\", "_");
        cleanTitle.Replace("*", "_");
        cleanTitle.Replace("?", "_");
        cleanTitle.Replace("\"", "_");
        cleanTitle.Replace("<", "_");
        cleanTitle.Replace(">", "_");
        cleanTitle.Replace("|", "_");
    }

 
    // Store format in the name if needed
    if (!format.IsEmpty()) {
        item.name = cleanTitle + " [" + format + "].mp4";
    } else {
        item.name = cleanTitle + ".mp4";
    }
    
    // Add to list
    m_downloads.push_back(item);
    
    // Save to database
    m_databaseManager->AddDownload(item);
    
    wxLogMessage("YouTube download added, id: %d, url: %s", item.id, item.url);
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    return item.id;
}

// Start download

void DownloadManager::StartDownload(int id)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Find download
    DownloadItem* item = GetDownloadById(id);
    if (!item) {
        wxLogError("Download not found, id: %d", id);
        return;
    }
    
    // Check if already downloading
    if (item->status == DownloadStatus::DOWNLOADING) {
        wxLogMessage("Download already in progress, id: %d", id);
        return;
    }
    
    // Set status
    item->status = DownloadStatus::DOWNLOADING;
    
    // Update database
    m_databaseManager->UpdateDownload(*item);
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    // Start download thread if not running
    if (!m_isRunning) {
        Start();
    }
    
    wxLogMessage("Download started, id: %d", id);
}

// Start multiple downloads

void DownloadManager::StartDownloads(const std::vector<int>& ids)
{
    for (int id : ids) {
        StartDownload(id);
    }
}

// Pause download
void DownloadManager::PauseDownload(int id)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Find download
    DownloadItem* item = GetDownloadById(id);
    if (!item) {
        wxLogError("Download not found, id: %d", id);
        return;
    }
    
    // Check if downloading
    if (item->status != DownloadStatus::DOWNLOADING) {
        wxLogMessage("Download not in progress, id: %d", id);
        return;
    }
    
    // Set status
    item->status = DownloadStatus::PAUSED;
    
    // Update database
    m_databaseManager->UpdateDownload(*item);
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    wxLogMessage("Download paused, id: %d", id);
}

// Pause multiple downloads

void DownloadManager::PauseDownloads(const std::vector<int>& ids)
{
    for (int id : ids) {
        PauseDownload(id);
    }
}

// Resume download
void DownloadManager::ResumeDownload(int id)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Find download
    DownloadItem* item = GetDownloadById(id);
    if (!item) {
        wxLogError("Download not found, id: %d", id);
        return;
    }
    
    // Check if paused
    if (item->status != DownloadStatus::PAUSED && item->status != DownloadStatus::ERROR) {
        wxLogMessage("Download not paused or in error state, id: %d", id);
        return;
    }
    
    // Set status
    item->status = DownloadStatus::DOWNLOADING;
    
    // Update database
    m_databaseManager->UpdateDownload(*item);
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    // Start download thread if not running
    if (!m_isRunning) {
        Start();
    }
    
    wxLogMessage("Download resumed, id: %d", id);
}

// Resume multiple downloads

void DownloadManager::ResumeDownloads(const std::vector<int>& ids)
{
    for (int id : ids) {
        ResumeDownload(id);
    }
}

// Cancel download
void DownloadManager::CancelDownload(int id)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Find download
    DownloadItem* item = GetDownloadById(id);
    if (!item) {
        wxLogError("Download not found, id: %d", id);
        return;
    }
    
    // Check if downloading or paused
    if (item->status != DownloadStatus::DOWNLOADING && item->status != DownloadStatus::PAUSED) {
        wxLogMessage("Download not in progress or paused, id: %d", id);
        return;
    }
    
    // Set status
    item->status = DownloadStatus::PENDING;
    item->progress = 0;
    item->downloadedSize = 0;
    item->speed = 0;
    
    // Update database
    m_databaseManager->UpdateDownload(*item);
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    wxLogMessage("Download canceled, id: %d", id);
}

// Cancel multiple downloads

void DownloadManager::CancelDownloads(const std::vector<int>& ids)
{
    for (int id : ids) {
        CancelDownload(id);
    }
}

// Delete download
void DownloadManager::DeleteDownload(int id)
{
    std::lock_guard<std::mutex> lock(g_downloadMutex);
    
    // Find download
    DownloadItem* item = GetDownloadById(id);
    if (!item) {
        wxLogError("Download not found, id: %d", id);
        return;
    }
    
    // Delete from database
    m_databaseManager->DeleteDownload(id);
    
    // Delete from list
    for (auto it = m_downloads.begin(); it != m_downloads.end(); ++it) {
        if (it->id == id) {
            m_downloads.erase(it);
            break;
        }
    }
    
    // Update UI
    if (m_mainFrame) {
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
        wxPostEvent(m_mainFrame, event);
    }
    
    wxLogMessage("Download deleted, id: %d", id);
}

// Delete multiple downloads
void DownloadManager::DeleteDownloads(const std::vector<int>& ids)
{
    for (int id : ids) {
        DeleteDownload(id);
    }
}

// Get download by ID
DownloadItem* DownloadManager::GetDownloadById(int id)
{
    for (auto& item : m_downloads) {
        if (item.id == id) {
            return &item;
        }
    }
    
    return nullptr;
}

// Get downloads
const std::vector<DownloadItem>& DownloadManager::GetDownloads() const
{
    return m_downloads;
}

// Set speed limit
void DownloadManager::SetSpeedLimit(long limit)
{
    m_speedLimit = limit;
    wxLogMessage("Speed limit set to %ld KB/s", limit);
}

// Get speed limit
long DownloadManager::GetSpeedLimit() const
{
    return m_speedLimit;
}

// Start download thread


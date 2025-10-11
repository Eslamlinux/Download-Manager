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


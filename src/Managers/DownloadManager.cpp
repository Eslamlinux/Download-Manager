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
void DownloadManager::Start()
{
    if (m_isRunning) {
        wxLogMessage("Download thread is already running");
        return;
    }
    
    wxLogMessage("Starting download thread");
    m_isRunning = true;
    
    // Start thread
    std::thread([this]() {
        wxLogMessage("Download thread started");
        
        while (m_isRunning) {
            // Check for downloads to process
            bool hasActiveDownloads = false;
            
            {
                std::lock_guard<std::mutex> lock(g_downloadMutex);
                
                for (auto& item : m_downloads) {
                    if (item.status == DownloadStatus::DOWNLOADING) {
                        hasActiveDownloads = true;
                        
                        wxLogMessage("Processing download ID: %d, URL: %s", item.id, item.url);
                        
                        // Process download outside the lock
                        std::thread downloadThread([this, &item]() {
                            ProcessDownload(&item);
                            
                            // Update database and UI after download completes
                            std::lock_guard<std::mutex> updateLock(g_downloadMutex);
                            m_databaseManager->UpdateDownload(item);
                            
                            if (m_mainFrame) {
                                wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
                                wxPostEvent(m_mainFrame, event);
                            }
                        });
                        
                        // Detach the thread to let it run independently
                        downloadThread.detach();
                        
                        // Only process one download at a time
                        break;
                    }
                }
            }
            
            // If no active downloads, sleep for a while
            if (!hasActiveDownloads) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } else {
                // If we started a download, wait a bit before checking again
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        wxLogMessage("Download thread stopped");
    }).detach();
    
    wxLogMessage("Download thread detached");
}

// Stop download thread
void DownloadManager::Stop()
{
    m_isRunning = false;
}

// Load downloads from database
void DownloadManager::LoadDownloads()
{
    // Get downloads from database
    m_downloads = m_databaseManager->GetAllDownloads();
    
    // Find next ID
    m_nextId = 1;
    for (const auto& item : m_downloads) {
        if (item.id >= m_nextId) {
            m_nextId = item.id + 1;
        }
    }
    
    wxLogMessage("Loaded %zu downloads from database", m_downloads.size());
}

// Custom write callback for libcurl
static size_t CustomWriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    FILE* fp = (FILE*)userp;
    size_t written = fwrite(contents, size, nmemb, fp);
    return written;
}

// Custom progress callback for libcurl
static int CustomProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    DownloadItem* item = (DownloadItem*)clientp;
    
    // Update progress
    if (dltotal > 0) {
        item->progress = static_cast<int>((dlnow * 100) / dltotal);
        item->size = dltotal;
        item->downloadedSize = dlnow;
    } else {
        // If total size is unknown, just show downloaded size
        item->downloadedSize = dlnow;
    }
    
    // Calculate speed (bytes per second)
    static auto lastTime = std::chrono::steady_clock::now();
    static curl_off_t lastBytes = 0;
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
    
    if (duration > 1000) {  // Update speed every second
        item->speed = static_cast<long long>((dlnow - lastBytes) * 1000 / duration);
        lastTime = now;
        lastBytes = dlnow;
    }
    
    return 0;  // Return 0 to continue download
}

// Process download
void DownloadManager::ProcessDownload(DownloadItem* item)
{
    // Check if the item is valid
    if (!item) {
        wxLogError("Invalid download item");
        return;
    }

    wxLogMessage("Processing download: %s", item->url);
    
    // Create save directory if it doesn't exist
    if (!wxDirExists(item->savePath)) {
        wxLogMessage("Creating directory: %s", item->savePath);
        if (!wxFileName::Mkdir(item->savePath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
            wxLogError("Failed to create directory: %s", item->savePath);
            item->status = DownloadStatus::ERROR;
            return;
        }
    }

    // Create full file path
    wxString filePath = item->savePath + wxFileName::GetPathSeparator() + item->name;
    wxLogMessage("File path: %s", filePath);
    
    // Check if this is a YouTube URL
    if (item->url.Contains("youtube.com") || item->url.Contains("youtu.be")) {
        wxLogMessage("Detected YouTube URL");
        if (!m_settings.youtubeExecutablePath.IsEmpty()) {
            wxLogMessage("Using YouTube-DL: %s", m_settings.youtubeExecutablePath);
            
            // Create a temporary file to store the output
            wxString tempFile = wxFileName::CreateTempFileName("yt_dl_output");
            
            // Build the command
            wxString command = wxString::Format("\"%s\" -o \"%s\" -f \"%s\" \"%s\" > \"%s\" 2>&1", 
                                              m_settings.youtubeExecutablePath,
                                              filePath,
                                              m_settings.youtubeDefaultFormat,
                                              item->url,
                                              tempFile);
            
            wxLogMessage("Executing command: %s", command);
            
            // Execute the command
            long exitCode = wxExecute(command, wxEXEC_SYNC);
            
            // Read the output
            wxTextFile outputFile;
            if (outputFile.Open(tempFile)) {
                for (wxString line = outputFile.GetFirstLine(); !outputFile.Eof(); line = outputFile.GetNextLine()) {
                    wxLogMessage("youtube-dl: %s", line);
                }
                outputFile.Close();
            }
            
            // Delete the temporary file
            wxRemoveFile(tempFile);
            
            if (exitCode == 0) {
                wxLogMessage("YouTube download completed successfully");
                item->status = DownloadStatus::COMPLETED;
                item->progress = 100;
                
                // Get file size
                wxFileName fn(filePath);
                if (fn.FileExists()) {
                    item->size = fn.GetSize().ToULong();
                    item->downloadedSize = item->size;
                }
                
                return;
            } else {
                wxLogError("YouTube download failed with exit code: %ld", exitCode);
                item->status = DownloadStatus::ERROR;
                return;
            }
        } else {
            wxLogError("YouTube-DL path not set in settings");
            item->status = DownloadStatus::ERROR;
            return;
        }
    }
    
    // Regular download process using libcurl
    wxLogMessage("Using libcurl for download: %s", item->url);
    
    // Initialize error buffer
    char errorBuffer[CURL_ERROR_SIZE];
    memset(errorBuffer, 0, CURL_ERROR_SIZE);
    
    // Maximum number of retries
    const int MAX_RETRIES = 3;
    bool downloadSuccess = false;
    
    for (int retryCount = 0; retryCount < MAX_RETRIES && !downloadSuccess; retryCount++) {
        if (retryCount > 0) {
            wxLogMessage("Retry attempt %d of %d for URL: %s", retryCount + 1, MAX_RETRIES, item->url);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
        CURL* curl = curl_easy_init();
        if (!curl) {
            wxLogError("Failed to initialize curl");
            continue;
        }
        
        // Open file for writing
        FILE* fp = fopen(filePath.c_str(), "wb");
        if (!fp) {
            wxLogError("Failed to open file for writing: %s", filePath);
            curl_easy_cleanup(curl);
            continue;
        }
        
        // Process URL - encode spaces and special characters
        wxString processedUrl = item->url;
        
        // Check for spaces or special characters and encode them properly
        if (processedUrl.Contains(" ") || processedUrl.Contains("\"") || processedUrl.Contains("'") || 
            processedUrl.Contains("<") || processedUrl.Contains(">") || processedUrl.Contains("[") || 
            processedUrl.Contains("]")) {
            wxLogMessage("URL contains spaces or special characters, encoding it");
            
            // Use curl's URL encoding function
            char* output = curl_easy_escape(curl, processedUrl.c_str(), processedUrl.length());
            if (output) {
                // We need to preserve the http:// or https:// part
                wxString protocol;
                if (processedUrl.StartsWith("http://")) {
                    protocol = "http://";
                } else if (processedUrl.StartsWith("https://")) {
                    protocol = "https://";
                }
                
                // Combine protocol with encoded URL, but be careful not to double-encode
                if (!protocol.IsEmpty()) {
                    wxString encodedPart = wxString(output);
                    // Remove the protocol part from the encoded string if it's there
                    if (encodedPart.StartsWith("http%3A%2F%2F")) {
                        encodedPart = encodedPart.Mid(13);
                        processedUrl = protocol + encodedPart;
                    } else if (encodedPart.StartsWith("https%3A%2F%2F")) {
                        encodedPart = encodedPart.Mid(14);
                        processedUrl = protocol + encodedPart;
                    } else {
                        // If the protocol wasn't encoded, just use the encoded string
                        processedUrl = encodedPart;
                    }
                } else {
                    processedUrl = wxString(output);
                }
                
                curl_free(output);
            } else {
                // Manual encoding for basic cases
                processedUrl.Replace(" ", "%20");
                processedUrl.Replace("\"", "%22");
                processedUrl.Replace("'", "%27");
                processedUrl.Replace("<", "%3C");
                processedUrl.Replace(">", "%3E");
                processedUrl.Replace("[", "%5B");
                processedUrl.Replace("]", "%5D");
            }
            
            wxLogMessage("Encoded URL: %s", processedUrl);
        }
        
        // Set up headers to mimic a browser
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.9,ar;q=0.8");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        
        // Extract the domain from the URL to use as the Referer
        wxString domain;
        if (processedUrl.StartsWith("http://")) {
            domain = processedUrl.Mid(7).BeforeFirst('/');
        } else if (processedUrl.StartsWith("https://")) {
            domain = processedUrl.Mid(8).BeforeFirst('/');
        }
        
        if (!domain.IsEmpty()) {
            wxString referer = "Referer: http://" + domain + "/";
            headers = curl_slist_append(headers, referer.c_str());
            
            wxString origin = "Origin: http://" + domain;
            headers = curl_slist_append(headers, origin.c_str());
        }
        
        // Special handling for mp3quran.net
        if (item->url.Contains("mp3quran.net")) {
            wxLogMessage("Adding special headers for mp3quran.net");
            headers = curl_slist_append(headers, "Referer: https://mp3quran.net/");
            headers = curl_slist_append(headers, "Origin: https://mp3quran.net");
            headers = curl_slist_append(headers, "Accept: audio/webm,audio/ogg,audio/mp3,audio/*;q=0.9");
        }
        
        // Set up libcurl options
        curl_easy_setopt(curl, CURLOPT_URL, processedUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CustomWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, CustomProgressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, item);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // Enable cookies
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Enable verbose output for debugging
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer); // Set error buffer
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L); // 5 minute timeout
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L); // 30 second connect timeout
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Don't verify SSL certificates
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Don't verify host
        
        // Apply speed limit if set
        if (m_speedLimit > 0) {
            curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)m_speedLimit * 1024);
        }
        
        // Execute the request
        wxLogMessage("Executing curl request");
        CURLcode res = curl_easy_perform(curl);
        
        // Close the file
        fclose(fp);
        
        // Free the headers
        curl_slist_free_all(headers);
        
        // Check the result
        if (res != CURLE_OK) {
            wxLogError("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            
            // Get more detailed error information
            long response_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            
            if (response_code > 0) {
                wxLogError("HTTP response code: %ld", response_code);
            }
            
            // Log the error buffer content
            wxLogError("Error details: %s", errorBuffer);
            
            // Clean up libcurl
            curl_easy_cleanup(curl);
            
            // Continue to next retry
            continue;
        } else {
            // Get download information
            curl_off_t downloadedSize;
            curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &downloadedSize);
            item->downloadedSize = static_cast<long long>(downloadedSize);
            item->size = item->downloadedSize;
            item->status = DownloadStatus::COMPLETED;
            item->progress = 100;
            
            wxLogMessage("Download completed: %s", item->name);
            downloadSuccess = true;
            
            // Clean up libcurl
            curl_easy_cleanup(curl);
            
            // Break out of retry loop
            break;
        }
    }
    
    // If all retries failed, set status to ERROR
    if (!downloadSuccess) {
        item->status = DownloadStatus::ERROR;
        wxLogError("All download attempts failed for URL: %s", item->url);
    }
}

// Transform tvquran.com URL to a more direct format
wxString DownloadManager::TransformTvQuranUrl(const wxString& originalUrl) {
    // Example: https://download.tvquran.com/download/recitations/83/229/001.mp3
    // Try to transform to a more direct URL format
    
    wxRegEx reRecitation("https?://download\\.tvquran\\.com/download/recitations/(\\d+)/(\\d+)/(\\d+)\\.mp3");
    
    if (reRecitation.Matches(originalUrl)) {
        wxString reciterId = reRecitation.GetMatch(originalUrl, 1);
        wxString surahId = reRecitation.GetMatch(originalUrl, 2);
        wxString fileId = reRecitation.GetMatch(originalUrl, 3);
        
        // Try alternative URL formats
        // Format 1: Direct CDN
        return wxString::Format("https://cdn.tvquran.com/recitations/%s/%s/%s.mp3", 
                               reciterId, surahId, fileId);
    }
    
    return originalUrl; // Return original if no transformation is possible
}

// Properly encode a URL
wxString DownloadManager::EncodeURL(const wxString& url) {
    CURL* curl = curl_easy_init();
    wxString encodedUrl = url;
    
    if (curl) {
        // Use curl's URL encoding function
        char* output = curl_easy_escape(curl, url.c_str(), url.length());
        if (output) {
            encodedUrl = wxString(output);
            curl_free(output);
        }
        curl_easy_cleanup(curl);
    }
    
    return encodedUrl;
}

// Save settings
void DownloadManager::SaveSettings(const AppSettings& settings)
{
    m_settings = settings;
    m_settings.Save();
    wxLogMessage("Settings saved");
}

// Get settings
const AppSettings& DownloadManager::GetSettings() const
{
    return m_settings;
}

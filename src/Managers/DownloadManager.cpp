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


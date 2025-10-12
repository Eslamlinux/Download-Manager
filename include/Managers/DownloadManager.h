#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "Models/DownloadItem.h"
#include "Models/AppSettings.h"
#include "Database/DatabaseManager.h"
#include <vector>
#include <wx/event.h>

// Custom event for download operations
wxDECLARE_EVENT(wxEVT_DOWNLOAD_OPERATION, wxCommandEvent);

// Event IDs
enum {
    ID_DownloadOperation = 10000,
    ID_YouTubeDownload
};

// Forward declarations
class MainFrame;

// Download manager class
class DownloadManager {
public:
    // Constructors and destructor
    DownloadManager();
    DownloadManager(MainFrame* mainFrame, const AppSettings& settings);
    ~DownloadManager();
    
    // Public methods
    int AddDownload(const wxString& url, const wxString& savePath);
    int AddYouTubeDownload(const wxString& url, const wxString& savePath, const wxString& title, const wxString& format);
    void StartDownload(int id);
    void StartDownloads(const std::vector<int>& ids);
    void PauseDownload(int id);
    void PauseDownloads(const std::vector<int>& ids);
    void ResumeDownload(int id);
    void ResumeDownloads(const std::vector<int>& ids);
    void CancelDownload(int id);
    void CancelDownloads(const std::vector<int>& ids);
    void DeleteDownload(int id);
    void DeleteDownloads(const std::vector<int>& ids);
    DownloadItem* GetDownloadById(int id);
    const std::vector<DownloadItem>& GetDownloads() const;
    
    // Speed limit methods
    void SetSpeedLimit(long limit);
    long GetSpeedLimit() const;
    
    // Settings methods
    void SaveSettings(const AppSettings& settings);
    const AppSettings& GetSettings() const;
    
private:
    // Private methods
    void Start();
    void Stop();
    void LoadDownloads();
    void ProcessDownload(DownloadItem* item);
    wxString TransformTvQuranUrl(const wxString& originalUrl);
    wxString EncodeURL(const wxString& url);
    
    // Member variables
    MainFrame* m_mainFrame;
    AppSettings m_settings;
    DatabaseManager* m_databaseManager;
    std::vector<DownloadItem> m_downloads;
    int m_nextId;
    bool m_isRunning;
    long m_speedLimit; // in KB/s
};

#endif // DOWNLOADMANAGER_H

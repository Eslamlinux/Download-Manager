#ifndef YOUTUBEDOWNLOADER_H
#define YOUTUBEDOWNLOADER_H

#include <wx/wx.h>
#include <functional>
#include "Models/AppSettings.h"

// Forward declaration
class MainFrame;

// YouTube video information
struct YouTubeVideoInfo {
    wxString title;
    wxString description;
    wxString thumbnail;
    wxString duration;
    wxString author;
    wxString viewCount;
    wxString uploadDate;
    wxArrayString formats;
};

class YouTubeDownloader {
public:
    YouTubeDownloader();
    YouTubeDownloader(MainFrame* mainFrame, const AppSettings& settings);
    ~YouTubeDownloader();
    
    // Changed to match implementation
    void GetVideoInfo(const wxString& url, std::function<void(const wxString&)> callback);
    
    // Add this method to match what we declared in MainFrame
    YouTubeVideoInfo GetVideoInfo(const wxString& url);
    
    // Add this method to access the executable path
    wxString GetExecutablePath() const { return m_executablePath; }
    
private:
    MainFrame* m_mainFrame;
    AppSettings m_settings;
    wxString m_executablePath; // Add this missing member
};

#endif // YOUTUBEDOWNLOADER_H

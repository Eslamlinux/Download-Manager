#include "Managers/YouTubeDownloader.h"
#include "UI/MainFrame.h"
#include <wx/process.h>
#include <wx/txtstrm.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

YouTubeDownloader::YouTubeDownloader()
    : m_mainFrame(nullptr)
{
    // Find youtube-dl executable
    wxString path = wxStandardPaths::Get().GetExecutablePath();
    wxFileName fileName(path);
    fileName.SetFullName("youtube-dl");
    path = fileName.GetFullPath();
    
    if (!wxFileExists(path)) {
        path = "";
    }
    
    m_executablePath = path.IsEmpty() ? "youtube-dl" : path;
    
    wxLogMessage("YouTubeDownloader initialized, executable: %s", m_executablePath);
}

YouTubeDownloader::YouTubeDownloader(MainFrame* mainFrame, const AppSettings& settings)
    : m_mainFrame(mainFrame), m_settings(settings)
{
    // Find youtube-dl executable (same as default constructor)
    wxString path = wxStandardPaths::Get().GetExecutablePath();
    wxFileName fileName(path);
    fileName.SetFullName("youtube-dl");
    path = fileName.GetFullPath();
    
    if (!wxFileExists(path)) {
        path = "";
    }
    
    m_executablePath = path.IsEmpty() ? "youtube-dl" : path;
    
    wxLogMessage("YouTubeDownloader initialized, executable: %s", m_executablePath);
}

YouTubeDownloader::~YouTubeDownloader()
{
    wxLogMessage("YouTubeDownloader destroyed");
}

void YouTubeDownloader::GetVideoInfo(const wxString& url, std::function<void(const wxString&)> callback) {
    // Execute youtube-dl to get video info
    wxString command = wxString::Format("%s --dump-json \"%s\"", m_executablePath, url);
    
    wxLogMessage("Executing command: %s", command);
    
    wxArrayString output;
    wxExecute(command, output, wxEXEC_SYNC);
    
    wxString json;
    for (const auto& line : output) {
        json += line;
    }
    
    callback(json);
}

// Implement the synchronous version that returns a YouTubeVideoInfo
YouTubeVideoInfo YouTubeDownloader::GetVideoInfo(const wxString& url) {
    YouTubeVideoInfo info;
    
    // Execute youtube-dl to get video info
    wxString command = wxString::Format("%s --dump-json \"%s\"", m_executablePath, url);
    
    wxLogMessage("Executing command: %s", command);
    
    wxArrayString output;
    wxExecute(command, output, wxEXEC_SYNC);
    
    wxString json;
    for (const auto& line : output) {
        json += line;
    }
    
    // Parse JSON and fill info (simplified for now)
    if (!json.IsEmpty()) {
        // Just set a dummy title for now
        info.title = "YouTube Video";
        info.formats.Add("best");
    }
    
    return info;
}

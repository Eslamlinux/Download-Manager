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



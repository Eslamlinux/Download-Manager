#include "Models/AppSettings.h"
#include <wx/config.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

// Constructor
AppSettings::AppSettings()
    : defaultSavePath(wxStandardPaths::Get().GetDocumentsDir())
    , maxSimultaneousDownloads(3)
    , showNotifications(true)
    , minimizeToTray(false)
    , startWithWindows(false)
    , youtubeExecutablePath("")
    , youtubeDefaultFormat("best")
{
}

// Load settings
void AppSettings::Load()
{
    // Load settings from config file or registry
    wxConfig config("AdvancedDownloadManager");
    
    config.Read("DefaultSavePath", &defaultSavePath, wxStandardPaths::Get().GetDocumentsDir());
    config.Read("MaxSimultaneousDownloads", &maxSimultaneousDownloads, 3);
    config.Read("ShowNotifications", &showNotifications, true);
    config.Read("MinimizeToTray", &minimizeToTray, false);
    config.Read("StartWithWindows", &startWithWindows, false);
    config.Read("YouTubeExecutablePath", &youtubeExecutablePath, "");
    config.Read("YouTubeDefaultFormat", &youtubeDefaultFormat, "best");
}

// Save settings
void AppSettings::Save()
{
    // Save settings to config file or registry
    wxConfig config("AdvancedDownloadManager");
    
    config.Write("DefaultSavePath", defaultSavePath);
    config.Write("MaxSimultaneousDownloads", maxSimultaneousDownloads);
    config.Write("ShowNotifications", showNotifications);
    config.Write("MinimizeToTray", minimizeToTray);
    config.Write("StartWithWindows", startWithWindows);
    config.Write("YouTubeExecutablePath", youtubeExecutablePath);
    config.Write("YouTubeDefaultFormat", youtubeDefaultFormat);
}

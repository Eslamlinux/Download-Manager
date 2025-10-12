#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <wx/string.h>

class AppSettings {
public:
    // Constructor and destructor
    AppSettings();
    
    // Public methods
    void Load();
    void Save();
    
    // Settings
    wxString defaultSavePath;
    int maxSimultaneousDownloads;
    bool showNotifications;
    bool minimizeToTray;
    bool startWithWindows;
    wxString youtubeExecutablePath;
    wxString youtubeDefaultFormat;
};

#endif

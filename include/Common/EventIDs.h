#ifndef EVENTIDS_H
#define EVENTIDS_H

// Event IDs
enum {
    // UI events
    ID_Timer = 1000,
    ID_UpdateUI,
    ID_DownloadList,
    
    // Menu and toolbar events
    ID_AddDownload,
    ID_AddYouTubeDownload,
    ID_StartDownload,
    ID_PauseDownload,
    ID_ResumeDownload,
    ID_CancelDownload,
    ID_DeleteDownload,
    ID_OpenFile,
    ID_OpenFolder,
    ID_CopyURL,
    ID_Settings,
    ID_SpeedLimit
};

#endif // EVENTIDS_H

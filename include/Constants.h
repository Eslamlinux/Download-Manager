#pragma once

#include <wx/wx.h>

// معرفات الأحداث المخصصة
enum {
    ID_NewDownload = wxID_HIGHEST + 1,
    ID_StartDownload,
    ID_PauseDownload,
    ID_ResumeDownload,
    ID_CancelDownload,
    ID_DeleteDownload,
    ID_DownloadAdded,
    ID_DownloadStatusChanged,
    ID_DownloadProgressChanged,
    ID_Settings,
    ID_OpenFile,
    ID_OpenLocation,
    ID_CopyURL,
    ID_PasteURL,
    ID_YouTubeDownload,
    ID_Exit,
    ID_About,
    ID_CheckForUpdates
};

// حالة التنزيل
enum DownloadStatus {
    PENDING,
    DOWNLOADING,
    PAUSED,
    COMPLETED,
    ERROR
};

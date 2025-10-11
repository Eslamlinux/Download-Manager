#include "UI/MainFrame.h"
#include "UI/DownloadDialog.h"
#include "UI/YouTubeDialog.h"
#include "UI/SettingsDialog.h"
#include "UI/SpeedLimitDialog.h"
#include "Common/EventIDs.h"
#include <wx/msgdlg.h>
#include <wx/aboutdlg.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/utils.h>
#include <wx/process.h>
#include <wx/filename.h>
#include <wx/textdlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/log.h>
#include <wx/artprov.h>
#include <wx/textfile.h>
#include <mutex>

// Global mutex for UI updates
std::mutex g_uiMutex;

// Constructor
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // Load settings
    m_settings.Load();

    // Create download manager
    m_downloadManager = new DownloadManager(this, m_settings);

    // Create status bar first, before any SetStatusText() calls
    CreateStatusBar(2);

    // Create UI
    CreateUI();

    // Set icon
    // SetIcon(wxICON(appicon)); // Commented out until we have an icon

    // Set initial status text
    SetStatusText("Ready");

    // Set up timer for UI updates
    m_timer = new wxTimer(this, ID_Timer);
    m_timer->Start(1000); // 1 second interval

    // Connect event handlers
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAddDownload, this, ID_AddDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAddYouTubeDownload, this, ID_AddYouTubeDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnStartDownload, this, ID_StartDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnPauseDownload, this, ID_PauseDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnResumeDownload, this, ID_ResumeDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCancelDownload, this, ID_CancelDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnDeleteDownload, this, ID_DeleteDownload);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpenFile, this, ID_OpenFile);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpenFolder, this, ID_OpenFolder);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnCopyURL, this, ID_CopyURL);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSettings, this, ID_Settings);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSpeedLimit, this, ID_SpeedLimit);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnUpdateUI, this, ID_UpdateUI);
    Bind(wxEVT_TIMER, &MainFrame::OnTimer, this, ID_Timer);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);

    // Connect download operation event handler
    Bind(wxEVT_DOWNLOAD_OPERATION, &MainFrame::OnDownloadOperation, this);

    wxLogMessage("MainFrame created");
}


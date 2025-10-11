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


// Destructor
MainFrame::~MainFrame()
{
    // Stop timer
    if (m_timer) {
        m_timer->Stop();
        delete m_timer;
        m_timer = nullptr;
    }
    
    // Delete download manager
    if (m_downloadManager) {
        delete m_downloadManager;
        m_downloadManager = nullptr;
    }
    
    // Save settings
    m_settings.Save();
    
    wxLogMessage("MainFrame destroyed");
}

// Create UI
void MainFrame::CreateUI()
{
    // Create menu bar
    wxMenuBar* menuBar = new wxMenuBar();
    
    // File menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_AddDownload, "&Add Download...\tCtrl+N", "Add a new download");
    fileMenu->Append(ID_AddYouTubeDownload, "Add &YouTube Download...\tCtrl+Y", "Add a new YouTube download");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Settings, "&Settings...\tCtrl+S", "Configure settings");
    fileMenu->Append(ID_SpeedLimit, "Speed &Limit...\tCtrl+L", "Set download speed limit");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4", "Exit the application");
    menuBar->Append(fileMenu, "&File");
    
    // Download menu
    wxMenu* downloadMenu = new wxMenu();
    downloadMenu->Append(ID_StartDownload, "&Start\tF9", "Start selected download(s)");
    downloadMenu->Append(ID_PauseDownload, "&Pause\tF10", "Pause selected download(s)");
    downloadMenu->Append(ID_ResumeDownload, "&Resume\tF11", "Resume selected download(s)");
    downloadMenu->Append(ID_CancelDownload, "&Cancel\tF12", "Cancel selected download(s)");
    downloadMenu->AppendSeparator();
    downloadMenu->Append(ID_DeleteDownload, "&Delete\tDel", "Delete selected download(s)");
    downloadMenu->AppendSeparator();
    downloadMenu->Append(ID_OpenFile, "Open &File\tCtrl+O", "Open downloaded file");
    downloadMenu->Append(ID_OpenFolder, "Open F&older\tCtrl+F", "Open containing folder");
    downloadMenu->Append(ID_CopyURL, "&Copy URL\tCtrl+C", "Copy download URL to clipboard");
    menuBar->Append(downloadMenu, "&Download");



    // Help menu
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT, "&About...", "Show about dialog");
    menuBar->Append(helpMenu, "&Help");
    
    // Set menu bar
    SetMenuBar(menuBar);
    
    // Create toolbar
    wxToolBar* toolBar = CreateToolBar();
    toolBar->AddTool(ID_AddDownload, "Add Download", wxArtProvider::GetBitmap(wxART_NEW), "Add a new download");
    toolBar->AddTool(ID_AddYouTubeDownload, "Add YouTube Download", wxArtProvider::GetBitmap(wxART_NEW), "Add a new YouTube download");
    toolBar->AddSeparator();
    toolBar->AddTool(ID_StartDownload, "Start", wxArtProvider::GetBitmap(wxART_GO_FORWARD), "Start selected download(s)");
    toolBar->AddTool(ID_PauseDownload, "Pause", wxArtProvider::GetBitmap(wxART_TICK_MARK), "Pause selected download(s)");
    toolBar->AddTool(ID_ResumeDownload, "Resume", wxArtProvider::GetBitmap(wxART_GO_FORWARD), "Resume selected download(s)");
    toolBar->AddTool(ID_CancelDownload, "Cancel", wxArtProvider::GetBitmap(wxART_CROSS_MARK), "Cancel selected download(s)");
    toolBar->AddSeparator();
    toolBar->AddTool(ID_DeleteDownload, "Delete", wxArtProvider::GetBitmap(wxART_DELETE), "Delete selected download(s)");
    toolBar->AddSeparator();
    toolBar->AddTool(ID_Settings, "Settings", wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE), "Configure settings");
    toolBar->Realize();
    
    // Create download list
    m_downloadList = new wxListCtrl(this, ID_DownloadList, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    
    // Add columns
    m_downloadList->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT, 50);
    m_downloadList->InsertColumn(1, "Name", wxLIST_FORMAT_LEFT, 200);
    m_downloadList->InsertColumn(2, "Status", wxLIST_FORMAT_LEFT, 100);
    m_downloadList->InsertColumn(3, "Progress", wxLIST_FORMAT_LEFT, 100);
    m_downloadList->InsertColumn(4, "Size", wxLIST_FORMAT_LEFT, 100);
    m_downloadList->InsertColumn(5, "Speed", wxLIST_FORMAT_LEFT, 100);
    m_downloadList->InsertColumn(6, "URL", wxLIST_FORMAT_LEFT, 300);
    m_downloadList->InsertColumn(7, "Date Added", wxLIST_FORMAT_LEFT, 150);
    
    // Create sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_downloadList, 1, wxEXPAND | wxALL, 5);
    
    // Set sizer
    SetSizer(sizer);
    
    // Connect download list event handlers
    m_downloadList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &MainFrame::OnDownloadListItemActivated, this);
    m_downloadList->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &MainFrame::OnDownloadListItemRightClick, this);
    
    // Update UI
    UpdateUI();
}
 

// Update UI
void MainFrame::UpdateUI()
{
    std::lock_guard<std::mutex> lock(g_uiMutex);
    
    // Save selected items before clearing the list
    std::vector<int> selectedIds = GetSelectedDownloadIds();
    
    // Remember the focused item if any
    long focusedItem = m_downloadList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    int focusedId = -1;
    if (focusedItem != -1) {
        focusedId = wxAtoi(m_downloadList->GetItemText(focusedItem));
    }
    
    // Clear download list
    m_downloadList->DeleteAllItems();
    
    // Get downloads
    const std::vector<DownloadItem>& downloads = m_downloadManager->GetDownloads();
    
    // Add downloads to list
    for (size_t i = 0; i < downloads.size(); i++) {
        const DownloadItem& item = downloads[i];
        
        // Add item
        long index = m_downloadList->InsertItem(i, wxString::Format("%d", item.id));
        m_downloadList->SetItem(index, 1, item.name);
        
        // Set status
        wxString status;
        switch (item.status) {
            case DownloadStatus::PENDING:
                status = "Pending";
                break;
            case DownloadStatus::DOWNLOADING:
                status = "Downloading";
                break;
            case DownloadStatus::PAUSED:
                status = "Paused";
                break;
            case DownloadStatus::COMPLETED:
                status = "Completed";
                break;
            case DownloadStatus::ERROR:
                status = "Error";
                break;
            default:
                status = "Unknown";
                break;
        }
        m_downloadList->SetItem(index, 2, status);
        
        // Set progress
        m_downloadList->SetItem(index, 3, wxString::Format("%d%%", item.progress));
        
        // Set size
        wxString size;
        if (item.size > 0) {
            if (item.size < 1024) {
                size = wxString::Format("%lld B", item.size);
            } else if (item.size < 1024 * 1024) {
                size = wxString::Format("%.2f KB", item.size / 1024.0);
            } else if (item.size < 1024 * 1024 * 1024) {
                size = wxString::Format("%.2f MB", item.size / (1024.0 * 1024.0));
            } else {
                size = wxString::Format("%.2f GB", item.size / (1024.0 * 1024.0 * 1024.0));
            }
        } else {
            size = "Unknown";
        }
        m_downloadList->SetItem(index, 4, size);
        
        // Set speed
        wxString speed;
        if (item.status == DownloadStatus::DOWNLOADING && item.speed > 0) {
            if (item.speed < 1024) {
                speed = wxString::Format("%lld B/s", item.speed);
            } else if (item.speed < 1024 * 1024) {
                speed = wxString::Format("%.2f KB/s", item.speed / 1024.0);
            } else {
                speed = wxString::Format("%.2f MB/s", item.speed / (1024.0 * 1024.0));
            }
        } else {
            speed = "-";
        }
        m_downloadList->SetItem(index, 5, speed);
        
        // Set URL
        m_downloadList->SetItem(index, 6, item.url);
        
        // Set date added
        m_downloadList->SetItem(index, 7, item.dateAdded);
        
        // Restore selection if this item was previously selected
        if (std::find(selectedIds.begin(), selectedIds.end(), item.id) != selectedIds.end()) {
            m_downloadList->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
        
        // Restore focus if this was the focused item
        if (item.id == focusedId) {
            m_downloadList->SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        }
    }


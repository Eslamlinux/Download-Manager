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

   
    // Update status bar
    int totalDownloads = downloads.size();
    int activeDownloads = 0;
    int completedDownloads = 0;
    
    for (const auto& item : downloads) {
        if (item.status == DownloadStatus::DOWNLOADING) {
            activeDownloads++;
        } else if (item.status == DownloadStatus::COMPLETED) {
            completedDownloads++;
        }
    }
    
    SetStatusText(wxString::Format("Total: %d | Active: %d | Completed: %d", totalDownloads, activeDownloads, completedDownloads), 0);
    
    // Update speed limit status
    long speedLimit = m_downloadManager->GetSpeedLimit();
    if (speedLimit > 0) {
        SetStatusText(wxString::Format("Speed Limit: %ld KB/s", speedLimit), 1);
    } else {
        SetStatusText("Speed Limit: Unlimited", 1);
    }
}

// Event handlers
void MainFrame::OnAddDownload(wxCommandEvent& event)
{
    // Show download dialog
    DownloadDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK) {
        // Add download
        wxString url = dialog.GetURL();
        wxString savePath = dialog.GetSavePath();
        
        if (!url.IsEmpty() && !savePath.IsEmpty()) {
            m_downloadManager->AddDownload(url, savePath);
            UpdateUI();
        }
    }
}

void MainFrame::OnAddYouTubeDownload(wxCommandEvent& event)
{
    // Check if youtube-dl path is set
    if (m_settings.youtubeExecutablePath.IsEmpty()) {
        wxMessageBox("YouTube-DL path is not set. Please set it in the settings.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Show YouTube dialog
    YouTubeDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK) {
        // Add YouTube download
        wxString url = dialog.GetURL();
        wxString savePath = dialog.GetSavePath();
        wxString title = dialog.GetTitle();
        wxString format = dialog.GetFormat();
        
        if (!url.IsEmpty() && !savePath.IsEmpty()) {
            m_downloadManager->AddYouTubeDownload(url, savePath, title, format);
            UpdateUI();
        }
    }
}

void MainFrame::OnStartDownload(wxCommandEvent& event)
{
    // Get selected items
    std::vector<int> selectedIds = GetSelectedDownloadIds();
    
    if (selectedIds.empty()) {
        wxMessageBox("No downloads selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Start downloads
    m_downloadManager->StartDownloads(selectedIds);
    UpdateUI();
}

void MainFrame::OnPauseDownload(wxCommandEvent& event)
{
    // Get selected items
    std::vector<int> selectedIds = GetSelectedDownloadIds();
    
    if (selectedIds.empty()) {
        wxMessageBox("No downloads selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Pause downloads
    m_downloadManager->PauseDownloads(selectedIds);
    UpdateUI();
}

void MainFrame::OnResumeDownload(wxCommandEvent& event)
{
    // Get selected items
    std::vector<int> selectedIds = GetSelectedDownloadIds();
    
    if (selectedIds.empty()) {
        wxMessageBox("No downloads selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Resume downloads
    m_downloadManager->ResumeDownloads(selectedIds);
    UpdateUI();
}

void MainFrame::OnCancelDownload(wxCommandEvent& event)
{
    // Get selected items
    std::vector<int> selectedIds = GetSelectedDownloadIds();
    
    if (selectedIds.empty()) {
        wxMessageBox("No downloads selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Cancel downloads
    m_downloadManager->CancelDownloads(selectedIds);
    UpdateUI();
}

void MainFrame::OnDeleteDownload(wxCommandEvent& event)
{
    // Get selected items
    std::vector<int> selectedIds = GetSelectedDownloadIds();
    
    if (selectedIds.empty()) {
        wxMessageBox("No downloads selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Confirm deletion
    wxMessageDialog dialog(this, wxString::Format("Are you sure you want to delete %zu selected download(s)?", selectedIds.size()), "Confirm Deletion", wxYES_NO | wxICON_QUESTION);
    if (dialog.ShowModal() == wxID_YES) {
        // Delete downloads
        m_downloadManager->DeleteDownloads(selectedIds);
        UpdateUI();
    }
}



void MainFrame::OnOpenFile(wxCommandEvent& event)
{
    // Get selected item
    long selectedIndex = m_downloadList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selectedIndex == -1) {
        wxMessageBox("No download selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Get download ID
    int id = wxAtoi(m_downloadList->GetItemText(selectedIndex));
    
    // Get download
    DownloadItem* item = m_downloadManager->GetDownloadById(id);
    if (!item) {
        wxMessageBox("Download not found.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Check if completed
    if (item->status != DownloadStatus::COMPLETED) {
        wxMessageBox("Download is not completed.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Open file
    wxString filePath = item->savePath + wxFileName::GetPathSeparator() + item->name;
    if (!wxFileExists(filePath)) {
        wxMessageBox("File not found: " + filePath, "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    wxLaunchDefaultApplication(filePath);
}


void MainFrame::OnOpenFolder(wxCommandEvent& event)
{
    // Get selected item
    long selectedIndex = m_downloadList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selectedIndex == -1) {
        wxMessageBox("No download selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Get download ID
    int id = wxAtoi(m_downloadList->GetItemText(selectedIndex));
    
    // Get download
    DownloadItem* item = m_downloadManager->GetDownloadById(id);
    if (!item) {
        wxMessageBox("Download not found.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Open folder
    if (!wxDirExists(item->savePath)) {
        wxMessageBox("Folder not found: " + item->savePath, "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    wxLaunchDefaultApplication(item->savePath);
}

void MainFrame::OnCopyURL(wxCommandEvent& event)
{
    // Get selected item
    long selectedIndex = m_downloadList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selectedIndex == -1) {
        wxMessageBox("No download selected.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Get download ID
    int id = wxAtoi(m_downloadList->GetItemText(selectedIndex));
    
    // Get download
    DownloadItem* item = m_downloadManager->GetDownloadById(id);
    if (!item) {
        wxMessageBox("Download not found.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Copy URL to clipboard
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(item->url));
        wxTheClipboard->Close();
        SetStatusText("URL copied to clipboard", 0);
    }
}


void MainFrame::OnSettings(wxCommandEvent& event)
{
    // Show settings dialog
    SettingsDialog dialog(this, m_settings);
    if (dialog.ShowModal() == wxID_OK) {
        // Save settings
        m_settings = dialog.GetSettings();
        m_downloadManager->SaveSettings(m_settings);
        
        // Update UI
        UpdateUI();
    }
}

void MainFrame::OnSpeedLimit(wxCommandEvent& event)
{
    // Show speed limit dialog
    SpeedLimitDialog dialog(this, m_downloadManager->GetSpeedLimit());
    if (dialog.ShowModal() == wxID_OK) {
        // Set speed limit
        m_downloadManager->SetSpeedLimit(dialog.GetSpeedLimit());
        
        // Update UI
        UpdateUI();
    }
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    // Show about dialog
    wxAboutDialogInfo info;
    info.SetName("Advanced Download Manager");
    info.SetVersion("1.0");
    info.SetDescription("A download manager application built with wxWidgets and libcurl.");
    info.SetCopyright("(C) 2023");
    info.AddDeveloper("Developer");
    info.SetWebSite("https://example.com");
    info.SetLicense("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.");
    
    wxAboutBox(info);
}


void MainFrame::OnUpdateUI(wxCommandEvent& event)
{
    UpdateUI();
}

void MainFrame::OnTimer(wxTimerEvent& event)
{
    static int updateCounter = 0;
    
    // Only update UI every 2 seconds instead of every 1 second
    // This reduces the frequency of selection resets
    if (++updateCounter >= 2) {
        UpdateUI();
        updateCounter = 0;
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    // Check if there are active downloads
    const std::vector<DownloadItem>& downloads = m_downloadManager->GetDownloads();
    bool hasActiveDownloads = false;
    
    for (const auto& item : downloads) {
        if (item.status == DownloadStatus::DOWNLOADING) {
            hasActiveDownloads = true;
            break;
        }
    }
    
    if (hasActiveDownloads) {
        // Ask for confirmation
        wxMessageDialog dialog(this, "There are active downloads. Are you sure you want to exit?", "Confirm Exit", wxYES_NO | wxICON_QUESTION);
        if (dialog.ShowModal() != wxID_YES) {
            event.Veto();
            return;
        }
    }
    
    // Destroy the frame
    Destroy();
}

void MainFrame::OnDownloadListItemActivated(wxListEvent& event)
{
    // Get download ID
    int id = wxAtoi(m_downloadList->GetItemText(event.GetIndex()));
    
    // Get download
    DownloadItem* item = m_downloadManager->GetDownloadById(id);
    if (!item) {
        return;
    }
    
    // Handle based on status
    switch (item->status) {
        case DownloadStatus::PENDING:
            m_downloadManager->StartDownload(id);
            break;
        case DownloadStatus::DOWNLOADING:
            m_downloadManager->PauseDownload(id);
            break;
        case DownloadStatus::PAUSED:
            m_downloadManager->ResumeDownload(id);
            break;
        case DownloadStatus::COMPLETED:
            // Open file
            {
                wxString filePath = item->savePath + wxFileName::GetPathSeparator() + item->name;
                if (wxFileExists(filePath)) {
                    wxLaunchDefaultApplication(filePath);
                }
            }
            break;
        case DownloadStatus::ERROR:
            // Retry
            m_downloadManager->StartDownload(id);
            break;
    }
    
    // Update UI
    UpdateUI();
}


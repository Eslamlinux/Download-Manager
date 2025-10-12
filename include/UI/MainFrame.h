#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/timer.h>
#include "Models/AppSettings.h"
#include "Managers/DownloadManager.h"

// Main frame class
class MainFrame : public wxFrame {
public:
  // Constructor and destructor
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  ~MainFrame();
  
  // Public methods
  void UpdateUI();
  
  // Event handler for download operations
  void OnDownloadOperation(wxCommandEvent& event);
  
  // Getter for download manager
  DownloadManager* GetDownloadManager() const { return m_downloadManager; }
  
private:
  // Private methods
  void CreateUI();
  std::vector<int> GetSelectedDownloadIds();
  
  // Event handlers
  void OnAddDownload(wxCommandEvent& event);
  void OnAddYouTubeDownload(wxCommandEvent& event);
  void OnStartDownload(wxCommandEvent& event);
  void OnPauseDownload(wxCommandEvent& event);
  void OnResumeDownload(wxCommandEvent& event);
  void OnCancelDownload(wxCommandEvent& event);
  void OnDeleteDownload(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void OnOpenFolder(wxCommandEvent& event);
  void OnCopyURL(wxCommandEvent& event);
  void OnSettings(wxCommandEvent& event);
  void OnSpeedLimit(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnUpdateUI(wxCommandEvent& event);
  void OnTimer(wxTimerEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnDownloadListItemActivated(wxListEvent& event);
  void OnDownloadListItemRightClick(wxListEvent& event);
  
  // Member variables
  wxListCtrl* m_downloadList;
  wxTimer* m_timer;
  DownloadManager* m_downloadManager;
  AppSettings m_settings;
};

#endif // MAINFRAME_H

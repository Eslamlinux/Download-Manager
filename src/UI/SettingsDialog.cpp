#include "UI/SettingsDialog.h"
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>

// Constructor
SettingsDialog::SettingsDialog(wxWindow* parent, const AppSettings& settings)
  : wxDialog(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(500, 400))
{
  // Copy settings
  m_settings = settings;
  
  // Create UI
  CreateUI();
  
  // Center dialog
  CenterOnParent();
}

// Create UI
void SettingsDialog::CreateUI()
{
  // Create main sizer
  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  
  // Create notebook
  wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
  
  // Create general panel
  wxPanel* generalPanel = new wxPanel(notebook);
  wxBoxSizer* generalSizer = new wxBoxSizer(wxVERTICAL);
  
  // Default save path
  wxBoxSizer* savePathSizer = new wxBoxSizer(wxHORIZONTAL);
  savePathSizer->Add(new wxStaticText(generalPanel, wxID_ANY, "Default Save Path:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_savePathCtrl = new wxTextCtrl(generalPanel, wxID_ANY, m_settings.defaultSavePath);
  savePathSizer->Add(m_savePathCtrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  wxButton* browseSavePathButton = new wxButton(generalPanel, wxID_ANY, "Browse...");
  savePathSizer->Add(browseSavePathButton, 0, wxALIGN_CENTER_VERTICAL);
  generalSizer->Add(savePathSizer, 0, wxEXPAND | wxALL, 10);
  
  // Max downloads
  wxBoxSizer* maxDownloadsSizer = new wxBoxSizer(wxHORIZONTAL);
  maxDownloadsSizer->Add(new wxStaticText(generalPanel, wxID_ANY, "Max Simultaneous Downloads:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_maxDownloadsCtrl = new wxSpinCtrl(generalPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, m_settings.maxSimultaneousDownloads);
  maxDownloadsSizer->Add(m_maxDownloadsCtrl, 0, wxALIGN_CENTER_VERTICAL);
  generalSizer->Add(maxDownloadsSizer, 0, wxEXPAND | wxALL, 10);
  
  // Show notifications
  m_showNotificationsCheck = new wxCheckBox(generalPanel, wxID_ANY, "Show Notifications");
  m_showNotificationsCheck->SetValue(m_settings.showNotifications);
  generalSizer->Add(m_showNotificationsCheck, 0, wxEXPAND | wxALL, 10);
  
  // Start with Windows
  m_startWithWindowsCheck = new wxCheckBox(generalPanel, wxID_ANY, "Start with Windows");
  m_startWithWindowsCheck->SetValue(m_settings.startWithWindows);
  generalSizer->Add(m_startWithWindowsCheck, 0, wxEXPAND | wxALL, 10);
  
  // Minimize to tray
  m_minimizeToTrayCheck = new wxCheckBox(generalPanel, wxID_ANY, "Minimize to Tray");
  m_minimizeToTrayCheck->SetValue(m_settings.minimizeToTray);
  generalSizer->Add(m_minimizeToTrayCheck, 0, wxEXPAND | wxALL, 10);
  
  // Set sizer
  generalPanel->SetSizer(generalSizer);
  
  // Create YouTube panel
  wxPanel* youtubePanel = new wxPanel(notebook);
  wxBoxSizer* youtubeSizer = new wxBoxSizer(wxVERTICAL);
  
  // YouTube-DL path
  wxBoxSizer* youtubeDlPathSizer = new wxBoxSizer(wxHORIZONTAL);
  youtubeDlPathSizer->Add(new wxStaticText(youtubePanel, wxID_ANY, "YouTube-DL Path:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_youtubeDlPathCtrl = new wxTextCtrl(youtubePanel, wxID_ANY, m_settings.youtubeExecutablePath);
  youtubeDlPathSizer->Add(m_youtubeDlPathCtrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  wxButton* browseYoutubeDlPathButton = new wxButton(youtubePanel, wxID_ANY, "Browse...");
  youtubeDlPathSizer->Add(browseYoutubeDlPathButton, 0, wxALIGN_CENTER_VERTICAL);
  youtubeSizer->Add(youtubeDlPathSizer, 0, wxEXPAND | wxALL, 10);
  
  // YouTube default format
  wxBoxSizer* youtubeFormatSizer = new wxBoxSizer(wxHORIZONTAL);
  youtubeFormatSizer->Add(new wxStaticText(youtubePanel, wxID_ANY, "Default Format:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  wxArrayString formatChoices;
  formatChoices.Add("best");
  formatChoices.Add("bestvideo+bestaudio");
  formatChoices.Add("mp4");
  formatChoices.Add("webm");
  formatChoices.Add("mp3");
  m_youtubeFormatCtrl = new wxChoice(youtubePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, formatChoices);
  m_youtubeFormatCtrl->SetStringSelection(m_settings.youtubeDefaultFormat);
  youtubeFormatSizer->Add(m_youtubeFormatCtrl, 0, wxALIGN_CENTER_VERTICAL);
  youtubeSizer->Add(youtubeFormatSizer, 0, wxEXPAND | wxALL, 10);
  
  // Set sizer
  youtubePanel->SetSizer(youtubeSizer);
  
  // Add panels to notebook
  notebook->AddPage(generalPanel, "General");
  notebook->AddPage(youtubePanel, "YouTube");
  
  // Add notebook to main sizer
  mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);
  
  // Add buttons
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxRIGHT, 5);
  buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0);
  mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);
  
  // Set sizer
  SetSizer(mainSizer);
  
  // Connect events
  browseSavePathButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnBrowseSavePath, this);
  browseYoutubeDlPathButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnBrowseYoutubeDlPath, this);
  Bind(wxEVT_BUTTON, &SettingsDialog::OnOK, this, wxID_OK);
}

// Event handlers
void SettingsDialog::OnBrowseSavePath(wxCommandEvent& event)
{
  // Show directory dialog
  wxDirDialog dialog(this, "Select Default Save Path", m_savePathCtrl->GetValue());
  if (dialog.ShowModal() == wxID_OK) {
      m_savePathCtrl->SetValue(dialog.GetPath());
  }
}

void SettingsDialog::OnBrowseYoutubeDlPath(wxCommandEvent& event)
{
  // Show file dialog
  wxFileDialog dialog(this, "Select YouTube-DL Executable", wxEmptyString, wxEmptyString, "Executable Files (*.exe)|*.exe|All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() == wxID_OK) {
      m_youtubeDlPathCtrl->SetValue(dialog.GetPath());
  }
}

void SettingsDialog::OnOK(wxCommandEvent& event)
{
  // Validate settings
  if (m_savePathCtrl->GetValue().IsEmpty()) {
      wxMessageBox("Default save path cannot be empty.", "Error", wxOK | wxICON_ERROR);
      return;
  }
  
  // Update settings
  m_settings.defaultSavePath = m_savePathCtrl->GetValue();
  m_settings.maxSimultaneousDownloads = m_maxDownloadsCtrl->GetValue();
  m_settings.showNotifications = m_showNotificationsCheck->GetValue();
  m_settings.startWithWindows = m_startWithWindowsCheck->GetValue();
  m_settings.minimizeToTray = m_minimizeToTrayCheck->GetValue();
  m_settings.youtubeExecutablePath = m_youtubeDlPathCtrl->GetValue();
  m_settings.youtubeDefaultFormat = m_youtubeFormatCtrl->GetStringSelection();
  
  // Close dialog
  EndModal(wxID_OK);
}

// Get settings
const AppSettings& SettingsDialog::GetSettings() const
{
  return m_settings;
}

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


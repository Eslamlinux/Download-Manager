#include "UI/DownloadDialog.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>

// Constructor
DownloadDialog::DownloadDialog(wxWindow* parent)
  : wxDialog(parent, wxID_ANY, "Add Download", wxDefaultPosition, wxSize(500, 200))
{
  // Create UI
  CreateUI();
  
  // Center dialog
  CenterOnParent();
}

// Create UI
void DownloadDialog::CreateUI()
{
  // Create main sizer
  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  
  // URL
  wxBoxSizer* urlSizer = new wxBoxSizer(wxHORIZONTAL);
  urlSizer->Add(new wxStaticText(this, wxID_ANY, "URL:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_urlCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
  urlSizer->Add(m_urlCtrl, 1, wxALIGN_CENTER_VERTICAL);
  mainSizer->Add(urlSizer, 0, wxEXPAND | wxALL, 10);
  
  // Save path
  wxBoxSizer* savePathSizer = new wxBoxSizer(wxHORIZONTAL);
  savePathSizer->Add(new wxStaticText(this, wxID_ANY, "Save Path:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_savePathCtrl = new wxTextCtrl(this, wxID_ANY);
  savePathSizer->Add(m_savePathCtrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  wxButton* browseButton = new wxButton(this, wxID_ANY, "Browse...");
  savePathSizer->Add(browseButton, 0, wxALIGN_CENTER_VERTICAL);
  mainSizer->Add(savePathSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
  
  // Add buttons
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxRIGHT, 5);
  buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0);
  mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);

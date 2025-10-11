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
  
  // Set sizer
  SetSizer(mainSizer);
  
  // Connect events
  browseButton->Bind(wxEVT_BUTTON, &DownloadDialog::OnBrowse, this);
  m_urlCtrl->Bind(wxEVT_TEXT_ENTER, &DownloadDialog::OnOK, this);
  Bind(wxEVT_BUTTON, &DownloadDialog::OnOK, this, wxID_OK);
  
  // Set focus to URL control
  m_urlCtrl->SetFocus();
}

// Event handlers
void DownloadDialog::OnBrowse(wxCommandEvent& event)
{
  // Show directory dialog
  wxDirDialog dialog(this, "Select Save Path");
  if (dialog.ShowModal() == wxID_OK) {
      m_savePathCtrl->SetValue(dialog.GetPath());
  }
}

void DownloadDialog::OnOK(wxCommandEvent& event)
{
  // Validate URL
  if (m_urlCtrl->GetValue().IsEmpty()) {
      wxMessageBox("URL cannot be empty.", "Error", wxOK | wxICON_ERROR);
      m_urlCtrl->SetFocus();
      return;
  }
  
  // Validate save path
  if (m_savePathCtrl->GetValue().IsEmpty()) {
      wxMessageBox("Save path cannot be empty.", "Error", wxOK | wxICON_ERROR);
      m_savePathCtrl->SetFocus();
      return;
  }
  
  // Close dialog
  EndModal(wxID_OK);
}

// Get URL
wxString DownloadDialog::GetURL() const
{
  return m_urlCtrl->GetValue();
}

// Get save path
wxString DownloadDialog::GetSavePath() const
{
  return m_savePathCtrl->GetValue();
}


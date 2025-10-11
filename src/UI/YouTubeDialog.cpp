#include "UI/YouTubeDialog.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dirdlg.h>
#include <wx/choice.h>
#include <wx/msgdlg.h>

// Constructor
YouTubeDialog::YouTubeDialog(wxWindow* parent)
  : wxDialog(parent, wxID_ANY, "Add YouTube Download", wxDefaultPosition, wxSize(500, 300))
{
  // Create UI
  CreateUI();
  
  // Center dialog
  CenterOnParent();
}

// Create UI
void YouTubeDialog::CreateUI()
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
  
  // Title
  wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
  titleSizer->Add(new wxStaticText(this, wxID_ANY, "Title:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_titleCtrl = new wxTextCtrl(this, wxID_ANY);
  titleSizer->Add(m_titleCtrl, 1, wxALIGN_CENTER_VERTICAL);
  mainSizer->Add(titleSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
  
  // Format
  wxBoxSizer* formatSizer = new wxBoxSizer(wxHORIZONTAL);
  formatSizer->Add(new wxStaticText(this, wxID_ANY, "Format:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  wxArrayString formatChoices;
  formatChoices.Add("best");
  formatChoices.Add("bestvideo+bestaudio");
  formatChoices.Add("mp4");
  formatChoices.Add("webm");
  formatChoices.Add("mp3");
  m_formatCtrl = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, formatChoices);
  m_formatCtrl->SetSelection(0);
  formatSizer->Add(m_formatCtrl, 0, wxALIGN_CENTER_VERTICAL);
  mainSizer->Add(formatSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);


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


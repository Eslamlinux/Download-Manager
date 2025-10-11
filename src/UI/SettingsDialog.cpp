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


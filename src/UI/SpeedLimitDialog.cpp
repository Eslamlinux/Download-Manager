#include "UI/SpeedLimitDialog.h"
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/spinctrl.h>

// Constructor
SpeedLimitDialog::SpeedLimitDialog(wxWindow* parent, long currentLimit)
  : wxDialog(parent, wxID_ANY, "Speed Limit", wxDefaultPosition, wxSize(300, 200))
{
  // Set current limit
  m_speedLimit = currentLimit;
  
  // Create UI
  CreateUI();
  
  // Center dialog
  CenterOnParent();
}

// Create UI
void SpeedLimitDialog::CreateUI()
{
  // Create main sizer
  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  
  // Create unlimited radio button
  m_unlimitedRadio = new wxRadioButton(this, wxID_ANY, "Unlimited", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  m_unlimitedRadio->SetValue(m_speedLimit == 0);
  mainSizer->Add(m_unlimitedRadio, 0, wxEXPAND | wxALL, 10);
  
  // Create limited radio button
  m_limitedRadio = new wxRadioButton(this, wxID_ANY, "Limited:");
  m_limitedRadio->SetValue(m_speedLimit > 0);
  mainSizer->Add(m_limitedRadio, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
  
  // Create limit control
  wxBoxSizer* limitSizer = new wxBoxSizer(wxHORIZONTAL);
  m_limitCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100000, m_speedLimit > 0 ? m_speedLimit : 100);
  limitSizer->Add(m_limitCtrl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  limitSizer->Add(new wxStaticText(this, wxID_ANY, "KB/s"), 0, wxALIGN_CENTER_VERTICAL);
  mainSizer->Add(limitSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);


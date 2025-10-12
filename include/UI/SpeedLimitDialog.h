#ifndef SPEEDLIMITDIALOG_H
#define SPEEDLIMITDIALOG_H

#include <wx/dialog.h>

// Forward declarations
class wxRadioButton;
class wxSpinCtrl;

// Speed limit dialog class
class SpeedLimitDialog : public wxDialog {
public:
    // Constructor and destructor
    SpeedLimitDialog(wxWindow* parent, long currentLimit);
    
    // Get speed limit
    long GetSpeedLimit() const;
    
private:
    // Private methods
    void CreateUI();
    void UpdateUI();
    
    // Event handlers
    void OnRadioButton(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    
    // Member variables
    long m_speedLimit;
    wxRadioButton* m_unlimitedRadio;
    wxRadioButton* m_limitedRadio;
    wxSpinCtrl* m_limitCtrl;
};

#endif // SPEEDLIMITDIALOG_H

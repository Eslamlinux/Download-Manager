#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <wx/dialog.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include "Models/AppSettings.h"

// Forward declarations
class wxTextCtrl;
class wxCheckBox;

// Settings dialog class
class SettingsDialog : public wxDialog {
public:
    // Constructor and destructor
    SettingsDialog(wxWindow* parent, const AppSettings& settings);
    
    // Get settings
    const AppSettings& GetSettings() const;
    
private:
    // Private methods
    void CreateUI();
    
    // Event handlers
    void OnBrowseSavePath(wxCommandEvent& event);
    void OnBrowseYoutubeDlPath(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    
    // Member variables
    AppSettings m_settings;
    wxTextCtrl* m_savePathCtrl;
    wxSpinCtrl* m_maxDownloadsCtrl;
    wxCheckBox* m_showNotificationsCheck;
    wxCheckBox* m_startWithWindowsCheck;
    wxCheckBox* m_minimizeToTrayCheck;
    wxTextCtrl* m_youtubeDlPathCtrl;
    wxChoice* m_youtubeFormatCtrl;
};

#endif // SETTINGSDIALOG_H

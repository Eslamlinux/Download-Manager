#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <wx/dialog.h>

// Forward declarations
class wxTextCtrl;
class wxButton;

// Download dialog class
class DownloadDialog : public wxDialog {
public:
    // Constructor and destructor
    DownloadDialog(wxWindow* parent);
    
    // Get URL and save path
    wxString GetURL() const;
    wxString GetSavePath() const;
    
private:
    // Private methods
    void CreateUI();
    
    // Event handlers
    void OnBrowse(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    
    // Member variables
    wxTextCtrl* m_urlCtrl;
    wxTextCtrl* m_savePathCtrl;
};

#endif // DOWNLOADDIALOG_H

#ifndef YOUTUBEDIALOG_H
#define YOUTUBEDIALOG_H

#include <wx/dialog.h>

// Forward declarations
class wxTextCtrl;
class wxButton;
class wxChoice;

// YouTube dialog class
class YouTubeDialog : public wxDialog {
public:
    // Constructor and destructor
    YouTubeDialog(wxWindow* parent);
    
    // Get URL, save path, title, and format
    wxString GetURL() const;
    wxString GetSavePath() const;
    wxString GetTitle() const;
    wxString GetFormat() const;
    
private:
    // Private methods
    void CreateUI();
    
    // Event handlers
    void OnBrowse(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    
    // Member variables
    wxTextCtrl* m_urlCtrl;
    wxTextCtrl* m_savePathCtrl;
    wxTextCtrl* m_titleCtrl;
    wxChoice* m_formatCtrl;
};

#endif // YOUTUBEDIALOG_H

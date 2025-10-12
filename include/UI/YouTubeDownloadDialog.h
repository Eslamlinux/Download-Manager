#pragma once

#include <wx/wx.h>
#include <wx/valtext.h>
#include "Managers/YouTubeDownloader.h"

class YouTubeDownloadDialog : public wxDialog {
public:
    YouTubeDownloadDialog(wxWindow* parent, YouTubeDownloader* downloader, const wxString& url);
    virtual ~YouTubeDownloadDialog();
    
private:
    YouTubeDownloader* m_downloader;
    wxString m_url;
    
    // عناصر واجهة المستخدم
    wxTextCtrl* m_titleCtrl;
    wxChoice* m_formatChoice;
    wxTextCtrl* m_savePathCtrl;
    wxButton* m_browseButton;
    
    // إنشاء واجهة المستخدم
    void CreateUI();
    
    // معالجات الأحداث
    void OnBrowse(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
};

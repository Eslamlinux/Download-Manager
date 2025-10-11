#include "UI/YouTubeDownloadDialog.h"
#include "UI/MainFrame.h"
#include "Managers/DownloadManager.h"
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/valtext.h>
#include <wx/log.h>
#include <wx/filename.h>

YouTubeDownloadDialog::YouTubeDownloadDialog(wxWindow* parent, YouTubeDownloader* downloader, const wxString& url)
    : wxDialog(parent, wxID_ANY, "YouTube Download", wxDefaultPosition, wxSize(500, 300)),
      m_downloader(downloader), m_url(url) {
    CreateUI();
}

YouTubeDownloadDialog::~YouTubeDownloadDialog() {
}

void YouTubeDownloadDialog::CreateUI() {
    // إنشاء الحاويات
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // إنشاء عناصر واجهة المستخدم
    wxStaticText* titleLabel = new wxStaticText(this, wxID_ANY, "عنوان الفيديو:");
    m_titleCtrl = new wxTextCtrl(this, wxID_ANY, "");
    
    wxStaticText* formatLabel = new wxStaticText(this, wxID_ANY, "تنسيق التنزيل:");
    wxArrayString formatChoices;
    formatChoices.Add("أفضل جودة");
    formatChoices.Add("MP4 360p");
    formatChoices.Add("MP4 720p");
    formatChoices.Add("M4A (صوت فقط)");
    formatChoices.Add("3GP 144p");
    m_formatChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, formatChoices);
    m_formatChoice->SetSelection(0);
    
    wxStaticText* savePathLabel = new wxStaticText(this, wxID_ANY, "مسار الحفظ:");
    m_savePathCtrl = new wxTextCtrl(this, wxID_ANY, wxGetHomeDir() + wxFileName::GetPathSeparator() + "Downloads");
    m_browseButton = new wxButton(this, wxID_ANY, "تصفح...");
    

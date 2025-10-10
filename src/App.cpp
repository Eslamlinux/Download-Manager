#include "UI/MainFrame.h"
#include <wx/wx.h>
#include <wx/log.h>
#include <curl/curl.h>

// تعريف الفئة الرئيسية للتطبيق
class AdvancedDownloadManagerApp : public wxApp {
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;
};

// تنفيذ التطبيق
wxIMPLEMENT_APP(AdvancedDownloadManagerApp);

bool AdvancedDownloadManagerApp::OnInit() {
    // تهيئة libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    
    // إنشاء النافذة الرئيسية
    MainFrame* frame = new MainFrame("مدير التنزيلات المتقدم", wxDefaultPosition, wxSize(800, 600));
    frame->Show(true);
    
    // تعيين النافذة الرئيسية
    SetTopWindow(frame);
    
    // إضافة تسجيل
    wxLogMessage("Application started");
    
    return true;
}

int AdvancedDownloadManagerApp::OnExit() {
    // تنظيف libcurl
    curl_global_cleanup();
    
    // إضافة تسجيل
    wxLogMessage("Application exited");
    
    return wxApp::OnExit();
}

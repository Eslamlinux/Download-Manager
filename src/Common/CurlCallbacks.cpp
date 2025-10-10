#include "Common/CurlCallbacks.h"
#include "Common/EventIDs.h"
#include "UI/MainFrame.h"

// دالة رد النداء للكتابة
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    FILE* file = static_cast<FILE*>(userp);
    size_t written = fwrite(contents, size, nmemb, file);
    return written;
}

// دالة رد النداء للتقدم
int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    DownloadItem* item = static_cast<DownloadItem*>(clientp);
    if (item) {
        // تحديث التقدم
        item->downloadedSize = dlnow;
        if (dltotal > 0) {
            item->size = dltotal;
            item->progress = static_cast<int>((dlnow * 100) / dltotal);
        }
        
        // حساب السرعة
        // (يمكن تنفيذ هذا لاحقًا)
        
        // إرسال حدث تحديث واجهة المستخدم
        if (item->mainFrame) {
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UpdateUI);
            wxPostEvent((wxEvtHandler*)item->mainFrame, event);
        }
    }
    
    return 0;  // 0 للاستمرار، غير 0 للإلغاء
}

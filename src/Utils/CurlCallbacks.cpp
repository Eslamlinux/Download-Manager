#include "Utils/CurlCallbacks.h"
#include "Models/DownloadItem.h"
#include "UI/MainFrame.h"
#include "Common/EventIDs.h"
#include <wx/wx.h>
#include <ctime>

// دالة رد النداء للكتابة
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    FILE* fp = (FILE*)userp;
    return fwrite(contents, size, nmemb, fp);
}

// دالة رد النداء للتقدم
int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    DownloadItem* item = (DownloadItem*)clientp;
    
    // تحديث معلومات التنزيل
    if (dltotal > 0) {
        item->size = dltotal;
        item->downloaded = dlnow;
        
        // حساب السرعة (يمكن تحسينه باستخدام الوقت)
        static time_t lastTime = 0;
        static curl_off_t lastBytes = 0;
        time_t now = time(NULL);
        
        if (now - lastTime >= 1) {  // تحديث كل ثانية
            item->speed = (dlnow - lastBytes) / (now - lastTime);
            lastTime = now;
            lastBytes = dlnow;
        }
        
        // إرسال حدث تحديث واجهة المستخدم إذا كان هناك إشارة إلى MainFrame
        if (item->mainFrame) {
            wxCommandEvent event(wxEVT_MENU, ID_UpdateUI);
            wxPostEvent((wxEvtHandler*)item->mainFrame, event);
        }
    }
    
    return 0;  // 0 للاستمرار، غير صفر للإلغاء
}

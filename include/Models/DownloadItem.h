#pragma once

#include <wx/wx.h>

// إعلان مسبق للفئات
class MainFrame;

// تعداد لحالات التنزيل
enum class DownloadStatus {
    PENDING,
    DOWNLOADING,
    PAUSED,
    COMPLETED,
    ERROR,
    CANCELED
};

struct DownloadItem {
    // ثوابت لتنسيق الحجم
    static const double KB;
    static const double MB;
    static const double GB;
    
    // بيانات التنزيل
    int id;
    wxString name;
    wxString url;
    wxString savePath;
    DownloadStatus status;
    int progress;
    double size;
    double downloadedSize;  // حجم ما تم تنزيله
    double speed;
    wxString dateAdded;
    
    // بيانات إضافية لتنزيلات يوتيوب
    bool isYouTube;         // هل هذا تنزيل من يوتيوب
    wxString youtubeFormat; // تنسيق تنزيل يوتيوب
    
    // مرجع إلى النافذة الرئيسية للإشعارات
    MainFrame* mainFrame;
    
    // البناء والهدم
    DownloadItem();
    ~DownloadItem();
    
    // طرق مساعدة
    wxString GetStatusText() const;
    wxString GetFormattedSize() const;
    wxString GetFormattedSpeed() const;
    int GetProgress() const;
};

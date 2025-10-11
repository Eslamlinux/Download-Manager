#pragma once

#include <wx/string.h>

/**
 * @brief فئة إعدادات التطبيق
 */
class AppSettings {
public:
    /**
     * @brief المنشئ
     */
    AppSettings();
    
    /**
     * @brief المدمر
     */
    ~AppSettings();
    
    /**
     * @brief تحميل الإعدادات من الملف
     * @param filePath مسار ملف الإعدادات
     * @return true إذا تم التحميل بنجاح
     */
    bool LoadFromFile(const wxString& filePath);
    
    /**
     * @brief حفظ الإعدادات إلى الملف
     * @param filePath مسار ملف الإعدادات
     * @return true إذا تم الحفظ بنجاح
     */
    bool SaveToFile(const wxString& filePath);
    
    // إعدادات التطبيق
    wxString defaultSavePath;
    int maxSimultaneousDownloads;
    bool startDownloadsAutomatically;
    bool showNotifications;
    bool minimizeToTray;
    bool closeToTray;
    bool checkForUpdatesOnStartup;
    wxString youtubeDownloaderPath;
};

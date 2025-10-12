#pragma once

#include <wx/wx.h>

namespace FileUtils {
    /**
     * @brief التأكد من وجود مجلد وإنشائه إذا لم يكن موجوداً
     * @param path مسار المجلد
     * @return true إذا كان المجلد موجوداً أو تم إنشاؤه بنجاح، false خلاف ذلك
     */
    bool EnsureDirectoryExists(const wxString& path);

    /**
     * @brief الحصول على اسم الملف من URL
     * @param url الرابط
     * @return اسم الملف
     */
    wxString GetFilenameFromUrl(const wxString& url);

    /**
     * @brief فتح ملف باستخدام التطبيق الافتراضي
     * @param filePath مسار الملف
     * @return true إذا تم فتح الملف بنجاح، false خلاف ذلك
     */
    bool OpenFileWithDefaultApp(const wxString& filePath);

    /**
     * @brief فتح مجلد في مستعرض الملفات
     * @param dirPath مسار المجلد
     * @return true إذا تم فتح المجلد بنجاح، false خلاف ذلك
     */
    bool OpenDirectoryInExplorer(const wxString& dirPath);
}

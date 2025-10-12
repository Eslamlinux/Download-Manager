#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/log.h>

// دالة للتحقق من وجود مجلد وإنشائه إذا لم يكن موجودًا
bool EnsureDirectoryExists(const wxString& path) {
    if (wxDirExists(path)) {
        return true;
    }
    
    return wxMkdir(path, wxS_DIR_DEFAULT);
}

// دالة للحصول على حجم الملف
wxULongLong GetFileSize(const wxString& path) {
    wxFileName fileName(path);
    if (fileName.FileExists()) {
        return fileName.GetSize();
    }
    
    return 0;
}

// دالة للتحقق من وجود الملف
bool FileExists(const wxString& path) {
    return wxFileExists(path);
}


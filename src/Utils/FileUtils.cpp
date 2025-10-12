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

// دالة لحذف الملف
bool DeleteFile(const wxString& path) {
    if (wxFileExists(path)) {
        return wxRemoveFile(path);
    }
    
    return false;
}

// دالة للحصول على امتداد الملف
wxString GetFileExtension(const wxString& path) {
    wxFileName fileName(path);
    return fileName.GetExt();
}

// دالة للحصول على اسم الملف بدون المسار
wxString GetFileName(const wxString& path) {
    wxFileName fileName(path);
    return fileName.GetFullName();
}

// دالة للحصول على اسم الملف بدون الامتداد
wxString GetFileNameWithoutExtension(const wxString& path) {
    wxFileName fileName(path);
    return fileName.GetName();
}

// دالة للحصول على المسار بدون اسم الملف
wxString GetFilePath(const wxString& path) {
    wxFileName fileName(path);
    return fileName.GetPath();
}

// دالة لإنشاء مسار كامل
wxString CombinePath(const wxString& path, const wxString& fileName) {
    return path + wxFileName::GetPathSeparator() + fileName;
}

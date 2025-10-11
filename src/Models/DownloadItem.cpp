#include "Models/DownloadItem.h"
#include <wx/datetime.h>

// تعريف الثوابت الساكنة
const double DownloadItem::KB = 1024;
const double DownloadItem::MB = 1024 * 1024;
const double DownloadItem::GB = 1024 * 1024 * 1024;

DownloadItem::DownloadItem()
    : id(-1), status(DownloadStatus::PENDING), progress(0), size(0), downloadedSize(0), speed(0),
      isYouTube(false), youtubeFormat(""), mainFrame(nullptr) {
    // تعيين تاريخ الإضافة
    dateAdded = wxDateTime::Now().Format("%Y-%m-%d %H:%M:%S");
}

DownloadItem::~DownloadItem() {
}

wxString DownloadItem::GetStatusText() const {
    switch (status) {
        case DownloadStatus::PENDING: return "في الانتظار";
        case DownloadStatus::DOWNLOADING: return "جاري التنزيل";
        case DownloadStatus::PAUSED: return "متوقف مؤقتًا";
        case DownloadStatus::COMPLETED: return "مكتمل";
        case DownloadStatus::ERROR: return "فشل";
        case DownloadStatus::CANCELED: return "ملغى";
        default: return "غير معروف";
    }
}

wxString DownloadItem::GetFormattedSize() const {
    if (size < KB) {
        return wxString::Format("%.0f B", size);
    } else if (size < MB) {
        return wxString::Format("%.2f KB", size / KB);
    } else if (size < GB) {
        return wxString::Format("%.2f MB", size / MB);
    } else {
        return wxString::Format("%.2f GB", size / GB);
    }
}

wxString DownloadItem::GetFormattedSpeed() const {
    if (speed < KB) {
        return wxString::Format("%.0f B/s", speed);
    } else if (speed < MB) {
        return wxString::Format("%.2f KB/s", speed / KB);
    } else if (speed < GB) {
        return wxString::Format("%.2f MB/s", speed / MB);
    } else {
        return wxString::Format("%.2f GB/s", speed / GB);
    }
}

int DownloadItem::GetProgress() const {
    if (size <= 0) {
        return 0;
    }
    
    return static_cast<int>((downloadedSize / size) * 100);
}

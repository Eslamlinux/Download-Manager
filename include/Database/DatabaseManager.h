#pragma once

#include "Models/DownloadItem.h"
#include <sqlite3.h>
#include <wx/wx.h>
#include <vector>

class DatabaseManager {
public:
    // البناء والهدم
    DatabaseManager(const wxString& dbPath);
    ~DatabaseManager();
    
    // عمليات التنزيل
    bool AddDownload(const DownloadItem& item);
    bool UpdateDownload(const DownloadItem& item);
    bool DeleteDownload(int id);
    std::vector<DownloadItem> GetAllDownloads();
    DownloadItem GetDownloadById(int id);
    
private:
    // طرق مساعدة
    bool OpenDatabase();
    bool CreateTables();
    
    // متغيرات عضو
    wxString m_dbPath;
    sqlite3* m_db;
};

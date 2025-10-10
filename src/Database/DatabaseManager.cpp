#include "Database/DatabaseManager.h"
#include <wx/log.h>
#include <wx/filename.h>

DatabaseManager::DatabaseManager(const wxString& dbPath)
    : m_dbPath(dbPath), m_db(nullptr) {
    // فتح قاعدة البيانات
    if (!OpenDatabase()) {
        wxLogError("Failed to open database: %s", dbPath);
        return;
    }
    
    // إنشاء الجداول إذا لم تكن موجودة
    if (!CreateTables()) {
        wxLogError("Failed to create tables");
        return;
    }
    
    wxLogMessage("Database initialized: %s", dbPath);
}

DatabaseManager::~DatabaseManager() {
    // إغلاق قاعدة البيانات
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
    
    wxLogMessage("Database closed");
}

bool DatabaseManager::OpenDatabase() {
    // التأكد من وجود المجلد
    wxFileName dbFile(m_dbPath);
    if (!dbFile.DirExists()) {
        if (!wxFileName::Mkdir(dbFile.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
            wxLogError("Failed to create directory: %s", dbFile.GetPath());
            return false;
        }
    }
    
    // فتح قاعدة البيانات
    int result = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (result != SQLITE_OK) {
        wxLogError("Failed to open database: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    return true;
}

bool DatabaseManager::CreateTables() {
    // إنشاء جدول التنزيلات
    const char* sql = "CREATE TABLE IF NOT EXISTS downloads ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "url TEXT NOT NULL,"
                      "save_path TEXT NOT NULL,"
                      "status INTEGER NOT NULL,"
                      "size INTEGER NOT NULL,"
                      "downloaded INTEGER NOT NULL,"
                      "date_added TEXT NOT NULL,"
                      "is_youtube INTEGER NOT NULL,"
                      "youtube_format TEXT"
                      ");";
    
    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        wxLogError("Failed to create tables: %s", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool DatabaseManager::AddDownload(const DownloadItem& item) {
    // إعداد الاستعلام
    const char* sql = "INSERT INTO downloads (name, url, save_path, status, size, downloaded, date_added, is_youtube, youtube_format) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        wxLogError("Failed to prepare statement: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    // ربط القيم
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.savePath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, static_cast<int>(item.status));
    sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(item.size));
    sqlite3_bind_int64(stmt, 6, static_cast<sqlite3_int64>(item.downloadedSize));
    sqlite3_bind_text(stmt, 7, item.dateAdded.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, item.isYouTube ? 1 : 0);
    sqlite3_bind_text(stmt, 9, item.youtubeFormat.c_str(), -1, SQLITE_STATIC);
    
    // تنفيذ الاستعلام
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        wxLogError("Failed to add download: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    wxLogMessage("Download added to database, id: %lld", sqlite3_last_insert_rowid(m_db));
    return true;
}

bool DatabaseManager::UpdateDownload(const DownloadItem& item) {
    // إعداد الاستعلام
    const char* sql = "UPDATE downloads SET name = ?, url = ?, save_path = ?, status = ?, size = ?, downloaded = ?, is_youtube = ?, youtube_format = ? "
                      "WHERE id = ?;";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        wxLogError("Failed to prepare statement: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    // ربط القيم
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.savePath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, static_cast<int>(item.status));
    sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(item.size));
    sqlite3_bind_int64(stmt, 6, static_cast<sqlite3_int64>(item.downloadedSize));
    sqlite3_bind_int(stmt, 7, item.isYouTube ? 1 : 0);
    sqlite3_bind_text(stmt, 8, item.youtubeFormat.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 9, item.id);
    
    // تنفيذ الاستعلام
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        wxLogError("Failed to update download: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    wxLogMessage("Download updated in database, id: %d", item.id);
    return true;
}

bool DatabaseManager::DeleteDownload(int id) {
    // إعداد الاستعلام
    const char* sql = "DELETE FROM downloads WHERE id = ?;";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        wxLogError("Failed to prepare statement: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    // ربط القيم
    sqlite3_bind_int(stmt, 1, id);
    
    // تنفيذ الاستعلام
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        wxLogError("Failed to delete download: %s", sqlite3_errmsg(m_db));
        return false;
    }
    
    wxLogMessage("Download deleted from database, id: %d", id);
    return true;
}

std::vector<DownloadItem> DatabaseManager::GetAllDownloads() {
    std::vector<DownloadItem> downloads;
    
    // إعداد الاستعلام
    const char* sql = "SELECT id, name, url, save_path, status, size, downloaded, date_added, is_youtube, youtube_format FROM downloads;";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        wxLogError("Failed to prepare statement: %s", sqlite3_errmsg(m_db));
        return downloads;
    }
    
    // تنفيذ الاستعلام وقراءة النتائج
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DownloadItem item;
        
        item.id = sqlite3_column_int(stmt, 0);
        
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        if (name) {
            item.name = wxString::FromUTF8(name);
        }
        
        const char* url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        if (url) {
            item.url = wxString::FromUTF8(url);
        }
        
        const char* savePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        if (savePath) {
            item.savePath = wxString::FromUTF8(savePath);
        }
        
        item.status = static_cast<DownloadStatus>(sqlite3_column_int(stmt, 4));
        item.size = sqlite3_column_int64(stmt, 5);
        item.downloadedSize = sqlite3_column_int64(stmt, 6);
        
        const char* dateAdded = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        if (dateAdded) {
            item.dateAdded = wxString::FromUTF8(dateAdded);
        }
        
        item.isYouTube = sqlite3_column_int(stmt, 8) != 0;
        
        const char* youtubeFormat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        if (youtubeFormat) {
            item.youtubeFormat = wxString::FromUTF8(youtubeFormat);
        }
        
        downloads.push_back(item);
    }
    
    sqlite3_finalize(stmt);
    
    wxLogMessage("Retrieved %zu downloads from database", downloads.size());
    return downloads;
}

DownloadItem DatabaseManager::GetDownloadById(int id) {
    DownloadItem item;
    
    // إعداد الاستعلام
    const char* sql = "SELECT id, name, url, save_path, status, size, downloaded, date_added, is_youtube, youtube_format FROM downloads WHERE id = ?;";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        wxLogError("Failed to prepare statement: %s", sqlite3_errmsg(m_db));
        return item;
    }
    
    // ربط القيم
    sqlite3_bind_int(stmt, 1, id);
    
    // تنفيذ الاستعلام وقراءة النتائج
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        item.id = sqlite3_column_int(stmt, 0);
        
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        if (name) {
            item.name = wxString::FromUTF8(name);
        }
        
        const char* url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        if (url) {
            item.url = wxString::FromUTF8(url);
        }
        
        const char* savePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        if (savePath) {
            item.savePath = wxString::FromUTF8(savePath);
        }
        
        item.status = static_cast<DownloadStatus>(sqlite3_column_int(stmt, 4));
        item.size = sqlite3_column_int64(stmt, 5);
        item.downloadedSize = sqlite3_column_int64(stmt, 6);
        
        const char* dateAdded = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        if (dateAdded) {
            item.dateAdded = wxString::FromUTF8(dateAdded);
        }
        
        item.isYouTube = sqlite3_column_int(stmt, 8) != 0;
        
        const char* youtubeFormat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        if (youtubeFormat) {
            item.youtubeFormat = wxString::FromUTF8(youtubeFormat);
        }
    }
    
    sqlite3_finalize(stmt);
    
    wxLogMessage("Retrieved download from database, id: %d", id);
    return item;
}

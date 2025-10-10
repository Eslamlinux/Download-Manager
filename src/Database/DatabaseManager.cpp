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


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

#pragma once

// تضمين جميع رؤوس wxWidgets الشائعة الاستخدام
#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/artprov.h>
#include <wx/log.h>

// تضمين رؤوس المكتبات الخارجية
#include <curl/curl.h>
#include <sqlite3.h>

// تضمين رؤوس C++ القياسية
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <functional>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// تضمين رؤوس المشروع المشتركة
#include "Common/EventIDs.h"

#include "UI/MainFrame.h"
#include "UI/DownloadDialog.h"
#include "UI/YouTubeDialog.h"
#include "UI/SettingsDialog.h"
#include "UI/SpeedLimitDialog.h"
#include "Common/EventIDs.h"
#include <wx/msgdlg.h>
#include <wx/aboutdlg.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/utils.h>
#include <wx/process.h>
#include <wx/filename.h>
#include <wx/textdlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/log.h>
#include <wx/artprov.h>
#include <wx/textfile.h>
#include <mutex>

// Global mutex for UI updates
std::mutex g_uiMutex;

// Constructor
MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // Load settings
    m_settings.Load();

    // Create download manager
    m_downloadManager = new DownloadManager(this, m_settings);

    // Create status bar first, before any SetStatusText() calls
    CreateStatusBar(2);

    // Create UI
    CreateUI();

    // Set icon
    // SetIcon(wxICON(appicon)); // Commented out until we have an icon

    // Set initial status text
    SetStatusText("Ready");

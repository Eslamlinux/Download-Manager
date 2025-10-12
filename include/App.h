#pragma once

#include <wx/wx.h>

class MainFrame;

class App : public wxApp {
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;

private:
    MainFrame* m_mainFrame;
};

/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.h
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_MAINFRAME_H_
#define _WX_SAMPLES_MAINFRAME_H_

class MainFrame : public wxDocParentFrame
{
public:
    MainFrame(wxDocManager *manager);
    ~MainFrame();
private:
    void InitToolBar(wxToolBar* toolBar);

    void OnConfigureODBC(wxCommandEvent &event);
    void OnDatabaseProfile(wxCommandEvent &event);
    Database *m_db;
    wxDynamicLibrary *m_lib;
//    void *m_db;
    wxDECLARE_EVENT_TABLE();
};

#endif
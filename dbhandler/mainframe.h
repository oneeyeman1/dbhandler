/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.h
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_MAINFRAME_H_
#define _WX_SAMPLES_MAINFRAME_H_

class MainFrame : public wxDocMDIParentFrame
{
public:
    MainFrame(wxDocManager *manager);
    ~MainFrame();
protected:
    void Connect();
private:
    void InitToolBar(wxToolBar* toolBar);
    void InitMenuBar(int id);
    void DatabaseMenu();
    void OnConfigureODBC(wxCommandEvent &event);
    void OnDatabaseProfile(wxCommandEvent &event);
    void OnDatabase(wxCommandEvent &event);
    void OnNewIndex(wxCommandEvent &event);
    Database *m_db;
    wxDynamicLibrary *m_lib;
    wxDynamicLibrary *m_lib1;
    wxMenu *m_menuFile;
    wxDocManager *m_manager;
#if defined __WXMSW__ || defined __WXGTK__
    wxToolBar *m_tb;
#endif
    wxDECLARE_EVENT_TABLE();
};

#endif
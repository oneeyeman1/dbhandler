/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.h
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_MAINFRAME_H_
#define _WX_SAMPLES_MAINFRAME_H_

class NewTableHandler;

class MainFrame : public wxDocMDIParentFrame
{
public:
    MainFrame(wxDocManager *manager);
    ~MainFrame();
    wxCriticalSection m_threadCS;
    NewTableHandler *m_handler;
protected:
    void Connect();
private:
    void InitToolBar(wxToolBar* toolBar);
    void InitMenuBar(int id);
    void DatabaseMenu();
    void QueryMenu();
    void TableMenu();
    void OnConfigureODBC(wxCommandEvent &event);
    void OnDatabaseProfile(wxCommandEvent &event);
    void OnTable(wxCommandEvent &event);
    void OnDatabase(wxCommandEvent &event);
    void OnQuery(wxCommandEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnAttachDatabase(wxCommandEvent &event);
    void OnDetachDatabase(wxCommandEvent &event);
    void OnUpdateUIDetachDB(wxUpdateUIEvent &event);
    Database *m_db;
    wxMenu *m_menuFile;
    wxDocManager *m_manager;
#if defined __WXMSW__ || defined __WXGTK__
    wxToolBar *m_tb;
#endif
    std::map<wxString, wxDynamicLibrary *> m_painters;
    int m_countAttached;
    wxString m_pgLogfile;
    wxFileSystemWatcher *m_oldPGWatcher;
    wxDECLARE_EVENT_TABLE();
};

#endif

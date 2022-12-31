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

struct Profile
{
    wxString m_name;
    bool m_isCurrent;

    Profile(wxString name, bool isCurrent) : m_name( name ), m_isCurrent( isCurrent ) {}
};

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
    void OnLibrary(wxCommandEvent &event);
    Database *m_db;
    wxDocManager *m_manager;
#if defined __WXMSW__ || defined __WXGTK__
    wxToolBar *m_tb;
#endif
    std::map<wxString, wxDynamicLibrary *> m_painters;
    std::vector<Profile> m_profiles;
    int m_countAttached;
    wxString m_pgLogfile;
#if !( defined( __sun ) && defined( __SVR4 ) )
    wxFileSystemWatcher *m_oldPGWatcher;
#endif
    wxDECLARE_EVENT_TABLE();
};

#endif

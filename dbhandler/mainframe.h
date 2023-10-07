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

struct QueryInfo
{
    wxString name, comment;
    void operator=(const QueryInfo &info)
    {
        name = info.name;
        comment = info.comment;
    }
};

struct LibrariesInfo
{
    wxString m_path;
    bool m_isActive;
    LibrariesInfo(const wxString &path, bool active) : m_path(path), m_isActive(active) {}
};

struct ToolbarSetup
{
    bool m_hideShow, m_showTooltips, m_showText;
    int m_orientation;
};

class MainFrame : public wxDocMDIParentFrame
{
public:
    MainFrame(wxDocManager *manager);
    ~MainFrame();
    ToolbarSetup GetViewToolbarSettings() { return m_tbSettings["ViewBar"]; }
    wxCriticalSection m_threadCS;
    NewTableHandler *m_handler;
protected:
    void LoadApplication(const std::vector<LibrariesInfo> &path);
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
//    void OnCustomize(wxMouseEvent &event);
    Database *m_db;
    wxDocManager *m_manager;
#if defined __WXMSW__ || defined __WXGTK__
    wxToolBar *m_tb;
#endif
    std::map<wxString, wxDynamicLibrary *> m_painters;
    std::vector<Profile> m_profiles;
    std::vector<LibrariesInfo> m_path;
    int m_countAttached;
    wxString m_pgLogfile;
#if !( defined( __sun ) && defined( __SVR4 ) )
    wxFileSystemWatcher *m_oldPGWatcher;
#endif
    std::vector<QueryInfo> queries;
    wxXmlDocument doc;
    std::map<wxString, ToolbarSetup> m_tbSettings;
    wxDECLARE_EVENT_TABLE();
};

#endif

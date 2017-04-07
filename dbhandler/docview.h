/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.h
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_DOCVIEW_DOCVIEW_H_
#define _WX_SAMPLES_DOCVIEW_DOCVIEW_H_

#include "wx/docview.h"
#include "wx/vector.h"

class MyCanvas;

// Define a new application
class MyApp : public wxApp
{
public:
    MyApp();

    // override some wxApp virtual methods
    virtual bool OnInit() wxOVERRIDE;
    virtual int OnExit() wxOVERRIDE;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) wxOVERRIDE;
    wxString GetDBName();
    wxString GetDBEngine();
    void SetDBEngine(const wxString &engine);
    void SetDBName(const wxString &name);
#ifdef __WXMAC__
    virtual void MacNewFile() wxOVERRIDE;
#endif // __WXMAC__

    // our specific methods
    wxFrame *CreateChildFrame(wxView *view, bool isCanvas);
#if wxUSE_TOOLBAR
    void CreateMainToolbar();
#endif
    // these accessors should only be called in single document mode, otherwise
    // the pointers are NULL and an assert is triggered
    MyCanvas *GetMainWindowCanvas() const
        { wxASSERT(m_canvas); return m_canvas; }
    wxMenu *GetMainWindowEditMenu() const
        { wxASSERT(m_menuEdit); return m_menuEdit; }

private:
    // append the standard document-oriented menu commands to this menu
    void AppendDocumentFileCommands(wxMenu *menu, bool supportsPrinting);

    // create the edit menu for drawing documents
    wxMenu *CreateDrawingEditMenu();

    // create and associate with the given frame the menu bar containing the
    // given file and edit (possibly NULL) menus as well as the standard help
    // one
//    void CreateMenuBarForFrame(wxFrame *frame, wxMenu *file, wxMenu *edit);


    // show the about box: as we can have different frames it's more
    // convenient, even if somewhat less usual, to handle this in the
    // application object itself
    void OnAbout(wxCommandEvent& event);

    // contains the file names given on the command line, possibly empty
    wxVector<wxString> m_filesFromCmdLine;

    // only used if m_mode == Mode_Single
    MyCanvas *m_canvas;
    wxMenu *m_menuEdit;
    wxString m_dbName, m_dbEngine;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MyApp);
};

enum ViewType
{
    DatabaseView,
    QueryView
}

wxDECLARE_APP(MyApp);

#define wxID_CONFIGUREODBC          1
#define wxID_DATABASEWINDOW         2
#define wxID_TABLE                  3
#define wxID_DATABASE               4
#define wxID_QUERY                  5
#define wxID_TABLEDROPTABLE        24
#define wxID_PROPERTIES            50
#define wxID_CREATEDATABASE       100
#define wxID_DELETEDATABASE       101
#define wxID_SELECTTABLE          102
#define wxID_OBJECTNEWTABLE       103
#define wxID_OBJECTNEWINDEX       104
#define wxID_OBJECTNEWVIEW        105
#define wxID_OBJECTNEWFF          106
#define wxID_FIELDDEFINITION      107
#define wxID_FIELDPROPERTIES      108
#define wxID_OBJECTNEWPK          109
#define wxID_STARTLOG             110
#define wxID_STOPLOG              111
#define wxID_SAVELOG              112
#define wxID_CLEARLOG             113

#endif // _WX_SAMPLES_DOCVIEW_DOCVIEW_H_

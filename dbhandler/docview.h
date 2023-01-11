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
    wxString GetConnectString();
    wxString GetConnectedUser();
    void SetDBEngine(const wxString &engine);
    void SetDBName(const wxString &name);
    void SetConnectString(const wxString &connString);
    void SetConnectedUser(const wxString &user);
    wxDocManager *GetDocManager();
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
    wxDocManager *m_docManager;
    MyCanvas *m_canvas;
    wxMenu *m_menuEdit;
    wxString m_dbName, m_dbEngine, m_subtype, m_connectString, m_connectedUser;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MyApp);
};

enum ViewType
{
    DatabaseView,
    QueryView
};

wxDECLARE_APP(MyApp);

#define wxID_CONFIGUREODBC          1
#define wxID_DATABASEWINDOWPROFILE  2
#define wxID_TABLE                  3
#define wxID_DATABASE               4
#define wxID_QUERY                  5
#define wxID_LIBRARY                6
#define wxID_PROPERTIES            50
#define wxID_CREATEDATABASE       100
#define wxID_DELETEDATABASE       101
#define wxID_SELECTTABLE          102
#define wxID_OBJECTNEWTABLE       103
#define wxID_OBJECTNEWINDEX       104
#define wxID_OBJECTNEWVIEW        105
#define wxID_OBJECTNEWFF          106
#define wxID_FIELDDEFINITION      107
#define wxID_EXPORTSYNTAX         108
#define wxID_OBJECTNEWPK          109
#define wxID_STARTLOG             110
#define wxID_STOPLOG              111
#define wxID_SAVELOG              112
#define wxID_CLEARLOG             113
#define wxID_SELECTALLFIELDS      114
#define wxID_DESELECTALLFIELDS    115
#define wxID_ARRANGETABLES        116
#define wxID_SHOWDATATYPES        117
#define wxID_SHOWLABELS           118
#define wxID_SHOWCOMMENTS         119
#define wxID_SHOWSQLTOOLBOX       120
#define wxID_FKDEFINITION         121
#define wxID_FKOPENREFTABLE       122
#define wxID_DROPOBJECT           123
#define wxID_ATTACHDATABASE       124
#define wxID_DETACHDATABASE       125
#define WHEREPAGECOLUMNS          194
#define WHEREPAGEFUNCTIONS        195
#define WHEREPAGEARGUMENTS        196
#define WHEREPAGEVALUE            197
#define WHEREPAGESELECT           198
#define WHEREPAGECLEAR            199
#define wxID_EDITCUTCOLUMN        200
#define wxID_EDITCOPYCOLUMN       201
#define wxID_EDITPASTECOLUMN      202
#define wxID_EDITINSERTCOLUMN     203
#define wxID_EDITDELETECOLUMN     204
#define wxID_EDITTABLEPROPERTY    205
#define wxID_DESIGNSYNTAX         206
#define wxID_DATASOURCE           301
#define wxID_PREVIEW              302
#define wxID_UNIONS               303
#define wxID_RETRIEVEARGS         304
#define wxID_CHECKOPTION          305
#define wxID_DISTINCT             306
#define wxID_CUSTOMCOLORS         308
#define wxID_OPTIONS              309
#define wxID_SELECTALL            310
#define wxID_SELECTABOVE          311
#define wxID_SELECTBELOW          312
#define wxID_SELECTLEFT           313
#define wxID_SELECTRIGHT          314
#define wxID_SELECTCOLUMNS        315
#define wxID_SELECTTEXT           316
#define wxID_BRINGTOFRONT         317
#define wxID_SENDTOBACK           318
#define wxID_FORMATCURRENCY       319
#define wxID_FORMATFORMAT         320
#define wxID_SELECTOBJECT         321
#define wxID_TEXTOBJECT           322
#define wxID_PICTUREOBJECT        323
#define wxID_LINEOBJECT           324
#define BOLDFONT                  325
#define ITALICFONT                326
#define UNDERLINEFONGT            327
#define wxID_DESIGNTABORDER       328
#define wxID_COLUMNSPEC           329
#define wxID_PREVIEWQUERY         330
#define wxID_CONVERTTOSYNTAX      331
#define wxID_SHOWJOINS            332
#define myID_FINDNEXT             333
#define wxID_GOTOLINE             334
#define wxID_CONVERTTOGRAPHICS    335
#define wxID_UNDOALL              336
#define wxID_QUERYCANCEL          400
#define wxID_TOP                  402
#define wxID_BACK                 403
#define wxID_SAVEQUERY            404
#define wx_SAVEQUERYAS            405
#endif // _WX_SAMPLES_DOCVIEW_DOCVIEW_H_

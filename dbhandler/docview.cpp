/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/stockitem.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

/*#if defined __WXMSW__ && defined __MEMORYLEAKS__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
*/
#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include <vector>
#include <map>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/xml/xml.h"
#include "wx/fswatcher.h"
#include "database.h"
#include "configuration.h"
#include "newtablehandler.h"
#include "mainframe.h"
#include "docview.h"
#include "doc.h"
#include "view.h"

#include "wx/cmdline.h"
#include "wx/config.h"

#ifdef __WXMAC__
    #include "wx/filename.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "doc.xpm"
    #include "chart.xpm"
    #include "notepad.xpm"
#endif

// ----------------------------------------------------------------------------
// MyApp implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_MENU(wxID_ABOUT, MyApp::OnAbout)
wxEND_EVENT_TABLE()

MyApp::MyApp()
{
    m_canvas = NULL;
    m_menuEdit = NULL;
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    // save any files given on the command line: we'll open them in OnInit()
    // later, after creating the frame
    for( size_t i = 0; i != parser.GetParamCount(); ++i )
        m_filesFromCmdLine.push_back( parser.GetParam( i ) );

    return wxApp::OnCmdLineParsed( parser );
}

#ifdef __WXMAC__
void MyApp::MacNewFile()
{
}
#endif // __WXMAC__

bool MyApp::OnInit()
{
#if defined __WXMSW__ && defined __MEMORYLEAKS__
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
    if( !wxApp::OnInit() )
        return false;

    ::wxInitAllImageHandlers();

    // Fill in the application information fields before creating wxConfig.
    SetVendorName( "wxWidgets" );
    SetAppName( "dbhandler" );
    SetAppDisplayName( "DB Handler" );
    auto config = wxConfigBase::Get( "DBManager" );
    auto path = config->GetPath();
    config->SetPath( "CurrentDB" );
    m_dbName = config->Read( "DatabaseName", "" );
    m_dbEngine = config->Read( "Engine", "" );
    m_subtype = config->Read( "Subtype", "" );
    config->SetPath( path );

    //// Create a document manager
    m_docManager = new wxDocManager;

    // Create a template relating text documents to their views
    new wxDocTemplate( m_docManager, "Text", "*.txt;*.text", "", "txt;text", "Text Doc", "Text View", CLASSINFO( TextEditDocument ), CLASSINFO( TextEditView ) );
    // Create a template relating image documents to their views
    new wxDocTemplate( m_docManager, "Image", "*.png;*.jpg", "", "png;jpg", "Image Doc", "Image View", CLASSINFO( ImageDocument ), CLASSINFO( ImageView ) );
    // create the main frame window
    MainFrame *frame;
    frame = new MainFrame( m_docManager );

//    CreateMenuBarForFrame( frame, menuFile, m_menuEdit );

    frame->SetIcon( wxICON( doc ) );
    frame->Maximize();
    frame->Show();
    return true;
}

int MyApp::OnExit()
{
    wxDocManager * const manager = wxDocManager::GetDocumentManager();
#if wxUSE_CONFIG
    manager->FileHistorySave( *wxConfig::Get() );
#endif // wxUSE_CONFIG
    delete manager;
    return wxApp::OnExit();
}

wxString MyApp::GetDBName()
{
    return m_dbName;
}

wxString MyApp::GetDBEngine()
{
    return m_dbEngine;
}

wxString MyApp::GetConnectString()
{
    return m_connectString;
}

wxString MyApp::GetConnectedUser()
{
    return m_connectedUser;
}

void MyApp::SetDBEngine(const wxString &engine)
{
    m_dbEngine = engine;
}

void MyApp::SetDBName(const wxString &name)
{
    m_dbName = name;
}

void MyApp::SetConnectString(const wxString &connString)
{
    m_connectString = connString;
}

void MyApp::SetConnectedUser(const wxString &user)
{
    m_connectedUser = user;
}

void MyApp::AppendDocumentFileCommands(wxMenu *menu, bool supportsPrinting)
{
    menu->Append( wxID_CLOSE );
    menu->Append( wxID_SAVE );
    menu->Append( wxID_SAVEAS );
    menu->Append( wxID_REVERT, _( "Re&vert..." ) );

    if ( supportsPrinting )
    {
        menu->AppendSeparator();
        menu->Append( wxID_PRINT );
        menu->Append( wxID_PRINT_SETUP, "Print &Setup..." );
        menu->Append( wxID_PREVIEW );
    }
}

wxMenu *MyApp::CreateDrawingEditMenu()
{
    wxMenu * const menu = new wxMenu;
    menu->Append( wxID_UNDO );
    menu->Append( wxID_REDO );
    menu->AppendSeparator();
    menu->Append( wxID_CUT, "&Cut last segment" );

    return menu;
}

wxFrame *MyApp::CreateChildFrame(wxView *view, bool isCanvas)
{
    // create a child frame of appropriate class for the current mode
    wxFrame *subframe;
    wxDocument *doc = view->GetDocument();
    wxDocParentFrame *frame = wxStaticCast( GetTopWindow(), wxDocParentFrame );
	wxSize childSize = frame->GetClientSize();
    subframe = new wxDocChildFrame( doc, view, frame, wxID_ANY, "Child Frame", wxDefaultPosition, childSize );
    subframe->Centre();
    wxMenu *menuFile = new wxMenu;
    menuFile->Append( wxID_NEW );
    menuFile->Append( wxID_OPEN );
    AppendDocumentFileCommands( menuFile, isCanvas );
    menuFile->AppendSeparator();
    menuFile->Append( wxID_EXIT );
    wxMenu *menuEdit;
    if( isCanvas )
    {
        menuEdit = CreateDrawingEditMenu();
        doc->GetCommandProcessor()->SetEditMenu(menuEdit);
        doc->GetCommandProcessor()->Initialize();
    }
    else // text frame
    {
        menuEdit = new wxMenu;
        menuEdit->Append (wxID_COPY );
        menuEdit->Append( wxID_PASTE );
        menuEdit->Append( wxID_SELECTALL );
    }
//    CreateMenuBarForFrame( subframe, menuFile, menuEdit );
    subframe->SetIcon( isCanvas ? wxICON( chrt ) : wxICON( notepad ) );
    return subframe;
}

void MyApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    const int docsCount = wxDocManager::GetDocumentManager()->GetDocumentsVector().size();

    wxLogMessage( "This is the DB Handler Application\n%d open documents.\n\n", docsCount );
}

#if wxUSE_TOOLBAR
void MyApp::CreateMainToolbar()
{
}
#endif

wxDocManager *MyApp::GetDocManager()
{
    return m_docManager;
}

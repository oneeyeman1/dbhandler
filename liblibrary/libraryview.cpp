// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifdef __WXGTK__
#include "libraryopen.h"
#include "libraryclosed.h"
#include "selectall.h"
#endif

#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include "wx/xml/xml.h"
#ifndef __WXGTK__
#include "wx/volume.h"
#endif
#include "wx/dynlib.h"
#include "wx/artprov.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/bmpcbox.h"
#include "wx/generic/dirctrlg.h"
#if wxCHECK_VERSION( 3, 3, 0 )
#else
#include "wx/imaglist.h"
#endif
#include "wx/treelist.h"
#include "wx/dir.h"
#include "configuration.h"
#include "painterobjects.h"
#include "propertieshandlerbase.h"
#include "librarypropertieshandler.h"
#include "librarydocument.h"
#include "libraryview.h"

typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, std::unique_ptr<PropertiesHandler> &, const wxString &, wxString &, bool, wxCriticalSection &);

wxIMPLEMENT_DYNAMIC_CLASS(LibraryViewPainter, wxView);

wxBEGIN_EVENT_TABLE(LibraryViewPainter, wxView)
    EVT_MENU(wxID_LIBRARYNEW, LibraryViewPainter::OnLibraryCreate)
    EVT_MENU(wxID_LIBRARYDELETE, LibraryViewPainter::OnLibraryDelete)
    EVT_UPDATE_UI(wxID_LIBRARYSELECTALL, LibraryViewPainter::OnSelectAllUpdateUI)
    EVT_UPDATE_UI(wxID_LIBRARYDELETE, LibraryViewPainter::OnDeleteLibraryUpdateUI)
    EVT_UPDATE_UI(wxID_EXPANDCOLLAPS, LibraryViewPainter::OnExpandCollapsUpdateUI)
    EVT_MENU(wxID_LIBRARYSELECTALL, LibraryViewPainter::OnSelectAll)
    EVT_MENU(wxID_EXPANDCOLLAPS, LibraryViewPainter::OnExpandCollapse)
wxEND_EVENT_TABLE()

bool LibraryViewPainter::OnCreate(wxDocument *doc, long flags)
{
    m_tb = nullptr;
    wxWindowList children;
    wxRect clientRect = m_parent->GetClientRect();
    if( !wxView::OnCreate( doc, flags ) )
        return false;
#ifndef __WXMSW__
    children = m_parent->GetChildren();
#else
    children = m_parent->GetClientWindow()->GetChildren();
#endif
    auto found = false;
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        auto tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "ViewBar" )
        {
            found = true;
            m_tb = tb;
        }
    }
    auto stdPath = wxStandardPaths::Get();
#ifdef __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    m_libPath = fn.GetPathWithSep() + "Frameworks/";
#elif __WXGTK__ || __WXQT__
    m_libPath = stdPath.GetInstallPrefix() + "/lib/";
#elif __WXMSW__
    wxFileName fn( stdPath.GetExecutablePath() );
    m_libPath = fn.GetPathWithSep();
#endif
    wxPoint pos;
#ifdef __WXOSX__
    pos.y = ( m_parent->GetClientWindow()->GetClientRect().GetHeight() - m_parent->GetClientRect().GetHeight() ) - m_parent->GetToolBar()->GetRect().GetHeight();
#else
    pos = wxDefaultPosition;
#endif
    wxArrayString vs;
#ifndef __WXGTK__
    auto volumes = wxFSVolume::GetVolumes();
#else
    vs.Add( "/" );
#endif
    auto path = GetDocument()->GetFilename();
    wxFileName fileName( path );
//    auto volume = fileName.GetVolume();
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, _( "Library" ), pos, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    auto sizer = new wxBoxSizer( wxVERTICAL );
    m_drive = new wxBitmapComboBox( m_frame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, vs, wxCB_READONLY  );
#ifndef __WXGTK__
    for( auto volume : volumes )
    {
        wxFSVolume vol( volume );
        m_drive->Append( volume, vol.GetIcon( wxFS_VOL_ICO_SMALL ) );
    }
#endif
    wxString str = "";
#ifdef __WXMSW__
    str = fileName.GetVolume() + ":\\";
#else
    str = "/";
#endif
    m_drive->SetStringSelection( str );
    sizer->Add( m_drive, 0 , wxEXPAND, 0 );
    m_tree = new wxTreeListCtrl( m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_NO_HEADER | wxTL_MULTIPLE );
    m_tree->AppendColumn( "Name" );
    m_tree->AppendColumn( "Last Modified" );
    m_tree->AppendColumn( "Date Compiled" );
    m_tree->AppendColumn( "Size (in Bytes)" );
    m_tree->AppendColumn( "Comments" );
#ifdef __WXMSW__
    HANDLE gs_wxMainThread = NULL;
    const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "library", &gs_wxMainThread );
    const void *dataLibOpen = nullptr, *dataLibClosed = nullptr;
    size_t libOpen, libClosed;
    if( !wxLoadUserResource( &dataLibOpen, &libOpen, "libOpen", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        libraryOpen = wxBitmapBundle::FromSVG( (const char *) dataLibOpen, wxSize( 16, 16 ) );
    }
    if( !wxLoadUserResource( &dataLibClosed, &libClosed, "libClosed", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        libraryClosed = wxBitmapBundle::FromSVG( (const char *) dataLibClosed, wxSize( 16, 16 ) );
    }
    if( libraryClosed.IsOk() )
        wxMessageBox( "Bundle is good" );
    if( libraryOpen.IsOk() )
        wxMessageBox( "Bundle is good" );
#elif __WXOSX__
    libraryOpen = wxBitmapBundle::FromSVGResource( "libOpen", wxSize( 16, 16 ) );
    libraryClosed = libraryClosed = wxBitmapBundle::FromSVGResource( "libClosed", wxSize( 16, 16 ) );
#else
    libraryOpen = wxBitmapBundle::FromSVG( libOpen, wxSize( 16, 16 ) );
    libraryClosed = wxBitmapBundle::FromSVG( libClosed, wxSize( 16, 16 ) );
#endif
#if wxCHECK_VERSION( 3, 3, 0 )
    wxVector<wxBitmapBundle> images;
    images.push_back( wxArtProvider::GetBitmapBundle( wxART_FOLDER ) );
    images.push_back( wxArtProvider::GetBitmapBundle( wxART_FOLDER_OPEN ) );
    images.push_back( libraryClosed );
    images.push_back( libraryOpen );
    m_tree->SetStateImages( images );
#else
    wxImageList *images = new wxImageList( 16, 16 );
    auto size = wxArtProvider::GetSizeHint( wxART_OTHER, m_frame );
    images->Add( wxArtProvider::GetBitmap( wxART_FOLDER, wxART_OTHER, wxSize( 16, 16 ) ) );
    images->Add( wxArtProvider::GetBitmap( wxART_FOLDER_OPEN, wxART_OTHER, wxSize( 16, 16 ) ) );
    images->Add( libraryClosed.GetBitmap( wxDefaultSize ), wxNullBitmap );
    images->Add( libraryOpen.GetBitmap( wxSize( 16, 16 ) ), wxNullBitmap );
    m_tree->SetImageList( images );
#endif
    wxString rootName = "";
    wxDirItemData *rootData = new wxDirItemData( str, str, true );
#if defined(__WINDOWS__)
    rootName = _("Computer");
#else
    rootName = _("Sections");
#endif
    m_rootId = m_tree->AppendItem( m_tree->GetRootItem(), str, 1, 2, rootData );
    ExpandRoot( path );
    sizer->Add( m_tree, 1, wxEXPAND, 0 );
    m_frame->SetSizer( sizer );
    CreateViewToolBar();
    m_frame->Layout();
    m_frame->Show();
    m_tree->SetFocus();
    m_tree->Bind( wxEVT_TREELIST_ITEM_CONTEXT_MENU, &LibraryViewPainter::OnItemContextMenu, this );
    m_tree->Bind( wxEVT_TREELIST_SELECTION_CHANGED, &LibraryViewPainter::OnSelectionChanged, this );
    return true;
}

void LibraryViewPainter::OnDraw( wxDC * )
{
}

void LibraryViewPainter::CreateViewToolBar()
{
    long styleViewBar = wxNO_BORDER | wxTB_FLAT;
    switch( m_conf->m_tbSettings["ViewBar"].m_orientation )
    {
    case 0:
        styleViewBar |= wxTB_VERTICAL;
        break;
    case 1:
        styleViewBar |= wxTB_HORIZONTAL;
        break;
    case 2:
        styleViewBar |= wxTB_RIGHT;
        break;
    case 3:
        styleViewBar |= wxTB_BOTTOM;
        break;
    }
    if( !m_conf->m_tbSettings["ViewBar"].m_showTooltips )
        styleViewBar |= wxTB_NO_TOOLTIPS;
    if( m_conf->m_tbSettings["ViewBar"].m_showText )
        styleViewBar |= wxTB_TEXT ;
    wxWindow *parent = nullptr;
#ifdef __WXOSX__
    parent = m_frame;
#else
    parent = m_parent;
#endif
    auto size = m_parent->GetClientSize();
#ifdef __WXOSX__
    m_tb = m_frame->CreateToolBar();
    m_tb->SetName( "ViewBar" );
#else
    if( !m_tb )
    {
        m_tb = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, styleViewBar, "ViewBar" );
    }
    else
        m_tb->ClearTools();
#endif
    wxBitmapBundle selectall;
#ifdef __WXMSW__
    HANDLE gs_wxMainThread = NULL;
    const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "library", &gs_wxMainThread );
    const void* dataSelectAll = nullptr;
    size_t sizeSelectAll = 0;
    if( !wxLoadUserResource( &dataSelectAll, &sizeSelectAll, "selectAll", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        selectall = wxBitmapBundle::FromSVG( (const char *) dataSelectAll, wxSize( 16, 16 ) );
    }
#elif __WXGTK__
    selectall = wxBitmapBundle::FromSVG( selectAll, wxSize( 16, 16 ) );
#else
    selectall = wxBitmapBundle::FromSVGResource( "selectall", wxSize( 16, 16 ) );
#endif
    CreateLibraryMenu();
    m_tb->AddTool( wxID_LIBRARYNEW, _( "New Library" ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR, wxSize( 16, 16 ) ), wxArtProvider::GetIcon( wxART_NEW, wxART_TOOLBAR, wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Close" ), _( "Close Library View" ) );
    m_tb->AddTool( wxID_LIBRARYSELECTALL, _( "Select All" ), selectall, selectall, wxITEM_NORMAL, _( "Select All" ), _( "Select all library entries within selected library" ) );
    m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxArtProvider::GetBitmapBundle( wxART_QUIT, wxART_TOOLBAR, wxSize( 16, 16 ) ), wxArtProvider::GetIcon( wxART_QUIT, wxART_TOOLBAR, wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Close" ), _( "Close Library View" ) );
    m_tb->Realize();
}

void LibraryViewPainter::CreateLibraryMenu()
{
    auto mbar = new wxMenuBar;
    auto fileMenu = new wxMenu;
    fileMenu->Append( wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_EXIT );
    mbar->Insert( 0, fileMenu, _( "&File" ) );
    auto libraryMenu = new wxMenu;
    libraryMenu->Append( wxID_LIBRARYNEW, _( "&Create..." ), _( "Create new library" ) );
    libraryMenu->Append( wxID_LIBRARYDELETE, _( "&Delete" ), _( "Delete library" ) );
    libraryMenu->Append( wxID_LIBRARYSELECTALL, _( "Select &All" ), _( "Select all library entries wth selected library" ) );
    libraryMenu->AppendSeparator();
    mbar->Insert( 1, libraryMenu, _( "&Library" ) );
    auto entryMenu = new wxMenu;
    entryMenu->Append( wxID_IMPORTLIBRARY, _( "Import..." ), _( "Import library entry from a file" ) );
    mbar->Insert( 2, entryMenu, _( "&Entry" ) );
    auto designMenu = new wxMenu;
    designMenu->Append( wxID_EXPANDCOLLAPS, _( "Expand/Collapse Branch\tEnter" ), _("Expand or collapse branch" ) );
    mbar->Insert( 3, designMenu, _( "&Design" ) );
    m_frame->SetMenuBar( mbar );
}

void LibraryViewPainter::ExpandRoot(const wxString &path)
{
    if( !path.IsEmpty() )
        ExpandPath( path );
#ifdef __UNIX__
    else
    {
        // On Unix, there's only one node under the (hidden) root node. It
        // represents the / path, so the user would always have to expand it;
        // let's do it ourselves
        ExpandPath( "/" );
    }
#endif
}

void LibraryViewPainter::ExpandDir(wxTreeListItem parent)
{
    PopulateNode( parent );
}

void LibraryViewPainter::PopulateNode(wxTreeListItem parent)
{
    wxDirItemData *data = (wxDirItemData *) m_tree->GetItemData( parent );
    if( m_tree->IsExpanded( parent ) )
        return;
    wxASSERT( data );

    wxString path;

    wxString dirName( data->m_path );

#if defined(__WINDOWS__)
    // Check if this is a root directory and if so,
    // whether the drive is available.
    if( !IsDriveAvailable( dirName ) )
    {
        data->m_isExpanded = false;
        //wxMessageBox(wxT("Sorry, this drive is not available."));
        return;
    }
#endif
    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;
#if defined(__WINDOWS__)
    if( dirName.Last() == ':' )
        dirName += wxString( wxFILE_SEP_PATH );
#endif
    wxArrayString dirs;
    std::vector<std::unique_ptr<LibraryObject> > files;
    wxDir d;
    wxString eachFilename;
    wxLogNull log;
    int style;
    d.Open( dirName );
    if( d.IsOpened() )
    {
        style = wxDIR_DIRS;
        if( d.GetFirst( &eachFilename, wxEmptyString, style ) )
        {
            do
            {
                wxFileName name( eachFilename );
                if( ( eachFilename != "." ) && ( eachFilename != ".." ) )
                {
                    dirs.Add( eachFilename );
                }
            }
            while( d.GetNext( &eachFilename ) );
        }
        style = wxDIR_FILES;
        if( d.GetFirst( &eachFilename, "*.abl", style ) )
        {
            do
            {
                std::unique_ptr<LibraryObject> library;
                if( ( eachFilename != "." ) && ( eachFilename != ".." ) )
                {
                    LoadApplicationOject( d.GetName() + wxString( wxFILE_SEP_PATH ) + eachFilename, library );
                    files.push_back( std::move( library ) );
                }
            }
            while( d.GetNext( &eachFilename ) );
        }
    }
    // Add the sorted files
    size_t i;
    for( std::vector<std::unique_ptr<LibraryObject> >::iterator it = files.begin(); it < files.end(); ++it )
    {
        path = dirName;
        if( !wxEndsWithPathSeparator( path ) )
            path += wxString( wxFILE_SEP_PATH );
        path += eachFilename;

        wxDirItemData *dir_item = new wxDirItemData( path, eachFilename, false );
        wxTreeListItem treeid = m_tree->AppendItem( parent, eachFilename, 2, 3, dir_item );
        m_tree->SetItemText( treeid, COL_COMMENT, (*it)->GetComment() );
        for( std::vector<LibraryObjects>::iterator it1 = (*it)->GetObjects().begin(); it1 < (*it)->GetObjects().end(); ++it1 )
        {
            wxTreeListItem treeidobj = m_tree->AppendItem( treeid, (*it1).m_properties.m_name );
            m_tree->SetItemText( treeidobj, COL_MODIFIED, (*it1).m_properties.m_modified.FormatDate() + " " + (*it1).m_properties.m_modified.FormatTime() );
            m_tree->SetItemText( treeidobj, COL_COMPILED, (*it1).m_properties.m_compiled.FormatDate() + " " + (*it1).m_properties.m_compiled.FormatTime() );
            m_tree->SetItemText( treeidobj, COL_SIZE,     wxString::Format( "%d", (*it1).m_properties.m_size ) );
            m_tree->SetItemText( treeidobj, COL_COMMENT, (*it1).m_properties.m_comment );
        }
    }
    // And the dirs
    for (i = 0; i < dirs.GetCount(); i++)
    {
        eachFilename = dirs[i];
        path = dirName;
        if( !wxEndsWithPathSeparator( path ) )
            path += wxString( wxFILE_SEP_PATH );
        path += eachFilename;

        wxDirItemData *dir_item = new wxDirItemData( path, eachFilename, true );
        wxTreeListItem treeid = m_tree->AppendItem( parent, eachFilename, 0, 1, dir_item );
    }
}

const wxTreeListItem LibraryViewPainter::AddSection(const wxString& path, const wxString& name)
{
    wxDirItemData *dir_item = new wxDirItemData( path, name, true );
    wxTreeListItem treeid = m_tree->AppendItem( m_rootId, name, 1, 2, dir_item );
    return treeid;
}

size_t LibraryViewPainter::GetAvailableDrives(wxArrayString &paths, wxArrayString &names, wxArrayInt &icon_ids)
{
#if defined(wxHAS_FILESYSTEM_VOLUMES) || defined(__APPLE__)

#if (defined(__WIN32__) || defined(__WXOSX__)) && wxUSE_FSVOLUME
    // TODO: this code (using wxFSVolumeBase) should be used for all platforms
    //       but unfortunately wxFSVolumeBase is not implemented everywhere
    const wxArrayString as = wxFSVolumeBase::GetVolumes();

    for( size_t i = 0; i < as.GetCount(); i++ )
    {
        wxString path = as[i];
        wxFSVolume vol( path );
        int imageId;
        switch( vol.GetKind() )
        {
        case wxFS_VOL_FLOPPY:
            if( ( path == wxT( "a:\\" ) ) || ( path == wxT( "b:\\" ) ) )
                imageId = wxFileIconsTable::floppy;
            else
                imageId = wxFileIconsTable::removeable;
            break;
        case wxFS_VOL_DVDROM:
        case wxFS_VOL_CDROM:
            imageId = wxFileIconsTable::cdrom;
            break;
        case wxFS_VOL_NETWORK:
            if( path[0] == wxT( '\\' ) )
                continue; // skip "\\computer\folder"
            imageId = wxFileIconsTable::drive;
            break;
        case wxFS_VOL_DISK:
        case wxFS_VOL_OTHER:
        default:
            imageId = wxFileIconsTable::drive;
            break;
        }
        paths.Add( path );
        names.Add( vol.GetDisplayName() );
        icon_ids.Add( imageId );
    }
#else // !__WIN32__
    /* If we can switch to the drive, it exists. */
    for( char drive = 'A'; drive <= 'Z'; drive++ )
    {
        const wxString
            path = wxFileName::GetVolumeString( drive, wxPATH_GET_SEPARATOR );

        if( wxIsDriveAvailable( path ) )
        {
            paths.Add( path );
            names.Add( wxFileName::GetVolumeString( drive, wxPATH_NO_SEPARATOR ) );
            icon_ids.Add( drive <= 2 ? wxFileIconsTable::floppy : wxFileIconsTable::drive );
        }
    }
#endif // __WIN32__/!__WIN32__

#elif defined(__UNIX__)
    paths.Add( wxT( "/" ) );
    names.Add( wxT( "/" ) );
    icon_ids.Add( wxFileIconsTable::computer );
#else
#error "Unsupported platform in wxGenericDirCtrl!"
#endif
    wxASSERT_MSG( ( paths.GetCount() == names.GetCount() ), wxT( "The number of paths and their human readable names should be equal in number." ) );
    wxASSERT_MSG( ( paths.GetCount() == icon_ids.GetCount() ), wxT( "Wrong number of icons for available drives." ) );
    return paths.GetCount();
}

bool LibraryViewPainter::ExpandPath(const wxString &path)
{
    bool done = false;
    wxTreeListItem treeid = FindChild( m_rootId, path, done );
    wxTreeListItem lastId = treeid; // The last non-zero treeid
    while( treeid.IsOk() && !done )
    {
        ExpandDir( treeid );
        treeid = FindChild( treeid, path, done );
        if( treeid.IsOk() )
            lastId = treeid;
    }
    if( !lastId.IsOk() )
        return false;
    wxDirItemData *data = (wxDirItemData *) m_tree->GetItemData( lastId );
    if( data->m_isDir )
    {
        m_tree->Expand( lastId );
    }
    if( data->m_isDir )
    {
        // Find the first file in this directory
        wxTreeListItem childId = m_tree->GetFirstChild( lastId );
        bool selectedChild = false;
        while( childId.IsOk() )
        {
            data = (wxDirItemData *) m_tree->GetItemData( childId );

            if( data && !data->m_path.empty() && !data->m_isDir )
            {
                m_tree->Select( childId );
                m_tree->EnsureVisible( childId );
                selectedChild = true;
                break;
            }
            childId = m_tree->GetNextItem( childId );
        }
        if( !selectedChild )
        {
            m_tree->Select( lastId );
            m_tree->EnsureVisible( lastId );
        }
    }
    else
    {
        if( path == data->m_path )
            m_tree->Expand( lastId );
        m_tree->Select( lastId );
        m_tree->EnsureVisible( lastId );
    }

    return true;
}

wxTreeListItem LibraryViewPainter::FindChild(wxTreeListItem parentId, const wxString &path, bool &done)
{
    wxString path2( path );

    // Make sure all separators are as per the current platform
    path2.Replace( "\\", wxString( wxFILE_SEP_PATH ) );
    path2.Replace( "/", wxString( wxFILE_SEP_PATH ) );

    // Append a separator to foil bogus substring matching
    path2 += wxString( wxFILE_SEP_PATH );

    // In MSW case is not significant
#if defined(__WINDOWS__)
    path2.MakeLower();
#endif

    wxTreeListItem childId = m_tree->GetFirstChild( parentId );
    while( childId.IsOk() )
    {
        wxDirItemData* data = (wxDirItemData *) m_tree->GetItemData( childId );
        if( data && !data->m_path.empty() )
        {
            wxString childPath( data->m_path );
            if( !wxEndsWithPathSeparator( childPath ) )
                childPath += wxString( wxFILE_SEP_PATH );
            // In MSW case is not significant
#if defined(__WINDOWS__)
            childPath.MakeLower();
#endif
            if( childPath.length() <= path2.length() )
            {
                wxString path3 = path2.Mid( 0, childPath.length() );
                if( childPath == path3 )
                {
                    if( path3.length() == path2.length() )
                        done = true;
                    else
                        done = false;
                    return childId;
                }
            }
        }
        childId = m_tree->GetNextItem( childId );
    }
    wxTreeListItem invalid;
    if( parentId == m_rootId )
        return m_rootId;
    else
        return invalid;
}

bool LibraryViewPainter::IsDriveAvailable(const wxString& dirName)
{
#ifdef __WIN32__
    UINT errorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif
    bool success = true;

    // Check if this is a root directory and if so,
    // whether the drive is available.
    if (dirName.length() == 3 && dirName[(size_t)1] == wxT(':'))
    {
        wxString dirNameLower(dirName.Lower());
#ifndef wxHAS_DRIVE_FUNCTIONS
        success = wxDirExists(dirNameLower);
#else
        int currentDrive = _getdrive();
        int thisDrive = (int) (dirNameLower[(size_t)0] - 'a' + 1) ;
        int err = setdrive( thisDrive ) ;
        setdrive( currentDrive );

        if (err == -1)
        {
            success = false;
        }
#endif
    }
#ifdef __WIN32__
    (void) SetErrorMode(errorMode);
#endif

    return success;
}

void LibraryViewPainter::OnItemContextMenu(wxTreeListEvent &event)
{
    wxMenu menu;
    auto item = event.GetItem();
    wxDirItemData *data = dynamic_cast<wxDirItemData *>( m_tree->GetItemData( item ) );
    if( data->m_isDir )
        menu.Append( wxID_IMPORTLIBRARY, _( "Import..." ) );
    else
    {
        wxTreeListItems items;
        m_tree->GetSelections( items );
        auto text = m_tree->GetItemText( items[0] );
        if( text.EndsWith( "abl" ) )
        {
            menu.Append( wxID_SELECTALL, _( "Select All"), _( "Select all library entries within selected library" ) );
            menu.Append( wxID_PROPERTIES, _( "Propreties..."), _( "Specify the library properties" ) );
            menu.AppendSeparator();
        }
    }
    int rc = m_frame->GetPopupMenuSelectionFromUser( menu );
    switch( rc )
    {
        case wxID_SELECTALL:
            SelectAllLibraryObjects( item );
            break;
    }
}

void LibraryViewPainter::OnLibraryCreate(wxCommandEvent &WXUNUSED(event))
{
    wxTreeListItems items;
    LibraryObject *library = new LibraryObject;
    std::unique_ptr<PropertiesHandler> propertiesPtr;
    auto failed = false;
    auto title = _( "Properties" );
    auto id = m_tree->GetSelections( items );
    auto data = (wxDirItemData *) m_tree->GetItemData( items[0] );
    wxFileName path( data->m_path );
    auto dir = path.GetPath();
    while( !failed )
    {
        wxFileDialog dlg( m_parent, _( "Create Library" ), dir, wxEmptyString, "AB Library (*.abl)|*.abl", wxFD_SAVE );
        if( dlg.ShowModal() == wxID_CANCEL )
        {
            failed = true;
            return;
        }
        else
        {
            auto name = dlg.GetPath() + ".abl";
            if( wxFile::Exists( name ) )
                wxMessageBox( _( "Create failed. Possible cases:\n\r1.Library already exist.\n\r2.Invalid library name.\n\r3.Invalid path name,\n\r4.Out of disk space" ),
                              _( "Library creation failed" ),
                              wxOK | wxCENTRE | wxICON_EXCLAMATION );
            else
            {
                library->SetLibraryName( name );
                library->SetCreationTime( wxDateTime::Now() );
                failed = true;
                wxDynamicLibrary lib;
                wxString libName;
#ifdef __WXMSW__
                libName = m_libPath + "dialogs";
#elif __WXOSX__
                libName = m_libPath + "liblibdialogs.dylib" ;
#else
                libName = m_libPath + "libdialogs";
#endif
                lib.Load( libName );
                if( lib.IsLoaded() )
                {
                    wxAny any = library;
#if __cplusplus > 201300
                    auto ptr = std::make_unique<LibraryPropertiesHandler>( library->GetProperties() );
#else
                    auto ptr = std::unique_ptr<LibraryPropertiesHandler>( new LibraryPropertiesHandler( library->GetProperties() ) );
#endif
                    propertiesPtr = std::move( ptr );
                    wxCriticalSection *pcs;
                    propertiesPtr->SetHandlerObject( any );
                    wxString command = wxEmptyString;
                    propertiesPtr->SetType( LibraryPropertiesType );
                    CREATEPROPERTIESDIALOG func = (CREATEPROPERTIESDIALOG) lib.GetSymbol( "CreatePropertiesDialog" );
                    auto res = func( m_frame, propertiesPtr, title, command, false, *pcs );
                    if( res == wxID_OK )
                    {
                        wxMessageBox( "Creating new library!!" );
                    }
                }
                else
                {
                    wxMessageBox( "Error loading dialogs library!!" );
                }
            }
        }
    }
}

void LibraryViewPainter::OnLibraryDelete(wxCommandEvent &WXUNUSED(event))
{
    wxDirItemData *name = (wxDirItemData *)m_tree->GetItemData( m_tree->GetSelection() );
    auto res = wxMessageBox( _( "Delete library " + name->m_path + "?" ), _( "Delete Library" ), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT );
    if( res == wxYES )
        wxMessageBox( "Deleting" );
}

void LibraryViewPainter::OnSelectAllUpdateUI(wxUpdateUIEvent &event)
{
    wxTreeListItems items;
    auto count = m_tree->GetSelections( items );
    if( count > 1 )
        event.Enable( false );
    else
    {
        wxDirItemData *data = dynamic_cast<wxDirItemData *>( m_tree->GetItemData( items[0]) );
        if( ( data && data->m_isDir ) || !data )
            event.Enable( false );
        else
            event.Enable( true );
    }
}

bool LibraryViewPainter::LoadApplicationOject(const wxString &fileName, std::unique_ptr<LibraryObject> &library)
{
    wxXmlDocument doc;
    library = std::make_unique<LibraryObject>();
    library->SetLibraryName( fileName );
    if( !doc.Load( fileName ) )
    {
        wxMessageBox( wxString::Format( _( "Failed to load library file: %s" ) ), fileName );
        return false;
    }
    if( doc.GetRoot()->GetName().IsSameAs( "Library" ) )
    {
        QueryInfo query;
        wxString widthStr, objectComment, objectName;
        unsigned int size;
        wxDateTime lastmodified = wxDateTime::Now(), lastcompiled = wxDateTime::Now();
        wxXmlNode *children = doc.GetRoot()->GetChildren(), *bodyChildren, *queryChildren, *headerChildren;
        while( children )
        {
            if( children->GetName().IsSameAs( "Header" ) )
            {
                headerChildren = children->GetChildren();
                while( headerChildren )
                {
                    if( headerChildren->GetName().IsSameAs( "Created" ) )
                    {
                        wxDateTime dt;
                        wxString::const_iterator end;
                        widthStr = headerChildren->GetNodeContent();
                        auto res = dt.ParseISOCombined( widthStr );
                        if( res )
                            library->SetCreationTime( dt );
                    }
                    if( headerChildren->GetName().IsSameAs( "Comment" ) )
                    {
                        widthStr = headerChildren->GetNodeContent();
                        library->SetComment( widthStr );
                    }
                    headerChildren = headerChildren->GetNext();
                }
                children = children->GetNext();
            }
            if( children->GetName().IsSameAs( "Body" ) )
            {
                bodyChildren = children->GetChildren();
                while( bodyChildren )
                {
                    if( bodyChildren->GetName().IsSameAs( "Query" ) )
                    {
                        queryChildren = bodyChildren->GetChildren();
                        while( queryChildren )
                        {
                            if( queryChildren->GetName().IsSameAs( "name" ) )
                            {
                                widthStr = queryChildren->GetNodeContent();
                                if( widthStr.substr( widthStr.size() - 3 ) == "qry" )
                                {
                                    objectName = widthStr.substr( 0, widthStr.length() - 4 );
                                }
                            }
                            if( queryChildren->GetName().IsSameAs( "lastmodified" ) )
                            {
                                wxString::const_iterator end;
                                widthStr = queryChildren->GetNodeContent();
                                lastmodified.ParseISOCombined( widthStr );
                            }
                            if( queryChildren->GetName().IsSameAs( "lastcompiled" ) )
                            {
                                wxString::const_iterator end;
                                widthStr = queryChildren->GetNodeContent();
                                lastcompiled.ParseISOCombined( widthStr );
                            }
                            if( queryChildren->GetName().IsSameAs( "sizeinbytes" ) )
                            {
                                size = wxAtoi( queryChildren->GetNodeContent() );
                            }
                            if( queryChildren->GetName().IsSameAs( "comment" ) )
                            {
                                objectComment = queryChildren->GetNodeContent();
                            }
                            queryChildren = queryChildren->GetNext();
                        }
                        library->GetObjects().push_back( LibraryObjects( library->GetLibraryName(), objectName, lastmodified, lastcompiled, size, objectComment ) );
                    }
                    bodyChildren = bodyChildren->GetNext();
                }
            }
            children = children->GetNext();
        }
    }
    else
    {
        wxMessageBox( wxString::Format( _( "Invalid library file: %s" ) ), fileName );
        return false;
    }
    return true;
}

void LibraryViewPainter::OnSelectAll(wxCommandEvent &event)
{
    wxTreeListItems items;
    auto count = m_tree->GetSelections( items );
    SelectAllLibraryObjects( items[0] );
}

void LibraryViewPainter::SelectAllLibraryObjects(wxTreeListItem item)
{
    m_tree->Unselect( item );
    auto child = m_tree->GetFirstChild( item );
    while( child.IsOk() )
    {
        m_tree->Select( child );
        child = m_tree->GetNextSibling( child );
    }
}

void LibraryViewPainter::OnSelectionChanged(wxTreeListEvent &event)
{
    m_tree->UnselectAll();
    m_tree->Select( event.GetItem() );
}

void LibraryViewPainter::OnDeleteLibraryUpdateUI(wxUpdateUIEvent &event)
{
    wxTreeListItems items;
    m_tree->GetSelections( items );
    if( m_tree->GetItemText( items[0], 0 ).Right( 4 ) == ".abl" )
        event.Enable( true );
    else
        event.Enable( false );
}

void LibraryViewPainter::OnExpandCollapsUpdateUI(wxUpdateUIEvent &event)
{
    wxTreeListItems items;
    m_tree->GetSelections( items );
    auto item = m_tree->GetFirstChild( items[0] );
    if( item.IsOk() )
        event.Enable( true );
    else
        event.Enable( false );
}

void LibraryViewPainter::OnExpandCollapse(wxCommandEvent &event)
{
    wxTreeListItems items;
    m_tree->GetSelections( items );
    if( m_tree->IsExpanded( items[0] ) )
        m_tree->Collapse( items[0] );
    else
        m_tree->Expand( items[0] );
}

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dir.h"
#include "wx/listctrl.h"
#include "wx/dynlib.h"
#include "wx/filename.h"
#ifdef __WXGTK__
#include "gtk/gtk.h"
#include "library.h"
#if !GTK_CHECK_VERSION(3,6,0)
#include "wx/generic/stattextg.h"
#endif
#endif
#include "bitmappanel.h"
#include "newquery.h"
#include "getobjectname.h"

GetObjectName::GetObjectName(wxWindow *parent, int id, const wxString &title, int objectId, const std::vector<QueryInfo> &queries, const std::vector<LibrariesInfo> &path) : wxDialog( parent, id, title )
{
	long style;
#ifdef __WXMSW__
	style = wxTE_MULTILINE;
#else
	style = 0; 
#endif
    m_path = path;
    wxVector<wxBitmapBundle> images;
#ifdef __WXMSW__
    wxBitmapBundle library;
    HANDLE gs_wxMainThread = NULL;
    const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "diakigs", &gs_wxMainThread );
    const void* data1 = nullptr;
    size_t size1 = 0;
    if( !wxLoadUserResource( &data1, &size1, "library", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        library = wxBitmapBundle::FromSVG( (const char *) data1, wxSize( 16, 16 ) );
    }
    images.push_back( library );
#elif __WXGTK__
    images.push_back( wxBitmapBundle::FromSVG( library, wxSize( 16, 16 ) ) );
#else
    images.push_back( wxBitmapBundle::FromSVGResource( "library", wxSize( 16, 16 ) ) );
#endif
    m_id = objectId;
    m_title = title;
    auto sizer1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new wxPanel( this );
    sizer1->Add( m_panel, 0, wxEXPAND, 0 );
    auto sizer2 = new wxBoxSizer( wxHORIZONTAL );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto grid = new wxFlexGridSizer( 2, 2, 5, 5 );
    sizer3->Add( grid, 0, wxEXPAND, 0 );
    auto sizer4 = new wxBoxSizer( wxVERTICAL );
    grid->Add( sizer4, 0, wxEXPAND, 0 );
    m_painterNameLabel = new wxStaticText( m_panel, wxID_ANY, "" );
    if( objectId == 1 )
    {
        m_painterNameLabel->SetLabel( _( "Query Name:" ) );
    }
    else if( objectId == -1 )
    {
        m_painterNameLabel->SetLabel( _( "Save Query As:" ) );
    }
    sizer4->Add( m_painterNameLabel, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_painterName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer4->Add( m_painterName, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    long index = 0;
    m_objectList = new wxListCtrl( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER );
    m_objectList->AppendColumn( "" );
    for( std::vector<QueryInfo>::const_iterator it = queries.begin(); it < queries.end(); ++it )
    {
        m_objectList->InsertItem( index, (*it).name );
        m_objectList->SetItemPtrData( index++, wxUIntPtr( &(*it).comment ) );
    }
    sizer4->Add( m_objectList, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_comments = new wxStaticText( m_panel, wxID_ANY, _( "&Comments" ) );
    sizer4->Add( m_comments, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_commentsText = new wxTextCtrl( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
	m_commentsText->SetMaxLength( 256 );
    sizer4->Add( m_commentsText, 1, wxEXPAND, 0 );
    auto sizer5 = new wxBoxSizer( wxVERTICAL );
    grid->Add( sizer5, 0, wxEXPAND, 0 );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_ok->SetDefault();
    sizer5->Add( m_ok, 0, wxEXPAND, 0 );
    sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    sizer5->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
    if( m_id > 0 )
    {
        m_new = new wxButton( m_panel, wxID_NEWOBJECT, _( "&New" ) );
        sizer5->Add( m_new, 0, wxEXPAND, 0 );
        sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
        m_browse = new wxButton( m_panel, wxID_ANY, _( "&Browse" ) );
        sizer5->Add( m_browse, 0, wxEXPAND, 0 );
        sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
    }
    m_help = new wxButton( m_panel, wxID_HELP, _( "&Help" ) );
    sizer5->Add( m_help, 0, wxEXPAND, 0 );
    m_librariesList = new wxListCtrl( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER );
    m_librariesList->SetSmallImages(  images );
    wxListItem item;
    item.SetMask( wxLIST_MASK_IMAGE );
    item.SetImage( 0 );
    m_librariesList->InsertColumn( 0, item );
    for( std::vector<LibrariesInfo>::const_iterator it = path.begin(); it < path.end(); ++it )
    {
        m_librariesList->InsertItem( 0, (*it).m_path, (*it).m_isActive ? 0 : -1 );
    }
    grid->Add( m_librariesList, 0, wxEXPAND, 0 );
    if( m_id > 0 )
    {
        m_browseLibs = new wxButton( m_panel, wxID_ANY, _( "&Browse,,," ) );
        grid->Add( m_browseLibs, 0, wxALIGN_CENTER_VERTICAL, 0 );
    }
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer2 );
    SetSizer( sizer1 );
    sizer1->Fit( this );
    Layout();
    m_objectList->SetColumnWidth( 0, m_objectList->GetSize().GetWidth() );
    m_librariesList->SetColumnWidth( 0, m_librariesList->GetSize().GetWidth() );
    set_properties();
    if( m_id > 0 )
    {
        m_new->Bind( wxEVT_BUTTON, &GetObjectName::OnButtonNew, this );
        m_browse->Bind( wxEVT_BUTTON, &GetObjectName::OnButtonBrowse, this );
    }
    m_ok->Bind( wxEVT_BUTTON, &GetObjectName::OnOKButton, this );
    m_ok->Bind( wxEVT_UPDATE_UI, &GetObjectName::OnOKButtonUpdateUI, this );
    m_objectList->Bind( wxEVT_LIST_ITEM_SELECTED, &GetObjectName::OnNameSelected, this );
    m_objectList->Bind( wxEVT_LIST_ITEM_ACTIVATED, &GetObjectName::OnNameActivated, this );
    if( m_objectList->GetItemCount() > 0 )
        m_objectList->SetItemState( 0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED );
    m_painterName->SetFocus();
}

void GetObjectName::set_properties()
{
    SetTitle( m_title );
    wxString fileName;
    wxDir dir( wxGetCwd() );
    wxArrayString libs;
    dir.GetAllFiles( dir.GetName(), &libs, "*.abl", wxDIR_FILES );
    if( m_id == 1 )
    {
        bool res = dir.GetFirst( &fileName, "*.qry" );
        while( res )
        {
            res = dir.GetNext( &fileName );
        }
    }
    m_librariesList->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    m_ok->SetDefault();
}

void GetObjectName::OnButtonNew(wxCommandEvent &event)
{
    if( m_id == 1 )
    {
    }
    EndModal( dynamic_cast<wxButton *>( event.GetEventObject() )->GetId() );
}

void GetObjectName::OnButtonBrowse(wxCommandEvent &WXUNUSED(event))
{
    wxFileDialog dlg( NULL, _( "Open Query" ), wxEmptyString, wxEmptyString, "query files(*.qry)|*.qry", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    int res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        m_fileName = dlg.GetPath();
    }
}

const wxString &GetObjectName::GetFileName() const
{
    return m_fileName;
}

int GetObjectName::GetSource() const
{
    return m_source;
}

int GetObjectName::GetPresentation() const
{
    return m_presentation;
}

void GetObjectName::OnOKButton(wxCommandEvent &WXUNUSED(event))
{
    if( m_id > 0 )
    {
        auto name = m_painterName->GetValue();
        if( m_objectList->FindItem( -1, name ) == wxNOT_FOUND )
        {
            wxMessageBox( _( "Specified object is not found. Did you mean to create one with clicing 'New'?" ), _( "Error" ), wxICON_ERROR );
            return;;
        }
    }
    auto folder = m_librariesList->GetItemText( m_librariesList->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) );
    wxFileName name( folder );
    m_objectFileName = name.GetPathWithSep()+ m_painterName->GetValue();
    EndModal( wxID_OK );
}

void GetObjectName::OnOKButtonUpdateUI(wxUpdateUIEvent &event)
{
    if( m_painterName->GetValue().Length() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void GetObjectName::OnNameSelected(wxListEvent &event)
{
    m_painterName->SetValue( event.GetText() );
    auto item = event.GetIndex();
    auto data = reinterpret_cast<wxString *>( ((wxListCtrl *) event.GetEventObject() )->GetItemData( item ) );
    m_commentsText->SetValue( *data );
}

void GetObjectName::OnNameActivated(wxListEvent &event)
{
    m_painterName->SetValue( event.GetText() );
    EndModal( wxID_OK );
}

const wxString &GetObjectName::GetDocumentName() const
{
    return m_objectFileName;
}

const wxTextCtrl *GetObjectName::GetCommentObject() const
{
    return m_commentsText;
}

bool GetObjectName::isUpdating()
{
    if( m_objectList->FindItem( -1, m_painterName->GetValue() ) != wxNOT_FOUND )
        return true;
    else
        return false;
}


#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dir.h"
#include "wx/listctrl.h"
#ifdef __WXGTK__
#include "gtk/gtk.h"
#if !GTK_CHECK_VERSION(3,6,0)
#include "wx/generic/stattextg.h"
#endif
#endif
#include "bitmappanel.h"
#include "newquery.h"
#include "getobjectname.h"

GetObjectName::GetObjectName(wxWindow *parent, int id, const wxString &title, int objectId) : wxDialog( parent, id, title )
{
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
    m_objectList = new wxListCtrl( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER );
    m_objectList->AppendColumn( "" );
    sizer4->Add( m_objectList, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_comments = new wxStaticText( m_panel, wxID_ANY, _( "&Comments" ) );
    sizer4->Add( m_comments, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_commentsText = new wxTextCtrl( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
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
// TODO assign the SVG of the library and use it, when th wxListCtrl will adapt
/*    m_librariesList-SetImages(  wxBitmapBundle() );
    wxListItem item;
    item.SetMask( wxLIST_MASK_IMAGE );
    item.SetImage();
    m_librariesList-*/
    m_librariesList->AppendColumn( "" );
    grid->Add( m_librariesList, 0, wxEXPAND, 0 );
    m_browseLibs = new wxButton( m_panel, wxID_ANY, _( "&Browse,,," ) );
    grid->Add( m_browseLibs, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer2 );
    SetSizer( sizer1 );
    sizer1->Fit( this );
    Layout();
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

const wxString &GetObjectName::GetFileName()
{
    return m_fileName;
}

const int GetObjectName::GetSource()
{
    return m_source;
}

const int GetObjectName::GetPresentation()
{
    return m_presentation;
}

void GetObjectName::OnOKButton(wxCommandEvent &event)
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

}

void GetObjectName::OnNameActivated(wxListEvent &event)
{

}

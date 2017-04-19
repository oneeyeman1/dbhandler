#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "getobjectname.h"

GetObjectName::GetObjectName(wxWindow *parent, int id, const wxString &title, int objectId) : wxDialog( parent, id, title )
{
    m_panel = new wxPanel( this );
    m_painterNameLabel = new wxStaticText( m_panel, wxID_ANY, "" );
    if( objectId == QUERY )
    {
        SetTitle( _( "Query" ) );
        m_painterNameLabel->SetLabel( _( "Query Name:" ) );
    }
    m_painterName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    m_objectList = new wxListCtrl( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_ALWAYS_SB );
    m_comments = new wxStaticText( m_panel, wxID_ANY, _( "&Comments" ) );
    m_commentsText = new wxTextCtrl( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_new = new wxButton( m_panel, wxID_ANY, _( "&New" ) );
    m_browse = new wxButton( m_panel, wxID_ANY, _( "&Browse" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "&Help" ) );
    set_properties();
    do_layout();
}

void GetObjectName::set_properties()
{
    m_ok->SetDefault();
}

void GetObjectName::do_layout()
{
    wxBoxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_2 = new wxBoxSizer( wxVERTICAL );
    FlexGridSizer *sizer_3 = new FlexGridSizer( 2, 3, 0, 0 );
    wxBoxSizer *sizer_4 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_5 = new wxBoxSizer( wxVERTICAL );
    sizer_1->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_2->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_5->Add( m_painterNameLabel, 0, wxEXPAND, 0 );
    sizer_5->Add( m_painterName, 0, wxEXPAND, 0 );
    sizer_5->Add( 20, 10, 0, wxEXPAND, 0 );
    sizer_5->Add( m_objectList, 0, wxEXPAND, 0 );
    sizer_5->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_5->Add( m_comments, 0, wxEXPAND, 0 );
    sizer_5->Add( m_commentText, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0 );
    sizer_5->Add( 20, 10, 0, 0, 0 );
    sizer_3->Add( sizer_5, 0, wxEXPAND, 0 );
    sizer_3->Add( 10, 20, 0, wxEXPAND, 0 );
    sizer_4->Add( m_ok, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_new, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_browse, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_help, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_2->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
    sizer_1->Add( 20, 20, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer_1 );
    main->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( main );
    main->Fit( this );
    Layout();
}

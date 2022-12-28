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
    m_panel = new wxPanel( this );
    m_painterNameLabel = new wxStaticText( m_panel, wxID_ANY, "" );
    SetTitle( title );
    if( objectId == 1 )
    {
        m_painterNameLabel->SetLabel( _( "Query Name:" ) );
    }
    else if( objectId == -1 )
    {
        m_painterNameLabel->SetLabel( _( "Save Query As:" ) );
    }
    m_painterName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    m_objectList = new wxListCtrl( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_SINGLE | wxLB_ALWAYS_SB );
    m_comments = new wxStaticText( m_panel, wxID_ANY, _( "&Comments" ) );
    m_commentsText = new wxTextCtrl( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    if( m_id > 0 )
    {
        m_new = new wxButton( m_panel, wxID_NEWOBJECT, _( "&New" ) );
        m_browse = new wxButton( m_panel, wxID_ANY, _( "&Browse" ) );
    }
    m_help = new wxButton( m_panel, wxID_HELP, _( "&Help" ) );
    set_properties();
    do_layout();
    if( m_id > 0 )
    {
        m_new->Bind( wxEVT_BUTTON, &GetObjectName::OnButtonNew, this );
        m_browse->Bind( wxEVT_BUTTON, &GetObjectName::OnButtonBrowse, this );
    }
    m_ok->Bind( wxEVT_BUTTON, &GetObjectName::OnOKButton, this );
    m_ok->Bind( wxEVT_UPDATE_UI, &GetObjectName::OnOKButtonUpdateUI, this );
}

void GetObjectName::set_properties()
{
    wxString fileName;
    wxDir dir( wxGetCwd() );
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

void GetObjectName::do_layout()
{
    wxBoxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_2 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *sizer_3 = new wxFlexGridSizer( 2, 3, 0, 0 );
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
    sizer_5->Add( m_commentsText, 0, wxEXPAND, 0 );
    sizer_5->Add( 20, 10, 0, 0, 0 );
    sizer_3->Add( sizer_5, 0, wxEXPAND, 0 );
    sizer_3->Add( 10, 20, 0, wxEXPAND, 0 );
    sizer_4->Add( m_ok, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    if( m_id > 0 )
    {
        sizer_4->Add( m_new, 0, wxEXPAND, 0 );
        sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
        sizer_4->Add( m_browse, 0, wxEXPAND, 0 );
        sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    }
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

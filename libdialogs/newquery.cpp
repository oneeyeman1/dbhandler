// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "res/Quick_Select.xpm"
#include "res/SQL_Select.xpm"
#include "res/query.xpm"
#include "res/external.xpm"
#include "res/composite.xpm"
#include "res/group.xpm"
#include "bitmappanel.h"
#include "newquery.h"

NewQuery::NewQuery(wxWindow *parent, const wxString &title) : wxDialog(parent, wxID_ANY, title)
{
    m_source = m_presentation = -1;
    m_title = title;
    m_panel = new wxPanel( this );
    m_panels[0] = new BitmapPanel( m_panel, wxBitmap( Quick_Select ), _( "Quick Select" ) );
    m_panels[1] = new BitmapPanel( m_panel, wxBitmap( SQL_Select ), _( "SQL Select" ) );
    m_panels[2] = new BitmapPanel( m_panel, wxBitmap( query ), _( "Query" ) );
    m_panels[3] = new BitmapPanel( m_panel, wxBitmap( external ), _( "External" ) );
    m_panels[4] = new BitmapPanel( m_panel, wxBitmap( composite ), _( "Composite" ) );
    m_panels[9] = new BitmapPanel( m_panel, wxBitmap( group ), _( "Group" ) );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_options = new wxButton( m_panel, wxID_ANY, _( "&Options" ) );
    m_preview = new wxCheckBox( m_panel, wxID_ANY, _( "&Preview when built" ) );
    SetTitle( title );
    do_layout();
    m_panels[1]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_source = 2;
    for( int i = 0; i < 5; ++i )
    {
        m_panels[i]->Bind( wxEVT_LEFT_DOWN, &NewQuery::OnPanelClicked, this );
    }
}

NewQuery::~NewQuery()
{
}

void NewQuery::set_properties()
{
    SetTitle( m_title );
}

void NewQuery::do_layout()
{
    // begin wxGlade: NewQuery::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer( 3, 2, 5, 5 );
    wxStaticBoxSizer* sizer_5 = new wxStaticBoxSizer( new wxStaticBox( m_panel, wxID_ANY, _( "Presentation Style" ) ), wxHORIZONTAL );
    wxBoxSizer* sizer_6 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_9 = new wxBoxSizer( wxVERTICAL );
    wxStaticBoxSizer* sizer_4 = new wxStaticBoxSizer( new wxStaticBox( m_panel, wxID_ANY, _( "Data Source" ) ), wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[0], 0, 0, 0 );
    sizer_4->Add( 1, 1, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[1], 0, wxEXPAND, 0, 0 );
    sizer_4->Add( 1, 1, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[2], 0, wxEXPAND, 0, 0 );
    sizer_4->Add( 1, 1, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[3], 0, wxEXPAND, 0 );
    sizer_4->Add( 1, 1, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_9->Add( m_ok, 0, 0, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_9->Add( m_cancel, 0, 0, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_9->Add( m_help, 0, 0, 0 );
    grid_sizer_1->Add( sizer_9, 0, wxEXPAND, 0 );
    sizer_7->Add( m_panels[4], 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_7->Add( 0, 0, 0, 0, 0 );
    sizer_6->Add( sizer_7, 1, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_panels[9], 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_8->Add( 0, 0, 0, 0, 0 );
    sizer_6->Add( sizer_8, 1, wxEXPAND, 0 );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( sizer_5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( m_options, 0, wxALIGN_CENTER_VERTICAL, 0 );
    grid_sizer_1->Add( m_preview, 0, 0, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( grid_sizer_1, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer_2 );
    sizer_1->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
    // end wxGlade
}

void NewQuery::OnPanelClicked (wxMouseEvent &event)
{
    for( int i = 0; i < 5; ++i )
    {
        if( event.GetEventObject () == m_panels[i] )
        {
            if( i + 1 != m_source )
            {
                m_panels[m_source - 1]->GetLabel()->SetBackgroundColour( m_panel->GetBackgroundColour() );
                m_source = i + 1;
                m_panels[m_source - 1]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
            }
        }
    }
    event.Skip();
}

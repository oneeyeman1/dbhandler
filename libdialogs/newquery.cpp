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
#include "res/crosstab.xpm"
#include "res/freeform.xpm"
#include "res/graph.xpm"
#include "res/grid.xpm"
#include "res/group.xpm"
#include "res/label.xpm"
#include "res/n_up.xpm"
#include "res/ole.xpm"
#include "res/richtext.xpm"
#include "res/tabular.xpm"

#ifdef __WXGTK__
#include "gtk/gtk.h"
#if !GTK_CHECK_VERSION(3,6,0)
#include "wx/generic/stattextg.h"
#endif
#endif
#include "bitmappanel.h"
#include "newquery.h"

NewQuery::NewQuery(wxWindow *parent, const wxString &title, int source, int presentation) : wxDialog(parent, wxID_ANY, title)
{
    m_source = source;
    m_presentation = presentation;
    m_title = title;
    m_panel = new wxPanel( this );
    m_panels[0] = new BitmapPanel( m_panel, wxBitmap( Quick_Select ), _( "Quick Select" ) );
    m_panels[0]->SetExtraStyle( wxWS_EX_PROCESS_UI_UPDATES );
    m_panels[1] = new BitmapPanel( m_panel, wxBitmap( SQL_Select ), _( "SQL Select" ) );
    m_panels[1]->SetExtraStyle( wxWS_EX_PROCESS_UI_UPDATES );
    m_panels[2] = new BitmapPanel( m_panel, wxBitmap( query ), _( "Query" ) );
    m_panels[2]->SetExtraStyle( wxWS_EX_PROCESS_UI_UPDATES );
    m_panels[3] = new BitmapPanel( m_panel, wxBitmap( external ), _( "External" ) );
    m_panels[3]->SetExtraStyle( wxWS_EX_PROCESS_UI_UPDATES );
    m_panels[4] = new BitmapPanel( m_panel, wxBitmap( composite ), _( "Composite" ) );
    m_panels[5] = new BitmapPanel( m_panel, wxBitmap( crosstab ), _( "Crosstab" ) );
    m_panels[6] = new BitmapPanel( m_panel, wxBitmap( freeform ), _( "Freeform" ) );
    m_panels[7] = new BitmapPanel( m_panel, wxBitmap( graph ), _( "Graph" ) );
    m_panels[8] = new BitmapPanel( m_panel, wxBitmap( grid ), _( "Grid" ) );
    m_panels[9] = new BitmapPanel( m_panel, wxBitmap( group ), _( "Group" ) );
    m_panels[10] = new BitmapPanel( m_panel, wxBitmap( label ), _( "Label" ) );
    m_panels[11] = new BitmapPanel( m_panel, wxBitmap( n_up ), _( "N-Up" ) );
    m_panels[12] = new BitmapPanel( m_panel, wxBitmap( ole ), _( "Ole 2.0" ) );
    m_panels[13] = new BitmapPanel( m_panel, wxBitmap( richtext ), _( "RichText" ) );
    m_panels[14] = new BitmapPanel( m_panel, wxBitmap( tabular ), _( "Tabular" ) );
    wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_options = new wxButton( m_panel, wxID_ANY, _( "&Options" ) );
    m_preview = new wxCheckBox( m_panel, wxID_ANY, _( "&Preview when built" ) );
    SetTitle( title );
    do_layout();
    m_panels[m_presentation + 4]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    for( int i = 0; i < 4; ++i )
    {
        m_panels[i]->Bind( wxEVT_LEFT_DOWN, &NewQuery::OnPanelSourceClicked, this );
        m_panels[i]->Bind( wxEVT_UPDATE_UI, &NewQuery::OnDataSorceUpdateUI, this );
    }
    for( int i = 4; i < 15; ++i )
    {
        m_panels[i]->Bind( wxEVT_LEFT_DOWN, &NewQuery::OnPanelPresentationClicked, this );
    }
    m_options->Bind( wxEVT_UPDATE_UI, &NewQuery::OnOptionsUpdateUI, this );
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
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[1], 0, wxEXPAND, 0, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[2], 0, wxEXPAND, 0, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_panels[3], 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_9->Add( m_ok, 0, 0, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_9->Add( m_cancel, 0, 0, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_9->Add( m_help, 0, 0, 0 );
    grid_sizer_1->Add( sizer_9, 0, wxEXPAND, 0 );
    sizer_7->Add( m_panels[4], 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_panels[5], 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_panels[6], 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_panels[7], 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_panels[8], 0, wxEXPAND, 0 );
    sizer_6->Add( sizer_7, 1, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_panels[9], 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_panels[10], 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, 0, 0 );
    sizer_8->Add( m_panels[11], 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_panels[12], 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_panels[13], 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_panels[14], 0, wxEXPAND, 0 );
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

void NewQuery::OnPanelSourceClicked(wxMouseEvent &event)
{
    bool found = false;
    for( int i = 0; i < 4 || !found; ++i )
    {
        if( event.GetEventObject() == m_panels[i] )
        {
            if( i + 1 != m_source )
            {
                m_panels[m_source - 1]->GetLabel()->SetBackgroundColour( m_panel->GetBackgroundColour() );
                m_source = i + 1;
                m_panels[m_source - 1]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
            }
            found = true;
        }
    }
    Refresh();
    event.Skip();
}

void NewQuery::OnPanelPresentationClicked(wxMouseEvent &event)
{
    bool found = false;
    for( int i = 4; i < 14 || !found; ++i )
    {
        if( event.GetEventObject() == m_panels[i] )
        {
            if( i + 1 != m_presentation + 4 )
            {
                m_panels[m_presentation + 4]->GetLabel()->SetBackgroundColour( m_panel->GetBackgroundColour() );
                m_presentation = i - 4;
                m_panels[m_presentation + 4]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
            }
            found = true;
        }
    }
    Refresh();
    event.Skip();
}

void NewQuery::OnOptionsUpdateUI(wxUpdateUIEvent &event)
{
    if( m_presentation == 0 || m_presentation == 3 || m_presentation == 8 || m_presentation == 9 )
        event.Enable( false );
    else
        event.Enable( true );
}

const int NewQuery::GetSource()
{
    return m_source;
}

const int NewQuery::GetPresentation()
{
    return m_presentation;
}

void NewQuery::OnDataSorceUpdateUI(wxUpdateUIEvent &event)
{
    auto object = event.GetEventObject();
    if( m_presentation == 0 )
    {
        event.Enable( false );
        dynamic_cast<BitmapPanel *>( object )->GetLabel()->SetBackgroundColour( m_panel->GetBackgroundColour() );
    }
    else
    {
        event.Enable( true );
        if( m_source == 0 && object == m_panels[0] )
            m_panels[0]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
        if( m_source == 1 && object == m_panels[1] )
            m_panels[1]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
        if( m_source == 2 && object == m_panels[2] )
            m_panels[2]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
        if( m_source == 3 && object == m_panels[3] )
            m_panels[3]->GetLabel()->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    }
}

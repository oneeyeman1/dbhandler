// -*- C++ -*-
//
// generated by wxGlade 1.0.4 on Sun May 21 10:43:09 2023
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include <wx/wx.h>
#include <wx/image.h>
#include "toolbarsetup.h"

// begin wxGlade: ::extracode
// end wxGlade



ToolbarSetup::ToolbarSetup(wxWindow* parent, wxWindowID id, const wxString& title) :
    wxDialog(parent, id, title)
{
    // begin wxGlade: ToolbarSetup::ToolbarSetup
    SetTitle( _( "Toolbars" ) );
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new wxPanel( this, wxID_ANY );
    sizer_1->Add( m_panel, 1, wxEXPAND,  0 );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_4 = new wxBoxSizer( wxHORIZONTAL );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_5 = new wxBoxSizer( wxVERTICAL );
    sizer_4->Add( sizer_5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_6 = new wxBoxSizer( wxHORIZONTAL );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_7 = new wxBoxSizer( wxVERTICAL );
    sizer_6->Add( sizer_7, 1, wxEXPAND, 0 );
    wxStaticText* label_1 = new wxStaticText( m_panel, wxID_ANY, _( "Select Toolbar" ) );
    sizer_7->Add( label_1, 0, 0, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_toolbars = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_ALWAYS_SB | wxLB_SINGLE );
    sizer_7->Add( m_toolbars, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_orientation_choices[] = {
        _( "Left" ),
        _( "Top" ),
        _( "Right" ),
        _( "Bottom" ),
        _( "Floatng" )
    };
    m_orientation = new wxRadioBox( m_panel, wxID_ANY, wxT("Move"), wxDefaultPosition, wxDefaultSize, 1, m_orientation_choices, 5, wxRA_SPECIFY_ROWS );
    m_orientation->SetSelection( 1 );
    sizer_6->Add( m_orientation, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    sizer_5->Add( sizer_8, 0, wxEXPAND, 0 );
    m_showText = new wxCheckBox( m_panel, wxID_ANY, _( "Show Text" ) );
    sizer_8->Add( m_showText, 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    m_showToolTips = new wxCheckBox( m_panel, wxID_ANY, _( "Show ToolTips" ) );
    m_showToolTips->SetValue( 1 );
    sizer_8->Add( m_showToolTips, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer( 2, 2, 5, 5 );
    sizer_5->Add( grid_sizer_1, 1, wxEXPAND, 0 );
    wxStaticText* label_2 = new wxStaticText( m_panel, wxID_ANY, _( "Font Name" ) );
    grid_sizer_1->Add( label_2, 0, wxEXPAND, 0 );
    wxStaticText* label_3 = new wxStaticText( m_panel, wxID_ANY, _( "Font Size" ) );
    grid_sizer_1->Add( label_3, 0, wxEXPAND, 0 );
    m_fontNames = new wxComboBox( m_panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN );
    grid_sizer_1->Add( m_fontNames, 0, 0, 0 );
    m_fontSize = new wxComboBox( m_panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN );
    grid_sizer_1->Add( m_fontSize, 0, 0, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_9 = new wxBoxSizer( wxVERTICAL );
    sizer_4->Add( sizer_9, 1, wxEXPAND, 0 );
    m_close = new wxButton( m_panel, wxID_OK, _( "Close" ) );
    m_close->SetDefault();
    sizer_9->Add( m_close, 0, wxEXPAND, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    sizer_9->Add( m_help, 0, wxEXPAND, 0 );
    sizer_9->Add( 50, 50, 0, wxEXPAND, 0 );
    m_hideShow = new wxButton( m_panel, wxID_ANY, _( "Hide" ) );
    sizer_9->Add( m_hideShow, 0, wxEXPAND, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_customize = new wxButton( m_panel, wxID_ANY, _( "Customize..." ) );
    sizer_9->Add( m_customize, 0, wxEXPAND, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_new = new wxButton( m_panel, wxID_ANY, _( "New..." ) );
    m_new->Enable( 0 );
    sizer_9->Add( m_new, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    
    grid_sizer_1->AddGrowableRow( 0 );
    m_panel->SetSizer( sizer_2 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
    // end wxGlade
}

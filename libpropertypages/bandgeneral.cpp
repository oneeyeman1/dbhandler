// -*- C++ -*-
//
// generated by wxGlade 0.9.2 on Wed Jul 22 22:08:25 2020
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
#include <wx/wx.h>
#include <wx/image.h>

#include "wx/bmpcbox.h"
#include "wx/valnum.h"
#include "objectproperties.h"
#include "propertypagebase.h"
#include "bandgeneral.h"

// begin wxGlade: ::extracode
// end wxGlade

BandGeneralProperties::BandGeneralProperties(wxWindow* parent,  const BandProperties &props):
    PropertyPageBase(parent)
{
    BandColor m_colorNames[] =
    {
        { wxColour( *wxBLACK ), _( "Black" ) },
        { wxColour( *wxWHITE ), _( "White" ) },
        { wxColour( *wxRED ), _( "Red" ) },
        { wxColour( 0xFF0080 ), _( "Fuschia" ) },
        {wxTheColourDatabase->Find( "LIME GREEN" ), _( "Lime" ) },
        { wxColour( *wxYELLOW ), _( "Yellow" ) },
        { wxColour( *wxBLUE ), _( "Blue" ) },
        { wxTheColourDatabase->Find( "AQUAMARINE" ), _( "Aqua" ) },
        { wxTheColourDatabase->Find( "MAROON" ), _( "Maroon" ) },
        { wxTheColourDatabase->Find( "PURPLE" ), _( "Purple" ) },
        { wxColour( *wxGREEN ), _( "Green" ) },
//        { wxTheColourDatabase->Find( "OLIVE" ), _( "Olive" ) },
        { wxTheColourDatabase->Find( "NAVY" ), _( "Navy" ) },
        { wxColour( 0x008080 ), _( "Teal" ) },
        { wxTheColourDatabase->Find( "GREY" ), _( "Grey" ) },
//        { wxTheColourDatabase->Find( "SILVER" ), _( "Silver" ) },
//        { wxTheColourDatabase->Find( "MINT" ), _( "Mint" ) },
        { wxTheColourDatabase->Find( "SKY BLUE" ), _( "Sky" ) },
//        { wxTheColourDatabase->Find( "CREAM" ), _( "Cream" ) },
        { wxTheColourDatabase->Find( "MEDIUM GREY" ), _( "Medium Grey" ) },
        { wxTheColourDatabase->Find( "AQUAMARINE" ), _( "Cream" ) },
        { wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ), _( "WndBkgrnd" ) },
        { wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ), _( "AppWrkSpc" ) },
        { wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ), _( "ButtonFace" ) },
        { wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ), _( "WndText" ) },
        { wxColour( wxTransparentColour ), _( "Custom" ) },
        { wxColour( wxTransparentColour ), _( "Transparent" ) },
    };
    wxIntegerValidator<unsigned int> val( &m_heightValue );
    // begin wxGlade: BandGeneralProperties::BandGeneralProperties
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Color" ) );
    m_colors = new wxBitmapComboBox( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN );
    int size = sizeof( m_colorNames ) / sizeof( m_colorNames[0] );
    for( int i = 0; i < size; ++i )
    {
        wxMemoryDC dc;
        wxBitmap bmp( 20, 10 );
        dc.SelectObject( bmp );
        
        dc.SetPen( *wxBLACK_PEN );
        dc.DrawRectangle( 0, 0, 20, 10 );
        dc.SetBrush( wxBrush( m_colorNames[i].m_color ) );
        dc.DrawRectangle( 0, 0, 20, 10 );
        
        dc.SelectObject( wxNullBitmap );
        
        m_colors->Append( m_colorNames[i].m_colorName, bmp );
    }
    m_colors->SetSelection( size - 1 );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Height" ) );
    m_height = new wxTextCtrl( this, wxID_ANY, wxString::Format( "%d", props.m_height )/*, wxDefaultPosition, wxDefaultSize, 0, val*/ );

    set_properties();
    do_layout();
    // end wxGlade
    m_colors->Bind( wxEVT_COMBOBOX, &BandGeneralProperties::OnPageModified, this );
    m_height->Bind( wxEVT_TEXT, &BandGeneralProperties::OnPageModified, this );
}

void BandGeneralProperties::set_properties()
{
    // begin wxGlade: BandGeneralProperties::set_properties
    // end wxGlade
}

void BandGeneralProperties::do_layout()
{
    // begin wxGlade: BandGeneralProperties::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer_4 = new wxBoxSizer( wxHORIZONTAL );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( m_label1, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( m_colors, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 10, 0, wxEXPAND, 0 );
    sizer_3->Add( m_label2, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( m_height, 0, 0, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, 0, 0 );
    sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    // end wxGlade
}

void BandGeneralProperties::OnPageModified(wxCommandEvent &event)
{
    dynamic_cast<wxButton *>( GetParent()->GetParent()->FindWindowById( wxID_APPLY ) )->Enable( true );
    m_isModified = true;
}
//
//  colorcombobox.cpp
//  libpropertypages
//
//  Created by Igor Korot on 9/17/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/bmpcbox.h"
#include "colorcombobox.h"

CColorComboBox::CColorComboBox( wxWindow *parent, wxWindowID id, wxString selection, const wxPoint &pos, const wxSize &size, int n, const wxString choices[], long style ) :
wxBitmapComboBox( parent, id, selection, pos, size, n, choices, style )
{
    m_colors.push_back( ColorStruct( *wxBLACK, "Black" ) );
    m_colors.push_back( ColorStruct( *wxWHITE, "White" ) );
    m_colors.push_back( ColorStruct( *wxRED, "Red" ) );
    m_colors.push_back( ColorStruct( 0xFF00FF, "Fuchsia" ) );
    m_colors.push_back( ColorStruct( 0x32CC32, "Lime" ) );
    m_colors.push_back( ColorStruct( *wxYELLOW, "Yellow" ) );
    m_colors.push_back( ColorStruct( *wxBLUE, "Blue" ) );
    m_colors.push_back( ColorStruct( 0x70DB93, "Aqua" ) );
    m_colors.push_back( ColorStruct( 0x8E236B, "Maroon" ) );
    m_colors.push_back( ColorStruct( 0x8000FF, "Purple" ) );
    for( std::vector<ColorStruct>::iterator it = m_colors.begin(); it < m_colors.end(); ++it )
    {
        int w = 20, h = 10;
        wxMemoryDC dc;
        wxBitmap bmp( w, h );
        dc.SelectObject( bmp );
        wxColour magic( 255, 0, 255 );
        wxBrush magicBrush( magic );
        dc.SetBrush( magicBrush );
        dc.SetPen( *wxBLACK_PEN );
        dc.DrawRectangle( 0, 0, w, h );
        dc.SetBrush( wxBrush( (*it).m_color ) );
        dc.DrawRectangle( 0, 0, w, h );
        dc.SelectObject( wxNullBitmap );
        Append( (*it).m_name, bmp, &(*it).m_color );
    }
}

void CColorComboBox::SetColourValue(wxColour colour)
{
    std::vector<ColorStruct>::iterator it = std::find_if( m_colors.begin(), m_colors.end(), [&colour](const ColorStruct &x) { return x.m_color == colour; } );
    SetValue( (*it).m_name );
}

wxColour &CColorComboBox::GetColourValue()
{
    wxString sel;
#ifdef __WXOSX__
    sel = dynamic_cast<wxItemContainerImmutable *>( this )->GetStringSelection();
#else
    sel = GetStringSelection();
#endif
    std::vector<ColorStruct>::iterator it = std::find_if( m_colors.begin(), m_colors.end(), [&sel](const ColorStruct &x) { return x.m_name == sel; } );
    return ( (*it).m_color );
}

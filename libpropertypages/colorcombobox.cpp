/***************************************************************************
 *   Copyright (C) 2008 by Igor Korot   *
 *   igor@IgorsGentooOnNetwork   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/bmpcbox.h"
#include "wx/colour.h"
#include "wx/vector.h"
#include "colorcombobox.h"

CColorComboBox::CColorComboBox( wxWindow *parent, wxWindowID id, wxString selection, const wxPoint &pos, const wxSize &size, int n, const wxString choices[], long style ) :
wxBitmapComboBox( parent, id, selection, pos, size, n, choices, style )
{
    wxFont font;
//    wxVector<colorEntries> m_colourDialogNames = font.GetColors();
    for( unsigned i = 0; i < 27; i++ )
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
//        dc.SetBrush( wxBrush( m_colourDialogNames.at( i ).m_rgb ) );
        dc.DrawRectangle( 0, 0, w, h );
        dc.SelectObject( wxNullBitmap );
        wxMask *mask = new wxMask( bmp, magic );
        bmp.SetMask( mask );
//        Append( m_colourDialogNames.at( i ).m_name, bmp, &m_colourDialogNames.at( i ).m_rgb );
    }
}

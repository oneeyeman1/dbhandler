/***************************************************************************
 *   Copyright (C) 2005 by Igor Korot                                      *
 *   igor@IgorsGentoo                                                      *
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

#include "retrievalarguments.h"

RetrievalArguments::RetrievalArguments(wxWindow *parent) : wxDialog( parent, wxID_ANY, _( "" ) )
{
    m_panel = new wxPanel( this );
    m_arguments = new wxScrolled<wxWindow>( this );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_add = new wxButton( m_panel, wxID_ANY, _( "Add row" ) );
    m_remove = new wxButton( m_panel, wxID_ANY, _( "Delete row" ) );
}

RetrievalArguments::~RetrievalArguments(void)
{
}

void RetrievalArguments::set_properties()
{
}

void RetrievalArguments::do_layout()
{
}

ColumnLabels::ColumnLabels(wxScrolled<wxWindow> *parent ) : wxWindow( parent, wxID_ANY )
{
    m_parent = parent;
    Bind( wxEVT_PAINT, &ColumnLabels::OnPaint, this );
}

void ColumnLabels::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc( this );
    int scrollunits, origin;
    m_parent->GetViewStart( &origin, 0 );
    m_parent->GetScrollPixelsPerUnit( &scrollunits, 0 );
    dc.SetDeviceOrigin( -origin * scrollunits, 0 );
    dc.DrawText( _( "Column1" ), 5, 5 );
    dc.DrawText( _( "Column2" ), 105, 5 );
    dc.DrawText( _( "Column3" ), 205, 5 );
    dc.DrawText( _( "Column4" ), 305, 5 );
}

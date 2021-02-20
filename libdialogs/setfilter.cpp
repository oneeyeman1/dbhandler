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

#include "setfilter.h"

SetFilterDialog::SetFilterDialog(wxWindow *parent) : wxDialog( parent, wxID_ANY, _( "Specify Filter" ) )
{
    m_panel = new wxPanel( this );
    m_filterText = new wxTextCtrl( m_panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_less = new wxButton( m_panel, wxID_ANY, "<" );
    m_lessequal = new wxButton( m_panel, wxID_ANY, "<=" );
    m_greater = new wxButton( m_panel, wxID_ANY, ">" );
    m_greaterequal = new wxButton( m_panel, wxID_ANY, ">=" );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "Functions" ) );
    m_functions = new wxListBox( m_panel, wxID_ANY );
    m_label2 = new wxStaticText( m_panel, wxID_ANY, _( "Columns" ) );
    m_columns = new wxListBox( m_panel, wxID_ANY );
}

void SetFilterDialog::do_layout()
{
    auto *main = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer1 = new wxBoxSizer( wxVERTICAL );
    auto *sizer2 = new wxBoxSizer( wxHORIZONTAL );
    auto *buttons = new wxBoxSizer( wxVERTICAL );
    auto *signsbuttons = new wxFlexGridSizer( 3, 2, 5, 5 );
    auto *bottomsizer = new wxBoxSizer( wxHORIZONTAL );
    auto *funcs = new wxBoxSizer( wxVERTICAL );
    auto *cols = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_filterText, 1, wxEXPAND, 0 );
    buttons->Add( m_ok, 0, wxEXPAND, 0 );
    buttons->Add( 5, 5, 0, wxEXPAND, 0 );
    buttons->Add( m_cancel, 0, wxEXPAND, 0 );
    buttons->Add( 5, 5, 0, wxEXPAND, 0 );
    buttons->Add( m_help, 0, wxEXPAND, 0 );
    sizer2->Add( buttons, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    signsbuttons->Add( m_less, 0, wxEXPAND, 0 );
    signsbuttons->Add( m_lessequal, 0, wxEXPAND, 0 );
    signsbuttons->Add( m_greater, 0, wxEXPAND, 0 );
    signsbuttons->Add( m_greaterequal, 0, wxEXPAND, 0 );
    signsbuttons->Add( m_leftbrace, 0, wxEXPAND, 0 );
    signsbuttons->Add( m_rightbrace, 0, wxEXPAND, 0 );
    bottomsizer->Add( signsbuttons, 0, wxEXPAND, 0 );
    sizer1->Add( bottomsizer, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    funcs->Add( m_label1, 0, wxEXPAND, 0 );
    funcs->Add( 5, 5, 0, wxEXPAND, 0 );
    funcs->Add( m_functions, 0, wxEXPAND, 0 );
    sizer1->Add( funcs, 0, wxEXPAND, 0 );
    cols->Add( m_label2, 0, wxEXPAND, 0 );
    cols->Add( 5, 5, 0, wxEXPAND, 0 );
    cols->Add( m_columns, 0, wxEXPAND, 0 );
    sizer1->Add( cols, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
    Layout();
}

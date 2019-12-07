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

#include <list>
#include "arguments.c"
#include "typecombobox.h"
#include "retrievalarguments.h"

RetrievalArguments::RetrievalArguments(wxWindow *parent, std::vector<QueryArguments> &arguments, const wxString &dbType, const wxString &subType) : wxDialog( parent, wxID_ANY, _( "" ) )
{
    numArgs = 1;
    m_currentLine = 0;
    m_type = dbType;
    m_subType = subType;
    sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_6 = new wxBoxSizer( wxVERTICAL );
    m_panel = new wxPanel( this, wxID_ANY );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_add = new wxButton( m_panel, wxID_ANY, _( "Add" ) );
    m_insert = new wxButton( m_panel, wxID_ANY, _( "Insert" ) );
    m_remove = new wxButton( m_panel, wxID_ANY, _( "Delete" ) );
    wxStaticBoxSizer *main_sizer = new wxStaticBoxSizer( wxVERTICAL, m_panel, _( "Arguments" ) );

    argPanel = new wxPanel( main_sizer->GetStaticBox(), wxID_ANY );
    m_labe11 = new wxStaticText( argPanel, wxID_ANY, "Position", wxPoint( 4,4)  );
    m_label2 = new wxStaticText( argPanel, wxID_ANY, "Name", wxPoint( 40,4 ) );
    m_label3 = new wxStaticText( argPanel, wxID_ANY, "Type", wxPoint( 48,4 ) );

    main_sizer->Add( argPanel, 0, wxEXPAND | wxBOTTOM, 4 );

    args = new wxScrolledWindow( main_sizer->GetStaticBox(), wxID_ANY );

    bmp = wxBitmap::NewFromPNGData( arguments_pointer_png, WXSIZEOF( arguments_pointer_png ) );

    fgs = new wxFlexGridSizer( 4, 0, 0 );
    dummy_1 = new wxPanel( args, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_2 = new wxPanel( args, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_3 = new wxPanel( args, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_4 = new wxPanel( args, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    fgs->Add( dummy_1 );
    fgs->Add( dummy_2 );
    fgs->Add( dummy_3 );
    fgs->Add( dummy_4 );

    wxString pos;
    for( std::vector<QueryArguments>::iterator it = arguments.begin(); it < arguments.end(); ++it )
    {
        pos.Printf("%d", (*it).m_pos );
        wxStaticBitmap *statBmp = new wxStaticBitmap( args, wxID_ANY, bmp );
        wxStaticText *number = new wxStaticText( args, wxID_ANY, pos, wxDefaultPosition, wxSize( 30, -1 ), wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN );
        wxTextCtrl *name = new wxTextCtrl( args, wxID_ANY, (*it).m_name );
        name->Bind( wxEVT_KEY_DOWN, &RetrievalArguments::OnKeyDown, this );
        TypeComboBox *type = new TypeComboBox( args, dbType.ToStdWstring(), subType.ToStdWstring(), (*it).m_type.ToStdString() );
        fgs->Add( statBmp, 0, wxEXPAND | wxRIGHT | wxLEFT, 8 );
        fgs->Add( number, 0, wxRIGHT, 8 );
        fgs->Add( name, 1, wxEXPAND | wxRIGHT, 8 );
        fgs->Add( type, 0, wxEXPAND | wxRIGHT, 8 );
        m_lines.push_back( QueryLines( statBmp, number, name, type ) );
        m_lines.back().m_name->SetFocus();
        m_currentLine++;
    }
    numArgs = arguments.size();
    fgs->AddGrowableCol( 2 );

    args->SetSizer( fgs );
    args->SetScrollRate( 15, 15 );

    wxSize minsize = fgs->CalcMin();
    minsize.x += wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y );
    minsize.y = -1;
    args->SetMinClientSize( minsize );;

    main_sizer->Add( args, 1, wxEXPAND, 0 );
    sizer->Add( main_sizer, 1, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_ok, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_help, 0, wxEXPAND, 0 );
    sizer_6->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_6->Add( m_add, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_insert, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_remove, 0, wxEXPAND, 0 );
    sizer->Add( sizer_6, 0, wxEXPAND | wxRIGHT, 0 );
    m_panel->SetSizer( sizer );
    sizer->SetSizeHints( this );
    Layout();
    m_panel->Bind( wxEVT_SIZE, &RetrievalArguments::OnSize, this );
    m_add->Bind( wxEVT_BUTTON, &RetrievalArguments::OnAddArgument, this );
    CallAfter( &RetrievalArguments::UpdateHeader );
    set_properties();
}

RetrievalArguments::~RetrievalArguments(void)
{
}

void RetrievalArguments::set_properties()
{
    SetTitle( _( "Specify Retrieval Arguments" ) );
}

void RetrievalArguments::do_layout()
{
    auto *sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer_2 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_3 = new wxBoxSizer( wxHORIZONTAL );
//    auto *sizer_4 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_5 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_7 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_6 = new wxBoxSizer( wxVERTICAL );
//    auto *sizer_8 = new wxStaticBoxSizer( box, wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
//    sizer_3->Add( sizer_8, 1, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( box, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5,  0, wxEXPAND, 0 );
    sizer_6->Add( m_ok, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_help, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    sizer_5->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_7->Add( m_add, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_insert, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_remove, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_7, 0, wxEXPAND, 0 );
    sizer_3->Add( sizer_5, 0, wxEXPAND, 0 );
//    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );*/
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer_2 );
    sizer_1->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
}

void RetrievalArguments::OnSize(wxSizeEvent &event)
{
    UpdateHeader();
    event.Skip();
}

void RetrievalArguments::OnAddArgument(wxCommandEvent &WXUNUSED(event))
{
    std::list<QueryLines>::iterator it = std::next( m_lines.begin(), m_currentLine - 1 );
    (*it).m_pointer->SetBitmap( wxNullBitmap );
    numArgs++;
    wxStaticBitmap *statBmp = new wxStaticBitmap( args, wxID_ANY, bmp );
    wxStaticText *number = new wxStaticText( args, wxID_ANY, wxString::Format( "%d", numArgs ), wxDefaultPosition, wxSize( 30, -1 ), wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN );
    wxTextCtrl *name = new wxTextCtrl( args, wxID_ANY, "" );
    name->Bind( wxEVT_KEY_DOWN, &RetrievalArguments::OnKeyDown, this );
    TypeComboBox *type = new TypeComboBox( args, m_type.ToStdWstring(), m_subType.ToStdWstring(), "" );
    fgs->Add( statBmp, 0, wxEXPAND | wxRIGHT | wxLEFT, 8 );
    fgs->Add( number, 0, wxRIGHT, 8 );
    fgs->Add( name, 1, wxEXPAND | wxRIGHT, 8 );
    fgs->Add( type, 0, wxEXPAND | wxRIGHT, 8 );
    m_lines.push_back( QueryLines( statBmp, number, name, type ) );
    m_lines.back().m_name->SetFocus();
    m_currentLine = numArgs;
    sizer->Layout();
}

void RetrievalArguments::OnRemoveArgument(wxCommandEvent &WXUNUSED(event))
{
    numArgs++;
}

void RetrievalArguments::UpdateHeader()
{
    m_label2->SetPosition( wxPoint( dummy_3->GetPosition().x, -1 ) );
    m_label3->SetPosition( wxPoint( dummy_4->GetPosition().x, -1 ) );
}

void RetrievalArguments::OnKeyDown(wxKeyEvent &event)
{
    if( event.GetKeyCode() == WXK_UP )
    {
        if( m_currentLine > 1 )
        {
            std::list<QueryLines>::iterator it = std::next( m_lines.begin(), m_currentLine - 1 );
            (*it).m_pointer->SetBitmap( wxNullBitmap );
            m_currentLine--;
            it = std::next( m_lines.begin(), m_currentLine - 1 );
            (*it).m_pointer->SetBitmap( bmp );
            (*it).m_name->SetFocus();
        }
        else
            event.Skip();
    }
    else if( event.GetKeyCode () == WXK_DOWN )
    {
        if( m_currentLine < numArgs )
        {
            std::list<QueryLines>::iterator it = std::next( m_lines.begin(), m_currentLine - 1 );
            (*it).m_pointer->SetBitmap( wxNullBitmap );
            m_currentLine++;
            it = std::next( m_lines.begin(), m_currentLine - 1 );
            (*it).m_pointer->SetBitmap( bmp );
            (*it).m_name->SetFocus();
        }
        else
            event.Skip();
    }
    event.Skip();
}

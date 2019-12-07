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
        name->Bind( wxEVT_LEFT_DOWN, &RetrievalArguments::OnMouse, this );
        name->Bind( wxEVT_RIGHT_DOWN, &RetrievalArguments::OnMouse, this );
        TypeComboBox *type = new TypeComboBox( args, dbType.ToStdWstring(), subType.ToStdWstring(), (*it).m_type.ToStdString() );
        type->Bind( wxEVT_LEFT_DOWN, &RetrievalArguments::OnMouse, this );
        type->Bind( wxEVT_RIGHT_DOWN, &RetrievalArguments::OnMouse, this );
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

void RetrievalArguments::OnSize(wxSizeEvent &event)
{
    UpdateHeader();
    event.Skip();
}

void RetrievalArguments::OnAddArgument(wxCommandEvent &WXUNUSED(event))
{
    Freeze();
    std::list<QueryLines>::iterator it = std::next( m_lines.begin(), m_currentLine - 1 );
    (*it).m_pointer->SetBitmap( wxNullBitmap );
    numArgs++;
    wxStaticBitmap *statBmp = new wxStaticBitmap( args, wxID_ANY, bmp );
    wxStaticText *number = new wxStaticText( args, wxID_ANY, wxString::Format( "%d", numArgs ), wxDefaultPosition, wxSize( 30, -1 ), wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN );
    wxTextCtrl *name = new wxTextCtrl( args, wxID_ANY, "" );
    name->Bind( wxEVT_KEY_DOWN, &RetrievalArguments::OnKeyDown, this );
    name->Bind( wxEVT_LEFT_DOWN, &RetrievalArguments::OnMouse, this );
    name->Bind( wxEVT_RIGHT_DOWN, &RetrievalArguments::OnMouse, this );
    TypeComboBox *type = new TypeComboBox( args, m_type.ToStdWstring(), m_subType.ToStdWstring(), "" );
    type->Bind( wxEVT_LEFT_DOWN, &RetrievalArguments::OnMouse, this );
    type->Bind( wxEVT_RIGHT_DOWN, &RetrievalArguments::OnMouse, this );
    fgs->Add( statBmp, 0, wxEXPAND | wxRIGHT | wxLEFT, 8 );
    fgs->Add( number, 0, wxRIGHT, 8 );
    fgs->Add( name, 1, wxEXPAND | wxRIGHT, 8 );
    fgs->Add( type, 0, wxEXPAND | wxRIGHT, 8 );
    m_lines.push_back( QueryLines( statBmp, number, name, type ) );
    m_lines.back().m_name->SetFocus();
    m_currentLine = numArgs;
    sizer->Layout();
    Thaw();
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
    Freeze();
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
    Thaw();
}

void RetrievalArguments::OnMouse(wxMouseEvent &event)
{
    Freeze();
    wxTextCtrl *name = nullptr;
    TypeComboBox *type = nullptr;
    bool found = false;
    name = dynamic_cast<wxTextCtrl *>( event.GetEventObject() );
    type = dynamic_cast<TypeComboBox *>( event.GetEventObject() );
    std::list<QueryLines>::iterator it;
    for( it = m_lines.begin(); it != m_lines.end() && !found; ++it )
    {
        if( name && (*it).m_name == name )
            found = true;
        else if( type && (*it).m_type == type )
            found = true;
    }
    if( found )
        --it;
    int newRow = wxAtoi( (*it).m_number->GetLabel() ) - 1;
    if( newRow != m_currentLine )
    {
        std::list<QueryLines>::iterator oldit = std::next( m_lines.begin(), m_currentLine - 1 );
        (*oldit).m_pointer->SetBitmap( wxNullBitmap );
        (*it).m_pointer->SetBitmap( bmp );
        m_currentLine = wxAtoi( (*it).m_number->GetLabel() );
    }
    event.Skip();
    Thaw();
}

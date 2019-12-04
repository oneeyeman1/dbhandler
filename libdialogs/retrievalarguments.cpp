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

#include <vector>
#include "arguments.c"
#include "typecombobox.h"
#include "retrievalarguments.h"

RetrievalArguments::RetrievalArguments(wxWindow *parent, std::vector<QueryArguments> &arguments, const wxString &dbType, const wxString &subType) : wxDialog( parent, wxID_ANY, _( "" ) )
{
/*    m_panel = new wxPanel( this );
    box = new wxStaticBoxSizer( wxHORIZONTAL, m_panel, _( "Arguments" ) );
    argPanel = new wxPanel( box->GetStaticBox() );
    m_labe11 = new wxStaticText( argPanel, wxID_ANY, _( "Position" ) );
    m_label2 = new wxStaticText( argPanel, wxID_ANY, _( "Name" ) );
    m_label3 = new wxStaticText( argPanel, wxID_ANY, _( "Type" ) );
    wxScrolledWindow *args = new wxScrolledWindow( argPanel, wxID_ANY );
    wxFlexGridSizer *flex = new wxFlexGridSizer( 4, 0, 0 );
    wxBoxSizer *argSizer1 = new wxBoxSizer( wxVERTICAL);
    wxBoxSizer *argSizer2 = new wxBoxSizer( wxHORIZONTAL );
    argSizer2->Add( m_labe11, 0, wxEXPAND, 0 );
    argSizer2->Add( m_label2, 0, wxEXPAND, 0 );
    argSizer2->Add( m_label3, 0, wxEXPAND, 0 );
    argSizer1->Add( argSizer2, 0, wxEXPAND, 0 );
    if( arguments.size() == 0 )
    {
        flex->Add( new wxStaticBitmap( args, wxID_ANY, wxBitmap::NewFromPNGData( arguments_pointer_png, WXSIZEOF( arguments_pointer_png ) ) ), 0, wxEXPAND, 0 );
        flex->Add( new wxTextCtrl( args, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxTE_READONLY ), 0, wxEXPAND, 0 );
        flex->Add( new wxTextCtrl( args, wxID_ANY, wxEmptyString ), 1, wxEXPAND, 0 );
        flex->Add( new TypeComboBox( args, dbType.ToStdWstring(), subType.ToStdWstring(), "" ), 0, wxEXPAND, 0 );
    }
    else
    {
        for( std::vector<QueryArguments>::iterator it = arguments.begin (); it < arguments.end (); ++it )
        {
            int i = 1;
            flex->Add( new wxStaticBitmap( args, wxID_ANY, wxBitmap::NewFromPNGData( arguments_pointer_png, WXSIZEOF( arguments_pointer_png ) ) ), 0, wxEXPAND, 0 );
            flex->Add( new wxTextCtrl( args, wxID_ANY, wxString::Format( "%d", i ), wxDefaultPosition, wxDefaultSize, wxTE_READONLY ), 0, wxEXPAND, 0 );
            flex->Add( new wxTextCtrl( args, wxID_ANY, (*it).m_name ), 1, wxEXPAND, 0 );
            flex->Add( new TypeComboBox( args, dbType.ToStdWstring(), subType.ToStdWstring(), (*it).m_type.ToStdString() ), 0, wxEXPAND, 0 );
            ++i;
        }
    }
    flex->AddGrowableCol( 2 );
    args->SetSizer( flex );
    args->SetScrollRate( 15, 15 );
    argSizer1->Add( flex, 0, wxEXPAND, 0 );
    argPanel->SetSizer( argSizer1 );
    box->Add( argPanel, 0, wxEXPAND, 0 );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_add = new wxButton( m_panel, wxID_ANY, _( "Add" ) );
    m_insert = new wxButton( m_panel, wxID_ANY, _( "Insert" ) );
    m_remove = new wxButton( m_panel, wxID_ANY, _( "Delete" ) );
    set_properties();
    do_layout();
    m_add->Bind( wxEVT_BUTTON, &RetrievalArguments::OnAddArgument, this );
    m_remove->Bind( wxEVT_BUTTON, &RetrievalArguments::OnRemoveArgument, this );*/




/*	m_panel = new wxPanel( this, wxID_ANY );
    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    argPanel = new wxPanel( m_panel, wxID_ANY );
    m_labe11 = new wxStaticText( argPanel, wxID_ANY, "Position", wxPoint( 4,4 ) );
    m_label2 = new wxStaticText( argPanel, wxID_ANY, "Name", wxPoint( 40,4 ) );
    m_label3 = new wxStaticText( argPanel, wxID_ANY, "Type", wxPoint( 48,4 ) );

    main_sizer->Add( argPanel, 0, wxEXPAND|wxBOTTOM, 4 );

    wxScrolledWindow *args = new wxScrolledWindow( m_panel, wxID_ANY );

    wxBitmap bmp = wxBitmap::NewFromPNGData( arguments_pointer_png, WXSIZEOF( arguments_pointer_png ) );

    wxFlexGridSizer *fgs = new wxFlexGridSizer( 4, 0, 0 );
/*    m_locator_dummy_1 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_locator_dummy_2 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_locator_dummy_3 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_locator_dummy_4 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    fgs->Add( m_locator_dummy_1 );
    fgs->Add( m_locator_dummy_2 );
    fgs->Add( m_locator_dummy_3 );
    fgs->Add( m_locator_dummy_4 );

    wxString pos;
    if( arguments.size() == 0 )
    {
//    for(int i=0; i<50; i++ )
        int i = 1;
        pos.Printf("%d", i);
        fgs->Add( new wxStaticBitmap( args, wxID_ANY, bmp ), 0, wxEXPAND|wxRIGHT|wxLEFT, 8 );
        fgs->Add( new wxTextCtrl( args, wxID_ANY, pos, wxDefaultPosition, wxDefaultSize ), 0, wxEXPAND|wxRIGHT, 8 ); 
        fgs->Add( new wxTextCtrl( args, wxID_ANY, "" ), 1, wxEXPAND|wxRIGHT, 8 );
        fgs->Add( new TypeComboBox( args, dbType.ToStdWstring(), subType.ToStdWstring(), "TEXT" ), 1, wxEXPAND|wxRIGHT, 8 ); 
    }
    fgs->AddGrowableCol(2);

    args->SetSizer(fgs);
    args->SetScrollRate( 15, 15 );

    main_sizer->Add(args, 1, wxEXPAND, 0);

    m_panel->SetSizer( main_sizer );
*/
    m_panel = new wxPanel( this, wxID_ANY );
    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    argPanel = new wxPanel( m_panel, wxID_ANY );
    m_labe11 = new wxStaticText( argPanel, wxID_ANY, "Position", wxPoint( 4,4)  );
    m_label2 = new wxStaticText( argPanel, wxID_ANY, "Name", wxPoint( 40,4 ) );
    m_label3 = new wxStaticText( argPanel, wxID_ANY, "Type", wxPoint( 48,4 ) );

    main_sizer->Add( argPanel, 0, wxEXPAND | wxBOTTOM, 4 );

    wxScrolledWindow *args = new wxScrolledWindow( m_panel, wxID_ANY );

    wxBitmap bmp = wxBitmap::NewFromPNGData( arguments_pointer_png, WXSIZEOF( arguments_pointer_png ) );

    wxFlexGridSizer *fgs = new wxFlexGridSizer(4, 0, 0);
/*    m_locator_dummy_1 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_locator_dummy_2 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_locator_dummy_3 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    m_locator_dummy_4 = new wxPanel(scrolled, wxID_ANY, wxDefaultPosition, wxSize(1,1));
    fgs->Add( m_locator_dummy_1 );
    fgs->Add( m_locator_dummy_2 );
    fgs->Add( m_locator_dummy_3 );
    fgs->Add( m_locator_dummy_4 );
*/
    wxString pos;
//    if( arguments.size() == 0 )
    {
        int i = 1;
        pos.Printf("%d", i );
        fgs->Add( new wxStaticBitmap( args, wxID_ANY, bmp ), 0, wxEXPAND | wxRIGHT | wxLEFT, 8 );
        fgs->Add( new wxTextCtrl( args, wxID_ANY, pos, wxDefaultPosition, wxDefaultSize, wxTE_READONLY ), 0, wxRIGHT, 8 ); 
        fgs->Add( new wxTextCtrl( args, wxID_ANY, "ddd" ), 1, wxEXPAND | wxRIGHT, 8 );
        fgs->Add( new TypeComboBox( args, dbType.ToStdWstring(), subType.ToStdWstring(), "TEXT" ), 1, wxEXPAND | wxRIGHT, 8 ); 
    }
    fgs->AddGrowableCol( 2 );

    args->SetSizer( fgs );
    args->SetScrollRate( 15, 15 );

    main_sizer->Add( args, 1, wxEXPAND, 0 );

    m_panel->SetSizer( main_sizer );
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

void RetrievalArguments::OnAddArgument(wxCommandEvent &WXUNUSED(event))
{
//    m_arguments->AddArgument();
}

void RetrievalArguments::OnRemoveArgument(wxCommandEvent &WXUNUSED(event))
{
//    m_arguments->DeleteArgument();
}

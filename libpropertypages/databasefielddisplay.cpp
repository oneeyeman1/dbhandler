// -*- C++ -*-
//
// generated by wxGlade 1.0.4 on Mon Jan 15 21:11:42 2024
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/intl.h>
#include "propertypagebase.h"
#include "databasefielddisplay.h"

// begin wxGlade: ::extracode
// end wxGlade



DatabaseFieldDisplay::DatabaseFieldDisplay(wxWindow* parent, wxWindowID id):  PropertyPageBase( parent, id )
{
    // begin wxGlade: DatabaseFieldDisplay::DatabaseFieldDisplay
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxVERTICAL );
    sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer( 3, 3, 5, 5 );
    sizer_3->Add( grid_sizer_1, 1, wxEXPAND, 0 );
    wxStaticText* label_1 = new wxStaticText( this, wxID_ANY, _( "Display Format:" ) );
    grid_sizer_1->Add( label_1, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_formats_choices[] = {
        _( "choice 1" ),
    };
    m_formats = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 1, m_formats_choices, wxLB_ALWAYS_SB | wxLB_SINGLE );
    grid_sizer_1->Add( m_formats, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_4 = new wxBoxSizer( wxVERTICAL );
    grid_sizer_1->Add( sizer_4, 0, wxEXPAND, 0 );
    m_edit = new wxButton( this, wxID_ANY, _( "Edit" ) );
    sizer_4->Add( m_edit, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_new = new wxButton( this, wxID_ANY, _( "New" ) );
    sizer_4->Add( m_new, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    wxStaticBoxSizer* sizer_5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
    sizer_3->Add( sizer_5, 0, wxEXPAND, 0 );
    wxFlexGridSizer* grid_sizer_2 = new wxFlexGridSizer( 3, 3, 5, 5 );
    sizer_5->Add( grid_sizer_2, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_6 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_6, 0, wxEXPAND, 0 );
    wxStaticText* label_2 = new wxStaticText( this, wxID_ANY, _( "Justify::" ) );
    sizer_6->Add( label_2, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_justify_choices[] = {
        "left",
        "rght",
    };
    m_justify = new wxComboBox( this, wxID_ANY, wxT( "" ), wxDefaultPosition, wxDefaultSize, 2, m_justify_choices, wxCB_DROPDOWN );
    sizer_6->Add( m_justify, 0, wxEXPAND, 0 );
    grid_sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_7, 0, wxEXPAND, 0 );
    wxStaticText* label_3 = new wxStaticText( this, wxID_ANY, _( "Display Height:" ) );
    sizer_7->Add( label_3, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_height = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_7->Add( m_height, 0, wxEXPAND, 0 );
    grid_sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_8, 0, wxEXPAND, 0 );
    wxStaticText* label_4 = new wxStaticText( this, wxID_ANY, _( "Display Width:" ) );
    sizer_8->Add( label_4, 0, 0, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    m_width = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_8->Add( m_width, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    // end wxGlade
    m_justify->Bind( wxEVT_COMBOBOX, &DatabaseFieldDisplay::OnPageModified, this );
    m_height->Bind( wxEVT_COMBOBOX, &DatabaseFieldDisplay::OnPageModified, this );
    m_width->Bind( wxEVT_COMBOBOX, &DatabaseFieldDisplay::OnPageModified, this );
}

void DatabaseFieldDisplay::OnPageModified( wxCommandEvent &WXUNUSED(event ))
{
    PageEdited();
}
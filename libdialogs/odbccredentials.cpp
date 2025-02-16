// -*- C++ -*-
//
// generated by wxGlade 0.7.2 (standalone edition) on Sun Jul 08 00:47:10 2018
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
#include <wx/wx.h>
#include "odbccredentials.h"

ODBCCredentials::ODBCCredentials(wxWindow *parent, wxWindowID id, const wxString& title, const wxString &dsn, const wxString &userID, const wxString &password):
    wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxCAPTION)
{
    m_dsn = dsn;
    // begin wxGlade: ODBCCredentials::ODBCCredentials
    m_panel = new wxPanel( this, wxID_ANY );
    label_1 = new wxStaticText( m_panel, wxID_ANY, _( "User ID" ) );
    m_userID = new wxTextCtrl( m_panel, wxID_ANY, userID );
    label_2 = new wxStaticText( m_panel, wxID_ANY, _( "Password" ) );
    m_password = new wxTextCtrl( m_panel, wxID_ANY, password, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    set_properties();
    do_layout();
    // end wxGlade
}

void ODBCCredentials::set_properties()
{
    // begin wxGlade: ODBCCredentials::set_properties
    SetTitle( _( "Connecting to " ) + m_dsn );
    // end wxGlade
}


void ODBCCredentials::do_layout()
{
    // begin wxGlade: ODBCCredentials::do_layout
    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    wxStdDialogButtonSizer *m_stdbuttonsizer = new wxStdDialogButtonSizer();
    m_stdbuttonsizer->AddButton( new wxButton( m_panel, wxID_OK, _( "OK" ) ) );
    m_stdbuttonsizer->AddButton( new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) ) );
    m_stdbuttonsizer->Realize();
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer( 2, 2, 5, 5 );
    grid_sizer_1->AddGrowableCol( 1, 1 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    grid_sizer_1->Add( label_1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 0 );
    grid_sizer_1->Add( m_userID, 1, wxEXPAND, 0 );
    grid_sizer_1->Add( label_2, 0, 0, 0 );
    grid_sizer_1->Add( m_password, 1, wxEXPAND, 0 );
    sizer_3->Add( grid_sizer_1, 1, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( m_stdbuttonsizer, 1, wxALIGN_RIGHT, 0 );
    sizer_2->Add( sizer_3, 1, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_1->Add( sizer_2, 1, 0, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer_1 );
    sizer->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
    Layout();
    wxSize size = GetMinSize();
    SetMinSize( wxSize( size.GetWidth() * 3, size.GetHeight() ) );
    // end wxGlade
}

const wxTextCtrl &ODBCCredentials::GetUserIDControl()
{
    return *m_userID;
}

const wxTextCtrl &ODBCCredentials::GetPasswordControl()
{
    return *m_password;
}

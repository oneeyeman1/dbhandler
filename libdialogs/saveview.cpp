// -*- C++ -*-
//
// generated by wxGlade 1.0.4 on Wed Apr 26 20:52:38 2023
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
#include <wx/wx.h>
#include "saveview.h"

SaveQuery::SaveQuery(wxWindow* parent) : wxDialog(parent, wxID_ANY, "")
{
    SetTitle( "Save View As" );
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    panel_1 = new wxPanel( this, wxID_ANY );
    sizer_1->Add( panel_1, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_4 = new wxBoxSizer( wxHORIZONTAL );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_6 = new wxBoxSizer( wxHORIZONTAL );
    sizer_4->Add( sizer_6, 0, wxALIGN_CENTER_VERTICAL, 0 );
    wxStaticText* label_1 = new wxStaticText( panel_1, wxID_ANY, _( "Name:" ) );
    sizer_6->Add( label_1, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    text_ctrl_1 = new wxTextCtrl( panel_1, wxID_ANY, wxEmptyString );
    sizer_6->Add( text_ctrl_1, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 20, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_5 = new wxBoxSizer( wxVERTICAL );
    sizer_4->Add( sizer_5, 1, wxEXPAND, 0 );
    button_1 = new wxButton( panel_1, wxID_OK, _( "Save" ) );
    button_1->Enable( 0 );
    button_1->SetDefault();
    sizer_5->Add( button_1, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    button_2 = new wxButton( panel_1, wxID_ANY, _( "Log Only" ) );
    button_2->Enable( 0 );
    sizer_5->Add( button_2, 0, 0, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    button_3 = new wxButton( panel_1, wxID_CANCEL, _( "Cancel" ) );
    sizer_5->Add( button_3, 0, 0, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    button_4 = new wxButton( panel_1, wxID_HELP, _( "Help" ) );
    sizer_5->Add( button_4, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    
    panel_1->SetSizer( sizer_2 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
    Centre();
    button_1->Bind( wxEVT_UPDATE_UI, &SaveQuery::OnButtonUpdateUI, this );
    button_2->Bind( wxEVT_UPDATE_UI, &SaveQuery::OnButtonUpdateUI, this );
}

void SaveQuery::OnButtonUpdateUI(wxUpdateUIEvent &event)
{
    if( text_ctrl_1->GetValue().size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}
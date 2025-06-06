// -*- C++ -*-
//
// generated by wxGlade 1.0.0a7 on Sun Sep 20 12:20:14 2020
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "propertypagebase.h"
#include "printspec.h"

PrintSpec::PrintSpec(wxWindow* parent) : PropertyPageBase(parent, wxID_ANY)
{
    m_isModified = false;
    // begin wxGlade: PrintSpec::PrintSpec
    auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_2 = new wxBoxSizer( wxVERTICAL );
    sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_3 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    auto m_label1 = new wxStaticText( this, wxID_ANY, _( "&Document Name" ) );
    sizer_3->Add( m_label1, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_name = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_3->Add( m_name, 1, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _( "Margin in Units" ) ), wxHORIZONTAL );
    sizer_2->Add( sizer_4, 0, wxEXPAND, 0 );
    auto grid_sizer_1 = new wxFlexGridSizer( 2, 2, 10, 10 );
    sizer_4->Add( grid_sizer_1, 0, wxEXPAND, 0 );
    auto sizer_5 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_5, 0, wxEXPAND, 0 );
    auto m_label2 = new wxStaticText( this, wxID_ANY, _( "&Left" ) );
    sizer_5->Add( m_label2, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    m_leftAlign = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_5->Add( m_leftAlign, 0, wxEXPAND, 0 );
    auto sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_7, 0, wxEXPAND, 0 );
    auto m_label3 = new wxStaticText( this, wxID_ANY, _( "&Right" ) );
    sizer_7->Add( m_label3, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_alignRight = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_7->Add( m_alignRight, 0, wxEXPAND, 0 );
    auto sizer_6 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_6, 0, wxEXPAND, 0 );
    auto m_label4 = new wxStaticText( this, wxID_ANY, _( "&Top" ) );
    sizer_6->Add( m_label4, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_topAlign = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_6->Add( m_topAlign, 0, wxEXPAND, 0 );
    auto sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_8, 1, wxEXPAND, 0 );
    auto m_label5 = new wxStaticText( this, wxID_ANY, _( "&Bottom" ) );
    sizer_8->Add( m_label5, 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    text_ctrl_1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_8->Add( text_ctrl_1, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_9 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _( "Paper" ) ), wxHORIZONTAL );
    sizer_2->Add( sizer_9, 0, wxEXPAND, 0 );
    auto grid_sizer_2 = new wxFlexGridSizer( 3, 2, 5, 5 );
    sizer_9->Add( grid_sizer_2, 0, wxEXPAND, 0 );
    auto m_label6 = new wxStaticText( this, wxID_ANY, _( "&Orientation" ) );
    grid_sizer_2->Add( m_label6, 0, wxALIGN_RIGHT, 0 );
    text_ctrl_2 = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    grid_sizer_2->Add( text_ctrl_2, 0, wxEXPAND, 0 );
    auto m_label7 = new wxStaticText( this, wxID_ANY, _( "&Size" ) );
    grid_sizer_2->Add( m_label7, 0, wxALIGN_RIGHT, 0 );
    text_ctrl_3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    grid_sizer_2->Add( text_ctrl_3, 0, wxEXPAND, 0 );
    auto m_label8 = new wxStaticText( this, wxID_ANY, _( "So&urce" ) );
    grid_sizer_2->Add( m_label8, 0, wxALIGN_RIGHT, 0 );
    text_ctrl_4 = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    grid_sizer_2->Add( text_ctrl_4, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    checkbox_1 = new wxCheckBox( this, wxID_ANY, _( "&Prompt before printing" ) );
    sizer_2->Add( checkbox_1, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_10 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _( "Newpaper Columns" ) ), wxHORIZONTAL );
    sizer_2->Add( sizer_10, 1, wxEXPAND, 0 );
    auto grid_sizer_3 = new wxFlexGridSizer( 1, 2, 0, 10 );
    sizer_10->Add( grid_sizer_3, 0, wxEXPAND, 0 );
    auto sizer_11 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_3->Add( sizer_11, 0, wxEXPAND, 0 );
    auto m_label9 = new wxStaticText( this, wxID_ANY, _( "A&cross" ) );
    sizer_11->Add( m_label9, 0, wxEXPAND, 0 );
    sizer_11->Add( 5, 5, 0, 0, 0 );
    text_ctrl_5 = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_11->Add( text_ctrl_5, 0, 0, 0 );
    auto sizer_12 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_3->Add( sizer_12, 0, wxEXPAND, 0 );
    auto label10 = new wxStaticText( this, wxID_ANY, _( "&Width" ) );
    sizer_12->Add( label10, 0, wxEXPAND, 0 );
    sizer_12->Add( 5, 5, 0, wxEXPAND, 0 );
    text_ctrl_6 = new wxTextCtrl( this, wxID_ANY, wxEmptyString );
    sizer_12->Add( text_ctrl_6, 0, wxEXPAND, 0 );
    sizer_12->Add( 5, 5, 0, wxEXPAND, 0 );
    auto label11 = new wxStaticText( this, wxID_ANY, _( "(Units)" ) );
    sizer_12->Add( label11, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_13 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _( "Display Buttons" ) ), wxHORIZONTAL );
    sizer_2->Add( sizer_13, 0, wxEXPAND, 0 );
    auto grid_sizer_4 = new wxFlexGridSizer( 1, 2, 0, 5 );
    sizer_13->Add( grid_sizer_4, 0, wxEXPAND, 0 );
    checkbox_2 = new wxCheckBox( this, wxID_ANY, _( "Pri&nt" ) );
    grid_sizer_4->Add( checkbox_2, 0, 0, 0 );
    checkbox_3 = new wxCheckBox( this, wxID_ANY, _( "Print Pre&view" ) );
    grid_sizer_4->Add( checkbox_3, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    
    grid_sizer_2->AddGrowableRow( 0 );
    grid_sizer_2->AddGrowableCol( 1 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    // end wxGlade
}


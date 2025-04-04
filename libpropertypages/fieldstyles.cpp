// -*- C++ -*-
//
// generated by wxGlade 1.1.0 on Sat Nov  9 18:19:35 2024
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
#include <wx/wx.h>
#include <wx/image.h>
#include "wx/dynlib.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "propertypagebase.h"
#include "fieldstyles.h"

typedef int (*NEWEDITSTYLE)(wxWindow *, bool );

// begin wxGlade: ::extracode
// end wxGlade

FieldStyles::FieldStyles(wxWindow* parent) : PropertyPageBase( parent )
{
    // begin wxGlade: MyDialog::MyDialog
    auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_2 = new wxBoxSizer( wxVERTICAL );
    sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto grid_sizer_1 = new wxFlexGridSizer( 2, 2, 5, 5 );
    sizer_2->Add( grid_sizer_1, 1, wxEXPAND, 0 );
    m_label1 = new wxStaticText(this, wxID_ANY, _( "Style Name:" ) );
    grid_sizer_1->Add( m_label1, 0, wxALIGN_CENTER_VERTICAL, 0 );
    grid_sizer_1->Add( 5, 5, 0, 0, 0 );
    m_styles = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE );
    grid_sizer_1->Add( m_styles, 0, 0, 0 );
    auto sizer_3 = new wxBoxSizer(wxVERTICAL);
    grid_sizer_1->Add( sizer_3, 0, wxEXPAND, 0 );
    m_edit = new wxButton( this, wxID_ANY, _( "Edit" ) );
    sizer_3->Add( m_edit, 0, 0, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_new = new wxButton( this, wxID_ANY, _( "New..." ) );
    sizer_3->Add( m_new, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );

    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    // end wxGlade
    m_new->Bind( wxEVT_BUTTON, &FieldStyles::OnNew, this );
}

void FieldStyles::OnNew(wxCommandEvent &WXUNUSED(event))
{
    wxString libName, path;
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    wxFileName fn( wxStandardPaths::Get().GetExecutablePath() );
    path = fn.GetPathWithSep();
    libName = path + "dialogs";
#elif __WXMAC__
    wxFileName fn( wxStandardPaths::Get().GetExecutablePath() );
    fn.RemoveLastDir();
    path = fn.GetPathWithSep() + "Frameworks/";
    libName = path + "liblibdialogs.dylib";
#else
    path = wxStandardPaths::Get().GetInstallPrefix() + "/lib/";
    libName = path + "libdialogs";
#endif
    lib.Load( libName );
    if( lib.IsLoaded() )
    {
        NEWEDITSTYLE func = (NEWEDITSTYLE) lib.GetSymbol( "NewEditStyle" );
        func( nullptr, true );
    }
}

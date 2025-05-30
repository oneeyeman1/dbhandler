// -*- C++ -*-
//
// generated by wxGlade 1.1.0 on Thu Apr 24 01:07:18 2025
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include <cfloat>
#include <wx/wx.h>
#include <wx/image.h>
#include "wx/filepicker.h"
#include "createtablespace.h"

// begin wxGlade: ::extracode
// end wxGlade



CreateTablespace::CreateTablespace(wxWindow* parent):
    wxDialog(parent, wxID_ANY, "")
{
    // begin wxGlade: CreateTablespace::CreateTablespace
    SetTitle( _( "Create Tablespace" ) );
    auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel1 = new wxPanel( this, wxID_ANY );
    sizer_1->Add( m_panel1, 1, wxEXPAND, 0 );
    auto sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_0 = new wxBoxSizer( wxHORIZONTAL );
    sizer_3->Add( sizer_0, 0, wxEXPAND, 0 );
    m_label0 = new wxStaticText( m_panel1, wxID_ANY, _( "Name" ) );
    sizer_0->Add( m_label0, 0, wxEXPAND, 0 );
    sizer_0->Add( 5, 5, 0, wxEXPAND, 0 );
    m_name = new wxTextCtrl( m_panel1, wxID_ANY, "" );
    sizer_0->Add( m_name, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_4 = new wxBoxSizer( wxHORIZONTAL );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    m_label1 = new wxStaticText( m_panel1, wxID_ANY, "OWNER" );
    sizer_4->Add( m_label1, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_owner_choices[] = {
        "CURRENT_USER",
        "CURRENT_ROLE",
        "SESSION_USER"
    };
    m_owner = new wxComboBox( m_panel1, wxID_ANY, "CURRENT_USER", wxDefaultPosition, wxDefaultSize, 3, m_owner_choices, wxCB_DROPDOWN );
    sizer_4->Add( m_owner, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_5 = new wxBoxSizer( wxHORIZONTAL );
    sizer_3->Add( sizer_5, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( m_panel1, wxID_ANY, "LOCATION" );
    sizer_5->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    m_location = new wxDirPickerCtrl( m_panel1, wxID_ANY, wxEmptyString );
    sizer_5->Add( m_location, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_6 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, "WITH" ), wxHORIZONTAL );
    sizer_3->Add( sizer_6, 0, wxEXPAND, 0 );
    auto grid_sizer_1 = new wxFlexGridSizer( 2, 2, 5, 5 );
    sizer_6->Add( grid_sizer_1, 1, wxEXPAND, 0 );
    auto sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_7, 0, wxEXPAND, 0 );
    m_label3 = new wxStaticText( sizer_6->GetStaticBox(), wxID_ANY, "Sequence_Page_Cost" );
    sizer_7->Add( m_label3, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_sequenceCost = new wxSpinCtrlDouble( sizer_6->GetStaticBox(), wxID_ANY, "1.0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP, 0.0, DBL_MAX );
    m_sequenceCost->SetIncrement( 1.0 );
    m_sequenceCost->SetDigits( 1 );
    sizer_7->Add( m_sequenceCost, 0, 0, 0 );
    auto sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_8, 0, wxEXPAND, 0 );
    m_label4 = new wxStaticText( sizer_6->GetStaticBox(), wxID_ANY, "Random Page Cost" );
    sizer_8->Add( m_label4, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    m_randomCost = new wxSpinCtrlDouble( sizer_6->GetStaticBox(), wxID_ANY, "4.0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP, 0.0, DBL_MAX );
    m_randomCost->SetIncrement( 1.0 );
    m_randomCost->SetDigits( 1 );
    sizer_8->Add( m_randomCost, 0, wxEXPAND, 0 );
    auto sizer_9 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_9, 0, wxEXPAND, 0 );
    m_label5 = new wxStaticText( sizer_6->GetStaticBox(), wxID_ANY, "Effective I/O" );
    sizer_9->Add( m_label5, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_effectveIO = new wxSpinCtrl( sizer_6->GetStaticBox(), wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP, 0, 1000 );
    sizer_9->Add( m_effectveIO, 0, wxEXPAND, 0 );
    auto sizer_10 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_1->Add( sizer_10, 1, wxEXPAND, 0 );
    m_label6 = new wxStaticText( sizer_6->GetStaticBox(), wxID_ANY, "Mantenance I/O" );
    sizer_10->Add( m_label6, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_10->Add( 5, 5, 0, wxEXPAND, 0 );
    m_maintenanceIO = new wxSpinCtrl( sizer_6->GetStaticBox(), wxID_ANY, "10", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP, 0, 1000 );
    sizer_10->Add( m_maintenanceIO, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_11 = new wxStdDialogButtonSizer();
    sizer_3->Add( sizer_11, 0, wxEXPAND, 0 );
    m_Ok = new wxButton( m_panel1, wxID_OK, "OK" );
    sizer_11->Add( m_Ok, 0, wxEXPAND, 0 );
    sizer_11->Add( new wxButton( m_panel1, wxID_CANCEL, "Cancel" ), 0, wxEXPAND, 0 );
    sizer_11->Add( new wxButton( m_panel1, wxID_HELP, "Help" ), 0, wxEXPAND, 0 );
    sizer_11->Realize();
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );

    m_panel1->SetSizer( sizer_2 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
    Centre();
    m_Ok->Enable( false );
    // end wxGlade
    m_name->Bind( wxEVT_TEXT, &CreateTablespace::OnNameChanged, this );
    m_Ok->Bind( wxEVT_BUTTON, &CreateTablespace::OnOk, this );
    m_location->Bind( wxEVT_DIRPICKER_CHANGED, &CreateTablespace::OnLocationChanged, this );
}

void CreateTablespace::OnNameChanged(wxCommandEvent &WXUNUSED(event))
{
    if( m_location->GetPath().IsEmpty() || m_name->GetValue().IsEmpty() )
        m_Ok->Enable( false );
    else
        m_Ok->Enable( true );
}

void CreateTablespace::OnOk(wxCommandEvent &WXUNUSED(event))
{
    auto location = m_location->GetDirName();
    auto owner = m_owner->GetValue();
    if( m_name->GetValue().starts_with( "pg_" ) )
    {
        wxMessageBox( _( "Names that starts with \'pg_\' are prohbited." ), _( "Create tablespase" ), wxICON_ERROR );
        return;
    }
    if( !location.DirExists() )
    {
        auto created = location.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL );
        if( !created )
        {
            wxMessageBox( "Error creatng the tablespace location" );
            return;
        }
    }
    else
    {
        m_command += L"CREATE TABLESPACE " + m_name->GetValue() + L"\n";
        m_command += L"OWNER " + owner + L"\n";
        m_command += L"LOCATION \'" + location.GetAbsolutePath() + L"\'\n";
        m_command += L"WITH( seq_page_cost = " + wxString::Format( "%f", m_sequenceCost->GetValue() ) + L", ";
        m_command += L"random_page_cost = " + wxString::Format( "%f", m_randomCost->GetValue() ) + L", ";
        m_command += L"effective_io_concurrency = " + wxString::Format( "%d", m_effectveIO->GetValue() ) + L", ";
        m_command += L"maintenance_io_concurrency = " + wxString::Format( "%d", m_maintenanceIO->GetValue() ) + L", ";
        EndModal( wxID_OK );
    }
}

void CreateTablespace::OnLocationChanged(wxCommandEvent &WXUNUSED(event))
{
    wxMessageBox( _( "Make sure that directory exist and has proper ownership" ), _( "Create Tablespace" ), wxICON_WARNING );
    if( m_location->GetPath().IsEmpty() || m_name->GetValue().IsEmpty() )
        m_Ok->Enable( false );
    else
        m_Ok->Enable( true );
    return;
}

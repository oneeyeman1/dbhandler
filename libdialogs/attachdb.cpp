//
//  attachdb.cpp
//  libdialogs
//
//  Created by Igor Korot on 10/8/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filepicker.h"
#include "database.h"
#include "attachdb.h"

AttachDB::AttachDB(wxWindow *parent, Database *db) : wxDialog( parent, wxID_ANY, "Attach the Database" )
{
    auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new wxPanel( this, wxID_ANY );
    sizer_1->Add( m_panel, 0, wxEXPAND, 0 );
    auto sizer_2 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_3 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_4 = new wxBoxSizer( wxVERTICAL );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    auto sizer_6 = new wxBoxSizer( wxVERTICAL );
    sizer_4->Add( sizer_6, 0, wxEXPAND, 0 );
    auto m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "Attach database" ) );
    sizer_6->Add( m_label1, 0, 0, 0 );
    m_picker = new wxFilePickerCtrl( m_panel, wxID_ANY, _( "" ), _( "Choose the DB file" ), "*.db" );
    sizer_6->Add( m_picker, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    sizer_6->Add( sizer_7, 0, wxEXPAND, 0 );
    auto m_label2 = new wxStaticText( m_panel, wxID_ANY, _( "AS" ) );
    sizer_7->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_schemaName = new wxTextCtrl( m_panel, wxID_ANY, wxEmptyString );
    sizer_7->Add( m_schemaName, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_dbList = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE );
    sizer_6->Add( m_dbList, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );

    // WARNING: Code for instance "sizer_5" of "wxStdDialogButtonSizer" not generated: no suitable writer found
    m_ok = new wxButton( m_panel, wxID_OK, _( "Attach" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _("Help" ) );
    auto buttonSizer = new wxStdDialogButtonSizer();
    buttonSizer->Add( m_ok );
    buttonSizer->Add( m_cancel );
    buttonSizer->Add( m_help );
    buttonSizer->Realize();

    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( buttonSizer, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );

    m_panel->SetSizer( sizer_2 );
    SetSizer( sizer_1 );
    if( db->GetTableVector().m_type == L"SQLite" )
        m_dbList->Hide();
    else
    {
        m_picker->Hide();
        m_label2->Hide();
        m_schemaName->Hide();
    }
    m_db = db;
    sizer_1->Fit( this );
    Layout();
    m_ok->Disable();
    m_ok->SetDefault();
    // end wxGlade
    m_ok->Bind( wxEVT_UPDATE_UI, &AttachDB::OnOUpdate, this );
    m_picker->Bind( wxEVT_FILEPICKER_CHANGED, &AttachDB::OnFileSelected, this );
    FindWindowById( wxID_OK )->Bind( wxEVT_BUTTON, &AttachDB::OnOk, this );
}

void AttachDB::OnFileSelected(wxCommandEvent &event)
{
    wxFileName name = m_picker->GetFileName();
    m_schemaName->SetValue( name.GetName() );
}

void AttachDB::OnOk(wxCommandEvent &event)
{
    wxString schema = m_schemaName->GetValue();
    if( schema == "master" || schema == "temp" )
        return;
    else
    {
        if( m_dbList->GetSelection() == wxNOT_FOUND )
        {
            m_catalog = m_picker->GetPath();
            m_schema = m_schemaName->GetValue();
        }
        else
        {
            m_catalog = m_dbList->GetStringSelection();
            m_schema = L"";
        }
        EndModal( wxID_OK );
    }
}

void AttachDB::OnOUpdate(wxUpdateUIEvent &event)
{
    if( !m_dbList->IsShown() )
    {
        if( m_schemaName->GetValue().Length() > 0 && m_picker->GetPath().Length() > 0 )
            event.Enable( true );
        else
            event.Enable( false );
    }
    else
    {
        if( m_dbList->GetSelection() != wxNOT_FOUND )
            event.Enable( true );
        else
            event.Enable( false );
    }
}

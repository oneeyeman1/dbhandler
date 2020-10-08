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
#include "attachdb.h"

AttachDB::AttachDB(wxWindow *parent) : wxDialog( parent, wxID_ANY, "" )
{
    auto mainSizer = new wxBoxSizer( wxHORIZONTAL );
    auto main = new wxBoxSizer( wxHORIZONTAL );
    auto sizer = new wxBoxSizer( wxVERTICAL );
    m_panel = new wxPanel( this );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "Choose filename for the database to attach" ) );
    m_picker = new wxFilePickerCtrl( m_panel, wxID_ANY );
    m_label2 = new wxStaticText( m_panel, wxID_ANY, _( "AS" ) );
    m_schemaName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    m_schemaName->Enable( false );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( m_label1, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( m_picker, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( m_label2, 0, wxEXPAND, 0 );
    sizer->Add( m_schemaName, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer, 0, wxEXPAND, 0 );
    main->Add( CreateStdDialogButtonSizer( wxOK | wxCANCEL | wxHELP ), 0, wxEXPAND, 0 );
    m_panel->SetSizer( main );
    mainSizer->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizerAndFit( mainSizer );
    FindWindowById( wxID_OK )->Enable( false );
    m_picker->Bind( wxEVT_FILEPICKER_CHANGED, &AttachDB::OnFileSelected, this );
}

void AttachDB::OnFileSelected(wxCommandEvent &event)
{
    wxFileName name = m_picker->GetFileName();
    m_schemaName->Enable( true );
    m_schemaName->SetValue( name.GetName() );
    FindWindowById( wxID_OK )->Enable( true );
}

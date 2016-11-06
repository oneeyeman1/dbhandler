/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <string>
#include "wx/listctrl.h"
#include "database.h"
#include "wxsf/ShapeCanvas.h"
#include "fieldwindow.h"
#include "createindex.h"

// begin wxGlade: ::extracode
// end wxGlade

CreateIndex::CreateIndex(wxWindow* parent, wxWindowID id, const wxString& title, DatabaseTable *table, Database *db):
    wxDialog(parent, id, title)
{
    m_dbTable = table;
    // begin wxGlade: CreateIndex::CreateIndex
    panel_1 = new wxPanel( this, wxID_ANY );
    m_label1 = new wxStaticText( panel_1, wxID_ANY, _( "Table" ) );
    m_tableName = new wxStaticText( panel_1, wxID_ANY, wxEmptyString );
    m_label2 = new wxStaticText( panel_1, wxID_ANY, _( "&Index Name" ) );
    m_indexName = new wxTextCtrl( panel_1, wxID_ANY, wxEmptyString );
    m_unique = new wxRadioButton( panel_1, wxID_ANY, _( "&Unique" ), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_duplicate = new wxRadioButton( panel_1, wxID_ANY, _( "&Duplicate" ) );
    m_ascending = new wxRadioButton( panel_1, wxID_ANY, _( "&Ascending" ), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_descending = new wxRadioButton( panel_1, wxID_ANY, _( "&Descending" ) );
    m_label3 = new wxStaticText( panel_1, wxID_ANY, _( "Index Columns:" ) );
    m_indexColumns = new FieldWindow( panel_1, 1 );
    m_table = new wxListCtrl( panel_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
    m_OK = new wxButton( panel_1, wxID_OK, _( "OK" ) );
    m_logOnly = new wxButton( panel_1, wxID_ANY, _( "&Log Only" ) );
    m_cancel = new wxButton( panel_1, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( panel_1, wxID_HELP, _( "&Help" ) );

    set_properties();
    do_layout();
    // end wxGlade
	m_table->Bind( wxEVT_LIST_ITEM_SELECTED, &CreateIndex::OnFieldSelection, this );
	m_table->Bind( wxEVT_LIST_ITEM_DESELECTED, &CreateIndex::OnFieldsDeselection, this );
	m_OK->Bind( wxEVT_BUTTON, &CreateIndex::OnOkShowLog, this );
	m_logOnly->Bind( wxEVT_BUTTON, &CreateIndex::OnOkShowLog, this );
}

void CreateIndex::set_properties()
{
	std::vector<Field *> fields = m_dbTable->GetFields();
	std::wstring tableName = m_dbTable->GetTableName();
	m_tableName->SetLabel( tableName );
	m_table->AppendColumn( tableName );
    int row = 0;
    for( std::vector<Field *>::iterator it = fields.begin(); it < fields.end(); it++ )
    {
        m_table->InsertItem( row++, (*it)->GetFieldName() );
    }
    // begin wxGlade: CreateIndex::set_properties
    SetTitle( _( "Create Index" ) );
    m_OK->SetDefault();
    // end wxGlade
}

void CreateIndex::do_layout()
{
    // begin wxGlade: CreateIndex::do_layout
    wxBoxSizer *sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_3 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_4 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_5 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_14 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_15 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_13 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_6 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_10 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_12 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_11 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_7 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_9 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_1 = new wxBoxSizer( wxVERTICAL );
    sizer_3->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_4->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_8->Add( m_label1, 0, wxEXPAND, 0 );
    sizer_8->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_8->Add( m_tableName, 0, wxEXPAND, 0 );
    sizer_7->Add( sizer_8, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_9->Add( m_label2, 0, wxEXPAND, 0 );
    sizer_9->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_9->Add( m_indexName, 0, wxEXPAND, 0 );
    sizer_7->Add( sizer_9, 0, wxEXPAND, 0 );
    sizer_6->Add( sizer_7, 0, wxEXPAND, 0 );
    sizer_6->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_11->Add( m_unique, 0, wxEXPAND, 0 );
    sizer_11->Add( m_duplicate, 0, wxEXPAND, 0 );
    sizer_10->Add( sizer_11, 0, wxEXPAND, 0 );
    sizer_10->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_12->Add( m_ascending, 0, wxEXPAND, 0 );
    sizer_12->Add( m_descending, 0, wxEXPAND, 0 );
    sizer_10->Add( sizer_12, 0, wxEXPAND, 0 );
    sizer_6->Add( sizer_10, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    sizer_5->Add( 30, 30, 0, wxEXPAND, 0 );
    sizer_13->Add( m_label3, 0, wxEXPAND, 0 );
    sizer_13->Add( 5, 5, 0, wxEXPAND, 0 );
	sizer_13->Add( m_indexColumns->GetFieldsWindow(), 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_13, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_14->Add( m_table, 0, wxALIGN_BOTTOM, 0 );
    sizer_14->Add( 30, 30, 0, wxEXPAND, 0 );
    sizer_15->Add( m_OK, 0, wxALIGN_TOP | wxALIGN_RIGHT, 0 );
    sizer_15->Add( m_logOnly, 0, wxALIGN_TOP | wxALIGN_RIGHT, 0 );
    sizer_15->Add( m_cancel, 0, wxALIGN_TOP | wxALIGN_RIGHT, 0 );
    sizer_15->Add( m_help, 0, wxALIGN_TOP | wxALIGN_RIGHT, 0 );
    sizer_1->Add( sizer_15, 0, wxALIGN_TOP | wxALIGN_RIGHT, 0 );
    sizer_14->Add( sizer_1, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_14, 0, wxEXPAND, 0 );
    sizer_4->Add( sizer_5, 0, wxEXPAND, 0 );
    sizer_4->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_3->Add( 10, 10, 0, wxEXPAND, 0 );
    panel_1->SetSizer( sizer_3 );
    sizer_2->Add( panel_1, 0, wxEXPAND, 0 );
    SetSizer( sizer_2 );
    sizer_2->Fit( this );
    Layout();
    // end wxGlade
}

CreateIndex::~CreateIndex()
{
    delete m_indexColumns;
    m_indexColumns = NULL;
}

std::wstring &CreateIndex::GetCommand()
{
    return m_command;
}

bool CreateIndex::Verify()
{
    bool success = true;
    if( m_indexName->GetValue().IsEmpty() )
    {
        wxMessageBox( _( "Key name is required" ), _( "Database" ) );
        success = false;
    }
    if( success && m_fields.empty() )
    {
        wxMessageBox( _( "At least one index column is required" ), _( "Database" ) );
        success = false;
    }
    return success;
}

void CreateIndex::OnFieldSelection(wxListEvent &event)
{
    wxString item = event.GetLabel();
    m_indexColumns->AddField( item );
    m_fields.push_back( item.ToStdWstring() );
    m_selectedItems.push_back( event.GetIndex() );
}

void CreateIndex::OnFieldsDeselection(wxListEvent &event)
{
    wxString item = event.GetLabel();
    m_indexColumns->RemoveField( item );
    m_fields.erase( std::remove( m_fields.begin(), m_fields.end(), item ), m_fields.end() );
}

void CreateIndex::OnOkShowLog(wxCommandEvent &event)
{
    std::vector<std::wstring> errorMsg;
    if( Verify() )
    {
        m_db->CreateIndex( m_command, m_unique->GetValue(), m_ascending->GetValue(), m_indexName->GetLabel().ToStdWstring(), m_dbTable->GetTableName(), m_fields, errorMsg );
		EndModal( event.GetId() );
    }
}

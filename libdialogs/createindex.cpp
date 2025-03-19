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
#include <vector>
#include <tuple>
#include "wx/listctrl.h"
#include "wx/grid.h"
#include "wx/spinctrl.h"
#include "database.h"
#include "wxsf/ShapeCanvas.h"
#include "field.h"
#include "fieldwindow.h"
#include "createindex.h"
#include "createindexsqlserver.h"

// begin wxGlade: ::extracode
// end wxGlade

CreateIndex::CreateIndex(wxWindow* parent, wxWindowID id, const wxString& title, DatabaseTable *table, const std::wstring &schemaName, Database *db):
    wxDialog(parent, id, title)
{
    m_schema = schemaName;
    m_dbTable = table;
    m_db = db;
    m_dbType = m_db->GetTableVector().m_type;
    m_dbSubType = m_db->GetTableVector().m_subtype;
    // begin wxGlade: MyDialog::MyDialog
    SetTitle( _( "Create Index" ) );
    wxBoxSizer *sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    panel_1 = new wxPanel( this, wxID_ANY );
    sizer_1->Add( panel_1, 1, wxEXPAND, 0 );
    wxBoxSizer *sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer *sizer_3 = new wxBoxSizer(wxVERTICAL);
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, 0, 0 );
    wxBoxSizer *sizer_4 = new wxBoxSizer( wxVERTICAL );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    wxFlexGridSizer *grid_sizer_1 = new wxFlexGridSizer( 2, 2, 5, 5 );
    sizer_4->Add( grid_sizer_1, 0, wxEXPAND, 0 );
    wxBoxSizer *sizer_5 = new wxBoxSizer( wxVERTICAL );
    grid_sizer_1->Add( sizer_5, 0, wxEXPAND, 0 );
    wxBoxSizer *sizer_6 = new wxBoxSizer( wxHORIZONTAL );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    std::wstring tableName = m_dbTable->GetTableName();
    m_label1 = new wxStaticText( panel_1, wxID_ANY, _( "Table" ) );
    sizer_6->Add( m_label1, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_tableName = new wxStaticText( panel_1, wxID_ANY, tableName );
    sizer_6->Add( m_tableName, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer *sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    sizer_5->Add( sizer_7, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( panel_1, wxID_ANY, _( "Index Name" ) );
    sizer_7->Add( m_label2, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, 0, 0 );
    m_indexName = new wxTextCtrl( panel_1, wxID_ANY, wxEmptyString );
    sizer_7->Add( m_indexName, 1, wxEXPAND, 0 );
    const wxString m_unique_choices[] = {
        _( "Unique" ),
        _( "Duplicate" ),
    };
    m_unique = new wxRadioBox( panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, m_unique_choices, 1, wxRA_SPECIFY_COLS );
    m_unique->SetSelection( 0 );
    grid_sizer_1->Add( m_unique, 0, wxALIGN_RIGHT, 0 );
    grid_sizer_1->Add( 5, 5, 0, 0, 0 );
    const wxString m_direction_choices[] = {
        _( "Ascending" ),
        _( "Descendng" ),
    };
    m_direction = new wxRadioBox( panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, m_direction_choices, 1, wxRA_SPECIFY_COLS );
    m_direction->SetSelection( 0 );
    grid_sizer_1->Add( m_direction, 0, wxALIGN_RIGHT, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_indexColumns = new FieldWindow( panel_1 );
    sizer_4->Add( m_indexColumns, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer *sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    sizer_4->Add( sizer_8, 0, wxEXPAND, 0 );
    m_table = new wxListCtrl( panel_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
    m_table->AppendColumn( tableName );
    std::vector<TableField *> fields = m_dbTable->GetFields();
    int row = 0;
    for( std::vector<TableField *>::iterator it = fields.begin(); it < fields.end(); ++it )
    {
        m_table->InsertItem( row++, (*it)->GetFieldName() );
        m_tableFields.push_back( (*it)->GetFieldName() );
    }
    sizer_8->Add( m_table, 0, wxEXPAND, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->AddStretchSpacer();
    wxBoxSizer *sizer_9 = new wxBoxSizer( wxVERTICAL );
    sizer_8->Add( sizer_9, 0, wxEXPAND, 0 );
    if( m_dbType == L"SQLite" )
    {
        m_label7 = new wxStaticText( panel_1, wxID_ANY, _( "Collate" ) );
        m_label7->Disable();
        const wxString collite_options[] =
        {
            "BINARY",
            "NOCASE",
            "RTRIM"
        };
        m_collite = new wxComboBox( panel_1, wxID_ANY, "BINARY", wxDefaultPosition, wxDefaultSize, 3, collite_options );
        m_collite->Disable();
        auto sizer_31 = new wxBoxSizer( wxHORIZONTAL );
        sizer_31->Add( m_label7, 0, wxEXPAND, 0 );
        sizer_31->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_31->Add( m_collite, 0, wxEXPAND, 0 );
        sizer_31->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_8->Add( sizer_31, 0, wxEXPAND, 0 );
    }
    m_OK = new wxButton( panel_1, wxID_OK, _( "OK" ) );
    m_OK->SetDefault();
    sizer_9->Add( m_OK, 0, wxALIGN_RIGHT, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_Cancel = new wxButton( panel_1, wxID_CANCEL, _( "Cancel" ) );
    sizer_9->Add( m_Cancel, 0, wxALIGN_RIGHT, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_logOnly = new wxButton( panel_1, wxID_ANY, _( "Log Only" ) );
    sizer_9->Add( m_logOnly, 0, wxALIGN_RIGHT, 0 );
    sizer_9->Add( 5, 5, 0, 0, 0 );
    m_advanced = new wxButton( panel_1, wxID_ANY, _( "Advanced..." ) );
    sizer_9->Add( m_advanced, 0, wxALIGN_RIGHT, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_help = new wxButton( panel_1, wxID_HELP, _( "Help" ) );
    sizer_9->Add( m_help, 0, wxALIGN_RIGHT, 0 );
    sizer_4->Add( 0, 0, 0, 0, 0 );
    sizer_4->Add( 0, 0, 0, 0, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );

    panel_1->SetSizer( sizer_2 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
// end wxGlade

    set_properties();
//    do_layout();
    // end wxGlade
    m_OK->Bind( wxEVT_BUTTON, &CreateIndex::OnOkShowLog, this );
    m_logOnly->Bind( wxEVT_BUTTON, &CreateIndex::OnOkShowLog, this );
    m_advanced->Bind( wxEVT_BUTTON, &CreateIndex::OnAdvanced, this );
    m_table->Bind( wxEVT_LIST_ITEM_SELECTED, &CreateIndex::OnFieldSelected, this );
    m_table->Bind( wxEVT_LIST_ITEM_DESELECTED, &CreateIndex::OnFieldDeselected, this );
    m_OK->Bind( wxEVT_UPDATE_UI, &CreateIndex::OnOKUpdateUI, this );
    m_logOnly->Bind( wxEVT_UPDATE_UI, &CreateIndex::OnOKUpdateUI, this );
}

void CreateIndex::set_properties()
{
    // begin wxGlade: CreateIndex::set_properties
    // end wxGlade
//    m_defaultIndex->SetValue( true );
//    m_ascending->SetValue( true );
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        m_nonConcurrently->SetValue( true );
        m_indextypeBtree->SetValue( true );
        m_nullsLast->SetValue( true );
        m_fillfactor->Bind( wxEVT_UPDATE_UI, &CreateIndex::OnFillFactorUpdateUI, this );
        m_fastUpdate->Bind( wxEVT_UPDATE_UI, &CreateIndex::OnFastUpdateUpdateUI, this );
        m_ascending->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnAscending, this );
        m_descending->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnDescending, this );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        m_algorythmDefault->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnAlgorythmLockDefault, this );
        m_lockDefault->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnAlgorythmLockDefault, this );
        m_algorythmInPlace->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnAlgorythm, this );
        m_algorythmCopy->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnAlgorythm, this );
        m_lockDefault->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnAlgorythmLockDefault, this );
        m_lockNone->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnLock, this );
        m_lockShared->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnLock, this );
        m_lockExclusive->Bind( wxEVT_RADIOBUTTON, &CreateIndex::OnLock, this );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        m_padIndex->Bind( wxEVT_CHECKBOX, &CreateIndex::OnPadIndex, this );
        m_fillfactor->Bind( wxEVT_SPINCTRL, &CreateIndex::OnFillFactor, this );
    }*/
}

/*void CreateIndex::do_layout()
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
    wxSizer *sizer_16 = NULL;
    wxBoxSizer *sizer_17 = NULL;
    wxBoxSizer *sizer_18 = NULL;
    wxBoxSizer *sizer_19 = NULL;
    wxBoxSizer *sizer_20 = NULL;
    wxBoxSizer *sizer_21 = NULL;
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        sizer_16 = new wxFlexGridSizer( 4, 3, 5, 5 );
        sizer_17 = new wxBoxSizer( wxHORIZONTAL );
        sizer_18 = new wxBoxSizer( wxHORIZONTAL );
        sizer_19 = new wxBoxSizer( wxHORIZONTAL );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        sizer_16 = new wxBoxSizer( wxHORIZONTAL );
        sizer_17 = new wxBoxSizer( wxVERTICAL );
        sizer_18 = new wxBoxSizer( wxVERTICAL );
        sizer_19 = new wxBoxSizer( wxVERTICAL );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        sizer_16 = new wxBoxSizer( wxHORIZONTAL );
        sizer_17 = new wxBoxSizer( wxVERTICAL );
        sizer_18 = new wxBoxSizer( wxVERTICAL );
        sizer_19 = new wxBoxSizer( wxHORIZONTAL );
        sizer_20 = new wxBoxSizer( wxHORIZONTAL );
        sizer_21 = new wxBoxSizer( wxHORIZONTAL );
    }
    sizer_3->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_4->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_8->Add( m_label1, 0, wxEXPAND, 0 );
    sizer_8->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_8->Add( m_tableName, 0, wxEXPAND, 0 );
    sizer_7->Add( sizer_8, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_9->Add( m_label2, 0, wxEXPAND, 0 );
    sizer_9->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_9->Add( m_indexName, 1, wxEXPAND, 0 );
    sizer_7->Add( sizer_9, 0, wxEXPAND, 0 );
    sizer_6->Add( sizer_7, 0, wxEXPAND, 0 );
    sizer_6->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_11->Add( m_defaultIndex, 0, wxEXPAND, 0 );
    sizer_11->Add( m_unique, 0, wxEXPAND, 0 );
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        sizer_11->Add( m_fullText, 0, wxEXPAND, 0 );
        sizer_11->Add( m_spatial, 0, wxEXPAND, 0 );
    }
    else
        sizer_11->Add( m_duplicate, 0, wxEXPAND, 0 );
    sizer_10->Add( sizer_11, 0, wxRIGHT | wxEXPAND, 0 );
    sizer_10->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_12->Add( m_ascending, 0, wxEXPAND, 0 );
    sizer_12->Add( m_descending, 0, wxEXPAND, 0 );
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        sizer_12->Add( 5, 5, 0, wxEXPAND, 0 );
//        sizer_12->Add( m_nonConcurrently, 0, wxEXPAND, 0 );
        sizer_12->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_12->Add( m_concurrently, 0, wxEXPAND, 0 );
    }
    sizer_10->Add( sizer_12, 0, wxEXPAND, 0 );
    sizer_6->Add( sizer_10, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        sizer_11->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_11->Add( m_label8, 0, wxEXPAND, 0 );
        sizer_11->Add( 5, 5, wxEXPAND, 0 );
        sizer_11->Add( m_where, 0, wxEXPAND, 0 );
        sizer_11->Add( 5, 5, wxEXPAND, 0 );
        sizer_5->Add( sizer_11, 0, wxEXPAND, 0 );
        sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        sizer_16->Add( m_padIndex, 0, wxEXPAND, 0 );
        sizer_17->Add( m_label4, 0, wxEXPAND, 0 );
        sizer_17->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_17->Add( m_fillfactor, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_17, 0, wxEXPAND, 0 );
        sizer_16->Add( m_sortTempDB, 0, wxEXPAND, 0 );
        sizer_16->Add( m_ignoreDupKeys, 0, wxEXPAND, 0 );
        sizer_16->Add( m_statisticsNoRecompute, 0, wxEXPAND, 0 );
        sizer_16->Add( m_statisticsIncremental, 0, wxEXPAND, 0 );
        sizer_16->Add( m_dropExisting, 0, wxEXPAND, 0 );
        sizer_16->Add( m_online, 0, wxEXPAND, 0 );
        sizer_16->Add( m_allowRowLocks, 0, wxEXPAND, 0 );
        sizer_16->Add( m_allowPageLocks, 0, wxEXPAND, 0 );
        sizer_18->Add( m_label5, 0, wxEXPAND, 0 );
        sizer_18->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_18->Add( m_maxDop, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_18, 0, wxEXPAND, 0 );
        sizer_5->Add( sizer_16, 0, wxEXPAND, 0 );
        sizer_19->Add( m_dataCompressionNone, 0, wxEXPAND, 0 );
        sizer_19->Add( m_dataCompressionPage, 0, wxEXPAND, 0 );
        sizer_5->Add( sizer_19, 0, wxEXPAND, 0 );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        sizer_17->Add( m_indextypeBtree, 0, wxEXPAND, 0 );
        sizer_17->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_17->Add( m_indextypeHash, 0, wxEXPAND, 0 );
        sizer_17->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_17->Add( m_indextypeGist, 0, wxEXPAND, 0 );
        sizer_17->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_17->Add( m_indextypeGin, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_17, 0, wxEXPAND, 0 );
        sizer_18->Add( m_nullsFirst, 0, wxEXPAND, 0 );
        sizer_18->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_18->Add( m_nullsLast, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_18, 0, wxEXPAND, 0 );
        sizer_5->Add( sizer_16, 0, wxEXPAND, 0 );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        sizer_17->Add( m_indextypeDefault, 0, wxEXPAND, 0 );
        sizer_17->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_17->Add( m_indextypeBtree, 0, wxEXPAND, 0 );
        sizer_17->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_17->Add( m_indextypeHash, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_17, 0, wxEXPAND, 0 );
        sizer_16->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_18->Add( m_algorythmDefault, 0, wxEXPAND, 0 );
        sizer_18->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_18->Add( m_algorythmInPlace, 0, wxEXPAND, 0 );
        sizer_18->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_18->Add( m_algorythmCopy, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_18, 0, wxEXPAND, 0 );
        sizer_16->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_19->Add( m_lockDefault, 0, wxEXPAND, 0 );
        sizer_19->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_19->Add( m_lockNone, 0, wxEXPAND, 0 );
        sizer_19->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_19->Add( m_lockShared, 0, wxEXPAND, 0 );
        sizer_19->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_19->Add( m_lockExclusive, 0, wxEXPAND, 0 );
        sizer_16->Add( sizer_19, 0, wxEXPAND, 0 );
        sizer_5->Add( sizer_16, 0, wxEXPAND, 0 );
    }
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_13->Add( m_label3, 0, wxEXPAND, 0 );
    sizer_13->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_13->Add( m_indexColumns, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_13, 0, wxEXPAND, 0 );
    sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        sizer_20->Add( m_label4, 0, wxEXPAND, 0 );
        sizer_20->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_20->Add( m_fillfactor, 0, wxEXPAND, 0 );
        sizer_19->Add( sizer_20, 0, wxEXPAND, 0 );
        sizer_19->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_19->Add( m_fastUpdate, 0, wxEXPAND, 0 );
        sizer_5->Add( sizer_19, 0, wxEXPAND, 0 );
        sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_21->Add( m_label6, 0, wxEXPAND, 0 );
        sizer_21->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_21->Add( m_tablespace, 0, wxEXPAND, 0 );
        sizer_5->Add( sizer_21, 0, wxEXPAND, 0 );
        sizer_5->Add( 5, 5, 0, wxEXPAND, 0 );
    }
    sizer_14->Add( m_table, 0, wxALIGN_BOTTOM, 0 );
    sizer_14->Add( 30, 30, 0, wxEXPAND, 0 );
    if( m_dbType == L"SQLite" )
    {
        auto sizer_31 = new wxBoxSizer( wxHORIZONTAL );
        sizer_31->Add( m_label7, 0, wxEXPAND, 0 );
        sizer_31->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_31->Add( m_collite, 0, wxEXPAND, 0 );
        sizer_14->Add( sizer_31, 0, wxEXPAND, 0 );
        sizer_31->Add( 5, 5, 0, wxEXPAND, 0 );
    }
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
*/
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
    std::vector<std::wstring> errors;
    if( m_fields.empty() )
    {
        wxMessageBox( _( "At least one index column is required" ), _( "Database" ), wxOK | wxICON_ERROR );
        success = false;
    }
    if( m_indexName->GetValue().IsEmpty() )
    {
        wxMessageBox( _( "Index name is required" ), _( "Database" ), wxOK | wxICON_ERROR );
        success = false;
    }
    return success;
}

void CreateIndex::OnFieldSelected( wxListEvent &event )
{
    auto label = m_table->GetItemText( event.GetItem() );
    m_indexColumns->AddField( label );
    m_fields.push_back( label.ToStdWstring() );
    if( m_dbType == L"SQLite" )
    {
        m_label7->Enable();
        m_collite->Enable();
    }
}

void CreateIndex::OnFieldDeselected( wxListEvent &event )
{
    auto label = m_table->GetItemText( event.GetItem() );
    m_fields.erase( std::remove( m_fields.begin(), m_fields.end(), label ), m_fields.end() );
    m_indexColumns->RemoveField( label );
    if( m_dbType == L"SQLite" && m_fields.size() == 0 )
    {
        m_label7->Disable();
        m_collite->Disable();
    }
}

void CreateIndex::OnOkShowLog(wxCommandEvent &event)
{
    std::vector<std::wstring> errorMsg;
    if( Verify() )
    {
        GenerateQuery();
        EndModal( event.GetId() );
    }
}

void CreateIndex::GenerateQuery()
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        for( auto i = 0; i < m_where->GetNumberRows(); ++i )
        {
            auto name = m_where->GetCellValue( i, 0 );
            auto sign = m_where->GetCellValue( i, 1 );
            auto value = m_where->GetCellValue( i, 2 );
            auto condition = m_where->GetCellValue( i, 3 );
            if( !name.empty() )
            {
                where += ( name + " " + sign + " " + value );
                if( !condition.empty() )
                    where += condition;
            }
        }
        if( m_padIndex->GetValue() )
        {
            auto fillfactor = m_fillfactor->GetValue();
            with += "PAD_INDEX = ON ";
            with += "FILLFACTOR = ";
            with << fillfactor;
            with += " ";
        }
        if( m_sortTempDB->GetValue() )
            with += "SORT_IN_TEMPDB = ON ";
        if( m_ignoreDupKeys->GetValue() )
            with += "IGNORE_DUP_KEY = ON ";
        if( m_statisticsNoRecompute->GetValue() )
            with += "STATISTICS_NORECOMPUTE = ON ";
        if( m_statisticsIncremental->GetValue() )
            with += "STATISTICS_INCREMENTAL = ON ";
        if( m_dropExisting->GetValue() )
            with += "DROP_EXISTING = ON ";
        if( m_online->GetValue() )
        {
            with += "ONLINE = ON ";
            if( m_resumable->GetValue() )
            {
                with += "RESUMABLE = ON ";
                auto duration = m_maxDuration->GetValue();
                with += "MAX_DURATION = ";
                with << duration;
                with += " ";
            }
        }
        if( !m_allowRowLocks->GetValue() )
            with += "ALLOW_ROW_LOCKS = OFF ";
        if( !m_allowPageLocks->GetValue() )
            with += "ALLOW_PAGE_LOCKS = OFF ";
        if( m_optimize->GetValue() )
            with += "OPTIMIZE_FOR_SEQUENTIAL_KEY = ON ";
        auto maxdop = m_maxDop->GetValue();
        if( maxdop > 0 )
        {
            with += "MAXDOP = ";
            with << maxdop;;
            with += " ";
        }
        auto datacompression = m_dataCompression->GetStringSelection();
        if( datacompression != "OFF" )
        {
            with += "DATA_COMPRESSION = ";
            with += datacompression;
            with += " ";
        }
    }*/
    m_command = L"CREATE ";
    if( m_unique->GetSelection() == 0 )
        m_command += L"UNIQUE ";
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        if( m_options->GetSelection() == 1 )
            m_command += L"UNCLUSTERED ";
        if( m_options->GetSelection() == 0 )
            m_command += L"CLUSTERED ";
    }*/
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        if( m_fullText->GetValue() )
            m_command += L"FULLTEXT ";
        if( m_spatial->GetValue() )
            m_command += L"SPATIAL ";
    }*/
    m_command += L"INDEX ";
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        if( m_concurrently->GetValue() )
            m_command += L"CONCURRENTLY ";
    }*/
    m_command += m_indexName->GetValue() + " ";
    if( ( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" ) ||
          ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        m_command += L"\n\r";
    }
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        if( m_indextypeBtree->GetValue() )
            m_command += L"USING BTREE ";
        if( m_indextypeHash->GetValue() )
            m_command += L"USING HASH ";
    }*/
    m_command += L"ON " + m_dbTable->GetTableName() + L" ";
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        if( m_indextypeBtree->GetValue() )
            m_command += L"USING BTREE ";
        if( m_indextypeHash->GetValue() )
            m_command += L"USING HASH ";
        if( m_indextypeGist->GetValue() )
            m_command += L"USING GIST ";
        if( m_indextypeGin->GetValue() )
            m_command += L"USING GIN ";
    }*/
    m_command += L"(";
    for( std::vector<std::wstring>::iterator it = m_fields.begin(); it < m_fields.end(); it++ )
    {
        m_command += L"\"";
        m_command += (*it);
        m_command += L"\"";
        if( it == m_fields.end() - 1 )
            m_command += L")";
        else
            m_command += L",";
    }
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        if( m_algorythmInPlace->IsEnabled() && m_algorythmInPlace->GetValue() )
            m_command += L" ALGORITHM=INPLACE";
        if( m_algorythmCopy->IsEnabled() && m_algorythmCopy->GetValue() )
            m_command += L" ALGORITHM=COPY";
        if( m_lockNone->IsEnabled() && m_lockNone->GetValue() )
            m_command += L" LOCK=NONE";
        if( m_lockShared->IsEnabled() && m_lockShared->GetValue() )
            m_command += L" LOCK=SHARED";
        if( m_lockExclusive->IsEnabled() && m_lockExclusive->GetValue() )
            m_command += L" LOCK=EXCLUSIVE";
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        if( m_fillfactor->IsEnabled() )
        {
            m_command += L" WITH FILLFACTOR=";
            m_command += wxString::Format( "%d", m_fillfactor->GetValue() );
        }
        if( m_fastUpdate->IsEnabled() )
        {
            if( m_fastUpdate->GetValue() )
                m_command = L" WITH FASTUPDATE=ON";
            else
                m_command = L" WITH FASTUPDATE=OFF";
        }
        wxString temp = m_tablespace->GetValue();
        if( temp != wxEmptyString )
            m_command += wxString::Format( " TABLESPACE %s", temp );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        bool withCheck = false;
        if( m_padIndex->GetValue() )
        {
            withCheck = true;
            m_command += L" WITH PAD_INDEX = ON | FILLFACTOR = ";
            m_command += wxString::Format( "%d", m_fillfactor->GetValue() ).ToStdWstring();
        }
        if( m_sortTempDB->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"SORT_IN_TEMPDB = ON";
        }
        if( m_ignoreDupKeys->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"IGNORE_DUP_KEY = ON";
        }
        if( m_statisticsNoRecompute->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"STATISTICS_NORECOMPUTE = ON";
        }
        if( m_statisticsIncremental->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"STATISTICS_INCREMENTAL = ON";
        }
        if( m_dropExisting->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"DROP_EXISTING = ON";
        }
        if( m_online->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"ONLINE = ON";
        }
        if( !m_allowRowLocks->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"ALLOW_ROW_LOCKS = OFF";
        }
        if( !m_allowPageLocks->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"ALLOW_PAGE_LOCKS = OFF";
        }
        if( m_maxDop->GetValue() > 0 )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"MAXDOP = ";
            m_command += wxString::Format( "%d", m_maxDop->GetValue() ).ToStdWstring();
        }
        if( m_dataCompressionPage->GetValue() )
        {
            if( withCheck )
                m_command += L" | ";
            else
            {
                withCheck = true;
                m_command += L" WITH ";
            }
            m_command += L"DATA_COMPRESSION = PAGE";
        }
    }*/
    m_command += L";";
}

void CreateIndex::OnFillFactorUpdateUI(wxUpdateUIEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        if( m_indextypeBtree->GetValue() || m_indextypeHash->GetValue() || m_indextypeGist->GetValue() )
        {
            event.Enable( true );
            m_label4->Enable( true );
        }
        else
        {
            event.Enable( false );
            m_label4->Enable( false );
        }
    }*/
}

void CreateIndex::OnFastUpdateUpdateUI(wxUpdateUIEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        if( m_indextypeGin->GetValue() )
            event.Enable( true );
        else
            event.Enable( false );
    }*/
}

void CreateIndex::OnAscending(wxCommandEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        m_nullsLast->SetValue( true );
    }*/
}

void CreateIndex::OnDescending(wxCommandEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        m_nullsFirst->SetValue( true );
    }*/
}

void CreateIndex::OnAlgorythmLockDefault(wxCommandEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        m_algorythmDefault->Enable( true );
        m_algorythmInPlace->Enable( true );
        m_algorythmCopy->Enable( true );
        m_lockDefault->Enable( true );
        m_lockNone->Enable( true );
        m_lockShared->Enable( true );
        m_lockExclusive->Enable( true );
    }*/
}

void CreateIndex::OnAlgorythm(wxCommandEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        m_algorythmDefault->Enable( true );
        m_algorythmInPlace->Enable( true );
        m_algorythmCopy->Enable( true );
        m_lockDefault->Enable( false );
        m_lockNone->Enable( false );
        m_lockShared->Enable( false );
        m_lockExclusive->Enable( false );
    }*/
}

void CreateIndex::OnLock(wxCommandEvent &WXUNUSED(event))
{
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        m_algorythmDefault->Enable( false );
        m_algorythmInPlace->Enable( false );
        m_algorythmCopy->Enable( false );
        m_lockDefault->Enable( true );
        m_lockNone->Enable( true );
        m_lockShared->Enable( true );
        m_lockExclusive->Enable( true );
    }*/
}

const wxTextCtrl *CreateIndex::GetIndexNameCtrl()
{
    return m_indexName;
}

void CreateIndex::OnOKUpdateUI(wxUpdateUIEvent &event)
{
    if( m_indexName->GetValue() != wxEmptyString && !m_fields.empty() )
        event.Enable( true );
    else
        event.Enable( false );
}

void CreateIndex::OnAdvanced( wxCommandEvent &WXUNUSED(event ))
{
    CreateIndexSQLServer dlg( nullptr, wxID_ANY, "Advanced options fr SQL Server", m_dbTable );
    dlg.CenterOnScreen();
    dlg.ShowModal();
}
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
#include "wx/propgrid/manager.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/propgridiface.h"
#include "wx/grid.h"
#include "wx/spinctrl.h"
#include "database.h"
#include "wxsf/ShapeCanvas.h"
#include "field.h"
#include "fieldwindow.h"
#include "createindex.h"
#include "createindexsqlserver.h"
#include "createindexpostgres.h"

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
    m_serverVersion = m_db->GetTableVector().m_versionMajor;
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
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        const wxString m_unique_choices[] = {
            _( "Unique" ),
            _( "Fulltext" ),
            _( "Spatial" ),
        };
        m_unique = new wxRadioBox( panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, m_unique_choices, 1, wxRA_SPECIFY_COLS );
        m_unique->SetSelection( 0 );
        grid_sizer_1->Add( m_unique, 0, wxALIGN_RIGHT, 0 );
    }
    else
    {
        const wxString m_unique_choices[] = {
        _( "Unique" ),
        _( "Duplicate" ),
        };
        m_unique = new wxRadioBox( panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, m_unique_choices, 1, wxRA_SPECIFY_COLS );
        m_unique->SetSelection( 0 );
        grid_sizer_1->Add( m_unique, 0, wxALIGN_RIGHT, 0 );
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        wxBoxSizer *pgsizer0 = new wxBoxSizer( wxVERTICAL );
        wxBoxSizer *pgsizer1 = new wxBoxSizer( wxHORIZONTAL );
        m_concurrently = new wxCheckBox( panel_1, wxID_ANY, "CONCURRENTLY" );
        pgsizer1->Add( m_concurrently, 0, wxEXPAND, 0 );
        if( m_serverVersion >= 11 )
        {
            m_only = new wxCheckBox( panel_1, wxID_ANY, "ONLY" );
            pgsizer1->Add( 5, 5, 0, wxEXPAND, 0 );
            pgsizer1->Add( m_only, 0, wxEXPAND, 0 );
        }
        pgsizer0->Add( pgsizer1, 0, wxEXPAND, 0 );
        const wxString method_choices[] =
        {
            "btree",
            "hash",
            "gist",
            "spgist",
            "gin",
            "brin",
        };
        m_method = new wxComboBox( panel_1, wxID_ANY, "btree", wxDefaultPosition, wxDefaultSize, 6, method_choices );
        pgsizer0->Add( m_method, 0, wxEXPAND, 0 );
        grid_sizer_1->Add( pgsizer0, 0, wxEXPAND, 0 );
    }
    else if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        const wxString method_choices[] =
        {
            "btree",
            "hash",
        };
        m_method = new wxComboBox( panel_1, wxID_ANY, "btree", wxDefaultPosition, wxDefaultSize, 6, method_choices );
        grid_sizer_1->Add( m_method, 0, wxEXPAND, 0 );
    }
    else
        grid_sizer_1->Add( 5, 5, 0, 0, 0 );
    const wxString m_direction_choices[] = {
        "Ascending",
        "Descendng",
    };
    m_direction = new wxRadioBox( panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, m_direction_choices, 1, wxRA_SPECIFY_COLS );
    m_direction->SetSelection( 0 );
    grid_sizer_1->Add( m_direction, 0, wxALIGN_RIGHT, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_indexColumns = new FieldWindow( panel_1, true );
    sizer_4->Add( m_indexColumns, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    wxBoxSizer *sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    sizer_4->Add( sizer_8, 0, wxEXPAND, 0 );
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        m_manager = new wxPropertyGridManager( panel_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_NO_INTERNAL_BORDER | wxPG_SPLITTER_AUTO_CENTER );
        m_manager->SetExtraStyle( wxPG_EX_MULTIPLE_SELECTION );
        m_tablePg = m_manager->GetGrid();
        auto page = m_manager->AddPage( tableName );
        std::vector<TableField *> fields = m_dbTable->GetFields();
        for( std::vector<TableField *>::iterator it = fields.begin(); it < fields.end(); ++it )
        {
            auto property = page->Append( new wxStringProperty( (*it)->GetFieldName(), wxPG_LABEL, "<composed>" ) );
            page->AppendIn( property, new wxStringProperty( "Expression", wxPG_LABEL, "" ) );
            page->AppendIn( property, new wxStringProperty( "Collate", wxPG_LABEL, "" ) );
            page->AppendIn( property, new wxStringProperty( "OpClass", wxPG_LABEL, "" ) );
            static const wxString null_choices[] = { "FIRST", "LAST" };
            static long enum_prop_values[] = { 1, 2 };
            page->AppendIn( property, new wxEnumProperty( "NULL", wxPG_LABEL, 
                wxArrayString( WXSIZEOF( null_choices ), null_choices ),
                wxArrayInt( enum_prop_values, enum_prop_values + WXSIZEOF( enum_prop_values ) ), 2 ) );
        }
        sizer_8->Add( m_manager, 1, wxEXPAND, 0 );
    }
    else
    {
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
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType != L"PostgreSQL" ) || m_dbType != L"PostgreSQL" )
    {
        sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer_8->AddStretchSpacer();
    }
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
    wxPGVIterator it;
    auto pg = m_manager->GetGrid();

    for( it = pg->GetVIterator( wxPG_ITERATE_ALL ); !it.AtEnd(); it.Next() )
        it.GetProperty()->SetExpanded( false );

    pg->RefreshGrid();
    // end wxGlade
    m_OK->Bind( wxEVT_BUTTON, &CreateIndex::OnOkShowLog, this );
    m_logOnly->Bind( wxEVT_BUTTON, &CreateIndex::OnOkShowLog, this );
    m_advanced->Bind( wxEVT_BUTTON, &CreateIndex::OnAdvanced, this );
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        m_manager->Bind( wxEVT_PG_SELECTED, &CreateIndex::OnPostgresFieldSelected, this );
    }
    else
    {
        m_table->Bind( wxEVT_LIST_ITEM_SELECTED, &CreateIndex::OnFieldSelected, this );
        m_table->Bind( wxEVT_LIST_ITEM_DESELECTED, &CreateIndex::OnFieldDeselected, this );
    }
    m_OK->Bind( wxEVT_UPDATE_UI, &CreateIndex::OnOKUpdateUI, this );
    m_logOnly->Bind( wxEVT_UPDATE_UI, &CreateIndex::OnOKUpdateUI, this );
    m_direction->Bind( wxEVT_RADIOBOX, &CreateIndex::OnDirection, this );
    m_indexColumns->Bind( wxEVT_LEFT_UP, &CreateIndex::OnIndexFieldsMouseUp, this );
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
    m_command = L"CREATE ";
    if( m_unique->GetSelection() == 0 )
        m_command += L"UNIQUE ";
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        m_command += m_clustered.ToStdWstring() + L" ";
    }
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        if( m_fullText->GetValue() )
            m_command += L"FULLTEXT ";
        if( m_spatial->GetValue() )
            m_command += L"SPATIAL ";
    }*/
    m_command += L"INDEX ";
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        if( m_concurrently->GetValue() )
            m_command += L"CONCURRENTLY ";
    }
    m_command += m_indexName->GetValue() + " ";
    if( ( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" ) ||
          ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        m_command += L"\r";
    }
/*    if( ( m_dbType == L"ODBC" && m_dbSubType == L"MySQL" ) || m_dbType == L"MySQL" )
    {
        if( m_indextypeBtree->GetValue() )
            m_command += L"USING BTREE ";
        if( m_indextypeHash->GetValue() )
            m_command += L"USING HASH ";
    }*/
    m_command += L"ON ";
    if( ( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" ) && m_serverVersion >= 11 && m_only->GetValue() )
    {
        m_command += L"ONLY ";
    }
    m_command += m_dbTable->GetTableName() + L" ";
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        auto method = m_method->GetValue();
        if( method != "btree" )
            m_command += "USING " + method;
    }
    m_command += L"\n";
    m_command += L"(";
    for( std::vector<std::wstring>::iterator it = m_fields.begin(); it < m_fields.end(); it++ )
    {
        m_command += L"\"";
        auto pos = (*it).find( L" " );
        if( pos != std::string::npos )
            (*it).replace( pos, 1, L"\" " );
        else
            (*it) += L"\"";
        m_command += (*it);
        if( it == m_fields.end() - 1 )
            m_command += L")";
        else
            m_command += L",";
    }
    if( ( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" ) ||
        ( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" ) )
    {
        for( std::vector<std::wstring>::iterator it = m_includeFields.begin(); it < m_includeFields.end(); ++it )
        {
            if( it == m_includeFields.begin() )
                m_command += L"\nINCLUDE( ";
            m_command += (*it);
            if( it == m_includeFields.end() - 1 )
                m_command += L")";
            else
                m_command += L",";
        }
        if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" && m_nullValue != wxEmptyString )
            m_command += m_nullValue;
        if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
        {
            if( !m_where.IsEmpty() )
            {
                m_command += "\n" + m_where;
            }
        }
        if( !m_with.IsEmpty() )
        {
            m_command += "\n" + m_with;
        }
        if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" && m_nullValue != wxEmptyString )
        {

        }
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
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
    {
        CreateIndexSQLServer dlg( nullptr, wxID_ANY, "Advanced options fr SQL Server", m_dbTable );
        dlg.CenterOnScreen();
        if( dlg.ShowModal() == wxID_OK )
        {
            m_includeFields = dlg.GetIncludeVector();
            m_where = dlg.GetWhereCondition();
            m_with = dlg.GetWithPredicate();
            m_clustered = dlg.GetClusteredChoce();
        }
    }
    if( ( m_dbType == L"ODBC" && m_dbSubType == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
    {
        std::vector<std::wstring> list, errors;
        if( m_db->GetTablespacesList( list, errors ) )
            return;
        CreateIndexPostgres dlg( nullptr, wxID_ANY, "Advanced options for PostgreSQL", m_method->GetValue(), m_dbTable, m_serverVersion, list );
        dlg.CenterOnScreen();
        if( dlg.ShowModal() == wxID_OK )
        {
            m_nullValue = dlg.GetNullValue();
            m_includeFields = dlg.GetIncludeFields();
            m_with = dlg.GetWithOptions();
        }
    }
}

void CreateIndex::OnDirection(wxCommandEvent &WXUNUSED(event))
{
    auto sel = m_direction->GetSelection();
    wxString direction = "";
    if( sel == 0 )
    {
        direction = "ASC";
    }
    else
    {
        direction = "DESC";
    }
    auto label = m_indexColumns->GetCurrentFieldLabel();
    if( !label.IsEmpty() )
    {
        for( std::vector<std::wstring>::iterator it = m_fields.begin(); it < m_fields.end(); ++it )
        {
            if( (*it) == label )
            {
                if( (*it).find( L' ' ) == std::string::npos )
                    (*it) += L" " + direction;
                else
                    (*it).replace( (*it).find( L" " ), std::string::npos, L" " + direction );
            }
        }
    }
    m_indexColumns->SetIndexDirection( direction );
}

void CreateIndex::OnIndexFieldsMouseUp(wxMouseEvent &WXUNUSED(event))
{
    auto label = m_indexColumns->GetCurrentFieldLabel();
    if( label.find( "DESC" ) != std::string::npos )
        m_direction->SetSelection( 1 );
    else
        m_direction->SetSelection( 0 );
}

void CreateIndex::OnPostgresFieldSelected(wxPropertyGridEvent &event)
{
    auto property = event.GetProperty();
    auto parent = property->GetParent();
    auto grid = m_manager->GetGrid();
    wxString indexField = "";
    auto found = false;
    indexField = property->GetName();
    for( auto it = grid->GetIterator( wxPG_ITERATE_FIXED_CHILDREN, property ); !it.AtEnd() && !found; it.Next() )
    {
        auto name = it.GetProperty()->GetName();
        if( it.GetProperty()->GetName().find( "Expression" ) != std::string::npos )
        {
            if( it.GetProperty()->GetValue().GetString() != wxEmptyString )
            {
                indexField = it.GetProperty()->GetValue().GetString();
                found = true;
            }
        }
    }
    if( parent && parent->IsRoot() )
    {
        if( m_manager->GetPage( 0 )->IsPropertySelected( property ) )
        {
            m_tablePg->AddToSelection( property );
            m_indexColumns->AddField( indexField );
            m_fields.push_back( indexField.ToStdWstring() );
        }
        else
        {
            m_tablePg->RemoveFromSelection( property );
            m_indexColumns->RemoveField( indexField );
            m_fields.erase( std::remove( m_fields.begin(), m_fields.end(), indexField ), m_fields.end() );
        }
    }
}


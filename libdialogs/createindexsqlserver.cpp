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
#include "createindexsqlserver.h"

CreateIndexSQLServer::CreateIndexSQLServer(wxWindow *parent, wxWindowID id, const wxString &title, DatabaseTable *table):
    wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
    // begin wxGlade: CreateIndexSQLServer::CreateIndexSQLServer
    SetTitle( _( "Advanced options for SQL Server " ) );
    auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    panel_1 = new wxPanel( this, wxID_ANY );
    sizer_1->Add( panel_1, 1, wxEXPAND, 0 );
    auto sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_3 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( sizer_3, 1, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_4 = new wxBoxSizer( wxVERTICAL );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    const wxString m_clustered_choices[] = {
        "Clustered",
        "Unclustered",
    };
    m_clustered = new wxRadioBox( panel_1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, m_clustered_choices, 2, wxRA_SPECIFY_COLS );
    m_clustered->SetSelection( 0 );
    sizer_4->Add( m_clustered, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    m_includeFields = new FieldWindow( panel_1 );
    sizer_4->Add( m_includeFields, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_16 = new wxBoxSizer( wxHORIZONTAL );
    sizer_4->Add( sizer_16, 1, wxEXPAND, 0 );
    auto sizer_5 = new wxStaticBoxSizer( new wxStaticBox( panel_1, wxID_ANY, "INCLUDE" ), wxHORIZONTAL );
    sizer_16->Add( sizer_5, 0, wxEXPAND, 0 );
    auto sizer_6 = new wxBoxSizer( wxVERTICAL );
    sizer_5->Add( sizer_6, 1, wxEXPAND, 0 );
    sizer_6->Add( 0, 0, 0, 0, 0 );
    sizer_6->Add( 5, 5, 0, 0, 0 );
    m_table = new wxListCtrl( sizer_5->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
    m_table->AppendColumn( table->GetTableName(), wxLIST_FORMAT_LEFT, -1 );
    std::vector<TableField *> fields = table->GetFields();
    int row = 0;
    for( std::vector<TableField *>::iterator it = fields.begin(); it < fields.end(); ++it )
    {
        m_table->InsertItem( row++, (*it)->GetFieldName() );
        m_tableFields.push_back( (*it)->GetFieldName() );
    }
    sizer_6->Add( m_table, 0, wxEXPAND, 0 );
    sizer_16->Add( 5, 5, 0, 0, 0 );
    wxBoxSizer* sizer_9 = new wxBoxSizer( wxVERTICAL );
    sizer_16->Add( sizer_9, 0, wxEXPAND, 0 );
    wxStaticText* label_3 = new wxStaticText( panel_1, wxID_ANY, "WHERE" );
    sizer_9->Add( label_3, 0, wxEXPAND, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    m_where = new wxGrid( panel_1, wxID_ANY );
    m_where->CreateGrid( 4, 4 );
    m_where->SetRowLabelSize( 0 );
    m_where->EnableDragColSize( false );
    m_where->EnableDragRowSize( false );
    m_where->SetColLabelValue( 0, _( "Name" ) );
    m_where->SetColLabelValue( 1, _( "Sign" ) );
    m_where->SetColLabelValue( 2, _( "Value" ) );
    m_where->SetColLabelValue( 3, _( "Condition" ) );
    wxString m_operatorChoices[] = 
    {
        "is",
        "is not",
        "=",
        "<>",
        "!=",
        ">",
        ">=",
        "!>",
        "<",
        "<=",
        "!<",
        "IN"
    };
    wxString m_logicalChoices[] = { "AND" };
    for( auto i = 0; i < 3; ++i )
    {
        m_where->SetCellEditor( i, 0, new wxGridCellChoiceEditor( 0, NULL, true ) );
        m_where->SetCellEditor( i, 1, new wxGridCellChoiceEditor( 11, m_operatorChoices ) );
        m_where->SetCellEditor( i, 3, new wxGridCellChoiceEditor( 1, m_logicalChoices ) );
    }
    sizer_9->Add( m_where, 1, 0, 0 );
    wxBoxSizer* sizer_7 = new wxBoxSizer( wxHORIZONTAL );
    sizer_4->Add( sizer_7, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( 5, 5, 0, 0, 0 );
    auto sizer_10 = new wxStaticBoxSizer( new wxStaticBox( panel_1, wxID_ANY, "WTH" ), wxHORIZONTAL );
    sizer_4->Add( sizer_10, 0, wxEXPAND, 0 );
    auto grid_sizer_2 = new wxFlexGridSizer( 4, 4, 5, 5 );
    sizer_10->Add( grid_sizer_2, 0, wxEXPAND, 0 );
    m_padIndex = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Pad Index" );
    grid_sizer_2->Add( m_padIndex, 0, wxALIGN_CENTER_VERTICAL, 0 );
    auto sizer_11 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_11, 0, wxEXPAND, 0 );
    auto label_4 = new wxStaticText( sizer_10->GetStaticBox(), wxID_ANY, "Fill Factr" );
    sizer_11->Add( label_4, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_11->Add( 5, 5, 0, wxEXPAND, 0 );
    m_fillFactor = new wxSpinCtrl( sizer_10->GetStaticBox(), wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100 );
    sizer_11->Add( m_fillFactor, 0, 0, 0 );
    m_sortInTemp = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Sort TempDB" );
    grid_sizer_2->Add( m_sortInTemp, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_ignoreDupKey = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Ignore Dup Key" );
    grid_sizer_2->Add( m_ignoreDupKey, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_noRecompute = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "NoRecompute" );
    grid_sizer_2->Add( m_noRecompute, 0, 0, 0 );
    m_incremental = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Incremental" );
    grid_sizer_2->Add( m_incremental, 0, 0, 0 );
    m_dropExisting = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Drop Existing" );
    grid_sizer_2->Add( m_dropExisting, 0, 0, 0 );
    m_online = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Online" );
    grid_sizer_2->Add( m_online, 0, 0, 0 );
    m_resumable = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Resumable" );
    grid_sizer_2->Add( m_resumable, 0, wxALIGN_CENTER_VERTICAL, 0 );
    wxBoxSizer* sizer_12 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_12, 0, wxEXPAND, 0 );
    wxStaticText* label_5 = new wxStaticText( sizer_10->GetStaticBox(), wxID_ANY, "Max Duration" );
    sizer_12->Add( label_5, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_12->Add( 5, 5, 0, 0, 0 );
    m_maxDuration = new wxSpinCtrl( sizer_10->GetStaticBox(), wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60 );
    sizer_12->Add( m_maxDuration, 0, 0, 0 );
    m_rowLocks = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Allow Row Locks" );
    m_rowLocks->SetValue( true );
    grid_sizer_2->Add( m_rowLocks, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_pageLocks = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Allow Page Locks" );
    m_pageLocks->SetValue( true );
    grid_sizer_2->Add( m_pageLocks, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_optimize = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "Optimize for Sequental" );
    grid_sizer_2->Add( m_optimize, 0, wxALIGN_CENTER_VERTICAL, 0 );
    wxBoxSizer* sizer_13 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_13, 0, wxEXPAND, 0 );
    wxStaticText* label_6 = new wxStaticText( sizer_10->GetStaticBox(), wxID_ANY, "Max DOP" );
    sizer_13->Add( label_6, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_13->Add( 5, 5, 0, 0, 0 );
    m_maxDOP = new wxSpinCtrl( sizer_10->GetStaticBox(), wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 64 );
    sizer_13->Add( m_maxDOP, 0, 0, 0 );
    wxBoxSizer* sizer_14 = new wxBoxSizer( wxHORIZONTAL );
    grid_sizer_2->Add( sizer_14, 0, wxEXPAND, 0 );
    wxStaticText* label_7 = new wxStaticText( sizer_10->GetStaticBox(), wxID_ANY, "Data Compressiion" );
    sizer_14->Add( label_7, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_14->Add( 5, 5, 0, 0, 0 );
    const wxString m_dataCompression_choices[] = {
        "OFF",
        "ROW",
        "PAGE",
    };
    m_dataCompression = new wxComboBox( sizer_10->GetStaticBox(), wxID_ANY, "OFF", wxDefaultPosition, wxDefaultSize, 3, m_dataCompression_choices, wxCB_DROPDOWN );
    m_dataCompression->SetSelection( 0 );
    sizer_14->Add( m_dataCompression, 0, 0, 0 );
    checkbox_8 = new wxCheckBox( sizer_10->GetStaticBox(), wxID_ANY, "XML Compression" );
    grid_sizer_2->Add( checkbox_8, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_17 = new wxStaticBoxSizer( new wxStaticBox( panel_1, wxID_ANY, "WAIT AT LOW PRIORITY" ), wxHORIZONTAL );
    sizer_17->Hide( sizer_17, true );
    sizer_4->Add( sizer_17, 0, wxEXPAND, 0 );
    auto sizer_18 = new wxBoxSizer( wxVERTICAL );
    sizer_17->Add( sizer_18 );
    auto sizer_19 = new wxBoxSizer( wxHORIZONTAL );
    sizer_18->Add( sizer_19, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( sizer_17->GetStaticBox(), wxID_ANY, "Max Duration" );
    sizer_19->Add( m_label2, 0, wxEXPAND, 0 );
    m_maxDurationWait = new wxTextCtrl( sizer_17->GetStaticBox(), wxID_ANY, "0" );
    sizer_19->Add( m_maxDurationWait, 0, wxEXPAND, 0 );
    auto sizer_20 = new wxBoxSizer( wxHORIZONTAL );
    sizer_18->Add( sizer_20, 0, wxEXPAND, 0 );
    m_label3 = new wxStaticText( sizer_17->GetStaticBox(), wxID_ANY, "Abort After Wait" );
    sizer_20->Add( m_label3, 0, wxEXPAND, 0 );
    const wxString abort_choices[] =
    {
        "NONE",
        "SELF",
        "BLOCKERS",
    };
    m_abort = new wxComboBox( sizer_17->GetStaticBox(), wxID_ANY, "NONE", wxDefaultPosition, wxDefaultSize, 3, abort_choices );
    sizer_20->Add( m_abort, 0, wxEXPAND, 0 );
    wxBoxSizer* sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    sizer_4->Add( sizer_8, 1, wxEXPAND, 0 );
    m_label1 = new wxStaticText( panel_1, wxID_ANY, "FILESTREAM_ON" );
    sizer_8->Add( m_label1, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_filestreamType_choices[] = {
        "NONE",
        "FileGroup Name",
        "Scheme Name",
    };
    m_filestreamType = new wxComboBox( panel_1, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 3, m_filestreamType_choices, wxCB_DROPDOWN );
    m_filestreamType->SetSelection( 0 );
    sizer_8->Add( m_filestreamType, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    m_name = new wxTextCtrl( panel_1, wxID_ANY, wxEmptyString );
    sizer_8->Add( m_name, 0, wxALIGN_CENTER_VERTICAL, 0 );
    auto sizer = new wxStdDialogButtonSizer();
    m_OK = new wxButton( panel_1, wxID_OK, _( "OK" ) );
    m_OK->SetDefault();
    SetAffirmativeId( wxID_OK ); 
    m_Cancel = new wxButton( panel_1, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( panel_1, wxID_HELP, _( "Help" ) );
    sizer->AddButton( m_OK );
    sizer->AddButton( m_Cancel );
    sizer->AddButton( m_help );
    sizer->Realize();
    sizer_4->Add( sizer );
    sizer_3->Add( 5, 5, 0, 0, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );

    panel_1->SetSizer( sizer_2 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
    // end wxGlade
    m_clustered->Bind( wxEVT_RADIOBOX, &CreateIndexSQLServer::OnClusteredUnclustered, this );
    m_filestreamType->Bind( wxEVT_COMBOBOX, &CreateIndexSQLServer::OnFileStreamSelected, this );
    m_where->Bind( wxEVT_GRID_EDITOR_CREATED, &CreateIndexSQLServer::OnColumnName, this );
    m_OK->Bind( wxEVT_BUTTON, &CreateIndexSQLServer::OnOkShowLog, this );
    m_table->Bind( wxEVT_LIST_ITEM_SELECTED, &CreateIndexSQLServer::OnFieldSelected, this );
    m_table->Bind( wxEVT_LIST_ITEM_DESELECTED, &CreateIndexSQLServer::OnFieldDeselected, this );
    m_padIndex->Bind( wxEVT_CHECKBOX, &CreateIndexSQLServer::OnPadIndex, this );
    m_fillFactor->Bind( wxEVT_SPINCTRL, &CreateIndexSQLServer::OnFillFactor, this );
}

void CreateIndexSQLServer::OnClusteredUnclustered(wxCommandEvent &WXUNUSED(event))
{
    if( m_clustered->GetSelection() == 0 )
    {
        m_label1->Enable( true );
        m_filestreamType->Enable( true );
        if( m_filestreamType->GetValue() == "NULL" )
            m_name->Enable( false );
        else
            m_name->Enable( true );
    }
    else
    {
        m_label1->Enable( false );
        m_filestreamType->Enable( false );
        m_name->Enable( false );
    }
}

void CreateIndexSQLServer::OnFileStreamSelected(wxCommandEvent &WXUNUSED(event))
{
    if( m_filestreamType->GetValue() != "NULL" )
    {
        m_name->Enable( true );
        CallAfter( [this]() { m_name->SetFocus(); } );
    }
}

void CreateIndexSQLServer::OnColumnName(wxGridEditorCreatedEvent &event)
{
    auto oldString = m_where->GetCellValue( event.GetRow(), 0 );
    oldString += " " + m_where->GetCellValue( event.GetRow(), 1 );
    oldString += " " + m_where->GetCellValue( event.GetRow(), 2 );
    if( event.GetCol() == 0 )
    {
        wxComboBox *editor = dynamic_cast<wxComboBox *>( event.GetControl() );
        for( std::vector<std::wstring>::iterator it = m_tableFields.begin(); it < m_tableFields.end(); it++ )
            editor->Append( (*it) );
    }
}

void CreateIndexSQLServer::OnOkShowLog(wxCommandEvent &event)
{
    std::vector<std::wstring> errorMsg;
    if( Verify() )
    {
        GenerateQuery();
        EndModal( event.GetId() );
    }
}

void CreateIndexSQLServer::OnFieldSelected(wxListEvent &event)
{
    auto label = m_table->GetItemText( event.GetItem() );
    m_includeFields->AddField( label );
    m_fields.push_back( label.ToStdWstring() );
}

void CreateIndexSQLServer::OnFieldDeselected(wxListEvent &event)
{
    auto label = m_table->GetItemText( event.GetItem() );
    m_fields.erase( std::remove( m_fields.begin(), m_fields.end(), label ), m_fields.end() );
    m_includeFields->RemoveField( label );
}

void CreateIndexSQLServer::OnPadIndex(wxCommandEvent &event)
{
    if( event.IsChecked() )
    {
        m_fillFactor->SetValue( 1 );
    }
    else
    {
        m_fillFactor->SetValue( 0 );
    }
}

void CreateIndexSQLServer::OnFillFactor(wxCommandEvent &WXUNUSED(event))
{
    if( m_fillFactor->GetValue() == 0 )
        m_padIndex->SetValue( false );
    else
        m_padIndex->SetValue( true );
}

bool CreateIndexSQLServer::Verify()
{
    bool success = true;
    std::vector<std::wstring> errors;
    for( auto i = 0; i < m_where->GetNumberRows(); ++i )
        m_whereData.push_back( std::make_tuple( m_where->GetCellValue( i, 0 ), m_where->GetCellValue( i, 1 ), m_where->GetCellValue( i, 2 ), m_where->GetCellValue( i, 3 ) ) );
    auto lastline = false;
    for( auto i = 0; i < m_whereData.size(); ++i )
    {
        if( std::get<0>( m_whereData.at( i ) ).empty() )
        {
            if( !std::get<1>( m_whereData.at( i ) ).empty() || !std::get<2>( m_whereData.at( i ) ).empty() )
            {
                success = false;
            }
        }
        else
        {
            if( std::get<1>( m_whereData.at( i ) ).empty() || std::get<2>( m_whereData.at( i ) ).empty() )
            {
                success = false;
            }
            if( std::get<3>( m_whereData.at( i ) ).empty() )
            {
                if( lastline )
                    success = false;
                else
                    lastline = true;
            }
        }
        if( !success )
        {
            wxMessageBox( wxString::Format( _( "Incomplete WHERE condition at line %d" ), i + 1 ), _( "Error" ), wxICON_EXCLAMATION | wxOK );
            break;
        }
        if( i == m_where->GetNumberRows() && m_where->GetCellValue( i, 4 ) == "AND" )
        {
            wxMessageBox( _( "Last line in WHERE is incorrect" ) );
            success = false;
            break;
        }
        if( m_where->GetCellValue( i, 1 ) == "IN" && m_where->GetCellValue( i, 2 ).find( ',' ) == std::string::npos )
        {
            wxMessageBox( _( "IN operation require a list" ), _( "Error" ), wxICON_EXCLAMATION | wxOK );
            success = false;
            break;
        }
    }
    return success;
}

void CreateIndexSQLServer::GenerateQuery()
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
        auto fillfactor = m_fillFactor->GetValue();
        with += "PAD_INDEX = ON ";
        with += "FILLFACTOR = ";
        with << fillfactor;
        with += " ";
    }
    if( m_sortInTemp->GetValue() )
        with += "SORT_IN_TEMPDB = ON ";
    if( m_ignoreDupKey->GetValue() )
        with += "IGNORE_DUP_KEY = ON ";
    if( m_noRecompute->GetValue() )
        with += "STATISTICS_NORECOMPUTE = ON ";
    if( m_incremental->GetValue() )
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
    if( !m_rowLocks->GetValue() )
        with += "ALLOW_ROW_LOCKS = OFF ";
    if( !m_pageLocks->GetValue() )
        with += "ALLOW_PAGE_LOCKS = OFF ";
    if( m_optimize->GetValue() )
        with += "OPTIMIZE_FOR_SEQUENTIAL_KEY = ON ";
    auto maxdop = m_maxDOP->GetValue();
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
}

// -*- C++ -*-
//
// generated by wxGlade 0.7.2 (standalone edition) on Wed Jan 25 23:33:43 2017
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
#endif

#include "wx/dialog.h"
#include "wx/grid.h"
#include "wx/gdicmn.h"
#include "database.h"
#include "quickselect.h"

QuickSelect::QuickSelect(wxWindow *parent, const Database *db) : wxDialog(parent, wxID_ANY, _(""))
{
    m_oldColumn = -1;
    m_db = const_cast<Database *>( db );
    m_cols = 0;
    m_panel = new wxPanel( this, wxID_ANY );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "1. Click on the table to select or deselect" ) );
    m_label2 = new wxStaticText( m_panel, wxID_ANY, _( "2. Select one or more column" ) );
    m_label3 = new wxStaticText( m_panel, wxID_ANY, _( "3. (Optional) Enter sorting and selection criteria below" ) );
    m_label4 = new wxStaticText( m_panel, wxID_ANY, _( "To display a comment for a table or column, click the right mouse button" ) );
    m_label5 = new wxStaticText( m_panel, wxID_ANY, _( "Tables:" ) );
    m_tables = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL );
    m_label6 = new wxStaticText( m_panel, wxID_ANY, _( "Columns:" ) );
    m_fields = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE );
    m_label7 = new wxStaticText( m_panel, wxID_ANY, _( "Comments" ) );
    m_comments = new wxStaticText( m_panel, wxID_ANY, "" );
    m_grid = new wxGrid( m_panel, wxID_ANY );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_addAll = new wxButton( m_panel, wxID_ANY, _( "Add All" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_label10 = new wxStaticText( m_panel, wxID_ANY, _( "Column:" ) );
    m_label11 = new wxStaticText( m_panel, wxID_ANY, _( "Sort:" ) );
    m_label12 = new wxStaticText( m_panel, wxID_ANY, _( "Criteria:" ) );
    set_properties();
    do_layout();
    for( size_t i = 0; i < m_sizer10->GetItemCount (); ++i )
    {
        m_sizer10->SetItemMinSize( i, -1, m_grid->GetRowSize( i ) );
    }
    m_sizer10->Layout();
    m_ok->Bind( wxEVT_UPDATE_UI, &QuickSelect::OnOkEnableUI, this );
    m_addAll->Bind( wxEVT_UPDATE_UI, &QuickSelect::OnAddAllUpdateUI, this );
    m_addAll->Bind( wxEVT_BUTTON, &QuickSelect::OnAllFieldsSelected, this );
    m_tables->Bind( wxEVT_LEFT_DOWN, &QuickSelect::OnSelectingTable, this );
    m_fields->Bind( wxEVT_LISTBOX, &QuickSelect::OnFieldSelected, this );
    m_tables->Bind( wxEVT_RIGHT_DOWN, &QuickSelect::OnDisplayComment, this);
    m_fields->Bind( wxEVT_RIGHT_DOWN, &QuickSelect::OnDisplayComment, this);
    m_grid->Bind( wxEVT_GRID_ROW_SIZE, &QuickSelect::OnGridRowLines, this );
    m_grid->Bind( wxEVT_GRID_CELL_LEFT_CLICK, &QuickSelect::OnCellLeftClicked, this );
    m_ok->Bind( wxEVT_BUTTON, &QuickSelect::OnOkButton, this );
//
//    m_grid->Bind( wxEVT_GRID_EDITOR_HIDDEN, &QuickSelect::OnSortClosing, this );
//
}

QuickSelect::~QuickSelect()
{
}

void QuickSelect::set_properties ()
{
    SetTitle( _( "Quick Select" ) );
    m_ok->Enable( false );
//    m_addAll->Enable( false );
    m_grid->CreateGrid( 4, 0 );
    m_grid->HideColLabels();
    m_grid->HideRowLabels();
    m_grid->DisableDragColSize();
    m_grid->DisableDragRowSize();
    m_grid->SetRowLabelAlignment( wxALIGN_RIGHT, wxALIGN_CENTER );
    m_grid->SetRowLabelValue( 0, _( "Column:" ) );
    m_grid->SetRowLabelValue( 1, _( "Sort:" ) );
    m_grid->SetRowLabelValue( 2, _( "Criteria:" ) );
    m_grid->SetRowLabelValue( 3, _( "Or:" ) );
    m_grid->SetRowLabelValue( 4, _( "" ) );
    FillTableListBox();
}

void QuickSelect::do_layout()
{
    auto *mainSizer = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer2 = new wxBoxSizer( wxVERTICAL );
    auto *sizer3 = new wxBoxSizer( wxVERTICAL );
    auto *sizer4 = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer5 = new wxFlexGridSizer( 2, 2, 5, 5 );
    auto *sizer6 = new wxBoxSizer( wxVERTICAL );
    auto *sizer7 = new wxBoxSizer( wxVERTICAL );
    auto *sizer8 = new wxBoxSizer( wxVERTICAL );
    auto *sizer9 = new wxBoxSizer( wxVERTICAL );
    m_sizer10 = new wxBoxSizer( wxVERTICAL );
    auto *sizer11 = new wxBoxSizer( wxHORIZONTAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer6->Add( m_label1, 0, wxEXPAND, 0 );
    sizer6->Add( m_label2, 0, wxEXPAND, 0 );
    sizer6->Add( m_label3, 0, wxEXPAND, 0 );
    sizer5->Add( sizer6, 0, wxEXPAND, 0 );
    sizer5->Add( m_label4, 0, wxEXPAND, 0 );
    sizer7->Add( m_label5, 0, wxEXPAND, 0 );
    sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer7->Add( m_tables, 0, wxEXPAND, 0 );
    sizer5->Add( sizer7, 0, wxEXPAND, 0 );
    sizer8->Add( m_label6, 0, wxEXPAND, 0 );
    sizer8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer8->Add( m_fields, 1, wxEXPAND, 0 );
    sizer5->Add( sizer8, 0, wxEXPAND, 0 );
    sizer4->Add( sizer5, 0, wxEXPAND, 0 );
    sizer9->Add( m_ok, 0, wxEXPAND, 0 );
    sizer9->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer9->Add( m_addAll, 0, wxEXPAND, 0 );
    sizer9->Add( m_help, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( sizer9, 0, wxEXPAND, 0 );
    sizer3->Add( sizer4, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_label7, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_comments, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_sizer10->Add( m_label10, wxSizerFlags().Border( wxRIGHT ).Right() );
    m_sizer10->Add( m_label11, wxSizerFlags().Border( wxRIGHT ).Right() );
    m_sizer10->Add( m_label12, wxSizerFlags().Border( wxRIGHT ).Right() );
    sizer11->Add( m_sizer10, 0, wxEXPAND, 0 );
    sizer11->Add( m_grid, 1, wxEXPAND, 0 );
    sizer3->Add( sizer11, 0, wxEXPAND, 0 );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer1 );
    sizer1->Fit( m_panel );
    mainSizer->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizerAndFit( mainSizer );
    Layout();
}

void QuickSelect::OnOkEnableUI(wxUpdateUIEvent &event)
{
    wxArrayInt arr;
    if( m_fields->GetSelections( arr ) > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void QuickSelect::OnAddAllUpdateUI(wxUpdateUIEvent &event)
{
    if( m_fields->GetCount() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void QuickSelect::FillTableListBox()
{
    std::wstring type = m_db->GetTableVector().m_type;
    std::wstring subType = m_db->GetTableVector().m_subtype;
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = m_db->GetTableVector ().m_tables.begin (); it != m_db->GetTableVector ().m_tables.end (); ++it )
    {
        for( std::vector<DatabaseTable *>::iterator it1 = ( *it ).second.begin (); it1 < ( *it ).second.end (); ++it1 )
        {
            std::wstring tableName = (*it1)->GetTableName();
            if( type == L"SQLite" && ( tableName.substr( 0, 6 ) == L"sqlite" || tableName.substr( 0, 3 ) == L"sys" ) )
                continue;
            if( ( type == L"ODBC" && subType == L"Microsoft SQL Server" ) || type == L"Microsoft SQL Server" )
            {
                if( tableName.substr( 0, 5 ) == L"abcat" || ( (*it1)->GetSchemaName() == L"sys" || (*it1)->GetSchemaName() == L"INFORMATION_SCHEMA" ) )
                    continue;
            }
            else if( ( type == L"ODBC" && subType == L"MySQL" ) || type == L"MySQL" )
            {
                if( tableName.substr( 0, 5 ) == L"abcat" || (*it1)->GetSchemaName() == L"information_schema" )
                    continue;
            }
            else if( ( type == L"ODBC" && subType == L"PostgreSQL" ) || type == L"PostgreSQL" )
            {
                if( tableName.substr( 0, 5 ) == L"abcat" || ( (*it1)->GetSchemaName() == L"information_schema" || (*it1)->GetSchemaName() == L"pg_catalog" ) )
                    continue;
            }
            m_tables->Append( tableName );
        }
    }
}

void QuickSelect::OnSelectingTable(wxMouseEvent &event)
{
    auto count = m_tables->GetCount();
    auto found = false;
    auto item = m_tables->HitTest( event.GetPosition() );
    if( item == wxNOT_FOUND )
        event.Skip();
    else
    {
        wxString selectedTable = m_tables->GetString( item );
        if( count > 1 )
        {
            for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = m_db->GetTableVector().m_tables.begin(); it != m_db->GetTableVector().m_tables.end() && !found; ++it )
            {
                for( std::vector<DatabaseTable *>::iterator it1 = (*it).second.begin(); it1 < (*it).second.end() && !found; ++it1 )
                {
                    if( selectedTable == (*it1)->GetTableName() )
                    {
                        found = true;
                        m_tableFields = (*it1)->GetFields();
                        for( std::vector<Field *>::iterator it2 = m_tableFields.begin(); it2 < m_tableFields.end(); ++it2 )
                            m_fields->Append( (*it2)->GetFieldName() );
                    }
                }
            }
            m_tables->Clear();
            m_tables->Append( selectedTable );
            m_tables->SetSelection( 0 );
            event.Skip();
        }
        else
        {
            m_tables->Delete( 0 );
            m_fields->Clear();
            auto countGrid = m_grid->GetNumberCols();
            while( countGrid > 0 )
            {
                m_grid->DeleteCols();
                countGrid = m_grid->GetNumberCols();
            }
            m_tableFields.clear();
            FillTableListBox();
            m_tables->SetStringSelection( m_tables->GetString( 0 ), false );
        }
    }
}

void QuickSelect::OnDisplayComment(wxMouseEvent &event)
{
    wxListBox *object = (wxListBox *) event.GetEventObject();
    auto item = m_tables->HitTest( event.GetPosition() );
    auto found = false;
    if( item != wxNOT_FOUND )
    {
        wxString stringClicked = object->GetString( item );
        if( object == m_fields )
        {
            for( std::vector<Field *>::iterator it = m_tableFields.begin (); it < m_tableFields.end () && !found; ++it )
            {
                if( (*it)->GetFieldName() == stringClicked )
                {
                    m_comments->SetLabel( (*it)->GetComment() );
                    found = true;
                }
            }
        }
        else
        {
            for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = m_db->GetTableVector().m_tables.begin(); it != m_db->GetTableVector().m_tables.end() && !found; ++it )
            {
                wxString name( (*it).first );
                for( std::vector<DatabaseTable *>::iterator it1 = (*it).second.begin(); it1 < (*it).second.end() && !found; ++it1 )
                {
                    if( stringClicked == name + "." + (*it1)->GetTableName() )
                    {
                        found = true;
                        m_comments->SetLabel( (*it1)->GetComment() );
                    }
                }
            }
        }
    }
    event.Skip();
}

void QuickSelect::OnGridRowLines(wxGridSizeEvent &event)
{
    for( size_t i = 0; i < m_sizer10->GetItemCount (); ++i )
    {
        m_sizer10->SetItemMinSize( i, -1, m_grid->GetRowSize( i ) );
    }
    m_sizer10->Layout();
    event.Skip();
}

void QuickSelect::OnFieldSelected(wxCommandEvent &event)
{
    if( event.IsSelection() )
        AddFieldToGrid( event.GetString(), true );
    else
        AddFieldToGrid( event.GetString(), false );
}

void QuickSelect::OnAllFieldsSelected(wxCommandEvent &WXUNUSED(event))
{
    m_grid->BeginBatch();
    auto i = 0;
    for( std::vector<Field *>::iterator it = m_tableFields.begin(); it < m_tableFields.end(); ++it )
    {
        if( !m_fields->IsSelected( i ) )
        {
            AddFieldToGrid( (*it)->GetFieldName(), true );
            m_fields->SetSelection( i );
        }
        i++;
    }
    m_grid->EndBatch();
}

void QuickSelect::AddFieldToGrid(const wxString &field, bool isAdded)
{
    bool found = false;
    const wxString choices[] =
    {
        _( "Ascending" ),
        _( "Descending" ),
        _( "(not sorted)" ),
    };
    if( isAdded )
    {
        for( auto i = 0; i < m_grid->GetNumberCols() && !found; i++ )
        {
            if( m_grid->GetCellValue( 0, i ) == field )
            {
                found = true;
            }
        }
        if( !found )
        {
            m_grid->AppendCols();
            m_cols++;
            m_grid->SetCellValue( 0, m_cols - 1, field );
            m_grid->SetCellAlignment( 0, m_cols - 1, wxALIGN_CENTRE, wxALIGN_CENTRE );
            m_grid->SetCellEditor( 1, m_cols - 1, new wxGridCellChoiceEditor( WXSIZEOF( choices ), choices ) );
        }
    }
    else
    {
        for( auto i = 0; i < m_grid->GetNumberCols () && !found; ++i )
        {
            if( m_grid->GetCellValue( 0, i ) == field )
            {
                wxGridCellChoiceEditor *editor = (wxGridCellChoiceEditor *) m_grid->GetCellEditor( 1, i );
                wxComboBox *combo = (wxComboBox *) editor->GetControl();
                if( combo )
                    combo->Dismiss();
                editor->DecRef();
                m_grid->DeleteCols( i );
                m_cols--;
                found = true;
            }
        }
    }
}

void QuickSelect::OnCellLeftClicked(wxGridEvent &event)
{
    auto oldrow = m_grid->GetGridCursorRow();
//    auto oldcolumn = m_grid->GetGridCursorCol();
    auto column = event.GetCol();
    if( oldrow == 1 && m_oldColumn != column && m_oldColumn != -1 )
    {
        wxGridCellChoiceEditor *editor = (wxGridCellChoiceEditor *) m_grid->GetCellEditor( oldrow, m_oldColumn );
        wxComboBox *combo = (wxComboBox *) editor->GetControl();
        if( combo )
        {
            combo->Dismiss();
            combo->Unbind( wxEVT_COMBOBOX_CLOSEUP, &QuickSelect::OnFieldsSetFocus, this );
        }
        wxString value = m_grid->GetCellValue( oldrow, m_oldColumn );
        if( value == _( "(not sorted)" ) )
            m_grid->SetCellValue( oldrow, m_oldColumn, _T( "" ) );
        editor->DecRef();
    }
    auto row = event.GetRow();
    if( row == 1 )
    {
        m_grid->GoToCell( row, column );
        m_grid->EnableCellEditControl( true );
        m_grid->ShowCellEditControl();
        wxGridCellChoiceEditor *editor = (wxGridCellChoiceEditor *) m_grid->GetCellEditor( row, column );
        wxComboBox *combo = (wxComboBox *) editor->GetControl();
        //
        combo->Bind( wxEVT_COMBOBOX_CLOSEUP, &QuickSelect::OnSortClosing, this );
        //
        wxString value = m_grid->GetCellValue( row, column );
        combo->Popup();
        combo->SetSelection( 2 );
//        combo->SetFocus();
        editor->DecRef();
        m_column = column;
        m_oldColumn = column;
    }
    else if( event.GetRow() > 1 )
    {
        m_grid->GoToCell( row, column );
        m_grid->EnableCellEditControl( true );
        m_grid->ShowCellEditControl();
        event.Skip();
    }
    else
        event.Skip();
}

void QuickSelect::OnSortClosing(wxCommandEvent &WXUNUSED(event))
{
    if( m_grid->GetCellValue( 1, m_column ) == _( "(not sorted)" ) )
//    wxComboBox *object = wxDynamicCast( event.GetEventObject(), wxComboBox );
//    if( object->GetStringSelection() == _( "(not sorted)" ) )
    {
//        CallAfter( []{ m_grid->SetCellValue( 1, m_column, "" )};/*, 1, m_column, "" );*/
        CallAfter( [this](){ m_grid->SetCellValue( 1, m_column, "" ); } );
    }
}
/*
void QuickSelect::OnSortClosing(wxGridEvent &event)
{
    if( event.GetRow() == 1 && m_grid->GetCellValue( 1, m_column )  == _( "(not sorted)" ) )
        m_grid->SetCellValue( 1, m_column, "" );
}
*/
void QuickSelect::OnFieldsSetFocus(wxCommandEvent &WXUNUSED(event))
{
    auto pt = wxGetMousePosition();
    auto ptWindow = ScreenToClient( pt );
    auto pos1 = m_fields->GetPosition();
    wxPoint pos2( pos1.x + m_fields->GetSize().GetWidth(), pos1.y + m_fields->GetSize().GetHeight() );
    if( ptWindow.x >= pos1.x && ptWindow.x <= pos2.x && ptWindow.y >= pos1.y && ptWindow.y <= pos2.y )
    {
        auto item = m_fields->HitTest( m_fields->ScreenToClient( pt ) );
        if( item != wxNOT_FOUND )
        {
			auto selection = m_fields->IsSelected( item ); 
            if( selection )
                AddFieldToGrid( m_fields->GetString( item ), false );
            else
                AddFieldToGrid( m_fields->GetString( item ), true );
            m_fields->SetSelection( item, !selection );
        }
    }
}

std::vector<Field *> &QuickSelect::GetQueryFields()
{
    return m_queryFields;
}

void QuickSelect::OnOkButton(wxCommandEvent &WXUNUSED(event))
{
    auto found = false;
    auto it1 = m_db->GetTableVector().m_tables[m_db->GetTableVector().m_dbName];
    for( auto it2 = it1.begin(); it2 < it1.end() && !found; ++it2 )
    {
        if( ( *it2 )->GetTableName () == m_tables->GetStringSelection () )
        {
            m_table = (*it2);
            found = true;
        }
    }
    found = false;
    for( auto i = 0; i < m_grid->GetNumberCols(); ++i )
    {
        for( auto it3 = m_table->GetFields().begin(); it3 < m_table->GetFields().end() && !found; ++it3 )
        {
            if( m_grid->GetCellValue( 0, i ) == (*it3)->GetFieldName() )
            {
                m_queryFields.push_back( (*it3) );
                found = true;
            }
        }
        found = false;
    }
    EndModal( wxID_OK );
}

const wxListBox *QuickSelect::GetQueryTable()
{
    return m_tables;
}

DatabaseTable *QuickSelect::GetDatabaseTable()
{
    return m_table;
}
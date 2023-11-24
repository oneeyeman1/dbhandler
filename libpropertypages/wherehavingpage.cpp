// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#include <math.h>
#include <string>
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "wherehavingpage.h"

typedef int (*ADDCOLUMNSDIALOG)(wxWindow *, int, const wxPoint &, const std::vector<std::wstring> &, wxString &, const wxString &, const wxString &, const std::vector<QueryArguments> &);

const wxEventTypeTag<wxCommandEvent> wxEVT_CHANGE_QUERY( wxEVT_USER_FIRST + 3 );

WhereHavingPage::WhereHavingPage(wxWindow *parent, const wxString &type, const wxString &subtype, bool isWhere) : wxPanel( parent )
{
    m_type = type;
    m_subtype = subtype;
    m_isWherePage = isWhere;
    m_operatorChoices[0] = "=";
    m_operatorChoices[1] = "<";
    m_operatorChoices[2] = ">";
    m_operatorChoices[3] = "<=";
    m_operatorChoices[4] = ">=";
    m_operatorChoices[5] = "<>";
    m_operatorChoices[6] = "between";
    m_operatorChoices[7] = "exists";
    m_operatorChoices[8] = "in";
    m_operatorChoices[9] = "like";
    m_operatorChoices[10] = "not between";
    m_operatorChoices[11] = "not exists";
    m_operatorChoices[12] = "not in";
    m_operatorChoices[13] = "not like";
    m_operatorChoices[14] = "is";
    m_operatorChoices[15] = "is not";
    m_operatorChoices[16] = "= all";
    m_operatorChoices[17] = "!= all";
    m_operatorChoices[18] = "< all";
    m_operatorChoices[19] = "> all";
    m_operatorChoices[20] = "<= all";
    m_operatorChoices[21] = ">= all";
    m_operatorChoices[22] = "= any";
    m_operatorChoices[23] = "< any";
    m_operatorChoices[24] = "> any";
    m_operatorChoices[25] = "<= any";
    m_operatorChoices[26] = ">= any";
    m_operatorChoices[27] = "!= any";
    m_logicalChoices[0] = "And";
    m_logicalChoices[1] = "Or";
    m_grid = new wxGrid( this, wxID_ANY );
    m_scrollbarWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, m_grid);
    set_properties();
    do_layout();
    m_grid->ShowScrollbars( wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT );
    m_operatorSize = m_grid->GetColSize( 1 );
    m_logicalSize = m_grid->GetColSize( 3 );
    Bind( wxEVT_SIZE, &WhereHavingPage::OnSize, this );
    m_grid->Bind( wxEVT_GRID_CELL_LEFT_CLICK, &WhereHavingPage::OnGridLeftClick, this );
    m_grid->Bind( wxEVT_GRID_EDITOR_CREATED, &WhereHavingPage::OnColumnName, this );
    m_grid->Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &WhereHavingPage::OnCellRightClick, this );
    m_grid->Bind( wxEVT_GRID_CELL_CHANGED, &WhereHavingPage::OnGridCellChaqnged, this );
    Bind( wxEVT_MENU, &WhereHavingPage::OnMenuSelection, this, WHEREPAGECOLUMNS );
    Bind( wxEVT_MENU, &WhereHavingPage::OnMenuSelection, this, WHEREPAGEFUNCTIONS );
    Bind( wxEVT_MENU, &WhereHavingPage::OnMenuSelection, this, WHEREPAGEARGUMENTS );
}

WhereHavingPage::~WhereHavingPage(void)
{
}

void WhereHavingPage::set_properties()
{
    m_grid->CreateGrid( 10, 4 );
    m_grid->SetGridLineColour( *wxBLACK );
    m_grid->HideRowLabels();
    m_grid->SetColLabelValue( 0, _( "Column" ) );
    m_grid->SetColLabelValue( 1, _( "Operator" ) );
    m_grid->SetColLabelValue( 2, _( "Value" ) );
    m_grid->SetColLabelValue( 3, _( "Logical" ) );
    for (int i = 0; i < 4; i++ )
        m_grid->DisableColResize( i );
    for( int i = 0; i < 9; ++i )
    {
        m_grid->AppendRows();
        m_grid->SetRowSize( i, m_grid->GetRowSize( i ) + GRIDROW_ADJUSTMENT );
    }
    m_grid->DisableDragColMove();
    m_grid->DisableDragColSize();
    m_grid->DisableDragGridSize();
    m_grid->DisableDragRowSize();
    for( int i = 0; i < 9; i++ )
    {
        m_grid->SetCellEditor( i, 0, new wxGridCellChoiceEditor( 0, NULL, true ) );
        m_grid->SetCellEditor( i, 1, new wxGridCellChoiceEditor( 28, m_operatorChoices ) );
        m_grid->SetCellEditor( i, 3, new wxGridCellChoiceEditor( 2, m_logicalChoices ) );
    }
}

void WhereHavingPage::do_layout()
{
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_grid, 0, wxEXPAND, 0 );
    SetSizer( sizer );
}

void WhereHavingPage::AppendField(const std::wstring &field)
{
    m_fields.push_back( field );
}

void WhereHavingPage::AppendRowsToGrid(int count)
{
    auto numRows = m_grid->GetNumberRows();
    for( auto i = 0; i < count; ++i )
    {
        m_grid->AppendRows();
        m_grid->SetCellEditor( numRows + i, 0, new wxGridCellChoiceEditor( 0, NULL, true ) );
        m_grid->SetCellEditor( numRows + i, 1, new wxGridCellChoiceEditor( 28, m_operatorChoices ) );
        m_grid->SetCellEditor( numRows + i, 3, new wxGridCellChoiceEditor( 2, m_logicalChoices ) );
    }
}

void WhereHavingPage::OnSize(wxSizeEvent &event)
{
    int width = GetClientRect().GetWidth();
    if( width > 0 )
    {
        int grid_width = width - m_scrollbarWidth;
        int col_width = ( grid_width - ( m_operatorSize + m_logicalSize ) ) / 2;
        m_grid->SetColSize( 0, col_width );
        m_grid->SetColSize( 2, col_width );
        int height = m_grid->GetRowSize( 0 ) * 4;
        m_grid->SetMaxSize( wxSize( -1, height ) );
    }
    event.Skip();
}

void WhereHavingPage::OnColumnName(wxGridEditorCreatedEvent &event)
{
    m_oldString = m_grid->GetCellValue( event.GetRow(), 0 );
    m_oldString += " " + m_grid->GetCellValue( event.GetRow(), 1 );
    m_oldString += " " + m_grid->GetCellValue( event.GetRow(), 2 );
    if( event.GetCol() == 0 )
    {
        wxComboBox *editor = dynamic_cast<wxComboBox *>( event.GetControl() );
        for( std::vector<std::wstring>::iterator it = m_fields.begin(); it < m_fields.end(); it++ )
            editor->Append( (*it) );
        m_row = event.GetRow();
        editor->Bind( wxEVT_COMBOBOX, &WhereHavingPage::OnCellChanged, this );
    }
}

wxGrid *WhereHavingPage::GetGrid()
{
    return m_grid;
}

void WhereHavingPage::OnSelection()
{
    m_grid->SetCellHighlightPenWidth( 0 );
}

void WhereHavingPage::OnCellRightClick(wxGridEvent &event)
{
    m_col = event.GetCol();
    m_row = event.GetRow();
    m_pos = event.GetPosition();
    m_pos = ClientToScreen( m_pos );
    if( m_col == 0 || m_col == 2 )
    {
        wxMenu contextMenu;
        contextMenu.Append( WHEREPAGECOLUMNS, _( "Columns..." ) );
        contextMenu.Append( WHEREPAGEFUNCTIONS, _( "Functions..." ) );
        contextMenu.Append( WHEREPAGEARGUMENTS, _( "Arguments..." ) );
        contextMenu.Append( WHEREPAGEVALUE, _( "Value..." ) );
        if( m_col == 2 )
            contextMenu.Append( WHEREPAGESELECT, _( "Select..." ) );
        contextMenu.AppendSeparator();
        contextMenu.Append( WHEREPAGECLEAR, _( "Clear" ) );
        if( m_arguments.size() == 0 )
            contextMenu.Enable( WHEREPAGEARGUMENTS, false );
        else
            contextMenu.Enable( WHEREPAGEARGUMENTS, true );
        PopupMenu( &contextMenu );
    }
}

void WhereHavingPage::OnMenuSelection(wxCommandEvent &event)
{
    int id = event.GetId();
    int type = 0;
    std::vector<std::wstring> fields;
    fields = m_fields;
    switch( id )
    {
    case WHEREPAGECOLUMNS:
        type = 1;
        break;
    case WHEREPAGEFUNCTIONS:
        type = 2;
        break;
    case WHEREPAGEARGUMENTS:
        type = 3;
        break;
    }
    wxDynamicLibrary *lib;
    lib = new wxDynamicLibrary();
#ifdef __WXMSW__
    lib->Load( "dialogs" );
#elif __WXMAC__
    wxFileName fn( wxStandardPaths::Get().GetExecutablePath() );
    auto path = fn.GetPath() + wxFileName::GetPathSeparator() + ".." + wxFileName::GetPathSeparator() + "Frameworks" + wxFileName::GetPathSeparator();
    lib->Load( path + "liblibdialogs.dylib" );
#else
    lib->Load( "libdialogs" );
#endif
    if( lib->IsLoaded() )
    {
        wxString selection;
        ADDCOLUMNSDIALOG func = (ADDCOLUMNSDIALOG) lib->GetSymbol( "AddColumnToQuery" );
        func( this, type, m_pos, fields, selection, m_type, m_subtype, m_arguments );
        if( selection != wxEmptyString )
        {
            m_grid->SetCellValue( m_row, m_col, selection );
            if( m_col == 0 && m_grid->GetCellValue( m_row, 1 ) == wxEmptyString )
                m_grid->SetCellValue( m_row, 1, "=" );
            m_grid->SetGridCursor( m_row, m_col );
            m_grid->MakeCellVisible( m_row, m_col );
            m_grid->EnableCellEditControl();
            if( m_col )
            {
                wxGridCellEditor *editor = m_grid->GetCellEditor( m_row, m_col );
                wxComboBox *combo = dynamic_cast<wxComboBox *>( editor->GetControl() );
                if( combo )
                {
                    if( type == 2 )
                        combo->SetSelection( selection.size() - 1, selection.size() - 1 );
                    else
                        combo->SetSelection( selection.size(), selection.size() );
                    m_grid->GetCellEditor( m_row, m_col )->DecRef();
                }
                else
                {
                    wxTextCtrl *text = dynamic_cast<wxTextCtrl *>( editor->GetControl() );
                    if( text )
                    {
                        if( type == 2 )
                            text->SetSelection( selection.size() - 1, selection.size() - 1 );
                        else
                            text->SetSelection( selection.size(), selection.size() );
                    }
                }
                editor->DecRef();
            }
        }
    }
    delete lib;
    lib = NULL;
}

void WhereHavingPage::OnCellChanged(wxCommandEvent &event)
{
    wxString newString;
    if( m_grid->GetCellValue( m_row, 1 ) == wxEmptyString )
    {
        newString = event.GetString();
        m_grid->SetCellValue( m_row, 1, "=" );
    }
    else
        newString = m_grid->GetCellValue( m_row, 0 );
    newString += " " + m_grid->GetCellValue( m_row, 1 );
    newString += " " + m_grid->GetCellValue( m_row, 2 );
    wxCommandEvent evt( wxEVT_CHANGE_QUERY );
    evt.SetEventObject( this );
    evt.SetString( newString );
    evt.SetInt( m_row );
    evt.SetClientObject( (wxClientData *) &newString );
    wxWindow *parent = GetParent()->GetParent();
    parent->GetEventHandler()->ProcessEvent( evt );
}

void WhereHavingPage::OnGridCellChaqnged(wxGridEvent &event)
{
    if( event.GetCol() == 0 && m_grid->GetCellValue( m_row, 0 ) == wxEmptyString )
    {
        m_grid->SetCellValue( m_row, 1, "" );
        m_grid->SetCellValue( m_row, 2, "" );
        m_grid->SetCellValue( m_row, 3, "" );
        m_lines.at( m_row ).m_old = m_lines.at( m_row ).m_new;
        m_lines.at( m_row ).m_new = "";
    }
    if( event.GetCol () == 0 )
    {
        if( m_lines.size() == 0 )
            m_lines.push_back( WhereHavingLines( m_row, "", m_grid->GetCellValue( m_row, 0 ) + " " + m_grid->GetCellValue( m_row, 1 ) + " " + m_grid->GetCellValue( m_row, 2 ) + " " + m_grid->GetCellValue( m_row, 3 ) ) );
        else
        {
            bool found = false;
            for( std::vector<WhereHavingLines>::iterator it = m_lines.begin(); it < m_lines.end() && !found; ++it )
            {
                if( (*it).m_row == m_row )
                {
                    found = true;
                    (*it).m_old = (*it).m_new;
                    (*it).m_new = m_grid->GetCellValue( m_row, 0 ) + " " + m_grid->GetCellValue( m_row, 1 ) + " " + m_grid->GetCellValue( m_row, 2 ) + " " + m_grid->GetCellValue( m_row, 3 );
                }
            }
            if( !found )
                m_lines.push_back( WhereHavingLines( m_row, "", m_grid->GetCellValue( m_row, 0 ) + " " + m_grid->GetCellValue( m_row, 1 ) + " " + m_grid->GetCellValue( m_row, 2 ) + " " + m_grid->GetCellValue( m_row, 3 ) ) );
        }
    }
    bool found = false;
    for( std::vector<WhereHavingLines>::iterator it = m_lines.begin(); it < m_lines.end() && !found; ++it )
    {
        if( (*it).m_row == m_row )
        {
            found = true;
            wxCommandEvent evt( wxEVT_CHANGE_QUERY );
            evt.SetEventObject( this );
            evt.SetClientData( &(*it) );
            GetParent()->GetParent()->GetEventHandler()->ProcessEvent( evt );        }
    }
}

void WhereHavingPage::SetQueryArguments (const std::vector<QueryArguments> &arguments)
{
    m_arguments = arguments;
}

void WhereHavingPage::OnGridLeftClick(wxGridEvent &event)
{
    if( m_row == event.GetRow() && m_col == event.GetCol() && m_grid->IsCellEditControlShown() )
        return;
    m_grid->SetGridCursor( event.GetRow(), event.GetCol() );
    m_grid->MakeCellVisible( event.GetRow(), event.GetCol() );
    m_grid->EnableCellEditControl();
    m_row = event.GetRow();
    m_col = event.GetCol();
}

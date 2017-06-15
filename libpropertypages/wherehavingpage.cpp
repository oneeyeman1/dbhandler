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
#include "wx/grid.h"
#include "wx/headerctrl.h"
//#include "wx/settings.h"
#include "wherehavingpage.h"

WhereHavingPage::WhereHavingPage(wxWindow *parent) : wxPanel( parent )
{
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
    m_grid->Bind( wxEVT_GRID_EDITOR_CREATED, &WhereHavingPage::OnColumnName, this );
    m_grid->Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &WhereHavingPage::OnCellRightClick, this );
}

WhereHavingPage::~WhereHavingPage(void)
{
}

void WhereHavingPage::set_properties()
{
    m_grid->CreateGrid( 10, 4 );
    m_grid->HideRowLabels();
    m_grid->SetColLabelValue( 0, _( "Column" ) );
    m_grid->SetColLabelValue( 1, _( "Operator" ) );
    m_grid->SetColLabelValue( 2, _( "Value" ) );
    m_grid->SetColLabelValue( 3, _( "Logical" ) );
    for (int i = 0; i < 4; i++ )
        m_grid->DisableColResize( i );
    m_grid->AppendRows( 10 );
    m_grid->DisableDragColMove();
    m_grid->DisableDragColSize();
    m_grid->DisableDragGridSize();
    m_grid->DisableDragRowSize();
    for( int i = 0; i < 9; i++ )
    {
        m_grid->SetCellEditor( i, 0, new wxGridCellChoiceEditor() );
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

void WhereHavingPage::OnColumnDropDown(wxCommandEvent &event)
{
    wxComboBox *editor = wxDynamicCast( event.GetEventObject(), wxComboBox );
    for( std::vector<std::wstring>::iterator it = m_fields.begin(); it < m_fields.end(); it++ )
        editor->Append( (*it) );
}

void WhereHavingPage::OnColumnName(wxGridEditorCreatedEvent &event)
{
    if( event.GetCol() == 0 )
    {
        dynamic_cast<wxComboBox *>( event.GetControl() )->Bind( wxEVT_COMBOBOX_DROPDOWN, &WhereHavingPage::OnColumnDropDown, this );
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
    int col = event.GetCol();
    if( col == 0 || col == 2 )
    {
        wxMenu contextMenu;
        contextMenu.Append( WHEREPAGECOLUMNS, _( "Columns..." ) );
        contextMenu.Append( WHEREPAGEFUNCTIONS, _( "Functions..." ) );
        contextMenu.Append( WHEREPAGEARGUMENTS, _( "Arguments..." ) );
        contextMenu.Append( WHEREPAGEVALUE, _( "Value..." ) );
        if( col == 2 )
            contextMenu.Append( WHEREPAGESELECT, _( "Select..." ) );
        contextMenu.AppendSeparator();
        contextMenu.Append( WHEREPAGECLEAR, _( "Clear" ) );
        PopupMenu( &contextMenu );
    }
}

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#include <string>
#include "wx/grid.h"
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
    set_properties();
    do_layout();
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
    m_grid->AppendRows( 10 );
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
    sizer->Add( m_grid, 1, wxEXPAND, 0 );
    SetSizer( sizer );
}

void WhereHavingPage::AppendField(const std::wstring &field)
{
    m_fields.push_back( field );
}

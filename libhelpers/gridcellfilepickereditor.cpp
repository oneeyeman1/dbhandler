#include "wx/wx.h"
#include "wx/grid.h"
#include "wx/filepicker.h"
#include "gridcellfilepickereditor.h"

GridCellFilePickerEditor::GridCellFilePickerEditor(const wxString &message, const wxString &wildcards, long style) : m_message( message ), m_wildcard( wildcards ), m_style( style )
{
}

void GridCellFilePickerEditor::Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler)
{
    wxGridCellEditor::Create( parent, id, evtHandler );
    m_control = new wxFilePickerCtrl( parent, id, wxEmptyString, m_message, m_wildcard, wxDefaultPosition, wxDefaultSize, m_style );
}

void GridCellFilePickerEditor::SetSize(const wxRect &rect)
{
    m_control->SetSize( rect.x - 1, rect.y - 1, rect.width + 2, rect.height + 2 );
}

void GridCellFilePickerEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    m_startValue = grid->GetTable()->GetValue( row, col );
    GetPicker()->SetPath( m_startValue );
    GetPicker()->SetFocus();
}

bool GridCellFilePickerEditor::EndEdit(int WXUNUSED( row ), int WXUNUSED( col ), const wxGrid *WXUNUSED( grid ), const wxString &oldval, wxString *newval)
{
    wxString currentVal = GetPicker()->GetPath();
    if( currentVal == oldval )
        return false;
    m_value = currentVal;
    if( newval )
        *newval = m_value;
    return true;
}

void GridCellFilePickerEditor::ApplyEdit(int row, int col, wxGrid* grid)
{
    grid->GetTable()->SetValue( row, col, m_value );
}

void GridCellFilePickerEditor::Reset()
{
    GetPicker()->SetPath(m_startValue);
}

wxString GridCellFilePickerEditor::GetValue() const
{
    return GetPicker()->GetPath();
}

wxGridCellEditor *GridCellFilePickerEditor::Clone() const
{
    return new GridCellFilePickerEditor( m_message, m_wildcard, m_style );
}
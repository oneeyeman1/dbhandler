#include "wx/grid.h"
#include "wx/font.h"
#include "wx/renderer.h"
#include "typecombobox.h"

FieldTypeRenderer::FieldTypeRenderer(wxString type)
{
	m_type = type;
}

void FieldTypeRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected)
{
    wxRendererNative::Get().DrawComboBox( &grid, dc, rect, 0 );
    wxFont font = grid.GetFont();
    dc.SetFont( font );
    dc.DrawText( m_label, rect.x + 10, rect.y );
}

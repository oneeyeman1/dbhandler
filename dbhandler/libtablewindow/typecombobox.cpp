// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/grid.h"
#include "wx/dc.h"
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
    dc.DrawText( m_type, rect.x + 10, rect.y );
}

wxSize FieldTypeRenderer::GetBestSize(wxGrid &,wxGridCellAttr &,wxDC &,int,int)
{
    wxSize size;
    return size;
}


FieldTypeRenderer *FieldTypeRenderer::Clone() const
{
    return new FieldTypeRenderer( "" );
}

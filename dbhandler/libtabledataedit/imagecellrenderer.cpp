#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/grid.h"
#include "imagecellrenderer.h"

ImageCellRenderer::ImageCellRenderer(wxImage image) : wxGridCellRenderer()
{
    m_image = image;
}

void ImageCellRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
    wxGridCellRenderer::Draw( grid, attr, dc, rect, row, col, isSelected );
    wxBitmap bitmap( m_image );
    dc.DrawBitmap( bitmap, rect.x, rect.y );
}

wxSize ImageCellRenderer::GetBestSize(wxGrid &WXUNUSED(grid), wxGridCellAttr &attr, wxDC &dc, int WXUNUSED(row), int WXUNUSED(col))
{
    return wxSize( m_image.GetSize() );
}

wxGridCellRenderer *ImageCellRenderer::Clone() const
{
    return new ImageCellRenderer;
}
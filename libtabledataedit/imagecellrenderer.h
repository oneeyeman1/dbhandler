#pragma once

#ifndef _IMAGECELLRENDERER
#define _IMAGECELLRENDERER

class ImageCellRenderer : wxGridCellStringRenderer
{
public:
    ImageCellRenderer(wxImage image);
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected) wxOVERRIDE;
private:
    wxImage m_image;
    wxDECLARE_NO_COPY_CLASS(ImageCellRenderer);
};

#endif

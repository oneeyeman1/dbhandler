#pragma once

#ifndef _IMAGECELLRENDERER
#define _IMAGECELLRENDERER

class ImageCellRenderer : public wxGridCellRenderer
{
public:
    ImageCellRenderer() {};
    ImageCellRenderer(wxImage image);
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected) wxOVERRIDE;
    virtual wxSize GetBestSize(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int Wrow,  int WXUNUSED(col)) wxOVERRIDE;
    virtual wxGridCellRenderer *Clone() const wxOVERRIDE;
private:
    wxImage m_image;
    wxDECLARE_NO_COPY_CLASS(ImageCellRenderer);
};

#endif

#pragma once
class WXEXPORT MyComboCellRenderer :  public wxGridCellRenderer
{
public:
    virtual wxGridCellRenderer *Clone()	const wxOVERRIDE;
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected) wxOVERRIDE;
    virtual wxSize GetBestSize(wxGrid &grid, wxGridCellAttr& attr, wxDC& dc, int row, int col) wxOVERRIDE;
};


#ifndef __TYPECOMBOBOX_H__
#define __TYPECOMBOBOX_H__

class FieldTypeRenderer : public wxGridCellRenderer
{
public:
    FieldTypeRenderer(wxString type);
    virtual void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected);
private:
	wxString m_type;
};

#endif
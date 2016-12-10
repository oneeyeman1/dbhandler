#pragma once
class GridTableShape :	public wxSFGridShape
{
public:
    GridTableShape(void);
    ~GridTableShape(void);
    bool InsertToTableGrid(wxSFShapeBase *shape);
};


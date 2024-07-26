#ifndef __GRIDTABLESHAPE_H__
#define __GRIDTABLESHAPE_H__

class GridTableShape :	public wxSFGridShape
{
public:
    GridTableShape(ViewType type);
    ~GridTableShape(void);
    bool InsertToTableGrid(wxSFShapeBase *shape);
    void ShowDataTypes(bool show, int type);
protected:
    void DoChildrenLayout();
private:
    wxColour m_colour;
    ViewType m_type;
    bool m_showdatatypes, m_showcomments;
    int m_columns;
};

#endif
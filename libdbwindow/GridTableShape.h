#ifndef __GRIDTABLESHAPE_H__
#define __GRIDTABLESHAPE_H__

#if !defined CONSTRAINT_H && !defined __DATABASEVIEW__H
enum ViewType
{
    DatabaseView,
    QueryView
};
#endif

class GridTableShape :	public wxSFGridShape
{
public:
    GridTableShape(ViewType type);
    ~GridTableShape(void);
    bool InsertToTableGrid(wxSFShapeBase *shape);
    virtual void DoChildrenLayout();
protected:
//    virtual void DrawSelected(wxDC &dc);
    virtual void DrawNormal(wxDC &dc);
private:
    wxColour m_colour;
    ViewType m_type;
};

#endif
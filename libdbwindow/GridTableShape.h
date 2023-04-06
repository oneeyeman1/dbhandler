#ifndef __GRIDTABLESHAPE_H__
#define __GRIDTABLESHAPE_H__

#if !defined CONSTRAINT_H && !defined __DATABASEVIEW__H
enum ViewType
{
    DatabaseView,
    NewViewView,
    QueryView
};
#endif

class GridTableShape :	public wxSFGridShape
{
public:
    GridTableShape(ViewType type);
    ~GridTableShape(void);
    bool InsertToTableGrid(wxSFShapeBase *shape);
    void ShowDataTypes(bool show) { m_showdatatypes = show; }
    void ShowComments(bool show) { m_showcomments = show; }
    virtual void DoChildrenLayout();
private:
    wxColour m_colour;
    ViewType m_type;
    bool m_showdatatypes, m_showcomments;
};

#endif
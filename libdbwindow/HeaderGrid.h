#pragma once
class HeaderGrid : public wxSFGridShape
{
public:
    HeaderGrid(void);
    virtual ~HeaderGrid(void);
    bool InsertToTableGrid(wxSFShapeBase *shape);
    virtual void DoChildrenLayout();
    void ShowComments(bool show);
};


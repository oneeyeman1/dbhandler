#pragma once
class HeaderGrid : public wxSFGridShape
{
public:
    HeaderGrid(void);
    virtual ~HeaderGrid(void);
    virtual void DoChildrenLayout();
    void ShowComments(bool show);
};


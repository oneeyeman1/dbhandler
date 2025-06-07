#pragma once

class MyTableDefGrid :  public wxGrid
{
public:
    MyTableDefGrid(wxWindow *parent, wxWindowID id);
    virtual void DrawRowLabel(wxDC &dc, int row) wxOVERRIDE;
protected:
    void OnCellClicked(wxGridEvent &event);
private:
    int m_newRow, m_oldRow;
};

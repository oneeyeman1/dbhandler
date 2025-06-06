#pragma once

class MyTableDefGrid :  public wxGrid
{
public:
    MyTableDefGrid(wxWindow *parent, wxWindowID id);
    virtual void DrawRowLabel(wxDC &dc, int row) wxOVERRIDE;
private:
    int m_newRow, m_oldRow;
};

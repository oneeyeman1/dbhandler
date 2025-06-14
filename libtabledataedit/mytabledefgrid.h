#pragma once

class MyTableDefGrid :  public wxGrid
{
public:
    MyTableDefGrid(wxWindow *parent, wxWindowID id);
    virtual void DrawRowLabel(wxDC &dc, int row) wxOVERRIDE;
    int GetNewRow() const { return m_newRow; }
    void SetNewRow(int row) { m_newRow = row; }
    void SetOldRow(int row) { m_oldRow = row; }
private:
    int m_newRow, m_oldRow;
    wxBitmapBundle m_pointer;
};

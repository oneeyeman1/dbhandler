#pragma once
class WXEXPORT FieldWindow
{
public:
    FieldWindow(wxWindow *parent, int type);
    ~FieldWindow(void);
    wxBoxSizer *GetSizer();
private:
    wxWindow *m_win;
    wxBoxSizer *m_sizer;
    wxPoint m_startPoint;
    wxScrollBar *m_columnsScrollbar;
};


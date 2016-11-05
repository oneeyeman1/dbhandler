#pragma once
class WXEXPORT FieldWindow
{
public:
    FieldWindow(wxWindow *parent, int type);
    ~FieldWindow(void);
    wxSFShapeCanvas *GetFieldsWindow();
private:
    wxSFShapeCanvas *m_win;
    wxPoint m_startPoint;
    wxSFDiagramManager m_manager;
};


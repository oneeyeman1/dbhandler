#pragma once
class WXEXPORT FieldWindow
{
public:
    FieldWindow(wxWindow *parent, int type);
    ~FieldWindow(void);
    wxSFShapeCanvas *GetFieldsWindow();
    void AddField(const wxString &fieldName);
    void RemoveField(const wxString &fieldName);
private:
    wxSFShapeCanvas *m_win;
    wxPoint m_startPoint;
    wxSFDiagramManager m_manager;
};


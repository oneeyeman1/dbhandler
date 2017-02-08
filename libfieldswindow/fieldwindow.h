#pragma once
class WXEXPORT FieldWindow
{
public:
    FieldWindow(wxWindow *parent, int type, const wxPoint &pos = wxDefaultPosition, int width = -1);
    ~FieldWindow(void);
    wxSFShapeCanvas *GetFieldsWindow();
    void AddField(const wxString &fieldName);
    void RemoveField(const std::vector<std::wstring> &names);
    void Clear();
private:
    wxSFShapeCanvas *m_win;
    wxPoint m_startPoint;
    wxSFDiagramManager m_manager;
};


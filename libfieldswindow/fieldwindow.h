#pragma once
class WXEXPORT FieldWindow : public wxSFShapeCanvas
{
public:
    FieldWindow(wxWindow *parent, int type, const wxPoint &pos = wxDefaultPosition, int width = -1);
    virtual ~FieldWindow();
    void AddField(const wxString &fieldName);
    void RemoveField(const std::vector<std::wstring> &names);
    void RemoveField(const std::vector<wxString> &names);
    virtual void OnLeftDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnMouseMove(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftUp(wxMouseEvent &event) wxOVERRIDE;
    void Clear();
private:
    wxPoint m_startPoint;
    wxSFDiagramManager m_manager;
    bool m_isDragging;
    FieldWin *m_draggingField;
    wxRect m_initialDraggerPosition;
    std::vector<wxString> m_selectedFields;
};


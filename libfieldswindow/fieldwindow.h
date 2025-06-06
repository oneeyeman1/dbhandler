#pragma once
class WXEXPORT FieldWindow : public wxSFShapeCanvas
{
    enum Direction
    {
        UNINITIALIZED = -1,
        BEFORE = 0,
        AFTER = 1
    };
public:
    FieldWindow(wxWindow *parent, bool isIndex = false, const wxPoint &pos = wxDefaultPosition, int width = -1);
    virtual ~FieldWindow();
    void AddField(const wxString &fieldName);
    void RemoveField(const wxString &fieldName);
    void SetIndexDirection(const wxString &dir);
    const wxString &GetCurrentFieldLabel() const { return m_currentFieldLabel; }
    virtual void OnLeftDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnMouseMove(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftUp(wxMouseEvent &event) wxOVERRIDE;
    void Clear();
private:
    wxPoint m_startPoint;
    wxSFDiagramManager m_manager;
    bool m_isDragging, m_isIndex;
    FieldWin *m_draggingField, *m_currentField = nullptr;
    wxRect m_initialDraggerPosition;
    std::vector<wxString> m_selectedFields;
    wxString m_currentFieldLabel = "";
};

wxDECLARE_EVENT(wxEVT_FIELD_SHUFFLED, wxCommandEvent);

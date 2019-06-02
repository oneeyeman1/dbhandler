#pragma once
class DesignCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DesignCanvas (wxView *view, const wxPoint &point);
    virtual ~DesignCanvas ();
    void SetQuickQueryFields(const std::vector<wxString> &fields);
    void AddFieldLabelToCanvas(const wxFont labelFont/*, const wxFont *dataFont*/, const Field *label);
    void AddHeaderDivider();
    virtual void OnRightDown(wxMouseEvent &event) wxOVERRIDE;
protected:
    void OnProperties(wxCommandEvent &event);
private:
    wxSFDiagramManager m_pManager;
    wxPoint startPoint;
    MODE m_mode;
    wxView *m_view;
    std::vector<wxString> m_quickQueryFields;
    wxSFShapeBase *m_menuShape;
};


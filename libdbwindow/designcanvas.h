#pragma once
class DesignCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DesignCanvas (wxView *view, const wxPoint &point);
    virtual ~DesignCanvas ();
    void ClearDesignCanvas();
    void SetQuickQueryFields(const std::vector<wxString> &fields);
    void AddFieldLabelToCanvas(const wxFont labelFont, const TableField *label);
    void AddFieldToCanvas(const wxFont dataFont, const TableField *label);
    void InitialFieldSizing();
    void AddHeaderDivider();
    void AddDataDivider();
    virtual void OnRightDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftDown(wxMouseEvent &event) wxOVERRIDE;
//    virtual void OnMouseMove(wxMouseEvent &event) wxOVERRIDE;
    DesignOptions GetOptions() { return m_options; }
    void SetOptions(DesignOptions options) { m_options = options; }
    void PopulateQueryCanvas(const std::vector<TableField *> &queryFields, const std::vector<GroupFields> &groupByFields);
protected:
    void OnProperties(wxCommandEvent &event);
private:
    wxSFDiagramManager m_pManager;
    wxPoint startPoint;
    MODE m_mode;
    wxView *m_view;
    std::vector<wxString> m_quickQueryFields;
    wxSFShapeBase *m_menuShape;
    DesignOptions m_options;
    wxFont m_selectedFont;
};


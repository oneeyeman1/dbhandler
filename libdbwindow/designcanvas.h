#pragma once

class DesignCanvas : public wxSFShapeCanvas, public PropertiesHandler
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
    void ChangeLabel(const wxString &label);
    void ChangeFontName(const wxString &name);
    virtual void OnRightDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnMouseMove(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftDoubleClick(wxMouseEvent &event) wxOVERRIDE;

    virtual int ApplyProperties(const wxAny &any, bool logOnly) wxOVERRIDE;
    virtual wxAny &GetProperties() wxOVERRIDE { return any; }
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
    wxFont m_selectedFont;
    wxString m_libPath;
};


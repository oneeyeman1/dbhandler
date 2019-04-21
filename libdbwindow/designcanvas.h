#pragma once
class DesignCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DesignCanvas (wxView *view);
    virtual ~DesignCanvas ();
    void SetQuickQueryFields(const std::vector<wxString> &fields);
    void AddFieldToCanvas(const wxFont labelFont, const wxFont *dataFont, const Field *label);
private:
    wxSFDiagramManager m_pManager;
    wxPoint startPoint;
    MODE m_mode;
    wxView *m_view;
    std::vector<wxString> m_quickQueryFields;
};


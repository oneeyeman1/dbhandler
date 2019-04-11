#pragma once
class DesignCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DesignCanvas (wxView *view);
    virtual ~DesignCanvas ();
private:
    wxSFDiagramManager m_pManager;
    wxPoint startPoint;
    MODE m_mode;
};


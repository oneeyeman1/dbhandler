#pragma once
class FieldWin : public wxSFRectShape
{
public:
    FieldWin(const wxRealPoint &pt, const wxString &name, wxSFDiagramManager manager);
    virtual ~FieldWin(void);
private:
    wxSFTextShape *m_text;
};


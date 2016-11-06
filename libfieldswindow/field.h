#pragma once
class Field : public wxSFRectShape
{
public:
    Field(const wxRealPoint &pt, const wxString &name, wxSFDiagramManager manager);
    ~Field(void);
private:
    wxSFTextShape *m_text;
};


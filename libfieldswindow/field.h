#pragma once
class FieldWin : public wxSFRectShape
{
public:
    FieldWin(const wxRealPoint &pt, const wxString &name, wxSFDiagramManager manager);
    virtual ~FieldWin(void);
    wxString &GetFieldName();
private:
    wxSFTextShape *m_text;
    wxString m_fieldName;
};


#pragma once
class DesignField : public wxSFRectShape
{
public:
    DesignField();
    DesignField(const wxFont font, const wxString &label);
    virtual ~DesignField();
private:
    wxFont m_font;
    wxString m_label;
    wxSFTextShape *m_text;
    wxSFGridShape *m_grid;
};


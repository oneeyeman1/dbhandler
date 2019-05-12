#pragma once
class DesignLabel : public wxSFRectShape
{
public:
    DesignLabel();
    DesignLabel(const wxFont font, const wxString &label);
    virtual ~DesignLabel();
private:
    wxFont m_font;
    wxString m_label;
    wxSFTextShape *m_text;
    wxSFGridShape *m_grid;
};


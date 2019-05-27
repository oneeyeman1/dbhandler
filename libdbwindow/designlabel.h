#pragma once
class DesignLabel : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(DesignLabel);
    DesignLabel();
    DesignLabel(const wxFont font, const wxString &label);
    virtual ~DesignLabel();
private:
    wxFont m_font;
    wxString m_label;
    wxSFTextShape *m_text;
    wxSFGridShape *m_grid;
    wxString m_name, m_tag;
    bool m_supressPrint;
};


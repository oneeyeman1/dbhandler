#pragma once
struct LabelProperties
{
    wxString m_name, m_tag, m_text, m_cursor;
    bool m_supressPrint;
    int m_border, m_alignment;
    wxFont m_font;
    wxPoint m_position;
    wxSize m_size;
};

class DesignLabel : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(DesignLabel);
    DesignLabel();
    DesignLabel(const wxFont font, const wxString &label);
    virtual ~DesignLabel();
    LabelProperties GetProperties();
protected:
    void InitSerializable();
private:
    wxString m_label;
    wxSFTextShape *m_text;
    wxSFGridShape *m_grid;
    LabelProperties m_prop;
};


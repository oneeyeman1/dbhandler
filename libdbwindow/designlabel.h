#pragma once
/*struct Properties
{
    wxString m_name, m_tag, m_text, m_cursor;
    bool m_supressPrint;
    int m_border, m_alignment;
    wxFont m_font;
    wxPoint m_position;
    wxSize m_size;
};
*/
class DesignLabel : public wxSFRectShape, public PropertiesHandler
{
public:
    XS_DECLARE_CLONABLE_CLASS(DesignLabel);
    DesignLabel();
    DesignLabel(const wxFont font, const wxString &label, int alignment);
    virtual ~DesignLabel();
    virtual wxAny &GetProperties() wxOVERRIDE;
    void Select(bool selected);
    virtual int ApplyProperties(const wxAny &any) wxOVERRIDE;
protected:
    void InitSerializable();
private:
    wxString m_label;
    wxSFTextShape *m_text;
    wxSFGridShape *m_grid;
};


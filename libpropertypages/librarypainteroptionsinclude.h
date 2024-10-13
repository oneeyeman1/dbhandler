#pragma once
class LibraryPainterOptionsInclude : public PropertyPageBase
{
public:
    LibraryPainterOptionsInclude(wxWindow *parent);
private:
    wxStaticText *m_label;
    wxTextCtrl *m_name;
    wxStaticBitmap *m_icons[11];
    wxCheckBox *m_objects[11];
};


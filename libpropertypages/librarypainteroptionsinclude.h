#pragma once
class LibraryPainterOptionsInclude : public PropertyPageBase
{
public:
    LibraryPainterOptionsInclude(wxWindow *parent);
protected:
    void OnName(wxCommandEvent &event);
    void OnEntryChanged(wxCommandEvent &event);
    void PageEdited();
private:
    wxStaticText *m_label;
    wxTextCtrl *m_name;
    wxStaticBitmap *m_icons[11];
    wxCheckBox *m_objects[11];
};


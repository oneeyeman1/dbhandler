#ifndef TYPECOMBOBOX_H
#define TYPECOMBOBOX_H

class WXEXPORT TypeComboBox : public wxComboBox
{
public:
    TypeComboBox(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, const std::wstring &argType);
    ~TypeComboBox();
};

#endif
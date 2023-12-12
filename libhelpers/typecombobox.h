#pragma once

class TypeComboBox : public wxComboBox
{
public:
    TypeComboBox(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, const std::wstring &argType);
};


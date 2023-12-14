#pragma once

#ifdef HELPERS_EXPORT
#define HELPERS_EXPORT __declspec(dllexport)
#else
#define HELPERS_EXPORT __declspec(dllimport)
#endif

class HELPERS_EXPORT TypeComboBox : public wxComboBox
{
public:
    TypeComboBox(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, const std::wstring &argType);
};


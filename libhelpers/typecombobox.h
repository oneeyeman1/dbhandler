#ifndef TYPECOMBOBOX_H
#define TYPECOMBOBOX_H

#ifdef __WXMSW__
#ifndef HELPERS_EXPORT
#define HELPERS_EXPORT __declspec(dllimport)
#else
#define HELPERS_EXPORT __declspec(dllexport)
#endif
#else
#define HELPERS_EXPORT
#endif

class HELPERS_EXPORT TypeComboBox : public wxComboBox
{
public:
    TypeComboBox(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, const std::wstring &argType);
    ~TypeComboBox();
};

#endif
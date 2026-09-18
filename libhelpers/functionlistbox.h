#pragma once

class WXEXPORT FunctionListBox : public wxListBox
{
public:
    FunctionListBox(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, int intType);
};


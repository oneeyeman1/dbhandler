#pragma once
class WXEXPORT SyntaxPropPage :	public wxPanel
{
public:
    SyntaxPropPage(wxWindow *parent);
    ~SyntaxPropPage(void);
    void SetSyntaxText(const wxString &text);
    const wxTextCtrl *GetSyntaxCtrl();
protected:
    void do_layout();
private:
    wxTextCtrl *m_syntax;
    std::wstring m_text;
};

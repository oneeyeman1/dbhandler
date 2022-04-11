#pragma once
class WXEXPORT SyntaxPropPage :	public wxPanel
{
public:
    SyntaxPropPage(wxWindow *parent);
    ~SyntaxPropPage(void);
    void ClearQuery();
    void SetSyntaxText(const wxString &text);
    wxTextCtrl *GetSyntaxCtrl() const;
    void RemoveTableSort(const wxString tbl);
    void RemoveTableFromQuery(const wxString &tbl);
protected:
    void do_layout();
private:
    wxTextCtrl *m_syntax;
    std::wstring m_text;
};


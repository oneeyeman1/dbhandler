#pragma once
class WXEXPORT WhereHavingPage :	public wxPanel
{
public:
    WhereHavingPage(wxWindow *parent);
    ~WhereHavingPage(void);
    void AppendField(const std::wstring &field);
    wxGrid *GetGrid();
    void OnSize(wxSizeEvent &event);
    void OnColumnName(wxGridEditorCreatedEvent &event);
    void OnColumnDropDown(wxCommandEvent &event);
    void OnSelection();
protected:
    void do_layout();
    void set_properties();
private:
    int m_scrollbarWidth, m_operatorSize, m_logicalSize;
    wxGrid *m_grid;
    wxString m_operatorChoices[28], m_logicalChoices[2];
    std::vector<std::wstring> m_fields;
};


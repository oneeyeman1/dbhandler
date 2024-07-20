#pragma once
class WhereHavingLines
{
public:
    int m_row;
    wxString m_old, m_new;
    WhereHavingLines(int row, const wxString &oldValue, const wxString &newValue)
    {
        m_row = row;
        m_old = oldValue;
        m_new = newValue;
    }
};

#ifdef __WXMSW__
#define GRIDROW_ADJUSTMENT 4
#elif __WXOSX__
#define GRIDROW_ADJUSTMENT 6
#elif __WXGTK__
#define GRIDROW_ADJUSTMENT 8
#endif

class QueryArguments
{
public:
    int m_pos;
    wxString m_name, m_type;
    QueryArguments(int pos, const wxString &name, const wxString type) : m_pos(pos), m_name(name), m_type(type) {}
};

class WXEXPORT WhereHavingPage :	public wxPanel
{
public:
    WhereHavingPage(wxWindow *parent, const wxString &type, const wxString &subtype, bool isWhere);
    ~WhereHavingPage(void);
    void AppendField(const std::wstring &field);
    wxGrid *GetGrid();
    void SetQueryArguments(const std::vector<QueryArguments> &arguments);
    void AppendRowsToGrid(int count);
    void OnSize(wxSizeEvent &event);
    void OnCellChanged(wxCommandEvent &event);
    void OnGridCellChaqnged(wxGridEvent &event);
    void OnColumnName(wxGridEditorCreatedEvent &event);
    void OnCellRightClick(wxGridEvent &event);
    void OnMenuSelection(wxCommandEvent &event);
    void OnGridLeftClick(wxGridEvent &event);
    void OnSelection();
protected:
    void do_layout();
    void set_properties();
private:
    int m_scrollbarWidth, m_operatorSize, m_logicalSize, m_row, m_col;
    wxGrid *m_grid;
    wxString m_operatorChoices[28], m_logicalChoices[2], m_type, m_subtype;
    std::vector<std::wstring> m_fields;
    std::vector<WhereHavingLines> m_lines;
    std::vector<QueryArguments> m_arguments;
    bool m_isWherePage;
    wxPoint m_pos;
    wxString m_oldString;
};

#define WHEREPAGECOLUMNS          194
#define WHEREPAGEFUNCTIONS        195
#define WHEREPAGEARGUMENTS        196
#define WHEREPAGEVALUE            197
#define WHEREPAGESELECT           198
#define WHEREPAGECLEAR            199



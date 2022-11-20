#pragma once

struct FieldSorter
{
    wxString m_name;
    bool m_isAscending;
    long m_originalPosition;
    FieldSorter(wxString name, bool isAscending, long original_position) : m_name(name), m_isAscending(isAscending), m_originalPosition(original_position) {};
    FieldSorter &operator=(const FieldSorter &sorter)
    {
        if( m_name == sorter.m_name )
            return *this;
        else
        {
            m_name = sorter.m_name;
            m_isAscending = sorter.m_isAscending;
            m_originalPosition = sorter.m_originalPosition;
            return *this;
        }
    }
};

class QuickSelect : public wxDialog
{
public:
    QuickSelect(wxWindow *parent, const Database *db);
    ~QuickSelect();
    std::vector<TableField *> &GetQueryFields();
    std::vector<FieldSorter> &GetAllSorted() { return m_all; }
    std::vector<FieldSorter> &GetQuerySorted() { return m_query; }
    DatabaseTable *GetDatabaseTable();
    const wxListBox *GetQueryTable();
    void OnOkEnableUI(wxUpdateUIEvent &event);
    void OnAddAllUpdateUI(wxUpdateUIEvent &event);
    void OnSelectingTable(wxMouseEvent &event);
    void OnDisplayComment(wxMouseEvent &event);
protected:
    void do_layout();
    void set_properties();
    void AddFieldToGrid(const wxString &field, bool isAdded);
    void FillTableListBox();
    void OnGridRowLines(wxGridSizeEvent &event);
    void OnFieldSelected(wxCommandEvent &event);
    void OnAllFieldsSelected(wxCommandEvent &event);
    void OnCellLeftClicked(wxGridEvent &event);
    void OnFieldsSetFocus(wxCommandEvent &event);
    void OnOkButton(wxCommandEvent &event);
    void OnSortClosing(wxCommandEvent &event);
    void OnCancelQuickSelect(wxCommandEvent &event);
//    void OnSortClosing(wxGridEvent &event);
private:
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7, *m_comments;
    wxStaticText *m_label10, *m_label11, *m_label12;
    wxListBox *m_tables, *m_fields;
    wxGrid *m_grid;
    wxButton *m_ok, *m_cancel, *m_addAll, *m_help;
    Database *m_db;
    DatabaseTable *m_table;
    std::vector<TableField *> m_queryFields;
    std::vector<FieldSorter> m_all, m_query;
    wxBoxSizer *m_sizer10;
    int m_cols, m_column, m_oldColumn;
    ClientData data;
};

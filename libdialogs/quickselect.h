#pragma once
class QuickSelect : public wxDialog
{
public:
    QuickSelect(wxWindow *parent, const Database *db);
    ~QuickSelect();
    std::vector<TableField *> &GetQueryFields();
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
    wxBoxSizer *m_sizer10;
    int m_cols, m_column, m_oldColumn;
};

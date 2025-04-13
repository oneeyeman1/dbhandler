#pragma once
class CreateIndexSQLServer: public wxDialog
{
public:
    // begin wxGlade: CreateIndexSQLServer::ids
    // end wxGlade

    CreateIndexSQLServer(wxWindow *parent, wxWindowID id, const wxString& title, DatabaseTable *table);
    wxString &GetWhereCondition() { return where; }
    const wxString &GetClusteredChoce() const { return clustered; /*if( m_clustered->GetSelection() == 0 ) return "CLUSTERED"; else return "UNCLUSTERED";*/ }
    wxString &GetWithPredicate() { return with; }
    const std::vector<std::wstring> &GetIncludeVector() const { return m_fields; }
private:
    std::vector<std::wstring> m_tableFields, m_fields;
    std::vector<std::tuple<wxString, wxString, wxString, wxString> > m_whereData;
    wxString with = "WITH ", where = "WHERE ", clustered = "UNCLUSTERED";
protected:
    void OnClusteredUnclustered(wxCommandEvent &event);
    void OnFileStreamSelected(wxCommandEvent &event);
    void OnColumnName(wxGridEditorCreatedEvent &event);
    void OnPadIndex(wxCommandEvent &event);
    void OnOkShowLog(wxCommandEvent &event);
    void OnFieldSelected(wxListEvent &event);
    void OnFieldDeselected(wxListEvent &event);
    void OnFillFactor(wxCommandEvent &event);
    bool Verify();
    void GenerateQuery();
    // begin wxGlade: CreateIndexSQLServer::attributes
    wxPanel *panel_1;
    wxRadioBox *m_clustered;
    FieldWindow *m_includeFields;
    wxListCtrl *m_table;
    wxGrid *m_where;
    wxCheckBox *m_padIndex;
    wxSpinCtrl *m_fillFactor;
    wxCheckBox *m_sortInTemp;
    wxCheckBox *m_ignoreDupKey;
    wxCheckBox *m_noRecompute;
    wxCheckBox *m_incremental;
    wxCheckBox *m_dropExisting;
    wxCheckBox *m_online;
    wxCheckBox *m_resumable;
    wxSpinCtrl *m_maxDuration;
    wxCheckBox *m_rowLocks;
    wxCheckBox *m_pageLocks;
    wxCheckBox *m_optimize;
    wxSpinCtrl *m_maxDOP;
    wxComboBox *m_dataCompression;
    wxCheckBox *checkbox_8;
    wxStaticText *m_label2;
    wxTextCtrl *m_maxDurationWait;
    wxStaticText *m_label3;
    wxComboBox *m_abort;
    wxStaticText *m_label1;
    wxComboBox *m_filestreamType;
    wxTextCtrl *m_name;
    wxButton *m_OK;
    wxButton *m_Cancel;
    wxButton *m_help;
    wxStaticBoxSizer *sizer_17;
    // end wxGlade
}; // wxGlade: end class

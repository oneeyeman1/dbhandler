#pragma once

class WXEXPORT MySQLODBCSetupMeta : public wxPanel
{
public:
    MySQLODBCSetupMeta(wxWindow *parent);
    bool IsChanged() { return m_changed; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    wxCheckBox *m_bigintAsInt, *m_binaryResultsAsChar, *m_tableNameInDescribeCol, *m_disableCatalog, *m_disableSchema, *m_limitColumnSize;
    bool m_changed = false;
};


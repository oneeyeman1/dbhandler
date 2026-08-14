#pragma once

class WXEXPORT MySQLODBCSetupMeta : public wxPanel
{
public:
    MySQLODBCSetupMeta(wxWindow *parent, const std::map<std::wstring, std::wstring> &values);
    bool IsChanged() { return m_changed; }
    wxCheckBox *GetBigInt() const { return m_bigintAsInt; }
    wxCheckBox *GetNoBinary() const { return m_binaryResultsAsChar; }
    wxCheckBox *GetFullName() const { return m_tableNameInDescribeCol; }
    wxCheckBox *GetNoCatalog() const { return m_disableCatalog; }
    wxCheckBox *GetNoSchema() const { return m_disableSchema; }
    wxCheckBox *GetLimitColumn() const { return m_limitColumnSize; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    wxCheckBox *m_bigintAsInt, *m_binaryResultsAsChar, *m_tableNameInDescribeCol, *m_disableCatalog, *m_disableSchema, *m_limitColumnSize;
    bool m_changed = false;
};


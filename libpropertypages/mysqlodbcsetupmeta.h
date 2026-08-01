#pragma once

class WXEXPORT MySQLODBCSetupMeta : public wxPanel
{
public:
    MySQLODBCSetupMeta(wxWindow *parent);
private:
    wxCheckBox *m_bigintAsInt, *m_binaryResultsAsChar, *m_tableNameInDescribeCol, *m_disableCatalog, *m_disableSchema, *m_limitColumnSize;
};


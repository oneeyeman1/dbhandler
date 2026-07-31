#pragma once

class WXEXPORT MySQLODBCSetupGeneric : public wxPanel
{
public:
    MySQLODBCSetupGeneric(wxWindow *parent);
private:
    wxCheckBox *m_bigResultSet, *m_expiredPassword;
};

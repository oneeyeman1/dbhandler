#pragma once

class WXEXPORT MySQLODBCSetupDebug : public wxPanel
{
public:
    MySQLODBCSetupDebug(wxWindow *parent);
    bool IsChanged() { return m_changed; }
    wxCheckBox *GetLogQueries() const { return m_logQueries; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    wxCheckBox *m_logQueries;
    bool m_changed = false;
};

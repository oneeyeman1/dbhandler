#pragma once

class WXEXPORT MySQLODBCSetupCursor : public wxPanel
{
public:
    MySQLODBCSetupCursor(wxWindow *parent);
    bool IsChanged() { return m_changed; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    wxCheckBox *m_dynamicCursor, *m_disableDriverCursor, *m_cacheForwardOnlyCursor, *m_forceForwardOnlyCursor, *m_prefetch;
    wxTextCtrl *m_rows;
    wxStaticText *m_label;
    wxCheckBox *m_matchedRows, *m_autoISNULL, *m_padChar, *m_zeroDate;
    bool m_changed = false;
};


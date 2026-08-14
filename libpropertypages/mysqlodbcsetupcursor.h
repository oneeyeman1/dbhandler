#pragma once

class WXEXPORT MySQLODBCSetupCursor : public wxPanel
{
public:
    MySQLODBCSetupCursor(wxWindow *parent, const std::map<std::wstring, std::wstring> &values);
    bool IsChanged() { return m_changed; }
    wxCheckBox *GetDynamicCursor() const { return m_dynamicCursor; }
    wxCheckBox *GetDriverCursor() const { return m_disableDriverCursor; }
    wxCheckBox *GetCacheCursor() const { return m_cacheForwardOnlyCursor; }
    wxCheckBox *GetForwardOnlyCursor() const { return m_forceForwardOnlyCursor; }
    wxTextCtrl *GetPrefetchRows() const { return m_rows; }
    wxCheckBox *GetMatchedRows() const { return m_matchedRows; }
    wxCheckBox *GetIsNULL() const { return m_autoISNULL; }
    wxCheckBox *GetPadSpace() const { return m_padChar; }
    wxCheckBox *GetZeroDate() const { return m_zeroDate; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    wxCheckBox *m_dynamicCursor, *m_disableDriverCursor, *m_cacheForwardOnlyCursor, *m_forceForwardOnlyCursor, *m_prefetch;
    wxTextCtrl *m_rows;
    wxStaticText *m_label;
    wxCheckBox *m_matchedRows, *m_autoISNULL, *m_padChar, *m_zeroDate;
    bool m_changed = false;
    unsigned long m_value = 0;
};


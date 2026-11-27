#pragma once

class WXEXPORT GridCellFilePickerEditor :  public wxGridCellEditor
{
public:
    GridCellFilePickerEditor(const wxString &message = wxFileSelectorPromptStr, const wxString &wildcards = wxFileSelectorDefaultWildcardStr, long style = wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    void Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler) override;
    void SetSize(const wxRect& rect) override;
    void BeginEdit(int row, int col, wxGrid* grid) override;
    bool EndEdit(int row, int col, const wxGrid* grid, const wxString& oldval, wxString* newval) override;
    void ApplyEdit(int row, int col, wxGrid* grid) override;
    void Reset() override;
    wxString GetValue() const override;
    wxGridCellEditor* Clone() const override;

private:
    wxFilePickerCtrl* GetPicker() const
    {
        return static_cast<wxFilePickerCtrl*>( m_control );
    }

    wxString m_message, m_wildcard, m_startValue, m_value;
    long m_style;
};


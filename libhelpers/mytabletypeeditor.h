#pragma once
class WXEXPORT MyTableTypeEditor : public wxGridCellChoiceEditor
{
public:
    MyTableTypeEditor(const wxString &dbType = "", const wxString dbSubType = "", const wxString &initial = "") : wxGridCellChoiceEditor( 0, nullptr ), m_dbType(dbType), m_dbSubType(dbSubType), m_initial(initial) {}
    virtual ~MyTableTypeEditor() {}
    virtual void Create(wxWindow *parent, wxWindowID id, wxEvtHandler *handler) wxOVERRIDE;
    virtual wxGridActivationResult TryActivate(int row, int col, wxGrid* grid, const wxGridActivationSource& actSource) wxOVERRIDE;
    virtual void DoActivate(int row, int col, wxGrid* grid) wxOVERRIDE
    {
        grid->SetCellValue(row, col, m_value);
    }
    virtual wxGridCellChoiceEditor *Clone() const wxOVERRIDE;
private:
    wxString m_dbType, m_dbSubType, m_initial;
};


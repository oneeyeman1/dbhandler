#pragma once
class MyComboCellEditor : public wxGridCellChoiceEditor
{
public:
    MyComboCellEditor(int count = 0, const wxString choices[] = nullptr)  : wxGridCellChoiceEditor( count, choices) { };
    MyComboCellEditor(const wxArrayString &choices) : wxGridCellChoiceEditor( choices ) { };
    virtual wxGridActivationResult TryActivate(int row, int col, wxGrid* grid, const wxGridActivationSource& actSource) wxOVERRIDE;
    virtual void DoActivate(int row, int col, wxGrid* grid) wxOVERRIDE
    {
        grid->SetCellValue(row, col, m_value);
    }
    virtual MyComboCellEditor *Clone() const wxOVERRIDE
    {
        return new MyComboCellEditor();
    }
};


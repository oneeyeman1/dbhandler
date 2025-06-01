#pragma once
class MyComboCellEditor : public wxGridCellChoiceEditor, public wxGridCellActivatableEditor
{
public:
    virtual wxGridActivationResult TryActivate(int row, int col, wxGrid* grid, const wxGridActivationSource& actSource) wxOVERRIDE;
    virtual void DoActivate(int row, int col, wxGrid* grid) wxOVERRIDE
    {
        grid->SetCellValue(row, col, m_value);
    }
    virtual wxGridCellChoiceEditor *Clone() const wxOVERRIDE;
};


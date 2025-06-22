#pragma once
class WXEXPORT MyComboCellEditor : public wxGridCellChoiceEditor
{
public:
    MyComboCellEditor() : wxGridCellChoiceEditor( 0, nullptr ) {}
    MyComboCellEditor(int n, const wxString choices[]) : wxGridCellChoiceEditor( n, choices ) {}
    MyComboCellEditor(const wxArrayString &choices) : wxGridCellChoiceEditor( choices ) {}
    virtual wxGridActivationResult TryActivate(int row, int col, wxGrid* grid, const wxGridActivationSource& actSource) wxOVERRIDE;
    virtual void DoActivate(int row, int col, wxGrid* grid) wxOVERRIDE
    {
        grid->SetCellValue(row, col, m_value);
    }
    virtual wxGridCellChoiceEditor *Clone() const wxOVERRIDE;
};


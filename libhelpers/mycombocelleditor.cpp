#include <wx/grid.h>
#include <wx/combobox.h>
#include "mycombocelleditor.h"

wxGridActivationResult MyComboCellEditor::TryActivate(int WXUNUSED(row), int WXUNUSED(col), wxGrid *WXUNUSED(grid), const wxGridActivationSource& actSource)
{
    switch ( actSource.GetOrigin() )
    {
    case wxGridActivationSource::Program:
        return wxGridActivationResult::DoNothing();

    case wxGridActivationSource::Key:
        break;

    case wxGridActivationSource::Mouse:
        dynamic_cast<wxComboBox *>( this->wxGridCellChoiceEditor::GetControl() )->Popup();
        // Ideally we should use the mouse event position to determine
        // on which star the user clicked, but for now keep it simple
        // and just cycle through the star value.
        break;
    }

    return wxGridActivationResult::DoChange(m_value);
}

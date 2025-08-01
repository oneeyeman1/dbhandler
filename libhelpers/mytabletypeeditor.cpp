#include <wx/grid.h>
#include <wx/combobox.h>
#include "typecombobox.h"
#include "mytabletypeeditor.h"

wxGridActivationResult MyTableTypeEditor::TryActivate(int row, int col, wxGrid* grid, const wxGridActivationSource& actSource)
{
//    m_control = new TypeComboBox( parent, m_dbType.ToStdWstring(), m_dbSubType.ToStdWstring(), m_initial.ToStdWstring() );
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

wxGridCellChoiceEditor *MyTableTypeEditor::Clone() const
{
    return new MyTableTypeEditor();
}

void MyTableTypeEditor::StartingClick()
{
    wxGridCellEditor::StartingClick();
    dynamic_cast<wxComboBox *>( GetWindow() )->Popup();
}

MyTableTypeEditor::~MyTableTypeEditor()
{
    auto control = GetControl();
    delete control;
    control = nullptr;
}

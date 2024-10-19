#pragma once

class WXEXPORT LibraryPainterOptionsGeneralPage :  public PropertyPageBase
{
public:
    LibraryPainterOptionsGeneralPage(wxWindow *parent, LibraryPainterOptionsGeneral general);
protected:
    void OnPageChange(wxCommandEvent &event);
private:
    wxCheckBox *m_showStatus, *m_showModification, *m_showCompilation, *m_showSizes, *m_showComment;
    wxCheckBox *m_information, *m_obsolete;
    wxCheckBox *m_saveBackups, *m_confirmDelete;
};


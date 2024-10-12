#pragma once

class WXEXPORT LibraryPainterOptionsGeneral :  public PropertyPageBase
{
public:
    LibraryPainterOptionsGeneral(wxWindow *parent);
protected:
    void OnPageChange(wxCommandEvent &event);
private:
    wxCheckBox *m_showStatus, *m_showModification, *m_showCompilation, *m_showSizes, *m_showComment;
    wxCheckBox *m_information, *m_obsolete;
    wxCheckBox *m_saveBacups, *m_confirmDelete;
};


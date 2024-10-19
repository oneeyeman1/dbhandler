#pragma once

class WXEXPORT  LibraryPainterOptionsMgmtPage :  public PropertyPageBase
{
public:
    LibraryPainterOptionsMgmtPage(wxWindow *parent, LibraryPainterOptionsManagement mgmt);
protected:
    void OnLogFileUpdateUI(wxUpdateUIEvent &event);
private:
    wxCheckBox *m_logEverything, *m_requireComments;
    wxFilePickerCtrl *m_logFile;
    wxRadioBox *m_options;
};


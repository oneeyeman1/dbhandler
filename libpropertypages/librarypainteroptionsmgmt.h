#pragma once

class WXEXPORT  LibraryPainterOptionsMgmt :  public PropertyPageBase
{
public:
    LibraryPainterOptionsMgmt(wxWindow *parent);
protected:
    void OnLogFileUpdateUI(wxUpdateUIEvent &event);
private:
    wxCheckBox *m_logEverything, *m_requireComments;
    wxFilePickerCtrl *m_logFile;
};


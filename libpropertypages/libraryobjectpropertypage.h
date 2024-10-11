#pragma once

class WXEXPORT LibraryObjectPropertyPage :  public PropertyPageBase
{
public:
    LibraryObjectPropertyPage(wxWindow *parent, const LibraryProperty &object);
    LibraryObjectPropertyPage(wxWindow *parent, LibraryObjectProperty *object);
protected:
    void OnCommentChange(wxCommandEvent &event);
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5;
    wxStaticText *m_objectName, *m_library, *m_dateCreated, *m_dateModified, *m_dateCompiled, *m_objectSize;
    wxTextCtrl *m_comment;
};


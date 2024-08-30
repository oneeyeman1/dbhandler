#pragma once

class LibraryObjectPropertyPage :  public PropertyPageBase
{
public:
    LibraryObjectPropertyPage(wxWindow *parent, LibraryObject *object);
    LibraryObjectPropertyPage(wxWindow *parent, Object *object);
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5;
    wxStaticText *m_objectName, *m_library, *m_dateCreated, *m_dateModified, *m_dateCompiled, *m_objectSize;
    wxTextCtrl *m_comment;
};


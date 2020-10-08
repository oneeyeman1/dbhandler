//
//  attachdb.hpp
//  libdialogs
//
//  Created by Igor Korot on 10/8/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#ifndef attachdb_hpp
#define attachdb_hpp

class AttachDB : public wxDialog
{
public:
    AttachDB(wxWindow *parent);
protected:
    void OnFileSelected(wxCommandEvent &event);
private:
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2;
    wxFilePickerCtrl *m_picker;
    wxTextCtrl *m_schemaName;
};

#endif /* attachdb_hpp */

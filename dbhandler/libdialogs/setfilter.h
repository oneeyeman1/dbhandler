//
//  setfilter.hpp
//  libdialogs
//
//  Created by Igor Korot on 2/10/21.
//  Copyright © 2021 Igor Korot. All rights reserved.
//

#ifndef setfilter_hpp
#define setfilter_hpp

class SetFilterDialog : public wxDialog
{
public:
    SetFilterDialog(wxWindow *parent);
protected:
    void do_layout();
private:
    wxPanel *m_panel;
    wxTextCtrl *m_filterText;
    wxButton *m_ok, *m_cancel, *m_help, m_verify;
    wxButton *m_less, *m_lessequal, *m_greater, *m_greaterequal, *m_leftbrace, *m_rightbrace;
    wxStaticText *m_label1, *m_label2;
    wxListBox *m_functions, *m_columns;
};

#endif /* setfilter_hpp */

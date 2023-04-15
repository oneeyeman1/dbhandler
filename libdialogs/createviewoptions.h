//
//  createviewoptions.h
//  libdialogs
//
//  Created by Igor Korot on 4/15/23.
//  Copyright © 2023 Igor Korot. All rights reserved.
//

#ifndef createviewoptions_h
#define createviewoptions_h

class CreateViewOptions : public wxDialog
{
public:
    CreateViewOptions(wxWindow *parent);
private:
    wxPanel *m_panel;
    wxCheckBox *m_temp;
    wxButton *m_ok, *m_cancel, *m_help;
};
#endif /* createviewoptions_h */

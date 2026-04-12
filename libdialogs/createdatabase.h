//
//  createdatabase.h
//  libdialogs
//
//  Created by Igor Korot on 6/28/17.
//  Copyright (c) 2017 Igor Korot. All rights reserved.
//

#ifndef __libdialogs__createdatabase__
#define __libdialogs__createdatabase__

class WXEXPORT CreateDatabase : public wxDialog
{
public:
    CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, CreateDBOptions *options);
private:
    wxStaticText *m_label1;
};

#endif /* defined(__libdialogs__createdatabase__) */

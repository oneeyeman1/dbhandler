// -*- C++ -*- generated by wxGlade 0.5 on Sun Feb 10 01:29:26 2008
/***************************************************************************
 *   Copyright (C) 2005 by Igor Korot                                      *
 *   igor@IgorsGentoo                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FONTPROPERTYPAGE_H
#define FONTPROPERTYPAGE_H

class WXEXPORT CFontPropertyPage: public CFontPropertyPageBase
{
public:
    CFontPropertyPage(wxWindow* parent, const FontPropertyPage &font, bool colorEnabled);
    ~CFontPropertyPage() {};
    virtual void SetFont(const std::wstring &name, int size, bool italic, bool bold, bool underline, bool strikethrough) wxOVERRIDE;
    virtual FontPropertyPage &GetFont() wxOVERRIDE;
    bool IsDirty();
protected:
    void OnFontChange(wxCommandEvent &event);
private:
    wxFontPickerCtrl *m_holder;
};

#endif // FONTPROPERTYPAGE_H

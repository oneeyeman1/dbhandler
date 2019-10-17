/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * stylebar.h
 * Copyright (C) 2019 Igor Korot <igor@IgorReinCloud>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _STYLEBAR_H_
#define _STYLEBAR_H_

class WXEXPORT StyleBar: public wxToolBar 
{
public:
    StyleBar(wxWindow *parent, int painter);
    ~StyleBar();
protected:

private:
    wxTextCtrl *m_fieldText;
};

#endif // _STYLEBAR_H_


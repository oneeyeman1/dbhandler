/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tableeditview.h
 * Copyright (C) 2020 Igor Korot <igor@IgorReinCloud>
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

#ifndef _TABLEEDITVIEW_H_
#define _TABLEEDITVIEW_H_

#define wxID_TOP                  401
#define wxID_BACK                 402

class TableEditView: public wxView 
{
public:
    TableEditView() : wxView() {}
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
protected:

private:
    wxBoxSizer *sizer;
    wxDocMDIParentFrame *m_parent;
    wxDocMDIChildFrame *m_frame;
    wxToolBar *m_tb, *m_styleBar;
    wxDECLARE_DYNAMIC_CLASS(TableEditView);
};

#endif // _TABLEEDITVIEW_H_


/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * table-canvas.h
 * Copyright (C) 2017 Unknown <igor@IgorDellGentoo>
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

#ifndef _TABLECANVAS_H_
#define _TABLECANVAS_H_

class TableCanvas: public wxWindow
{
public:
    TableCanvas(wxView *view, const wxPoint &pt, Database *db, DatabaseTable *table = NULL, const wxString &fieldName = wxEmptyString, wxWindow *parent = NULL);
protected:

private:
    wxGrid *m_grid;
    wxStaticText *m_label1, *m_label2, *m_label3;
    wxTextCtrl *m_heading, *m_label, *m_comment;
    wxPanel *m_mainPanel;
};

#endif // _TABLE_CANVAS_H_

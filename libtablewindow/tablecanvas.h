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

#ifndef _DATABASECANVAS_H
#define _DATABASECANVAS_H

#define sfDONT_SAVE_STATE false

#define wxID_TABLECLOSE            20
#define wxID_TABLEALTERTABLE       21
#define wxID_TABLEPROPERTIES       22
#define wxID_TABLENEW              23
//#define wxID_TABLEDROPTABLE        24
#define wxID_TABLEEDITDATA         25
#define wxID_TABLEDATATRANSFER     26
#define wxID_TABLEPRINTDEFINITION  27
#define wxID_VIEWARRANGETABLES     31
#define wxID_VIEWSHOWINDEXKEYS     33
#define wxID_VIEWSHOWINTEGRITY     34
#define wxID_PROPERTIES            50
#define wxID_CREATEDATABASE       100
#define wxID_DELETEDATABASE       101
#define wxID_SELECTTABLE          102
#define wxID_OBJECTNEWTABLE       103
#define wxID_OBJECTNEWINDEX       104
#define wxID_OBJECTNEWVIEW        105
#define wxID_OBJECTNEWFF          106
#define wxID_FIELDDEFINITION      107
#define wxID_EXPORTSYNTAX         108
#define wxID_OBJECTNEWPK          109
#define wxID_STARTLOG             110
#define wxID_STOPLOG              111
#define wxID_SAVELOG              112
#define wxID_CLEARLOG             113
#define wxID_SELECTALLFIELDS      114
#define wxID_DESELECTALLFIELDS    115
#define wxID_ARRANGETABLES        116
#define wxID_SHOWDATATYPES        117
#define wxID_SHOWLABELS           118
#define wxID_SHOWCOMMENTS         119
#define wxID_SHOWSQLTOOLBOX       120
#define wxID_FKDEFINITION         121
#define wxID_FKOPENREFTABLE       122
#define wxID_DROPOBJECT           123
#define wxID_ATTACHDATABASE       124
#define wxID_DETACHDATABASE       125
#define wxID_CUSTOMCOLORS         126
#define wxID_DATABASEOPTIONS      127
#define WHEREPAGECOLUMNS          194
#define WHEREPAGEFUNCTIONS        195
#define WHEREPAGEARGUMENTS        196
#define WHEREPAGEVALUE            197
#define WHEREPAGESELECT           198
#define WHEREPAGECLEAR            199
#define wxID_EDITCUTCOLUMN        200
#define wxID_EDITCOPYCOLUMN       201
#define wxID_EDITPASTECOLUMN      202
#define wxID_EDITINSERTCOLUMN     203
#define wxID_EDITDELETECOLUMN     204
#define wxID_EDITTABLEPROPERTY    205
#define wxID_DESIGNSYNTAX         206
#define wxID_DATASOURCE           301
#define wxID_PREVIEW              302
#define wxID_UNIONS               303
#define wxID_RETRIEVEARGS         304
#define wxID_CHECKOPTION          305
#define wxID_DISTINCT             306
#define wxID_SELECTABOVE          311
#define wxID_SELECTBELOW          312
#define wxID_SELECTLEFT           313
#define wxID_SELECTRIGHT          314
#define wxID_SELECTCOLUMNS        315
#define wxID_SELECTTEXT           316
#define wxID_BRINGTOFRONT         317
#define wxID_SENDTOBACK           318
#define wxID_FORMATCURRENCY       319
#define wxID_FORMATFORMAT         320
#define wxID_SELECTOBJECT         321
#define wxID_TEXTOBJECT           322
#define wxID_PICTUREOBJECT        323
#define wxID_LINEOBJECT           324
#define BOLDFONT                  325
#define ITALICFONT                326
#define UNDERLINEFONGT            327
#define wxID_DESIGNTABORDER       328
#define wxID_COLUMNSPEC           329
#define wxID_PREVIEDWQUERY        330
#define wxID_CONVERTTOSYNTAX      331
#define wxID_SHOWJOINS            332
#define myID_FINDNEXT             333
#define wxID_GOTOLINE             334
#define wxID_CONVERTTOGRAPHICS    335
#define wxID_UNDOALL              336
#define wxID_SAVEQUERY            404
#define wxID_SAVEQUERYAS          405
#define wxID_CUTCOLUMN            501
#define wxID_COPYCOLUMN           502
#define wxID_PASTECOLUMN          503
#define wxID_INSERTCOLUMN         504
#define wxID_DELETECOLUMN         505
#define wxID_LIBRARYNEW           601
#define wxID_LIBRARYDELETE        602
#define wxID_IMPORTLIBRARY        603
#define wxID_LIBRARYSELECTALL     604
#define wxID_EXPANDCOLLAPS        605
#define wxID_SELECTDEVICE         696
#define wxID_LEFTALIGN            1000
#define wxID_CENTERALIGN          1001 
#define wxID_RIGHTALIGN           1002
#define wxID_COMMENTFIELD         1003
#define wxID_FONTNAME             1004
#define wxID_FONTSIZE             1005
#define wxID_JOINS                1006
#endif

#endif // _TABLE_CANVAS_H_


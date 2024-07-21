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

#define wxID_QUERYCANCEL          400
#define wxID_TOP                  402
#define wxID_BACK                 403

class DBTableEdit;

enum ViewType
{
    DatabaseView,
    NewViewView,
    QueryView,
    TableView
};

class TableEditView: public wxView
{
public:
    TableEditView() : wxView() {}
    ~TableEditView();
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual bool OnClose(bool deletedWndow = true) wxOVERRIDE;
    void GetTablesForView(Database *db, bool init);
    void SetToolbarOption(Configuration *conf);
    void SetViewType(ViewType type) { m_type = type; }
protected:
    void CreateMenuAndToolbar();
    void OnClose(wxCommandEvent &event);
private:
    wxBoxSizer *sizer_1;
    wxPanel *m_panel;
    Database *m_db;
    wxBoxSizer *sizer;
    wxDocMDIParentFrame *m_parent;
    wxDocMDIChildFrame *m_frame;
    wxToolBar *m_tb, *m_styleBar;
    wxScrolled<wxPanel> *m_grid;
    DatabaseTable *m_table;
    int m_processed;
    bool m_queryexecuting;
    wxString m_libPath;
    ToolbarSetup m_tbSettings;
    ViewType m_type;
    TableSettngs *attributes;
    int m_currentRow;
    wxDECLARE_DYNAMIC_CLASS(TableEditView);
    wxDECLARE_EVENT_TABLE();
};

#endif // _TABLEEDITVIEW_H_


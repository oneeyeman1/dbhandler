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
#ifndef CODBCCONFIGURE_H
#define CODBCCONFIGURE_H

/**
@author Igor Korot
*/

class CODBCConfigure : public wxDialog
{
public:
    CODBCConfigure(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
    ~CODBCConfigure();

private:
    void OnCreateDSN(wxCommandEvent &event);
    void OnEditDSN(wxCommandEvent &event);
    void OnRemoveDSN(wxCommandEvent &event);
    void OnDriverNameChange(wxCommandEvent &event);
    void OnDsnNameChange(wxCommandEvent &event);
    void set_properties();
    void do_layout();
    std::map<std::wstring, std::vector<std::wstring> > m_driversDSN;
    wxDynamicLibrary *m_lib;
    Database *m_db;

protected:
    void FillDSNComboBox();
    bool EditDsn(WXWidget hwnd, const wxString &driver, const wxString &dsn);
    wxStaticText* label_1;
    wxListBox* m_drivers;
    wxButton* m_OK;
    wxButton* m_Help;
    wxStaticText* label_2;
    wxListBox* m_dsn;
    wxButton* m_createdsn;
    wxButton* m_editdsn;
    wxButton* m_removedsn;
    wxPanel* panel_1;
 };
#endif // ODBCCONFIGURE_H

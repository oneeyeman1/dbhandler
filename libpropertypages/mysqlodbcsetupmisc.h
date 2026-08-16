/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mysqlodbcsetupmisc.h
 * Copyright (C) 2026 Igor Korot <igor@IgorReinCloud>
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

#pragma once

class MySQLODBCSetupMisc: public wxPanel 
{
public:
    MySQLODBCSetupMisc(wxWindow *parent, const std::map<std::wstring, std::wstring> &values);
    wxCheckBox *GetSafeOptions() const { return m_safeOptions; }
    wxCheckBox *GetBindBigInt() const { return m_bindBigInt; }
    wxCheckBox *GetNoLocale() const { return m_noSetLocale; }
    wxCheckBox *GetDateOverflow() const { return m_disableDateOverflow; }
    wxCheckBox *GetIgnoreSpace() const { return m_ignoreSpace; }
    wxCheckBox *GetConfigFile() const { return m_useConfigFile; }
    wxCheckBox *GetNoTransactions() const { return m_noTrans; }
    wxCheckBox *GetMinDateBind() const { return m_bindMinDate; }
    wxCheckBox *GetClientPrepare() const { return m_prepareOnClient; }
    wxCheckBox *GetLoadDataFile() const { return m_enableLoadData; }
    wxDirPickerCtrl *GetLoadDataDir() const { return m_dataLocalDir; }
    bool IsChanged() { return m_changed; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    bool m_changed = false;
    wxCheckBox *m_safeOptions, *m_bindBigInt, *m_noSetLocale, *m_disableDateOverflow, *m_ignoreSpace, *m_useConfigFile;
    wxCheckBox *m_noTrans, *m_bindMinDate, *m_prepareOnClient, *m_enableLoadData;
    wxStaticText *m_label1;
    wxDirPickerCtrl *m_dataLocalDir;
};

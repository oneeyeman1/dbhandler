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
#pragma once

class WXEXPORT MySQLODBCSetupSSL : public wxPanel
{
public:
    MySQLODBCSetupSSL(wxWindow *parent, const std::map<std::wstring, std::wstring> &values);
    wxFilePickerCtrl *GetSSLKey() const { return m_ssl; }
    wxFilePickerCtrl *GetSSLCert() const { return m_sslCert;}
    bool IsChanged() { return m_changed; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    bool m_changed = false;
    wxFilePickerCtrl *m_ssl, *m_sslCert, *m_sslCA, *m_rsaKey, *m_sslCRKFile;
    wxDirPickerCtrl *m_sslPath, *m_sslCRLPath;
    wxTextCtrl *m_sslCipher;
    wxComboBox *m_sslMode;
    wxCheckBox *m_disableTLS2, *m_disableTLS3;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7, *m_label8, *m_label9;
};

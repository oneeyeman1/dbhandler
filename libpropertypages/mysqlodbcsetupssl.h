/***************************************************************************
 *            mysqlodbcsetupssl.h
 *
 *  Fri August 14 14:23:21 2026
 *  Copyright  2026  Unknown
 *  <user@host>
 ****************************************************************************/
/*
 * mysqlodbcsetupssl.h
 *
 * Copyright (C) 2026 - Unknown
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

class WXEXPORT MySQLODBCSetupSSL : public wxPanel
{
public:
     MySQLODBCSetupSSL(wxWindow *parent, const std::map<std::wstring, std::wstring> &values);
};

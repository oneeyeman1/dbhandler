/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dataretriever.h
 * Copyright (C) 2020 Unknown <igor@IgorReinCloud>
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

#ifndef _DATARETRIEVER_H_
#define _DATARETRIEVER_H_

class TableEditView;

class DataRetriever
{
public:
    DataRetriever(TableEditView *handler);
    void SetProcessed(bool processed) { m_processed = processed; }
    bool GetProcessed() const { return m_processed; }
    void DisplayData(const std::vector<DataEditFiield> &row);
    void CompleteDataRetrieval(const std::vector<std::wstring> &errorMessages);
    TableEditView *m_handler;
protected:
private:
    bool m_processed;
};

#endif // _DATARETRIEVER_H_


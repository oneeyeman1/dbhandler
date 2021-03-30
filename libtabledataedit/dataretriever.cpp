/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dataretriever.cpp
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

#include <map>
#include <vector>
#include <algorithm>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/grid.h"
//#include "wx/thread.h"
#include "database.h"
#include "dataretriever.h"
#include "tableeditview.h"

DataRetriever::DataRetriever(TableEditView *handler)
{
    m_handler = handler;
    m_processed = false;
}

void DataRetriever::DisplayData(const std::vector<DataEditFiield> &row)
{
    m_handler->DisplayRecords( row );
//    row.clear();
    m_processed = false;
}

void DataRetriever::CompleteDataRetrieval(const std::vector<std::wstring> &errorMessages)
{
    m_handler->CompleteRetrieval( errorMessages );
}

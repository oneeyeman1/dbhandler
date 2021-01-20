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

class DataRetriever: public wxThread 
{
public:
    DataRetriever(const std::map<long,std::vector<int> > &dataHolder);
protected:

private:
    std::map<long,std::vector<int> > m_dataHolder;
};

#endif // _DATARETRIEVER_H_


#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <string>
#include <algorithm>
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
#include <mutex>
#endif
#include "wx/docmdi.h"
#include "wx/fswatcher.h"
#include "wx/grid.h"
#include "wx/dynlib.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "database.h"
#include "dataretriever.h"
#include "tableeditview.h"
#include "dbtableedit.h"

#if _MSC_VER >= 1900 || !(defined __WXMSW__)
std::mutex Database::Impl::my_mutex;
#endif

DBTableEdit::DBTableEdit(Database *db, const wxString &schema, const wxString &name, DataRetriever *retriever) : wxThread()
{
    m_db = db;
    m_tableName = name;
    m_schemaName = schema;
    m_retriever = retriever;
}

DBTableEdit::~DBTableEdit()
{
}

wxThread::ExitCode DBTableEdit::Entry()
{
    std::vector<std::wstring> errorMsg;
    int res;
    DataRetriever *retriever = m_retriever;
    while( !TestDestroy() )
    {
        {
#if defined __WXMSW__ && _MSC_VER < 1900
            wxCriticalSectionLocker( pCs->m_threadCS );
#else
            std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
#endif
            std::vector<DataEditFiield> row;
            res = m_db->PrepareStatement( m_schemaName.ToStdWstring(), m_tableName.ToStdWstring(), errorMsg );
            if( !res )
            {
                while( !res )
                {
                    row.clear();;
                    res = m_db->EditTableData( row, errorMsg );
                    if( res )
                        break;
                    m_retriever->SetProcessed( true );
                    wxTheApp->CallAfter( [row, retriever]
                    {
                        retriever->DisplayData( row );
                    } );
                    while( m_retriever->GetProcessed() )
                        wxSleep( 2 );
                }
                m_retriever->SetProcessed( false );
                res = m_db->FinalizeStatement( errorMsg );
            }
            else
            {
                m_db->FinalizeStatement( errorMsg );
                Delete();
            }
        }
        wxTheApp->CallAfter( [errorMsg, retriever] 
        {
            retriever->CompleteDataRetrieval( errorMsg );
        } );
        Delete();
    }
    return (wxThread::ExitCode) 0;
}

#include <string>
#include <algorithm>
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
#include <mutex>
#endif
#include "wx/docmdi.h"
#include "wx/fswatcher.h"
#include "wx/dynlib.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "database.h"
#include "dbtableedit.h"

DBTableEdit::DBTableEdit(Database *db, const wxString &schema, const wxString &name)
{
    db = db;
    m_tableName = name;
    m_schemaName = schema;
}

DBTableEdit::~DBTableEdit()
{
}

wxThread::ExitCode DBTableEdit::Entry()
{
    std::vector<std::wstring> errorMsg;
    int res;
    while( !TestDestroy() )
    {
        {
#if defined __WXMSW__ && _MSC_VER < 1900
            wxCriticalSectionLocker( pCs->m_threadCS );
#else
            std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
#endif
            res = m_db->EditTableData( m_schemaName.ToStdWstring(), m_tableName.ToStdWstring(), errorMsg );
            if( res )
                Delete();
        }
        Sleep( 5000 );
    }
    for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); ++it )
        wxMessageBox( (*it) );
    return (wxThread::ExitCode) 0;
}

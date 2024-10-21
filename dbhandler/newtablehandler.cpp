#include <string>
#include <algorithm>
#include <memory>
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
#include <mutex>
#endif
#include "wx/docmdi.h"
#include "wx/fswatcher.h"
#include "wx/dynlib.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "wx/xml/xml.h"
#include "database.h"
#include "configuration.h"
#include "painterobjects.h"
#include "newtablehandler.h"

#if _MSC_VER >= 1900 || !(defined __WXMSW__)
std::mutex Database::Impl::my_mutex;
#endif

NewTableHandler::NewTableHandler(MainFrame *frame, Database *db)
{
    m_db = db;
    pCs = frame;
}

NewTableHandler::~NewTableHandler(void)
{
    std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
    pCs->m_handler = NULL;
}

wxThread::ExitCode NewTableHandler::Entry()
{
    std::vector<std::wstring> errorMsg;
    int res;
    bool loop = true;
    while( !TestDestroy() && loop )
    {
        {
            std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
            res = m_db->NewTableCreation( errorMsg );
            if( res )
                loop = false;
        }
        Sleep( 5000 );
    }
    for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); ++it )
        wxMessageBox( (*it) );
    return (wxThread::ExitCode) 0;
}

#include <string>
#include <algorithm>
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
#include <mutex>
#endif
#include "wx/thread.h"
#include "database.h"
#include "newtablehandler.h"

#if _MSC_VER >= 1900 || !(defined __WXMSW__)
std::mutex Database::Impl::my_mutex;
#endif

NewTableHandler::NewTableHandler(Database *db)
{
    m_db = db;
}

NewTableHandler::~NewTableHandler(void)
{
}

wxThread::ExitCode NewTableHandler::Entry()
{
    std::vector<std::wstring> errorMsg;
    while( !TestDestroy() )
    {
        {
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
            std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
#endif
            m_db->NewTableCreation( errorMsg );
        }
        Sleep( 5000 );
    }
    return (wxThread::ExitCode) 0;
}

#include <string>
#include <algorithm>
#include <mutex>
#include "wx/thread.h"
#include "database.h"
#include "newtablehandler.h"

std::mutex Database::Impl::my_mutex;

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
            std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
            m_db->NewTableCreation( errorMsg );
        }
        Sleep( 5000 );
    }
    return (wxThread::ExitCode) 0;
}

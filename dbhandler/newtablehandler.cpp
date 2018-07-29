#include <string>
#include <algorithm>
#ifndef WIN32
#include <mutex>
#endif
#include "wx/thread.h"
#include "database.h"
#include "newtablehandler.h"

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
#ifdef WIN32
        m_db->my_mutex.Lock();
#else
		{
            std::lock_guard<std::mutex> locker( m_db->my_mutex );
#endif
        m_db->NewTableCreation( errorMsg );
#ifdef WIN32
        m_db->my_mutex.Unlock();
#else
		}
#endif
        Sleep( 5000 );
    }
    return (wxThread::ExitCode) 0;
}

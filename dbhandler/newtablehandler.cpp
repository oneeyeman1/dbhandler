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
        m_db->NewTableCreation( errorMsg );
    }
    return (wxThread::ExitCode) 0;
}

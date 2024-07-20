#ifdef WIN32
#include <windows.h>
#endif
#include <map>
#include <vector>
#include <string>
#include "database.h"

struct Database::Impl
{
    std::vector<Table> m_tables;
};

Database::Database() : pimpl( new Impl )
{
}

Database::~Database()
{
    delete pimpl;
}

void *Database::operator new(std::size_t size)
{
    return ::operator new( size );
}

void Database::operator delete(void *ptr, std::size_t size)
{
	return ::operator delete( ptr );
}

Database::Impl &Database::GetTableVector()
{
    return *pimpl;
}

int Database::Connect(const char *selectedDSN, std::vector<std::wstring> &errorMsg)
{
    selectedDSN = selectedDSN;
    errorMsg = errorMsg;
    return 0;
}
/*
int Database::ConnectODBC(const SQLWCHAR &selectedDSN, const SQLWCHAR &driver, std::vector<std::wstring> &errorMsg, HWND hwnd)
{
    return 0;
}*/
/*
Database &Database::operator=(Database &other)
{
    if( this != &other )
        pimpl = other.pimpl;
}
*/
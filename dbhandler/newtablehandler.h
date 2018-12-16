#pragma once

#include "mainframe.h"

class NewTableHandler :	public wxThread
{
public:
    NewTableHandler(MainFrame *frame, Database *db);
    ~NewTableHandler(void);
protected:
    virtual ExitCode Entry();
private:
    Database *m_db;
    MainFrame *pCs;
};


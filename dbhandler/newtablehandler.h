#pragma once
class NewTableHandler :	public wxThread
{
public:
    NewTableHandler(Database *db);
    ~NewTableHandler(void);
protected:
    virtual ExitCode Entry();
private:
    Database *m_db;
};


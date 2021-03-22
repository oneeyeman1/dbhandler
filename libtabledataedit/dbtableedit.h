#pragma once

class DBTableEdit : public wxThread
{
public:
    DBTableEdit(Database *db, const wxString &schema, const wxString &name, DataRetriever *retriever);
    ~DBTableEdit();
protected:
    virtual ExitCode Entry() wxOVERRIDE;
private:
    Database *m_db;
    wxString m_tableName, m_schemaName;
};
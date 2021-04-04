#pragma once

class DBTableEdit : public wxThread
{
public:
    DBTableEdit(Database *db, const wxString &schema, const wxString &name, DataRetriever *retriever);
    ~DBTableEdit();
    void CancelProcessing() { m_continueProcessing = false; };
protected:
    virtual ExitCode Entry() wxOVERRIDE;
private:
    Database *m_db;
    wxString m_tableName, m_schemaName;
    DataRetriever *m_retriever;
    bool m_continueProcessing;
};
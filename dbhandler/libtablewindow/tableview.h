#pragma once
// The view using MyCanvas to show its contents
class TableView : public wxView
{
public:
    TableView() : wxView() {}
//    std::vector<Table> &GetTablesForView(Database *db);
    void GetTablesForView(Database *db);
    DatabaseTable *GetDatabaseTable();
    wxDocMDIChildFrame *GetChildFrame();
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow = true) wxOVERRIDE;
    void OnViewSelectedTables(wxCommandEvent &event);
    void OnFieldDefinition(wxCommandEvent &event);
    void OnFieldProperties(wxCommandEvent &event);
    void OnLogUpdateUI(wxUpdateUIEvent &event);
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    TableDocument* GetDocument();
protected:
    void CreateMenu();
private:
    bool m_isCreated;
    wxToolBar *m_tb;
    wxDocMDIChildFrame *m_frame;
    DatabaseTable *m_table;
    wxString m_libPath;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(DrawingView);
};

wxDECLARE_EVENT(wxEVT_SET_TABLE_PROPERTY, wxCommandEvent);

#define wxID_DATABASEWINDOW 2

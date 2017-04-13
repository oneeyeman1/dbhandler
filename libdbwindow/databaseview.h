#ifndef __DATABASEVIEW__H
#define __DATABASEVIEW__H

enum ViewType
{
    DatabaseView,
    QueryView
};

// The view using MyCanvas to show its contents
class DrawingView : public wxView
{
public:
    DrawingView() : wxView(), m_canvas(NULL) {}
//    std::vector<Table> &GetTablesForView(Database *db);
    void GetTablesForView(Database *db);
    void SetViewType(ViewType type);
    ViewType GetViewType();
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow = true) wxOVERRIDE;
    void OnViewSelectedTables(wxCommandEvent &event);
    void OnNewIndex(wxCommandEvent &event);
    void OnFieldDefinition(wxCommandEvent &event);
    void OnFieldProperties(wxCommandEvent &event);
    void OnSetProperties(wxCommandEvent &event);
    void OnCloseLogWindow(wxCloseEvent &event);
    void OnForeignKey(wxCommandEvent &event);
    void OnLogUpdateUI(wxUpdateUIEvent &event);
    void OnStartLog(wxCommandEvent &event);
    void OnStopLog(wxCommandEvent &event);
    void OnSaveLog(wxCommandEvent &event);
    void OnClearLog(wxCommandEvent &event);
    void OnAlterTable(wxCommandEvent &event);
    void OnFieldDefinition(wxCommandEvent &event);
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    DrawingDocument* GetDocument();

private:
    wxToolBar *m_tb;
    wxFrame *m_log;
    wxTextCtrl *m_text;
    DatabaseCanvas *m_canvas;
	bool m_isCreated;
	ViewType m_type;
    wxDocMDIChildFrame *m_frame;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(DrawingView);
};

wxDECLARE_EVENT(wxEVT_SET_TABLE_PROPERTY, wxCommandEvent);

#define wxID_DATABASEWINDOW 2

#endif
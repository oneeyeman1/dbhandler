// The window showing the drawing itself
class MyCanvas : public wxScrolledWindow
{
public:
    // view may be NULL if we're not associated with one yet, but parent must
    // be a valid pointer
    MyCanvas(wxView *view, wxWindow *parent = NULL);
    virtual ~MyCanvas();

    virtual void OnDraw(wxDC& dc) wxOVERRIDE;

    // in a normal multiple document application a canvas is associated with
    // one view from the beginning until the end, but to support the single
    // document mode in which all documents reuse the same MyApp::GetCanvas()
    // we need to allow switching the canvas from one view to another one

    void SetView(wxView *view)
    {
        wxASSERT_MSG( !m_view, "shouldn't be already associated with a view" );

        m_view = view;
    }

    void ResetView()
    {
        wxASSERT_MSG( m_view, "should be associated with a view" );

        m_view = NULL;
    }

private:
    void OnMouseEvent(wxMouseEvent& event);

    wxView *m_view;

    // the segment being currently drawn or NULL if none
    DoodleSegment *m_currentSegment;

    // the last mouse press position
    wxPoint m_lastMousePos;

    wxDECLARE_EVENT_TABLE();
};

// The view using MyCanvas to show its contents
class DrawingView : public wxView
{
public:
    DrawingView() : wxView() {}
//    std::vector<Table> &GetTablesForView(Database *db);
    void GetTablesForView(Database *db);
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
    DrawingDocument* GetDocument();

private:
    wxToolBar *m_tb;
    wxDocMDIChildFrame *m_frame;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(DrawingView);
};

wxDECLARE_EVENT(wxEVT_SET_TABLE_PROPERTY, wxCommandEvent);

#define wxID_DATABASEWINDOW 2

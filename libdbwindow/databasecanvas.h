#ifndef _DATABASECANVAS_H
#define _DATABASECANVAS_H

#define sfDONT_SAVE_STATE false

class WXEXPORT DatabaseCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DatabaseCanvas(wxView *view, wxWindow *parent = NULL);
    void DisplayTables(const std::vector<wxString> &selections);
    virtual ~DatabaseCanvas();
    virtual void OnLeftDown(wxMouseEvent &event);
    virtual void OnRightDown(wxMouseEvent &event);
    void OnDropTable(wxCommandEvent &event);
    inline wxSFDiagramManager &GetDiagramManager() { return m_pManager; }
    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
protected:
    bool IsTableDisplayed(const std::wstring &name);
private:
    wxView *m_view;
    wxSFDiagramManager m_pManager;
    bool m_showComments, m_showIndexKeys, m_showIntegrity;
    wxSFShapeBase *m_selectedShape;
    MODE m_mode;
    wxPoint startPoint;
};

#define wxID_TABLECLOSE           20
#define wxID_TABLEALTERTABLE      21
#define wxID_TABLEPROPERTIES      22
#define wxID_TABLENEW             23
#define wxID_TABLEDROPTABLE       24
#define wxID_TABLEEDITDATA        25
#define wxID_TABLEDATATRANSFER    26
#define wxID_TABLEPRINTDEFINITION 27
#define wxID_VIEWARRANGETABLES    31
#define wxID_VIEWSHOWCOMMENTS     32
#define wxID_VIEWSHOWINDEXKEYS    33
#define wxID_VIEWSHOWINTEGRITY    34
#define wxID_SELECTTABLE          102
#define wxID_OBJECTNEWTABLE       103
#define wxID_OBJECTNEWINDEX       104
#define wxID_OBJECTNEWVIEW        105
#define wxID_OBJECTNEWFF          106

#endif
#pragma once

class GridRowLabelRenderer : public wxGridRowHeaderRenderer
{
public:
    GridRowLabelRenderer ()
    {
    }

    virtual void DrawLabel (const wxGrid &grid, wxDC &dc, const wxString &value, const wxRect &rect, int horizAlign, int vertAlign, int WXUNUSED(textOrientation)) const wxOVERRIDE
    {
        wxColour color = grid.GetParent()->GetBackgroundColour();
        dc.SetTextBackground( color );
        dc.SetTextForeground( *wxBLACK );
        wxFont font = dc.GetFont();
        font.SetWeight( wxFONTWEIGHT_NORMAL );
        dc.DrawLabel( value, rect, horizAlign | vertAlign );
    }

    virtual void DrawBorder (const wxGrid &WXUNUSED(grid), wxDC &dc, wxRect &rect) const wxOVERRIDE
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        dc.DrawRectangle( rect );
    }
    wxDECLARE_NO_COPY_CLASS( GridRowLabelRenderer );
};

class CustomRowHeaderProvider : public wxGridCellAttrProvider
{
public:
    CustomRowHeaderProvider () : m_customRowLabelRenderer ()
    {
    }
protected:
    virtual const wxGridRowHeaderRenderer &GetRowHeaderRenderer (int WXUNUSED(row)) wxOVERRIDE
    {
        return m_customRowLabelRenderer;
    }
private:
    GridRowLabelRenderer m_customRowLabelRenderer;
    wxDECLARE_NO_COPY_CLASS( CustomRowHeaderProvider );
};

class GridCornerHeaderRenderer : public wxGridCornerHeaderRenderer
{
public:
    GridCornerHeaderRenderer()
    {
    }

    virtual void DrawBorder(const wxGrid &WXUNUSED(grid), wxDC &dc, wxRect &rect) const wxOVERRIDE
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        dc.DrawRectangle( rect );
    }
    wxDECLARE_NO_COPY_CLASS( GridCornerHeaderRenderer );
};

class CustomCornerHeaderProvider : public wxGridCellAttrProvider
{
public:
    CustomCornerHeaderProvider() : m_customCornerLabelRenderer()
    {
    }
protected:
    virtual const wxGridCornerHeaderRenderer &GetCornerRenderer() wxOVERRIDE
    {
        return m_customCornerLabelRenderer;
    }
private:
    GridCornerHeaderRenderer m_customCornerLabelRenderer;
    wxDECLARE_NO_COPY_CLASS( CustomCornerHeaderProvider );
};

class QuickSelect : public wxDialog
{
public:
    QuickSelect(wxWindow *parent, const Database *db);
    ~QuickSelect();
    void OnOkEnableUI(wxUpdateUIEvent &event);
    void OnAddAllUpdateUI(wxUpdateUIEvent &event);
    void OnSelectingTable(wxCommandEvent &event);
protected:
    void do_layout();
    void set_properties();
    void FillTableListBox();
private:
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7, *m_comments;
    wxListBox *m_tables, *m_fields;
    wxGrid *m_grid;
    wxButton *m_ok, *m_cancel, *m_addAll, *m_help;
    Database *m_db;
};

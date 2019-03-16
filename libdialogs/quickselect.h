#pragma once

class GridRowLabelRenderer : public wxGridRowHeaderRenderer
{
public:
    GridRowLabelRenderer ()
    {
    }

    virtual void DrawLabel (const wxGrid &WXUNUSED(grid), wxDC &dc, const wxString &value, const wxRect &rect, int horizAlign, int vertAlign, int textOrientation) const wxOVERRIDE
    {
        dc.SetTextBackground( COLOR_BTNFACE/*wxSystemSettings::GetColour()*/ );
        dc.DrawLabel( value, rect, horizAlign | vertAlign );
    }

    virtual void DrawBorder (const wxGrid &WXUNUSED(grid), wxDC &dc, wxRect &rect) const wxOVERRIDE
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.SetBrush( wxBrush( COLOR_BTNFACE ) );
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
    virtual const wxGridRowHeaderRenderer &GetRowHeaderRenderer (int row) wxOVERRIDE
    {
        return m_customRowLabelRenderer;
    }
private:
    GridRowLabelRenderer m_customRowLabelRenderer;
    wxDECLARE_NO_COPY_CLASS( CustomRowHeaderProvider );
};

class QuickSelect : public wxDialog
{
public:
    QuickSelect(wxWindow *parent, const Database *db);
    ~QuickSelect();
    void OnOkEnableUI(wxUpdateUIEvent &event);
    void OnAddAllUpdateUI(wxUpdateUIEvent &event);
protected:
    void do_layout();
    void set_properties();
private:
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7, *m_comments;
    wxListBox *m_tables, *m_fields;
    wxGrid *m_grid;
    wxButton *m_ok, *m_cancel, *m_addAll, *m_help;
    Database *m_db;
};

#pragma once
class WXEXPORT ScrolledColumnLabel :  public wxWindow
{
public:
    ScrolledColumnLabel(wxScrolled<wxWindow> *parent, const std::vector<wxString> &labels);
    void SetLabelPositions(const std::vector<wxPoint> &pos) { m_pos = pos; Refresh(); };
private:
    void OnPaint(wxPaintEvent& WXUNUSED(event));

    wxScrolled<wxWindow> *m_owner;
    std::vector<wxString> m_labels;
    std::vector<wxPoint> m_pos;
};

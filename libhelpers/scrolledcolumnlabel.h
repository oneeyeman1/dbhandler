#pragma once
class WXEXPORT ScrolledColumnLabel :  public wxWindow
{
public:
    ScrolledColumnLabel(wxScrolled<wxWindow> *parent, const std::vector<wxString> &labels);

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event));

    wxScrolled<wxWindow> *m_owner;
    std::vector<wxString> m_labels;
};

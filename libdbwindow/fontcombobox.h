#pragma once

class FontComboBox : public wxBitmapComboBox
{
public:
    FontComboBox(wxWindow *parent);
protected:
    void PopuateFontNames();
#ifdef __WXMSW__
    static int CALLBACK EnumFontFamiliesCallback(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *, int FontType, LPARAM lParam);
#endif
private:
#ifndef __WXMSW__
    class MyFontEnumerator : public wxFontEnumerator
    {
    public:
        bool GotAny() const
        { return !m_facenames.IsEmpty(); }

        const wxArrayString& GetFacenames() const
        { return m_facenames; }

    protected:
        virtual bool OnFacename(const wxString& facename) wxOVERRIDE
        {
            m_facenames.Add(facename);
            return true;
        }

    private:
        wxArrayString m_facenames;
    };
    MyFontEnumerator m_enumerator;
#endif
};
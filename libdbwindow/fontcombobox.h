#pragma once

class FontComboBox : public wxComboBox
{
public:
    FontComboBox(wxWindow *parent);
private:
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
};
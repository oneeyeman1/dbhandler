#pragma once

struct Profile
{
    wxString m_name;
    bool m_isCurrent;
    
    Profile(wxString name, bool isCurrent) : m_name( name ), m_isCurrent( isCurrent ) {}
};

class QueryArguments
{
public:
    unsigned long m_pos;
    wxString m_name, m_type;
    QueryArguments(unsigned long pos, const wxString &name, const wxString type) : m_pos(pos), m_name(name), m_type(type) {}
};

struct ClientData
{
    std::wstring catalog, schema;
    ClientData() : catalog( L"" ), schema( L"" ) {}
    ClientData(const std::wstring &catalogName, const std::wstring &schemaName) : catalog( catalogName ), schema( schemaName ) {}
};

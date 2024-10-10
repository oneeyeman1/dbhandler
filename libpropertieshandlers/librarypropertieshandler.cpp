// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <vector>
#include "wx/xml/xml.h"
#include "wx/any.h"
#include "wx/datetime.h"
#include "wx/wfstream.h"
#include "painterobjects.h"
#include "propertieshandlerbase.h"
#include "librarypropertieshandler.h"

LibraryPropertiesHandler::LibraryPropertiesHandler(LibraryProperty prop)
{
    m_properties = prop;
}

wxAny LibraryPropertiesHandler::GetProperties(std::vector<std::wstring> &WXUNUSED(errors))
{
    return m_obj;
}

int LibraryPropertiesHandler::ApplyProperties()
{
    wxXmlDocument doc;
    wxXmlNode *root = new wxXmlNode( nullptr, wxXML_ELEMENT_NODE, "Library" );
    doc.SetRoot( root );
    wxXmlNode *header = new wxXmlNode( root, wxXML_ELEMENT_NODE, "Header" );
    wxXmlNode *libCreated = new wxXmlNode( header, wxXML_ELEMENT_NODE, "Created" );
    libCreated->AddChild( new wxXmlNode( wxXML_TEXT_NODE, "", m_properties.m_created.FormatISOCombined() ) );
    wxXmlNode *comment = new wxXmlNode( header, wxXML_ELEMENT_NODE, "Comment" );
    comment->AddChild( new wxXmlNode( wxXML_TEXT_NODE, "", m_properties.m_comment ) );
    wxFileOutputStream stream( m_properties.m_name );
    doc.Save( stream );
    return 0;
}

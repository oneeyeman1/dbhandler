// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif
#include <string>
#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/docview.h"
#if wxUSE_STD_IOSTREAM
#include "wx/ioswrap.h"
#else
#include "wx/txtstrm.h"
#endif
#include "propertieshandlerbase.h"
#include "painterobjects.h"
#include "librarydocument.h"

// ----------------------------------------------------------------------------
// DrawingDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(LibraryDocument, wxDocument);

LibraryDocument::~LibraryDocument()
{
//    m_successfulLoad = false;
}

DocumentOstream &LibraryDocument::SaveObject(DocumentOstream &ostream)
{
#if wxUSE_STD_IOSTREAM
    DocumentOstream &stream = ostream;
#else
    wxTextOutputStream stream( ostream );
#endif
//    m_querySaveSuccessfl = ((DrawingView *) GetFirstView() )->GetDatabaseCanvas()->GetDiagramManager().SerializeToXml( GetFilename(), xsWITH_ROOT );
    wxDocument::SaveObject( stream );

    return ostream;
}

DocumentIstream &LibraryDocument::LoadObject(DocumentIstream &istream)
{
#if wxUSE_STD_IOSTREAM
    DocumentIstream &stream = istream;
#else
    wxTextInputStream stream( istream );
#endif
    std::vector<std::wstring> errors;
/*    auto result = ((DrawingView *) GetFirstView() )->GetDatabaseCanvas()->GetDiagramManager().DeserializeFromXml( GetFilename() );
    if( result )
    {
        auto tables = dynamic_cast<QueryRoot *>( ((DrawingView *) GetFirstView() )->GetDatabaseCanvas()->GetDiagramManager().GetRootItem() )->GetTables();
        for( auto name : tables )
        {
            wxString catalog, schema, table;
            if( m_db->GetTableVector().m_type == L"SQLite" )
            {
                catalog = "";
                schema = name.substr( 0, name.find( '.' ) );
                table = name.substr( name.find( '.' ) + 1 );
            }
            else
            {
                catalog = name.substr( 0, name.find( '.' ) );
                schema = name.substr( name.find( '.' ) + 1, name.rfind( '.' ) );
                table = name.substr( name.rfind( '.' ) );
            }
            m_tableNames.push_back( table.ToStdWstring() );
            if( m_db->AddDropTable( catalog.ToStdWstring(), schema.ToStdWstring(), table.ToStdWstring(), errors ) )
                for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); ++it )
                    wxMessageBox( (*it) );
        }
        m_successfulLoad = true;
    }
*/
    wxDocument::LoadObject( stream );
    return istream;
}


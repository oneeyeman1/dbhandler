/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tableeditview.cc
 * Copyright (C) 2020 Igor Korot <igor@IgorReinCloud>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include <map>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/artprov.h"
#include "wx/grid.h"
#include "database.h"
#include "dataretriever.h"
#include "dbtableedit.h"
#include "tableeditdocument.h"
#include "tableeditview.h"

wxIMPLEMENT_DYNAMIC_CLASS(TableEditView, wxView);

bool TableEditView::OnCreate(wxDocument *doc, long flags)
{
    m_processed = 0;
    wxToolBar *tb = nullptr;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    m_parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    wxRect clientRect = m_parent->GetClientRect();
    wxWindowList children = m_parent->GetChildren();
    for( wxWindowList::iterator it = children.begin(); it != children.end(); it++ )
    {
        tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "ViewBar" )
        {
            m_tb = tb;
        }
        if( tb && tb->GetName() == "StyleBar" )
            m_styleBar = tb;
    }
    const wxString tableName = dynamic_cast<TableEditDocument *>( doc )->GetTableName();
    wxString title = "Data Management for " + tableName;
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, title, wxDefaultPosition, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    wxPoint ptCanvas;
    sizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    wxRect parentRect = m_parent->GetRect();
    wxSize parentClientSize = m_parent->GetClientSize();
    m_frame->Move( 0, parentRect.y - parentClientSize.y );
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->SetSize( pt.x, pt.y, parentRect.GetWidth(), parentClientSize.GetHeight() );
    m_tb = m_frame->CreateToolBar();
    m_tb->AddTool( wxID_TOP, _( "Top" ), wxArtProvider::GetBitmap( wxART_GOTO_FIRST ), _( "Top" ), wxITEM_NORMAL );
    m_tb->AddTool( wxID_BACK, _( "Back" ), wxArtProvider::GetBitmap( wxART_GO_BACK ), _( "Back" ), wxITEM_NORMAL );
    m_tb->Realize();
    int offset = m_tb->GetSize().y;
    ptCanvas.x = 0;
    ptCanvas.y = offset;
#else
    ptCanvas = wxDefaultPosition;
#endif
    bool found = false;
    m_db = dynamic_cast<TableEditDocument *>( doc )->GetDatabase();
    DatabaseTable *table = nullptr;
    for( auto it = m_db->GetTableVector().m_tables.begin(); it != m_db->GetTableVector().m_tables.end() && !found; ++ it )
        for( auto it1 = (*it).second.begin(); it1 < (*it).second.end() && !found; ++it1 )
            if( (*it1)->GetTableName() == tableName )
            {
                table = (*it1);
                found = true;
            }
    wxASSERT( m_frame == GetFrame() );
    m_grid = new wxGrid( m_frame, wxID_ANY );
    m_grid->CreateGrid( 0, 0 );
    for( int i = 0; i < table->GetNumberOfFields(); i++ )
    {
        m_grid->AppendCols();
        wxString label( table->GetFields().at( i )->GetFieldName() );
        m_grid->SetColLabelValue( i, label );
    }
    m_grid->HideRowLabels();
    sizer->Add( m_grid, 1, wxEXPAND, 0 );
    m_frame->SetSizer( sizer );
#ifndef __WXOSX__
    wxSize size = m_parent->GetClientSize();
    m_tb->ClearTools();
    m_tb->AddTool( wxID_TOP, _( "Top" ), wxArtProvider::GetBitmap( wxART_GOTO_FIRST ), _( "Top" ), wxITEM_NORMAL );
    m_tb->AddTool( wxID_BACK, _( "Back" ), wxArtProvider::GetBitmap( wxART_GO_BACK ), _( "Back" ), wxITEM_NORMAL );
    m_tb->Realize();
    wxMDIClientWindow *frame = (wxMDIClientWindow *) m_parent->GetClientWindow();
    m_tb->SetSize( 0, 0, size.x, wxDefaultCoord );
    int offset = m_tb->GetSize().y;
    frame->SetSize( 0, offset, size.x, size.y - offset );
    m_frame->SetSize( 0, 0, size.x, size.y - offset - 2 );
#endif
    sizer->Layout();
    m_frame->Layout();
    m_frame->Show();
    Bind( wxEVT_THREAD, &TableEditView::ThreadEventHandler, this );
    m_retriever = new DataRetriever( this );
    m_handler = new DBTableEdit( m_db, table->GetSchemaName(), table->GetTableName(), m_retriever );
    m_grid->BeginBatch();
    if( m_handler->Run() != wxTHREAD_NO_ERROR )
    {
        wxMessageBox( _( "Internal error. Try to clean some memory and try again!" ) );
        delete m_handler;
        m_handler = NULL;
    }
    return true;
}

TableEditView::~TableEditView()
{
    delete m_retriever;
    m_retriever = nullptr;
    if( m_handler )
    {
        delete m_handler;
        m_handler = nullptr;
    }
}

void TableEditView::OnDraw(wxDC *dc)
{
}

void TableEditView::ThreadEventHandler(wxThreadEvent &event)
{
    {
#if defined __WXMSW__ && _MSC_VER < 1900
        wxCriticalSectionLocker( pCs->m_threadCS );
#else
        std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
#endif
        if( m_handler )         // does the thread still exist?
        {
            if( m_handler->Delete() != wxTHREAD_NO_ERROR )
                wxLogError( "Can't delete the thread!" );
        }
    }       // exit from the critical section to give the thread
        // the possibility to enter its destructor
        // (which is guarded with m_pThreadCS critical section!)
    while( 1 )
    {
        { // was the ~MyThread() function executed?
#if defined __WXMSW__ && _MSC_VER < 1900
            wxCriticalSectionLocker( pCs->m_threadCS );
#else
            std::lock_guard<std::mutex> locker( m_db->GetTableVector().my_mutex );
#endif
            if( !m_handler ) break;
        }
    // wait for thread completion
        wxThread::This()->Sleep( 1 );
    }
}

void TableEditView::DisplayRecords(const std::vector<DataEditFiield> &row)
{
    int i = 0;
    m_grid->AppendRows();
    for( std::vector<DataEditFiield>::const_iterator it = row.begin(); it < row.end(); ++it )
    {
        if( (it)->type == STRING_TYPE )
        {
            wxString temp( (it)->value.stringValue );
            m_grid->SetCellValue( m_processed, i++, temp );
        }
    }
    wxString temp = wxString::Format( "Press Cancel to stop retrieval. Rows retrieved: %ld", ++m_processed );
    m_parent->SetStatusText( temp, 0 );
}

void TableEditView::CompleteRetrieval(const std::vector<std::wstring> &errorMessages)
{
    for( std::vector<std::wstring>::const_iterator it = errorMessages.begin(); it < errorMessages.end(); ++it )
        wxMessageBox( (*it) );
    m_grid->EndBatch();
}

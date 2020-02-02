//
//  sortcolumnrenderer.m
//  libpropertypages
//
//  Created by Igor Korot on 2/1/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/NSButtonCell.h>
#include "wx/stattext.h"
#include "wx/listctrl.h"
#include "wx/osx/cocoa/dataview.h"
#include "wx/renderer.h"
#include "sortgroupbypage.h"

NSTextAlignment ConvertToNativeHorizontalTextAlignment(int alignment)
{
    if( alignment & wxALIGN_CENTER_HORIZONTAL )
        return NSCenterTextAlignment;
    else if( alignment & wxALIGN_RIGHT )
        return NSRightTextAlignment;
    else
        return NSLeftTextAlignment;
}

SortColumnRenderer::SortColumnRenderer(wxCheckBoxState state, wxDataViewCellMode mode, int align)
: wxDataViewRenderer( GetDefaultType(), mode, mode ), m_checkedState( state )
{
    m_allow3rdStateForUser = false;
    NSButtonCell* cell;
    cell = [[NSButtonCell alloc] init];
    [cell setAlignment:ConvertToNativeHorizontalTextAlignment( GetAlignment() )];
    [cell setButtonType: NSSwitchButton];
    [cell setImagePosition:NSImageLeft];
    [cell setAllowsMixedState:YES];
    SetNativeData(new wxDataViewRendererNativeData( cell ) );
    [cell release];
}

bool SortColumnRenderer::MacRender()
{
    NSButtonCell* cell = (NSButtonCell*) GetNativeData()->GetItemCell();
    int nativecbvalue = 0;
    switch( GetCheckedState() )
    {
        case wxCHK_CHECKED:
            nativecbvalue = 1;
            break;
        case wxCHK_UNDETERMINED:
            nativecbvalue = -1;
            break;
        case wxCHK_UNCHECKED:
            nativecbvalue = 0;
            break;
    }
    [cell setIntValue:nativecbvalue];
    [cell setTitle:wxCFStringRef( m_value ).AsNSString()];
    return true;
}

long ObjectToLong(NSObject *object)
{
    wxCHECK_MSG( [object isKindOfClass:[NSNumber class]], -1,
                wxString::Format
                (
                 "number expected but got %s",
                 wxCFStringRef::AsString([object className])
                 ));
    
    return [(NSNumber *)object longValue];
}

void SortColumnRenderer::OSXOnCellChanged(NSObject *value, const wxDataViewItem& item, unsigned col)
{
    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    // The icon can't be edited so get its old value and reuse it.
    wxVariant valueOld;
    model->GetValue( valueOld, item, col );
    
    wxDataViewCheckIconText checkIconText;
    checkIconText << valueOld;
    
    wxCheckBoxState checkedState ;
    switch( ObjectToLong(value) )
    {
        case 1:
            checkedState = wxCHK_CHECKED;
            break;
            
        case 0:
            checkedState = wxCHK_UNCHECKED;
            break;
            
        case -1:
            checkedState = m_allow3rdStateForUser ? wxCHK_UNDETERMINED : wxCHK_CHECKED;
            break;
    }
    
    checkIconText.SetCheckedState( checkedState );
    
    model->ChangeValue( checkedState, item, col );
}

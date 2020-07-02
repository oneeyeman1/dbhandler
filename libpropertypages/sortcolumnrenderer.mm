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
: wxOSXDataViewDisabledInertRenderer( GetDefaultType(), mode, mode )
{
    m_toggle = true;
    m_allow3rdStateForUser = false;
    NSButtonCell* cell;
    cell = [[NSButtonCell alloc] init];
    [cell setAlignment:ConvertToNativeHorizontalTextAlignment( GetAlignment() )];
    [cell setButtonType: static_cast<NSButtonType>( NSSwitchButton )];
    [cell setImagePosition:NSImageLeft];
    [cell setAllowsMixedState:YES];
    SetNativeData(new wxDataViewRendererNativeData( cell ) );
    [cell release];
}

bool SortColumnRenderer::MacRender()
{
    NSButtonCell* cell = (NSButtonCell*) GetNativeData()->GetItemCell();
    int nativecbvalue = 0;
    wxVariant value;
    GetValue( value );
    [cell setIntValue:value.GetBool()];
    return true;
}

bool ObjectToBool(NSObject *object)
{
    // actually the value must be of NSCFBoolean class but it's private so we
    // can't check for it directly
    wxCHECK_MSG( [object isKindOfClass:[NSNumber class]], false,
    wxString::Format
    (
        "number expected but got %s",
        wxCFStringRef::AsString([object className])
    ));
    return [(NSNumber *)object boolValue];
}

void SortColumnRenderer::OSXOnCellChanged(NSObject *value, const wxDataViewItem& item, unsigned col)
{
    wxDataViewModel *model = GetOwner()->GetOwner()->GetModel();
    // The icon can't be edited so get its old value and reuse it.
    wxVariant valueToggle( ObjectToBool( value ) );
    if( !Validate( valueToggle ) )
        return;
    model->ChangeValue( valueToggle, item, col );
}

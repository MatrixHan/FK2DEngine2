/**
*	created:		2013-4-12   0:52
*	filename: 		FKScrollBar
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/Controls/FKScrollBar.h"
#include "../../Include/Controls/FKScrollBarButton.h"
#include "../../Include/Controls/FKScrollBarBar.h"
//------------------------------------------------------------------------
using namespace FK2DEngine2;
using namespace FK2DEngine2::Controls;
using namespace FK2DEngine2::ControlsInternal;
//------------------------------------------------------------------------
FK_CONTROL_CONSTRUCTOR( BaseScrollBar )
{
	for ( int i = 0; i < 2; i++ )
	{
		m_ScrollButton[i] = new ScrollBarButton( this );
	}

	m_Bar = new ScrollBarBar( this );
	SetBounds( 0, 0, 15, 15 );
	m_bDepressed = false;
	m_fScrolledAmount = 0;
	m_fContentSize = 0;
	m_fViewableContentSize = 0;
	SetNudgeAmount( 20 );
}

void BaseScrollBar::Render( Skin::Base* skin )
{
	skin->DrawScrollBar( this, IsHorizontal(), m_bDepressed );
}

void BaseScrollBar::OnBarMoved( Controls::Base* /*control*/ )
{
	onBarMoved.Call( this );
}

void BaseScrollBar::BarMovedNotification()
{
	OnBarMoved( this );
}

void BaseScrollBar::SetContentSize( float size )
{
	if ( m_fContentSize != size )
	{
		Invalidate();
	}

	m_fContentSize = size;
}
void BaseScrollBar::SetViewableContentSize( float size )
{
	if ( m_fViewableContentSize != size )
	{ Invalidate(); }

	m_fViewableContentSize = size;
}

bool BaseScrollBar::SetScrolledAmount( float amount, bool forceUpdate )
{
	if ( m_fScrolledAmount == amount && !forceUpdate ) { return false; }

	m_fScrolledAmount = amount;
	Invalidate();
	BarMovedNotification();
	return true;
}
//------------------------------------------------------------------------
/**
*	created:		2013-4-12   22:50
*	filename: 		FKWindowCanvas
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FK2DEngine2.h"
#include "../../Include/Controls/FKWindowCanvas.h"
#include "../../Include/FKSkin.h"
#include "../../Include/Controls/FKMenu.h"
#include "../../Include/FKDragAndDrop.h"
#include "../../Include/FKToolTip.h"
//------------------------------------------------------------------------
#ifndef FK_NO_ANIMATION
#include "../../Include/FKAnim.h"
#endif
//------------------------------------------------------------------------
using namespace FK2DEngine2;
using namespace FK2DEngine2::Controls;
//------------------------------------------------------------------------
WindowCanvas::WindowCanvas( int x, int y, int w, int h, FK2DEngine2::Skin::Base* pSkin, const FK2DEngine2::String & strWindowTitle ) : BaseClass( NULL )
{
	m_bQuit = false;
	m_bCanMaximize = true;
	m_bIsMaximized = false;
	SetPadding( Padding( 1, 0, 1, 1 ) );
	{
		int dw, dh;
		FK2DEngine2::Platform::GetDesktopSize( dw, dh );

		if ( x < 0 ) { x = ( dw - w ) * 0.5; }

		if ( y < 0 ) { y = ( dh - h ) * 0.5; }
	}
	m_pOSWindow = FK2DEngine2::Platform::CreatePlatformWindow( x, y, w, h, strWindowTitle );
	m_WindowPos  = FK2DEngine2::Point( x, y );
	pSkin->GetRender()->InitializeContext( this );
	pSkin->GetRender()->Init();
	m_pSkinChange = pSkin;
	SetSize( w, h );
	m_TitleBar = new FK2DEngine2::ControlsInternal::Dragger( this );
	m_TitleBar->SetHeight( 24 );
	m_TitleBar->SetPadding( Padding( 0, 0, 0, 0 ) );
	m_TitleBar->SetMargin( Margin( 0, 0, 0, 0 ) );
	m_TitleBar->Dock( Pos::Top );
	m_TitleBar->SetDoMove( false );
	m_TitleBar->onDragged.Add( this, &ThisClass::Dragger_Moved );
	m_TitleBar->onDragStart.Add( this, &ThisClass::Dragger_Start );
	m_TitleBar->onDoubleClickLeft.Add( this, &ThisClass::OnTitleDoubleClicked );
	m_Title = new FK2DEngine2::Controls::Label( m_TitleBar );
	m_Title->SetAlignment( Pos::Left | Pos::CenterV );
	m_Title->SetText( strWindowTitle );
	m_Title->Dock( Pos::Fill );
	m_Title->SetPadding( Padding( 8, 0, 0, 0 ) );
	m_Title->SetTextColor( GetSkin()->Colors.Window.TitleInactive );
	// CLOSE
	{
		m_pClose = new FK2DEngine2::Controls::WindowCloseButton( m_TitleBar, "Close" );
		m_pClose->Dock( Pos::Right );
		m_pClose->SetMargin( Margin( 0, 0, 4, 0 ) );
		m_pClose->onPress.Add( this, &WindowCanvas::CloseButtonPressed );
		m_pClose->SetTabable( false );
		m_pClose->SetWindow( this );
	}
	// MAXIMIZE
	{
		m_pMaximize = new FK2DEngine2::Controls::WindowMaximizeButton( m_TitleBar, "Maximize" );
		m_pMaximize->Dock( Pos::Right );
		m_pMaximize->onPress.Add( this, &WindowCanvas::MaximizeButtonPressed );
		m_pMaximize->SetTabable( false );
		m_pMaximize->SetWindow( this );
	}
	// MINIMiZE
	{
		m_pMinimize = new FK2DEngine2::Controls::WindowMinimizeButton( m_TitleBar, "Minimize" );
		m_pMinimize->Dock( Pos::Right );
		m_pMinimize->onPress.Add( this, &WindowCanvas::MinimizeButtonPressed );
		m_pMinimize->SetTabable( false );
		m_pMinimize->SetWindow( this );
	}
	// Bottom Right Corner Sizer
	{
		m_Sizer = new FK2DEngine2::ControlsInternal::Dragger( this );
		m_Sizer->SetSize( 16, 16 );
		m_Sizer->SetDoMove( false );
		m_Sizer->onDragged.Add( this, &WindowCanvas::Sizer_Moved );
		m_Sizer->onDragStart.Add( this, &WindowCanvas::Dragger_Start );
		m_Sizer->SetCursor( FK2DEngine2::CursorType::SizeNWSE );
	}
}

WindowCanvas::~WindowCanvas()
{
	DestroyWindow();
}

void* WindowCanvas::GetWindow()
{
	return m_pOSWindow;
}

void WindowCanvas::Layout( Skin::Base* skin )
{
	m_Sizer->BringToFront();
	m_Sizer->Position( Pos::Right | Pos::Bottom );
	BaseClass::Layout( skin );
}

void WindowCanvas::DoThink()
{
	Platform::MessagePump( m_pOSWindow, this );
	BaseClass::DoThink();
	RenderCanvas();
}

void WindowCanvas::RenderCanvas()
{
	if ( !NeedsRedraw() )
	{
		Platform::Sleep( 10 );
		return;
	}

	m_bNeedsRedraw = false;
	FK2DEngine2::Renderer::Base* render = m_Skin->GetRender();

	if ( render->BeginContext( this ) )
	{
		render->Begin();
		RecurseLayout( m_Skin );
		render->SetClipRegion( GetRenderBounds() );
		render->SetRenderOffset( FK2DEngine2::Point( X() * -1, Y() * -1 ) );
		render->SetScale( Scale() );

		if ( m_bDrawBackground )
		{
			render->SetDrawColor( m_BackgroundColor );
			render->DrawFilledRect( GetRenderBounds() );
		}

		DoRender( m_Skin );
		DragAndDrop::RenderOverlay( this, m_Skin );
		ToolTip::RenderToolTip( m_Skin );
		render->End();
	}

	render->EndContext( this );
	render->PresentContext( this );
}

void WindowCanvas::Render( Skin::Base* skin )
{
	bool bHasFocus = IsOnTop();

	if ( bHasFocus )
	{ m_Title->SetTextColor( GetSkin()->Colors.Window.TitleActive ); }
	else
	{ m_Title->SetTextColor( GetSkin()->Colors.Window.TitleInactive ); }

	skin->DrawWindow( this, m_TitleBar->Bottom(), bHasFocus );
}


void WindowCanvas::DestroyWindow()
{
	if ( m_pOSWindow )
	{
		GetSkin()->GetRender()->ShutdownContext( this );
		FK2DEngine2::Platform::DestroyPlatformWindow( m_pOSWindow );
		m_pOSWindow = NULL;
	}
}

bool WindowCanvas::InputQuit()
{
	m_bQuit = true;
	return true;
}

Skin::Base* WindowCanvas::GetSkin( void )
{
	if ( m_pSkinChange )
	{
		SetSkin( m_pSkinChange );
		m_pSkinChange = NULL;
	}

	return BaseClass::GetSkin();
}


void WindowCanvas::Dragger_Start()
{
	FK2DEngine2::Platform::GetCursorPos( m_HoldPos );
	m_HoldPos.x -= m_WindowPos.x;
	m_HoldPos.y -= m_WindowPos.y;
}

void WindowCanvas::Dragger_Moved()
{
	FK2DEngine2::Point p;
	FK2DEngine2::Platform::GetCursorPos( p );

	if ( m_bIsMaximized )
	{
		float fOldWidth = Width();
		SetMaximize( false );

		m_HoldPos.x = ( ( float ) m_HoldPos.x ) * ( ( float ) Width() / fOldWidth );
		m_HoldPos.y = 10;
	}

	SetPos( p.x - m_HoldPos.x, p.y - m_HoldPos.y );
}

void WindowCanvas::SetPos( int x, int y )
{
	int w, h;
	FK2DEngine2::Platform::GetDesktopSize( w, h );
	y = FK2DEngine2::Clamp( y, 0, h );
	m_WindowPos.x = x;
	m_WindowPos.y = y;
	FK2DEngine2::Platform::SetBoundsPlatformWindow( m_pOSWindow, x, y, Width(), Height() );
}

void WindowCanvas::CloseButtonPressed()
{
	InputQuit();
}

bool WindowCanvas::IsOnTop()
{
	return FK2DEngine2::Platform::HasFocusPlatformWindow( m_pOSWindow );
}


void WindowCanvas::Sizer_Moved()
{
	FK2DEngine2::Point p;
	FK2DEngine2::Platform::GetCursorPos( p );
	int w = ( p.x ) - m_WindowPos.x;
	int h = ( p.y ) - m_WindowPos.y;
	w = Clamp( w, 100, 9999 );
	h = Clamp( h, 100, 9999 );
	FK2DEngine2::Platform::SetBoundsPlatformWindow( m_pOSWindow, m_WindowPos.x, m_WindowPos.y, w, h );
	GetSkin()->GetRender()->ResizedContext( this, w, h );
	this->SetSize( w, h );
	BaseClass::DoThink();
	RenderCanvas();
}

void WindowCanvas::OnTitleDoubleClicked()
{
	if ( !CanMaximize() ) { return; }

	SetMaximize( !m_bIsMaximized );
}

void WindowCanvas::SetMaximize( bool b )
{
	m_bIsMaximized = b;
	m_pMaximize->SetMaximized( m_bIsMaximized );
	FK2DEngine2::Point pSize, pPos;
	FK2DEngine2::Platform::SetWindowMaximized( m_pOSWindow, m_bIsMaximized, pPos, pSize );
	SetSize( pSize.x, pSize.y );
	m_WindowPos = pPos;
	GetSkin()->GetRender()->ResizedContext( this, pSize.x, pSize.y );
	BaseClass::DoThink();
	RenderCanvas();
}

void WindowCanvas::MaximizeButtonPressed()
{
	if ( !CanMaximize() ) { return; }

	SetMaximize( !m_bIsMaximized );
}

void WindowCanvas::MinimizeButtonPressed()
{
	FK2DEngine2::Platform::SetWindowMinimized( m_pOSWindow, true );
}

void WindowCanvas::SetCanMaximize( bool b )
{
	if ( m_bCanMaximize == b ) { return; }

	m_bCanMaximize = b;
	m_pMaximize->SetDisabled( !b );
}
//------------------------------------------------------------------------
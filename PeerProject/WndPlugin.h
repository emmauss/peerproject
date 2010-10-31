//
// WndPlugin.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "WndPanel.h"


class CPluginWnd : public CPanelWnd
{
// Construction
public:
	CPluginWnd(LPCTSTR pszName = NULL, IPluginWindowOwner* pOwner = NULL);
	virtual ~CPluginWnd();

	DECLARE_DYNAMIC(CPluginWnd)

// Attributes
public:
	IPluginWindowOwner*	m_pOwner;
	CString				m_sName;
	UINT*				m_pHandled;
	DWORD				m_nHandled;
	CCoolBarCtrl*		m_pToolbar;
	int					m_nToolbar;
	BOOL				m_bAccel;

// Operations
public:
	virtual void	OnSkinChange();
	virtual HRESULT	GetGenericView(IGenericView** ppView);

// Overrides
public:
	//{{AFX_VIRTUAL(CPluginWnd)
	public:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CPluginWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// IPluginWindow
protected:
	BEGIN_INTERFACE_PART(PluginWindow, IPluginWindow)
		STDMETHOD(ListenForSingleMessage)(UINT nMessage);
		STDMETHOD(ListenForMultipleMessages)(SAFEARRAY FAR* pMessages);
		STDMETHOD(Create1)(BSTR bsCaption, HICON hIcon, VARIANT_BOOL bPanel, VARIANT_BOOL bTabbed);
		STDMETHOD(Create2)(UINT nCommandID, VARIANT_BOOL bPanel, VARIANT_BOOL bTabbed);
		STDMETHOD(GetHwnd)(HWND FAR* phWnd);
		STDMETHOD(HandleMessage)(LRESULT* plResult);
		STDMETHOD(LoadState)(VARIANT_BOOL bMaximise);
		STDMETHOD(SaveState)();
		STDMETHOD(ThrowMenu)(BSTR sName, LONG nDefaultID, POINT FAR* pPoint);
		STDMETHOD(AddToolbar)(BSTR sName, LONG nPosition, HWND FAR* phWnd, ISToolbar FAR* FAR* ppToolbar);
		STDMETHOD(AdjustWindowRect)(RECT FAR* pRect, VARIANT_BOOL bClientToWindow);
	END_INTERFACE_PART(PluginWindow)

	DECLARE_INTERFACE_MAP()
};

//
// LiveListSizer.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#if !defined(AFX_LIVELISTSIZER_H__22F38637_F279_48B6_A0A4_1B3CD5E27F76__INCLUDED_)
#define AFX_LIVELISTSIZER_H__22F38637_F279_48B6_A0A4_1B3CD5E27F76__INCLUDED_

#pragma once


class CLiveListSizer
{
// Construction
public:
	CLiveListSizer(CListCtrl* pCtrl = NULL);
	virtual ~CLiveListSizer();

// Attributes
protected:
	CListCtrl*	m_pCtrl;
	int			m_nWidth;
	int			m_nColumns;
	int*		m_pWidth;
	float*		m_pTake;

// Operations
public:
	void	Attach(CListCtrl* pCtrl, BOOL bScale = FALSE);
	void	Detach();
	BOOL	Resize(int nWidth = 0, BOOL bScale = FALSE);

};

#endif // !defined(AFX_LIVELISTSIZER_H__22F38637_F279_48B6_A0A4_1B3CD5E27F76__INCLUDED_)
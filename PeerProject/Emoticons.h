//
// Emoticons.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2014
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

class CImageFile;
class CRichDocument;


class CEmoticons
{
public:
	CEmoticons();
	virtual ~CEmoticons();

public:
	CImageList			m_pImage;
	CArray< CString >	m_pIndex;
	LPTSTR				m_pTokens;
	CArray< UINT >		m_pButtons;

public:
	int		Lookup(LPCTSTR pszText, int nLen = -1) const;
	LPCTSTR	FindNext(LPCTSTR pszText, int* pnIndex);
	LPCTSTR	GetText(int nIndex) const;
	void	Draw(CDC* pDC, int nIndex, int nX, int nY, COLORREF crBack = CLR_NONE);
	void	FormatText(CRichDocument* pDocument, LPCTSTR pszBody, BOOL bNewlines = FALSE, COLORREF cr = 0);
	CMenu*	CreateMenu();

	BOOL	Load();
	void	Clear();
protected:
	int		AddEmoticon(LPCTSTR pszText, CImageFile* pImage, CRect* pRect, COLORREF crBack, BOOL bButton);
	void	BuildTokens();
	BOOL	LoadXML(LPCTSTR pszFile);
};

extern CEmoticons Emoticons;

//
// PageDownloadActions.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

#include "PagePropertyAdv.h"

class CDownload;


class CDownloadActionsPage : public CPropertyPageAdv
{
public:
	CDownloadActionsPage();
	virtual ~CDownloadActionsPage();

	DECLARE_DYNAMIC(CDownloadActionsPage)

	enum { IDD = IDD_DOWNLOAD_ACTIONS };

protected:
	CStatic m_wndForgetVerify;
	CStatic m_wndForgetSources;
	CString m_sEraseFrom;
	CString m_sEraseTo;
	CStatic m_wndCompleteVerify;
	CStatic m_wndMergeVerify;
	CStatic m_wndCancelDownload;

	virtual void DoDataExchange(CDataExchange* pDX);

	void OnForgetVerify();
	void OnForgetSources();
	void OnCompleteVerify();
	void OnMergeAndVerify();
	void OnCancelDownload();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnErase();

	DECLARE_MESSAGE_MAP()
};

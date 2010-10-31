//
// WizardWelcomePage.cpp
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "WizardSheet.h"
#include "WizardWelcomePage.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CWizardWelcomePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWizardWelcomePage, CWizardPage)
	//{{AFX_MSG_MAP(CWizardWelcomePage)
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWizardWelcomePage property page

CWizardWelcomePage::CWizardWelcomePage() : CWizardPage(CWizardWelcomePage::IDD)
{
	//{{AFX_DATA_INIT(CWizardWelcomePage)
	//}}AFX_DATA_INIT
}

CWizardWelcomePage::~CWizardWelcomePage()
{
}

void CWizardWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardWelcomePage)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWizardWelcomePage message handlers

BOOL CWizardWelcomePage::OnInitDialog()
{
	CWizardPage::OnInitDialog();

	Skin.Apply( _T("CWizardWelcomePage"), this );

	SetWizardButtons( PSWIZB_NEXT );

	return TRUE;
}

BOOL CWizardWelcomePage::OnSetActive()
{
	//Wizard Window Caption Workaround
	CString strCaption;
	GetWindowText( strCaption );
	GetParent()->SetWindowText( strCaption );

	SetWizardButtons( PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

void CWizardWelcomePage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

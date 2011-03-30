//
// Plugins.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
// Portions copyright Shareaza Development Team, 2002-2006.
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
#include "Settings.h"
#include "Plugins.h"
#include "Application.h"
#include "CtrlCoolBar.h"
#include "WndPlugin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename


CPlugins Plugins;

//////////////////////////////////////////////////////////////////////
// CPlugins construction

CPlugins::CPlugins()
	: m_nCommandID	( ID_PLUGIN_FIRST )
{
	ZeroMemory( &m_inCLSID, sizeof( m_inCLSID ) );
}

void CPlugins::Register()
{
	CList< HINSTANCE > oModules; // Cache

//	LPCTSTR szParam = AfxGetPerUserRegistration() ? _T("/RegServerPerUser") : _T("/RegServer");

	CFileFind finder;
	BOOL bWorking = finder.FindFile( Settings.General.Path + _T("\\Plugins\\*.*") );	// .DLLs +.EXEs
	while ( bWorking )
	{
		bWorking = finder.FindNextFile();
		const CString sPath = finder.GetFilePath();
		const CString sName = finder.GetFileName();
		const CString sExt = PathFindExtension( sName );

		if ( ! sExt.CompareNoCase( _T(".dll") ) )
		{
			if ( sName == _T("WebHook.dll") && ! Settings.Downloads.WebHookEnable )
				continue;	// Skip WebHook Integration

			if ( HINSTANCE hDll = LoadLibrary( sPath ) )
			{
				HRESULT hr = S_FALSE;

				HRESULT (WINAPI *pfnDllInstall)(BOOL bInstall, LPCWSTR pszCmdLine);
				(FARPROC&)pfnDllInstall = GetProcAddress( hDll, "DllInstall" );
				if ( pfnDllInstall )
				{
					hr = pfnDllInstall( TRUE, L"user" );
				}
				else
				{
					HRESULT (WINAPI *pfnDllRegisterServer)(void);
					(FARPROC&)pfnDllRegisterServer = GetProcAddress( hDll, "DllRegisterServer" );
					if ( pfnDllRegisterServer )
						hr = pfnDllRegisterServer();
				}

				if ( hr == S_OK )
					theApp.Message( MSG_NOTICE, _T("Registered plugin: %s"), sName );
				else if ( FAILED( hr ) )
					theApp.Message( MSG_ERROR, _T("Failed to register plugin: %s : 0x%08x"), sName, hr );

				oModules.AddTail( hDll );
			}
		}
		else if ( ! sExt.CompareNoCase( _T(".exe") ) )
		{
			DWORD dwSize = GetFileVersionInfoSize( sPath, &dwSize );
			auto_array< BYTE > pBuffer( new BYTE[ dwSize ] );
			if ( GetFileVersionInfo( sPath, NULL, dwSize, pBuffer.get() ) )
			{
				LPCWSTR pValue = NULL;
				if ( VerQueryValue( pBuffer.get(),
					_T("\\StringFileInfo\\000004b0\\SpecialBuild"),
					(void**)&pValue, (UINT*)&dwSize ) &&
					pValue && dwSize &&
					_wcsicmp( pValue, _T("plugin") ) == 0 )
				{
					if ( (DWORD_PTR)ShellExecute( NULL, NULL, sPath, _T("/RegServerPerUser"), NULL, SW_HIDE ) > 32 )
						theApp.Message( MSG_NOTICE, _T("Registered plugin: %s"), sName );
					else
						theApp.Message( MSG_ERROR, _T("Failed to register plugin: %s : 0x%08x"), sName, GetLastError() );
				}
			}
		}
	}

	// Root DLLs	ToDo: Minimize duplicate code block ?
	bWorking = finder.FindFile( Settings.General.Path + _T("\\*.dll") );
	while ( bWorking )
	{
		bWorking = finder.FindNextFile();
		CString sDllPath = finder.GetFilePath().MakeLower();

		if ( HINSTANCE hDll = LoadLibrary( sDllPath ) )
		{
			HRESULT hr = S_FALSE;

			HRESULT (WINAPI *pfnDllInstall)(BOOL bInstall, LPCWSTR pszCmdLine);
			(FARPROC&)pfnDllInstall = GetProcAddress( hDll, "DllInstall" );
			if ( pfnDllInstall )
			{
				hr = pfnDllInstall( TRUE, L"user" );
			}
			else
			{
				HRESULT (WINAPI *pfnDllRegisterServer)(void);
				(FARPROC&)pfnDllRegisterServer = GetProcAddress( hDll, "DllRegisterServer" );
				if ( pfnDllRegisterServer )
					hr = pfnDllRegisterServer();
			}

			if ( hr == S_OK )
				theApp.Message( MSG_NOTICE, _T("Registered plugin: %s"), sDllPath );
			else if ( FAILED( hr ) )
				theApp.Message( MSG_ERROR, _T("Failed to register plugin: %s : 0x%08x"), sDllPath, hr );

			oModules.AddTail( hDll );
		}
	}

	for ( POSITION pos = oModules.GetHeadPosition() ; pos ; )
		FreeLibrary( oModules.GetNext( pos ) );
}

//////////////////////////////////////////////////////////////////////
// CPlugins enumerate

void CPlugins::Enumerate()
{
	HUSKEY hKey = NULL;
	if ( SHRegOpenUSKey( REGISTRY_KEY _T("\\Plugins\\General"),
		KEY_READ, NULL, &hKey, FALSE ) != ERROR_SUCCESS ) return;

	for ( DWORD nKey = 0 ; ; nKey++ )
	{
		TCHAR szName[128], szCLSID[64];
		DWORD dwType, dwName = 128, dwCLSID = 64 * sizeof(TCHAR);

		if ( SHRegEnumUSValue( hKey, nKey, szName, &dwName, &dwType,
			(LPBYTE)szCLSID, &dwCLSID, SHREGENUM_DEFAULT ) != ERROR_SUCCESS ) break;

		if ( dwType != REG_SZ ) continue;
		szCLSID[ 38 ] = 0;

		CLSID pCLSID;
		if ( ! Hashes::fromGuid( szCLSID, &pCLSID ) ) continue;

		for ( POSITION pos = GetIterator() ; pos ; )
		{
			if ( GetNext( pos )->m_pCLSID == pCLSID )
			{
				pCLSID = GUID_NULL;
				break;
			}
		}

		if ( pCLSID == GUID_NULL ) continue;

		if ( CPlugin* pPlugin = new CPlugin( pCLSID, szName ) )
		{
			CQuickLock oLock( m_pSection );

			m_pList.AddTail( pPlugin );

			if ( LookupEnable( pCLSID ) )
				pPlugin->Start();
		}
	}

	SHRegCloseUSKey( hKey );
}

//////////////////////////////////////////////////////////////////////
// CPlugins clear

void CPlugins::Clear()
{
	CloseThread();

	CQuickLock oLock( m_pSection );

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		delete GetNext( pos );
	}
	m_pList.RemoveAll();
}

//void CPlugins::UnloadPlugin(REFCLSID pCLSID)
//{
//	CQuickLock oLock( m_pSection );
//
//	// Delete from cache
//	CPluginPtr* pGITPlugin = NULL;
//	if ( m_pCache.Lookup( pCLSID, pGITPlugin ) )
//	{
//		m_pCache.RemoveKey( pCLSID );
//		delete pGITPlugin;
//	}
//
//	// Delete from generic plugins list
//	for ( POSITION pos = GetIterator() ; pos ; )
//	{
//		POSITION posOrig = pos;
//		CPlugin* pPlugin = GetNext( pos );
//		if ( pPlugin->m_pCLSID == pCLSID )
//		{
//			m_pList.RemoveAt( posOrig );
//			delete pPlugin;
//		}
//	}
//}

//////////////////////////////////////////////////////////////////////
// CPlugins CLSID helpers

BOOL CPlugins::LookupCLSID(LPCTSTR pszGroup, LPCTSTR pszKey, CLSID& pCLSID) const
{
	CString strCLSID = theApp.GetProfileString(
		CString( _T("Plugins\\") ) + pszGroup, pszKey, _T("") );
	return ! strCLSID.IsEmpty() &&
		Hashes::fromGuid( strCLSID, &pCLSID ) &&
		LookupEnable( pCLSID, pszKey );
}

BOOL CPlugins::LookupEnable(REFCLSID pCLSID, LPCTSTR pszExt) const
{
	HKEY hPlugins = NULL;

	CString strCLSID = Hashes::toGuid( pCLSID );

	if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER,
		REGISTRY_KEY _T("\\Plugins"), 0, KEY_ALL_ACCESS, &hPlugins ) )
	{
		DWORD nType = REG_SZ, nValue = 0;
		if ( ERROR_SUCCESS == RegQueryValueEx( hPlugins, strCLSID, NULL, &nType, NULL, &nValue ) )
		{
			// Upgrade here; Smart upgrade doesn't work
			if ( nType == REG_DWORD )
			{
				BOOL bEnabled = theApp.GetProfileInt( _T("Plugins"), strCLSID, TRUE );
				RegCloseKey( hPlugins );
				theApp.WriteProfileString( _T("Plugins"), strCLSID, bEnabled ? _T("") : _T("-") );
				return bEnabled;
			}
		}
		RegCloseKey( hPlugins );
	}

	CString strExtensions = theApp.GetProfileString( _T("Plugins"), strCLSID, _T("") );

	if ( strExtensions.IsEmpty() )
		return TRUE;
	if ( strExtensions == _T("-") ) // for plugins without associations
		return FALSE;
	if ( strExtensions.Left( 1 ) == _T("-") && strExtensions.GetLength() > 1 )
		strExtensions = strExtensions.Mid( 1 );

	if ( pszExt ) // Checking only a certain extension
	{
		CString strToFind;
		strToFind.Format( _T("|%s|"), pszExt );
		return strExtensions.Find( strToFind ) != -1;
	}

	// For Settings page
	CStringArray oTokens;
	Split( strExtensions, _T('|'), oTokens );
	INT_PTR nTotal = oTokens.GetCount();
	INT_PTR nChecked = 0;

	for ( INT_PTR nToken = 0 ; nToken < nTotal ; nToken++ )
	{
		CString strToken = oTokens.GetAt( nToken );
		if ( strToken.Left( 1 ) != _T("-") )
			nChecked++;
	}

	if ( nChecked == 0 ) return FALSE;

	return TRUE;
}

IUnknown* CPlugins::GetPlugin(LPCTSTR pszGroup, LPCTSTR pszType)
{
	HRESULT hr;

	CLSID pCLSID;
	if ( ! LookupCLSID( pszGroup, pszType, pCLSID ) )
		return NULL;	// Disabled

	for ( int i = 0 ; ; ++i )
	{
		{
			CQuickLock oLock( m_pSection );

			CComPtr< IUnknown > pPlugin;
			CPluginPtr* pGITPlugin = NULL;
			if ( m_pCache.Lookup( pCLSID, pGITPlugin ) )
			{
				if ( ! pGITPlugin )
					return NULL;

				if ( SUCCEEDED( hr = pGITPlugin->m_pGIT.CopyTo( &pPlugin ) ) )
					return pPlugin.Detach();

				TRACE( _T("Invalid plugin \"%s\"-\"%s\" %s\n"), pszGroup, pszType, (LPCTSTR)Hashes::toGuid( pCLSID ) );
			}

			if ( i == 1 )
				break;

			m_inCLSID = pCLSID;

			// Create new one
			if ( ! BeginThread( "PluginCache" ) )
				break;	// Something really bad
		}

		Wakeup();									// Start process
		WaitForSingleObject( m_pReady, INFINITE );	// Wait for result
	}

	return NULL;
}

BOOL CPlugins::ReloadPlugin(LPCTSTR pszGroup, LPCTSTR pszType)
{
	CLSID pCLSID;
	if ( ! LookupCLSID( pszGroup, pszType, pCLSID ) )
		return FALSE;	// Disabled

	{
		CQuickLock oLock( m_pSection );

		m_inCLSID = pCLSID;

		// Create new one
		if ( ! BeginThread( "PluginCache" ) )
			return FALSE;	// Something really bad
	}

	Wakeup();									// Start process
	WaitForSingleObject( m_pReady, INFINITE );	// Wait for result

	return TRUE;
}

void CPlugins::OnRun()
{
	HRESULT hr;

	while( IsThreadEnabled() )
	{
		Doze();

		if ( ! IsThreadEnabled() )
			break;

		CQuickLock oLock( m_pSection );

		// Revoke interface
		CPluginPtr* pGITPlugin = NULL;
		if ( m_pCache.Lookup( m_inCLSID, pGITPlugin ) )
		{
			delete pGITPlugin;

			TRACE( _T("Dropped plugin %s\n"), (LPCTSTR)Hashes::toGuid( m_inCLSID ) );
		}

		m_pCache.SetAt( m_inCLSID, NULL );

		HINSTANCE hRes = AfxGetResourceHandle();

		pGITPlugin = new CPluginPtr;
		if ( pGITPlugin )
		{
			// Create plugin & Add plugin interface to GIT
			if ( SUCCEEDED( hr = pGITPlugin->m_pIUnknown.CoCreateInstance( m_inCLSID ) ) &&
				 SUCCEEDED( hr = pGITPlugin->m_pGIT.Attach( pGITPlugin->m_pIUnknown ) ) )
			{
				m_pCache.SetAt( m_inCLSID, pGITPlugin );

				TRACE( _T("Created plugin %s\n"), (LPCTSTR)Hashes::toGuid( m_inCLSID ) );
			}
			else
				delete pGITPlugin;
		}

		AfxSetResourceHandle( hRes );

		ZeroMemory( &m_inCLSID, sizeof( m_inCLSID ) );

		m_pReady.SetEvent();
	}

	CQuickLock oLock( m_pSection );

	// Revoke all interfaces
	for ( POSITION pos = m_pCache.GetStartPosition() ; pos ; )
	{
		CLSID pCLSID;
		CPluginPtr* pGITPlugin = NULL;
		m_pCache.GetNextAssoc( pos, pCLSID, pGITPlugin );
		delete pGITPlugin;
	}
	m_pCache.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CPlugins skin changed event

void CPlugins::OnSkinChanged()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pPlugin )
			pPlugin->m_pPlugin->OnSkinChanged();
	}
}

void CPlugins::InsertCommands()
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pCommand )
			pPlugin->m_pCommand->InsertCommands();
	}
}

//////////////////////////////////////////////////////////////////////
// CPlugins command ID registration

void CPlugins::RegisterCommands()
{
	m_nCommandID = ID_PLUGIN_FIRST;

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );
		if ( pPlugin->m_pCommand )
			pPlugin->m_pCommand->RegisterCommands();
	}
}

UINT CPlugins::GetCommandID()
{
	return m_nCommandID++;
}

//////////////////////////////////////////////////////////////////////
// CPlugins command handling

BOOL CPlugins::OnUpdate(CChildWnd* pActiveWnd, CCmdUI* pCmdUI)
{
	UINT nCommandID		= pCmdUI->m_nID;
	TRISTATE bVisible	= TRI_TRUE;
	TRISTATE bEnabled	= TRI_TRUE;
	TRISTATE bChecked	= TRI_UNKNOWN;

	CCoolBarItem* pCoolUI = CCoolBarItem::FromCmdUI( pCmdUI );

	if ( pActiveWnd != NULL && pActiveWnd->IsKindOf( RUNTIME_CLASS(CPluginWnd) ) )
	{
		CPluginWnd* pPluginWnd = (CPluginWnd*)pActiveWnd;

		if ( pPluginWnd->m_pOwner )
		{
			if ( pPluginWnd->m_pOwner->OnUpdate( nCommandID, &bVisible, &bEnabled, &bChecked ) == S_OK )
			{
				if ( bVisible != TRI_UNKNOWN && pCoolUI != NULL )
					pCoolUI->Show( bVisible == TRI_TRUE );
				if ( bEnabled != TRI_UNKNOWN )
					pCmdUI->Enable( bEnabled == TRI_TRUE );
				if ( bChecked != TRI_UNKNOWN )
					pCmdUI->SetCheck( bChecked == TRI_TRUE );

				return TRUE;
			}
		}
	}

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pCommand )
		{
			if ( pPlugin->m_pCommand->OnUpdate( nCommandID, &bVisible, &bEnabled, &bChecked ) == S_OK )
			{
				if ( bVisible != TRI_UNKNOWN && pCoolUI != NULL )
					pCoolUI->Show( bVisible == TRI_TRUE );
				if ( bEnabled != TRI_UNKNOWN )
					pCmdUI->Enable( bEnabled == TRI_TRUE );
				if ( bChecked != TRI_UNKNOWN )
					pCmdUI->SetCheck( bChecked == TRI_TRUE );

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CPlugins::OnCommand(CChildWnd* pActiveWnd, UINT nCommandID)
{
	if ( pActiveWnd != NULL && pActiveWnd->IsKindOf( RUNTIME_CLASS(CPluginWnd) ) )
	{
		CPluginWnd* pPluginWnd = (CPluginWnd*)pActiveWnd;

		if ( pPluginWnd->m_pOwner )
		{
			if ( pPluginWnd->m_pOwner->OnCommand( nCommandID ) == S_OK ) return TRUE;
		}
	}

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pCommand )
		{
			if ( pPlugin->m_pCommand->OnCommand( nCommandID ) == S_OK ) return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CPlugins file execution events

BOOL CPlugins::OnExecuteFile(LPCTSTR pszFile, BOOL bUseImageViewer)
{
	CPlugin* pImageViewer = NULL;

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pExecute )
		{
			if ( pPlugin->m_sName == _T("PeerProject Image Viewer") )
			{
				pImageViewer = pPlugin;
				continue;
			}
			if ( pPlugin->m_pExecute->OnExecute( CComBSTR( pszFile ) ) == S_OK )
				return TRUE;
		}
	}

	if ( bUseImageViewer && pImageViewer )
		return ( pImageViewer->m_pExecute->OnExecute( CComBSTR( pszFile ) ) == S_OK );

	return FALSE;
}

BOOL CPlugins::OnEnqueueFile(LPCTSTR pszFile)
{
	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pExecute )
		{
			if ( pPlugin->m_pExecute->OnEnqueue( CComBSTR( pszFile ) ) == S_OK )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CPlugins::OnChatMessage(LPCTSTR pszChatID, BOOL bOutgoing, LPCTSTR pszFrom, LPCTSTR pszTo, LPCTSTR pszMessage)
{
	// IChatPlugin capturing (IRC/direct)

	for ( POSITION pos = GetIterator() ; pos ; )
	{
		CPlugin* pPlugin = GetNext( pos );

		if ( pPlugin->m_pChat )
		{
			pPlugin->m_pChat->OnChatMessage(
				CComBSTR( pszChatID ),
				( bOutgoing ? VARIANT_TRUE : VARIANT_FALSE ),
				CComBSTR( pszFrom ),
				CComBSTR( pszTo ),
				CComBSTR( pszMessage ) );
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPlugin construction

CPlugin::CPlugin(REFCLSID pCLSID, LPCTSTR pszName)
	: m_pCLSID	( pCLSID )
	, m_sName	( pszName )
	, m_nCapabilities ( 0 )
{
}

CPlugin::~CPlugin()
{
	Stop();
}

//////////////////////////////////////////////////////////////////////
// CPlugin start / stop operations

BOOL CPlugin::Start()
{
	HRESULT hr;

	if ( m_pPlugin )
		return FALSE;	// Already initialized

	CComPtr< IApplication > pApplication;
	hr = CApplication::GetApp( &pApplication );
	if ( FAILED( hr ) )
		return FALSE;	// Something very bad

	hr = m_pPlugin.CoCreateInstance( m_pCLSID );
	if ( FAILED( hr ) )
	{
		m_pPlugin.Release();
		return FALSE;
	}

	hr = m_pPlugin->SetApplication( pApplication );

	m_nCapabilities = 0;
	hr = m_pPlugin->QueryCapabilities( &m_nCapabilities );

	ASSERT( ! m_pCommand );
	ASSERT( ! m_pExecute );
	ASSERT( ! m_pChat );

	hr = m_pPlugin->QueryInterface( IID_ICommandPlugin, (void**)&m_pCommand );
	hr = m_pPlugin->QueryInterface( IID_IExecutePlugin, (void**)&m_pExecute );
	hr = m_pPlugin->QueryInterface( IID_IChatPlugin, (void**)&m_pChat );

	return TRUE;
}

void CPlugin::Stop()
{
	__try
	{
		m_pChat.Release();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	__try
	{
		m_pExecute.Release();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	__try
	{
		m_pCommand.Release();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	__try
	{
		m_pPlugin.Release();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

//////////////////////////////////////////////////////////////////////
// CPlugin CLSID helper

CString CPlugin::GetStringCLSID() const
{
	return Hashes::toGuid( m_pCLSID );
}

//////////////////////////////////////////////////////////////////////
// CPlugin icon helper

HICON CPlugin::LookupIcon() const
{
	CString strName;
	HKEY hKey;

	strName.Format( _T("CLSID\\%s\\InprocServer32"), (LPCTSTR)GetStringCLSID() );

	if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, strName, 0, KEY_QUERY_VALUE, &hKey ) )
		return NULL;

	DWORD dwType = REG_SZ, dwSize = 256 * sizeof(TCHAR);
	LONG lResult = RegQueryValueEx( hKey, _T(""), NULL, &dwType, (LPBYTE)strName.GetBuffer( 256 ), &dwSize );
	strName.ReleaseBuffer( dwSize / sizeof(TCHAR) );
	RegCloseKey( hKey );

	if ( lResult != ERROR_SUCCESS ) return NULL;

	HICON hIcon = NULL;
	ExtractIconEx( strName, 0, NULL, &hIcon, 1 );
	return hIcon;
}

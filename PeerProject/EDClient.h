//
// EDClient.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2006.
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

#pragma once

#include "Transfer.h"
#include "HostBrowser.h"

class CEDPacket;
class CDownload;
class CDownloadSource;
class CDownloadTransferED2K;
class CUploadTransferED2K;


class CEDClient : public CTransfer
{
// Construction
public:
	CEDClient();
protected:
	virtual ~CEDClient();

// Attributes
public:
	CEDClient*	m_pEdPrev;
	CEDClient*	m_pEdNext;

// ClientID/Version
	Hashes::Guid m_oGUID;
	SOCKADDR_IN	m_pServer;
	DWORD		m_nClientID;
	WORD		m_nUDP;

	CString		m_sNick;
	BOOL		m_bEmule;
	int			m_nVersion;
	int			m_nEmVersion;
	int			m_nEmCompatible;
	DWORD		m_nSoftwareVersion;

// Client capabilities
	BOOL		m_bEmAICH;			// Not supported
	BOOL		m_bEmUnicode;
	BOOL		m_bEmUDPVersion;
	BOOL		m_bEmDeflate;
	BOOL		m_bEmSecureID;		// Not supported
	BOOL		m_bEmSources;
	BOOL		m_bEmRequest;
	BOOL		m_bEmComments;
	BOOL		m_bEmPeerCache;		// Not supported
	BOOL		m_bEmBrowse;		// Browse supported
	BOOL		m_bEmMultiPacket;	// Not supported
	BOOL		m_bEmPreview;		// Preview support
	BOOL		m_bEmLargeFile;		// Large file support

// Other
	CDownloadTransferED2K*	m_pDownloadTransfer;
	CUploadTransferED2K*	m_pUploadTransfer;
	Hashes::Ed2kHash		m_oUpED2K;
	bool		m_bCallbackRequested;
	BOOL		m_bSeeking;
	BOOL		m_bLogin;
	QWORD		m_nUpSize;
	DWORD		m_nRunExCookie;
	DWORD		m_nDirsWaiting;

	BOOL		m_bOpenChat;
	BOOL		m_bCommentSent;

// Operations
public:
	BOOL	ConnectTo(DWORD nClientID, WORD nClientPort, IN_ADDR* pServerAddress, WORD nServerPort, const Hashes::Guid& oGUID);
	BOOL	Equals(CEDClient* pClient);
	BOOL	Connect();
	void	Remove();
	void	Merge(CEDClient* pClient);
	void	CopyCapabilities(CEDClient* pClient);
	void	Send(CEDPacket* pPacket, BOOL bRelease = TRUE);
	void	OnRunEx(DWORD tNow);

	BOOL	AttachDownload(CDownloadTransferED2K* pDownload);
	void	OnDownloadClose();
	void	OnUploadClose();
	CString	GetSourceURL();
	void	WritePartStatus(CEDPacket* pPacket, CDownload* pDownload);
	BOOL	SeekNewDownload(CDownloadSource* pExcept = NULL);
	BOOL	SendCommentsPacket(int nRating, LPCTSTR pszComments);
	void	SendPreviewRequest(CDownload* pDownload);
	inline  void OpenChat() { m_bOpenChat = TRUE; }

protected:
	void	DetermineUserAgent();
	void	DetachDownload();
	void	DetachUpload();
	void	NotifyDropped();
	BOOL	OnLoggedIn();

	CHostBrowser*	GetBrowser() const;
	CDownloadSource* GetSource() const;			// Get download transfer source

public:
	virtual void	AttachTo(CConnection* pConnection);
	virtual void	Close();

protected:
	virtual BOOL	OnRun();
	virtual BOOL	OnConnected();
	virtual void	OnDropped();
	virtual BOOL	OnWrite();
	virtual BOOL	OnRead();

protected:
	BOOL	OnPacket(CEDPacket* pPacket);
	void	SendHello(BYTE nType);
	BOOL	OnHello(CEDPacket* pPacket);
	void	SendEmuleInfo(BYTE nType);
	BOOL	OnEmuleInfo(CEDPacket* pPacket);
	BOOL	OnFileRequest(CEDPacket* pPacket);
	BOOL	OnFileStatusRequest(CEDPacket* pPacket);
	BOOL	OnHashsetRequest(CEDPacket* pPacket);
	BOOL	OnQueueRequest(CEDPacket* pPacket);
	BOOL	OnSourceRequest(CEDPacket* pPacket);
	BOOL	OnSourceAnswer(CEDPacket* pPacket);
	BOOL	OnRequestPreview(CEDPacket* pPacket);
	BOOL	OnPreviewAnswer(CEDPacket* pPacket);
// Chat:
	BOOL	OnMessage(CEDPacket* pPacket);
// Browse us:
	BOOL	OnAskSharedDirs(CEDPacket* pPacket);
	BOOL	OnViewSharedDir(CEDPacket* pPacket);
// Browse remote host:
	BOOL	OnAskSharedDirsAnswer(CEDPacket* pPacket);
	BOOL	OnViewSharedDirAnswer(CEDPacket* pPacket);
	BOOL	OnAskSharedDirsDenied(CEDPacket* pPacket);

public:
	BOOL	OnUdpReask(CEDPacket* pPacket);
	BOOL	OnUdpReaskAck(CEDPacket* pPacket);
	BOOL	OnUdpQueueFull(CEDPacket* pPacket);
	BOOL	OnUdpFileNotFound(CEDPacket* pPacket);

	inline BOOL IsOnline() const { return m_bConnected && m_bLogin; }

	DWORD GetID() const;
};

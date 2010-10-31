//
// ChatWindows.h
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

class CChatFrame;
class CPrivateChatFrame;


class CChatWindows
{
public:
	CChatWindows();
	virtual ~CChatWindows();

// Attributes
protected:
	CList< CChatFrame* > m_pList;

// Operations
public:
	POSITION	GetIterator() const;
	CChatFrame*	GetNext(POSITION& pos) const;
	INT_PTR		GetCount() const { return m_pList.GetCount(); }

	void		Close();
	void		Add(CChatFrame* pFrame);
	void		Remove(CChatFrame* pFrame);

	// Start new or reopen existing chat session
	CPrivateChatFrame*	OpenPrivate(const Hashes::Guid& oGUID, SOCKADDR_IN* pHost, BOOL bMustPush = FALSE, PROTOCOLID nProtocol = PROTOCOL_NULL, SOCKADDR_IN* pServer = NULL );
	// Start chat session, nPort and nServerPort in host byte order
	CPrivateChatFrame*	OpenPrivate(const Hashes::Guid& oGUID, IN_ADDR* pAddress, WORD nPort = 6346, BOOL bMustPush = FALSE, PROTOCOLID nProtocol = PROTOCOL_NULL, IN_ADDR* pServerAddress = NULL, WORD nServerPort = 0 );
	CPrivateChatFrame*	FindPrivate(const Hashes::Guid& oGUID);
	CPrivateChatFrame*	FindPrivate(IN_ADDR* pAddress);
	CPrivateChatFrame*  FindED2KFrame(SOCKADDR_IN* pAddress);
	CPrivateChatFrame*  FindED2KFrame(DWORD nClientID, SOCKADDR_IN* pServerAddress);

protected:
	friend class CChatFrame;
};

extern CChatWindows ChatWindows;

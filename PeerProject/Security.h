//
// Security.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
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

class CPeerProjectFile;

class CSecureRule;
class CQuerySearch;
class CXMLElement;


#define SECURITY_SER_VERSION	1000	//5
// nVersion History:
// 5 - extended security rule type (ryo-oh-ki)
// 1000 - Added banCustom (PeerProject 1.0) (5+)

enum
{
	banSession, ban5Mins, ban30Mins, ban2Hours, banWeek, banCustom, banForever
};

class CSecurity
{
// Construction
public:
	CSecurity();
	~CSecurity();

// Attributes
public:
	mutable CCriticalSection	m_pSection;
	BOOL						m_bDenyPolicy;

	static LPCTSTR				xmlns;

protected:
	typedef struct
	{
		DWORD	m_nExpire;
		BYTE	m_nScore;
	} CComplain;
	typedef CMap< DWORD, DWORD, CComplain*, CComplain* > CComplainMap;

	CComplainMap				m_Complains;
	CList< CSecureRule* >		m_pRules;
	std::set< DWORD >			m_Cache;	// Known good addresses

// Operations
public:
	POSITION		GetIterator() const;
	CSecureRule*	GetNext(POSITION& pos) const;
	INT_PTR			GetCount() const;
	BOOL			Check(CSecureRule* pRule) const;
	void			Add(CSecureRule* pRule);
	void			Remove(CSecureRule* pRule);
	void			MoveUp(CSecureRule* pRule);
	void			MoveDown(CSecureRule* pRule);

	inline void		Ban(const IN_ADDR* pAddress, int nBanLength, BOOL bMessage = TRUE, LPCTSTR szComment = NULL)
	{
		BanHelper( pAddress, NULL, nBanLength, bMessage, szComment );
	}

	inline void		Ban(const CPeerProjectFile* pFile, int nBanLength, BOOL bMessage = TRUE, LPCTSTR szComment = NULL)
	{
		BanHelper( NULL, pFile, nBanLength, bMessage, szComment );
	}

	void			Clear();
	bool			Complain(const IN_ADDR* pAddress, int nBanLength = ban5Mins, int nExpire = 15, int nCount = 3);
	BOOL			IsDenied(const IN_ADDR* pAddress);
	BOOL			IsDenied(LPCTSTR pszContent);
	BOOL			IsDenied(const CPeerProjectFile* pFile);
	BOOL			IsDenied(const CQuerySearch* pQuery, const CString& strContent);
	BOOL			Import(LPCTSTR pszFile);
	BOOL			Load();
	BOOL			Save();
	void			Expire();

	// Don't ban GPL breakers, but don't offer leaf slots to them. Ban others.
	BOOL			IsClientBad(const CString& sUserAgent) const;
	BOOL			IsClientBanned(const CString& sUserAgent);
	BOOL			IsAgentBlocked(const CString& sUserAgent) const;	// User-defined names
	BOOL			IsVendorBlocked(const CString& sVendor) const;		// G1/G2 code

protected:
	void			BanHelper(const IN_ADDR* pAddress, const CPeerProjectFile* pFile, int nBanLength, BOOL bMessage, LPCTSTR szComment);
	CSecureRule*	GetGUID(const GUID& oGUID) const;
	CXMLElement*	ToXML(BOOL bRules = TRUE);
	BOOL			FromXML(CXMLElement* pXML);
	void			Serialize(CArchive& ar);
};

class CSecureRule
{
public:
	CSecureRule(BOOL bCreate = TRUE);
	CSecureRule(const CSecureRule& pRule);
	CSecureRule& operator=(const CSecureRule& pRule);
	~CSecureRule();

	typedef enum { srAddress, srContentAny, srContentAll, srContentRegExp, srContentHash, srSizeType } RuleType;
	enum { srNull, srAccept, srDeny };
	enum { srIndefinite = 0, srSession = 1 };

	RuleType	m_nType;
	BYTE		m_nAction;
	CString		m_sComment;
	GUID		m_pGUID;
	DWORD		m_nExpire;
	DWORD		m_nToday;
	DWORD		m_nEver;
	BYTE		m_nIP[4];
	BYTE		m_nMask[4];
	TCHAR*		m_pContent;
	DWORD		m_nContentLength;

	void		Remove();
	void		Reset();
	void		MaskFix();
	BOOL		IsExpired(DWORD nNow, BOOL bSession = FALSE) const;
	BOOL		Match(const IN_ADDR* pAddress) const;
	BOOL		Match(LPCTSTR pszContent) const;
	BOOL		Match(const CPeerProjectFile* pFile) const;
	BOOL		Match(const CQuerySearch* pQuery, const CString& strContent) const;
	CString 	GetContentWords() const;
	void		SetContentWords(const CString& strContent);
	void		Serialize(CArchive& ar, int nVersion);

	CXMLElement* ToXML();
	BOOL		FromXML(CXMLElement* pXML);
	CString		ToGnucleusString() const;
	BOOL		FromGnucleusString(CString& str);
};

// An adult filter class, used in searches, chat, etc
class CAdultFilter
{
// Construction
public:
	CAdultFilter();
	~CAdultFilter();

// Attributes
private:
	LPTSTR		m_pszBlockedWords;					// Definitely adult content
	LPTSTR		m_pszDubiousWords;					// Possibly adult content
	LPTSTR		m_pszChildWords;					// Words related to child ponography

// Operations
public:
	void		Load();
	BOOL		IsHitAdult(LPCTSTR) const;			// Does this search result have adult content?
	BOOL		IsChildPornography(LPCTSTR) const;	// Word combination indicates underage
	BOOL		IsSearchFiltered(LPCTSTR) const;	// Check if search is filtered
	BOOL		IsChatFiltered(LPCTSTR) const;		// Check filter for chat
	BOOL		Censor(TCHAR*) const;				// Censor (remove) bad words from a string
private:
	BOOL		IsFiltered(LPCTSTR) const;
};

// A message filter class for chat messages. (Spam protection)
class CMessageFilter
{
// Construction
public:
	CMessageFilter();
	~CMessageFilter();

// Attributes
private:
	LPTSTR		m_pszED2KSpam;				// Known ED2K spam phrases
	LPTSTR		m_pszFilteredPhrases;		// Known spam phrases

// Operations
public:
	void		Load();
	BOOL		IsED2KSpam( LPCTSTR );		// ED2K message spam filter (ED2K only, always on)
	BOOL		IsFiltered( LPCTSTR );		// Chat message spam filter
};

extern CMessageFilter MessageFilter;
extern CAdultFilter AdultFilter;
extern CSecurity Security;

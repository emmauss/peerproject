//
// WorldGPS.h
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

class CWorldCountry;
class CWorldCity;
class CXMLElement;


class CWorldGPS
{
public:
	CWorldGPS();
	~CWorldGPS();

public:
	CWorldCountry*	m_pCountry;
	DWORD			m_nCountry;

public:
	BOOL		Load();
	void		Clear();
protected:
	void		Serialize(CArchive& ar);
	BOOL		LoadFrom(CXMLElement* pRoot);
};


class CWorldCountry
{
public:
	CWorldCountry();
	~CWorldCountry();

public:
	CHAR		m_szID[2];
	CString		m_sName;

	CWorldCity*	m_pCity;
	DWORD		m_nCity;

public:
	void		Serialize(CArchive& ar);
	BOOL		LoadFrom(CXMLElement* pRoot);
	void		Clear();
};


class CWorldCity
{
public:
	CString		m_sName;
	CString		m_sState;
	float		m_nLatitude;
	float		m_nLongitude;

public:
	void		Serialize(CArchive& ar);
	BOOL		LoadFrom(CXMLElement* pRoot);
};

#pragma once
#include "ituniacinfohandler.h"

class CGenericInfoHandler :
	public ITuniacInfoHandler
{
public:
	CGenericInfoHandler(void);
public:
	~CGenericInfoHandler(void);

public:
	void			Destroy(void);

	bool			GetName(WCHAR ** nameString);
	GUID			GetPluginID(void);

	unsigned long	GetFlags(void);

	bool			About(HWND hWndParent);
	bool			Configure(HWND hWndParent);

	void			SetHelper(ITuniacPluginHelper * pHelper);

public:
	bool	CanHandle(String filename, unsigned long * Ability, unsigned long * Merit);

	ITuniacInfoAccessor	*	CreateAccessor(String filename);

};

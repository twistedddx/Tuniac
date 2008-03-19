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
	bool	CanHandle(wchar_t * filename, unsigned long * Ability, unsigned long * Merit);

	ITuniacInfoAccessor	*	CreateAccessor(wchar_t * filename);

};

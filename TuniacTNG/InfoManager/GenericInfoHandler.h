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
	void					Destroy(void);

	bool					CanHandle(wchar_t * filename, unsigned long * Ability, unsigned long * Merit);

	ITuniacInfoAccessor	*	CreateAccessor(wchar_t * filename);

};

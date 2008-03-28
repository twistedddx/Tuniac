#pragma once
#include "iinfohandler.h"

class CGenericInfoHandler :
	public IInfoHandler
{
public:
	CGenericInfoHandler(void);
public:
	~CGenericInfoHandler(void);

public:
	void					Destroy(void);

	bool					CanHandle(wchar_t * filename, unsigned long * Ability, unsigned long * Merit);

	IInfoAccessor		*	CreateAccessor(wchar_t * filename);

};

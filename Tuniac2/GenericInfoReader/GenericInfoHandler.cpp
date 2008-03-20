#include "StdAfx.h"
#include "GenericInfoHandler.h"
#include "GenericInfoAccessor.h"

CGenericInfoHandler::CGenericInfoHandler(void)
{
}

CGenericInfoHandler::~CGenericInfoHandler(void)
{
}

void			CGenericInfoHandler::Destroy(void)
{
	delete this;
}

bool			CGenericInfoHandler::GetName(WCHAR ** nameString)
{
	*nameString = TEXT("Generic Info Handler");

	return true;
}

GUID			CGenericInfoHandler::GetPluginID(void)
{
// {BA153191-A68D-4f9f-95DB-D32BA32A8801}
static const GUID GUID_GENERICINFOHANDLER = { 0xba153191, 0xa68d, 0x4f9f, { 0x95, 0xdb, 0xd3, 0x2b, 0xa3, 0x2a, 0x88, 0x1 } };

	return GUID_GENERICINFOHANDLER;
}

unsigned long	CGenericInfoHandler::GetFlags(void)
{
	return 0;
}

bool			CGenericInfoHandler::About(HWND hWndParent)
{
	return false;
}

bool			CGenericInfoHandler::Configure(HWND hWndParent)
{
	return false;
}

void			CGenericInfoHandler::SetHelper(ITuniacPluginHelper * pHelper)
{
}

bool	CGenericInfoHandler::CanHandle(wchar_t * filename, unsigned long * Ability, unsigned long * Merit)
{
	TagLib::String sFilename = filename;

	// lets only deal with formats we know about pls....
	TagLib::StringList list =  TagLib::FileRef::defaultFileExtensions();
	for(unsigned long x=0; x<list.size(); x++)
	{
		if(sFilename.find(list[x]) != -1)
		{
			*Ability	= 100;
			*Merit		= 0;
			return true;
		}
	}

	return false;
}

ITuniacInfoAccessor	*	CGenericInfoHandler::CreateAccessor(wchar_t * filename)
{
	CGenericInfoAccessor * t = new CGenericInfoAccessor;

	if(t->Open(filename))
		return t;

	t->Destroy();
	return NULL;
}

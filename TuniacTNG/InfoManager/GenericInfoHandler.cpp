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

IInfoAccessor	*	CGenericInfoHandler::CreateAccessor(wchar_t * filename)
{
	CGenericInfoAccessor * t = new CGenericInfoAccessor;

	if(t->Open(filename))
		return t;

	t->Destroy();
	return NULL;
}

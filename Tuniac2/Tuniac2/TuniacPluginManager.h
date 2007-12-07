#pragma once

/*
ok make an array of all the .dlls loaded up

then make an array for each plugin type, scan the .dlls for the relevent create function
*/

#include "ITuniacPluginWindow.h"
#include "ITuniacInfoHandler.h"

class CTuniacPluginManager
{
protected:
	std::vector<HINSTANCE>						hDLLArray;

	std::vector<ITuniacInfoHandler *>			InfoHandlerArray;
	std::vector<ITuniacPluginWindow *>			PluginWindowArray;

public:
	CTuniacPluginManager(void);
	~CTuniacPluginManager(void);

	bool Initialize(void);
	bool Shutdown(void);

	unsigned long GetPluginCount(GUID InterfaceID);
	void * GetPluginAtIndex(GUID InterfaceID, unsigned long Index);
};

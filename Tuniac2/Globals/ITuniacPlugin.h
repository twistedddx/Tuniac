#pragma once

#include <string>
#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>
using namespace std;

#include "StdString.h"

typedef CStdString				String;	
typedef std::vector<String>		StringArray;
typedef std::vector<int>		IntArray;

#include "ITuniacLog.h"

class ITuniacPluginHelper
{
public:
	virtual ITuniacLog * getLog();
};

#define PLUGINFLAGS_ABOUT		0x00000001
#define PLUGINFLAGS_CONFIG		0x00000002

class ITuniacPlugin
{
public:
	virtual void			Destroy(void)								= 0;

	virtual bool			GetName(String & nameString)				= 0;
	virtual GUID			GetPluginID(void)							= 0;		// use guidgen to make a custom guid to return

	virtual unsigned long	GetFlags(void)								= 0;

	virtual bool			About(HWND hWndParent)						= 0;
	virtual bool			Configure(HWND hWndParent)					= 0;

	virtual void			SetHelper(ITuniacPluginHelper * pHelper)	= 0;
};

typedef void * (*CreateTuniacPluginFunc)(GUID InterfaceID, unsigned long InterfaceNumber);
extern "C" __declspec(dllexport) void * CreateTuniacPlugin(GUID InterfaceID, unsigned long InterfaceNumber);
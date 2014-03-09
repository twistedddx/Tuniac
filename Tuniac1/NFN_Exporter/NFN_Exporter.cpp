// NFN_Exporter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NumberedFileExporter.h"


extern "C" __declspec(dllexport) ITuniacExportPlugin * CreateTuniacExportPlugin(void)
{
	ITuniacExportPlugin * pExporter = new CNumberedFileExporter;
	return(pExporter);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacExportPluginVersion(void)
{
	return ITUNIACEXPORTPLUGIN_VERSION;
}

/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
*/

#pragma once

#include "IPlaylist.h"

#include "ITuniacImportPlugin.h"
#include "ITuniacExportPlugin.h"


class CImportExportManager
{
protected:

	typedef struct
	{
		HINSTANCE				hDLL;
		ITuniacImportPlugin *	pImporter;
		ITuniacExportPlugin *	pExporter;
	} PluginEntry;
	Array<PluginEntry, 3>		m_PluginArray;

	typedef struct
	{
		ITuniacExportPlugin *	pExporter;
		LPTSTR					szExt;
	} ExportExtension;
	Array<ExportExtension, 3>	m_ExportExtensions;

	bool			ImportFrom(ITuniacImportPlugin * pImporter, LPTSTR szSource); 
	bool			ExportTo(ITuniacExportPlugin * pExporter, LPTSTR szSource, EntryArray & entryArray);

public:
	CImportExportManager(void);
	~CImportExportManager(void);

	bool			Initialize(void);
	bool			Shutdown(void);

	ITuniacImportPlugin *	GetImporterAtIndex(unsigned long iImporter);
	ITuniacExportPlugin *	GetExporterAtIndex(unsigned long iExporter);

	unsigned long	NumExportExtensions(void);
	LPTSTR			GetExportExtension(unsigned long ulIndex);
	void			PopuplateExportMenu(HMENU hMenu, unsigned long ulBase);

	bool			CanImport(LPTSTR szSource);
	bool			CanExport(LPTSTR szSource);

	bool			Import(LPTSTR szSource);
	
	bool			Export(EntryArray & entryArray, LPTSTR szSource);

};

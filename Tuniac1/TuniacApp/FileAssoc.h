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
	Copyright (C) 2003-2014 Brett Hoyle
*/

#include <vector>

#pragma once

#define FILEASSOC_TYPE_OPEN		0x00000001
#define FILEASSOC_TYPE_PLAY		0x00000002
#define FILEASSOC_TYPE_QUEUE	0x00000004

class CFileAssoc
{
protected:
	Array<std::wstring, 1>	m_ExtList;

public:
	CFileAssoc(void);
	~CFileAssoc(void);

	bool				CleanAssociations();

	bool				UpdateExtensionList(void);
	int					GetExtensionCount(void);

	bool				AssociateExtension(unsigned long ulIndex);

	int					GetDefaultType(void);
	bool				SetDefaultType(int iType);

	bool				ReAssociate(int iTypes);
};

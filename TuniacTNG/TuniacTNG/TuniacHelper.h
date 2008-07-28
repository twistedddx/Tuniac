#pragma once

#include <Singleton.h>

class CTuniacHelper : public CSingleton<CTuniacHelper>

{
public:
	CTuniacHelper(void);
	~CTuniacHelper(void);

	bool GetTuniacRunFolder(String & toHere);

	bool GetFolderContents(String folder, StringArray & tohere, bool recurse);
	bool GetAllFilesInFolderWithExtension(String folder, String Extension, StringArray & toHere);

	bool FormatSystemTime(String & toHere, SYSTEMTIME & st);
};

#ifndef APE_WINDOWS_ENVIRONMENT_H
#define APE_WINDOWS_ENVIRONMENT_H

/*****************************************************************************************
Windows version
*****************************************************************************************/
#ifndef WINVER
	#define WINVER         0x0400
	#define _WIN32_WINNT   0x0400
	#define _WIN32_WINDOWS 0x0400
	#define _WIN32_IE      0x0400
#endif

/*****************************************************************************************
Unicode
*****************************************************************************************/
#ifndef UNICODE
	#define UNICODE
#endif
#ifndef _UNICODE
	#define _UNICODE
#endif

#endif // #APE_WINDOWS_ENVIRONMENT_H APE_ALL_H
#define VI_WAVEFORM			0x0001		// set if you need the waveform
#define VI_SPECTRUM			0x0002		// set if you need the FFT values 
#define SONIQUEVISPROC		0x0004		// set if you want to allow Soniques user pref vis to affect your vis
										//   for example - blur, smoke and zoom

#pragma pack (push, 8)

typedef struct 
{
	unsigned long	MillSec;			// Sonique sets this to the time stamp of end this block of data
	signed char		Waveform[2][512];	// Sonique sets this to the PCM data being outputted at this time
	unsigned char	Spectrum[2][256];	// Sonique sets this to a lowfidely version of the spectrum data
										//   being outputted at this time
} VisData;

class QueryInterface
{
public:
	virtual bool QueryInt(char* expression, int* value) = 0;
	virtual char* QueryString(char* expression) = 0;
	virtual void FreeString(char* String) = 0;
};

typedef struct _VisInfo
{
	unsigned long Version;				// 1 = supports Clicked(x,y,buttons)

	char	*PluginName;				// Set to the name of the plugin
	long	lRequired;					// Which vis data this plugin requires (set to a combination of
										//   the VI_WAVEFORM, VI_SPECTRUM and SONIQEUVISPROC flags)

	void	(*Initialize)(void);		// Called some time before your plugin is asked to render for
										// the first time
	BOOL	(*Render)( unsigned long *Video, int width, int height, int pitch, VisData* pVD);
										// Called for each frame. Pitch is in pixels and can be negative.
										// Render like this:
										// for (y = 0; y < height; y++)
										// {
										//    for (x = 0; x < width; x++)
										//       Video[x] = <pixel value>;
										//	  Video += pitch;
										// }
										//				OR
										// void PutPixel(int x, int y, unsigned long Pixel)
										// {
										//    _ASSERT( x >= 0 && x < width && y >= 0 && y < height );
										//	  Video[y*pitch+x] = Pixel;
										// }
	BOOL	(*SaveSettings)( char* FileName );
										// Use WritePrivateProfileString to save settings when this is called
										// Example:
										// WritePrivateProfileString("my plugin", "brightness", "3", FileName);
	BOOL	(*OpenSettings)( char* FileName );
										// Use GetPrivateProfileString similarly:
										// char BrightnessBuffer[256];
										// GetPrivateProfileString("my plugin", "brightness", "3", BrightnessBuffer, sizeof(BrightnessBuffer), FileName);

	BOOL	(*Deinit)( );
	BOOL	(*Clicked)( int x, int y, int buttons );
	BOOL	(*ReceiveQueryInterface)( QueryInterface* Interface );

} VisInfo;

#pragma pack (pop, 8)

#include "ituniacvisplugin.h"
class SoniqueQueryInterface : public QueryInterface
{
public:
	ITuniacVisHelper	*	m_pQueryHelper;

	void SetQueryHelper(ITuniacVisHelper *pHelper)
	{
		m_pQueryHelper = pHelper;
	}

private:
	bool QueryInt(char* expression, int* result)
	{
		if(strcmp(expression, "currentsonglength") == 0)
			*result = (int)m_pQueryHelper->GetVariable(Variable_LengthMS);

		else if(strcmp(expression, "currentsongposition") == 0)
			*result = (int)m_pQueryHelper->GetVariable(Variable_PositionMS);

		else if(strcmp(expression, "scheme_visual_left") == 0)
			*result = 1;

		else if(strcmp(expression, "scheme_visual_right") == 0)
			*result = 1;

		else if(strcmp(expression, "scheme_visual_blend") == 0)
			*result = 1;

		else
			*result = 0;

		return true;
	}

	char* QueryString(char* expression)
	{
		if(strcmp(expression, "currentsongtitle") == 0)
		{
			LPTSTR szTitle = (LPTSTR)m_pQueryHelper->GetVariable(Variable_SongTitle);
			char * mbString = (char *)malloc(512 * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, szTitle, 512, mbString, 512, NULL, NULL);
			return mbString;
		}

		else if(strcmp(expression, "currentsongauthor") == 0)
		{
			LPTSTR szArtist = (LPTSTR)m_pQueryHelper->GetVariable(Variable_Artist);
			char * mbString = (char *)malloc(512 * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, szArtist, 512, mbString, 512, NULL, NULL);
			return mbString;
		}

		else if(strcmp(expression, "currentsongfilename") == 0)
		{
			TCHAR szData[512];
			m_pQueryHelper->GetTrackInfo(szData, 512, TEXT("@F"), 0);
			char * mbString = (char *)malloc(512 * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, szData, 512, mbString, 512, NULL, NULL);
			return mbString;
		}

		else if(strcmp(expression, "currentsongdisplaystring") == 0)
		{
			TCHAR szData[512];
			m_pQueryHelper->GetTrackInfo(szData, 512, NULL, 0);
			char * mbString = (char *)malloc(512 * sizeof(char));
			WideCharToMultiByte(CP_ACP, 0, szData, 512, mbString, 512, NULL, NULL);
			return mbString;
		}

		else if(strcmp(expression, "currentskinname") == 0)
			return (char*)"Default Skin";

		else
			//OutputDebugStringA(expression);
			return (char*)"Unknown Query";
	}

	void FreeString(char* String)
	{
		if(String)
			free(String);

		return;
	}
};

typedef VisInfo* (WINAPI *QUERYMODULE)(void);

class SoniqueVisExternal
{
private:

	HINSTANCE				m_DllInst;
	VisInfo					*p_PluginInfo;

	SoniqueQueryInterface	QInterface;
	QUERYMODULE				QueryModule;

public:
	SoniqueVisExternal();
	~SoniqueVisExternal();

	void Init();
	void DeInit();

	virtual char* GetName();

	virtual BOOL SaveSettings( char* szFileName );
	virtual BOOL LoadSettings( char* szFileName );

	virtual BOOL Render(unsigned long *Video, int width, int height, int pitch, VisData* pVD );

	void Clicked(int x, int y);

	bool LoadFromExternalDLL(LPTSTR PluginName);
	bool Shutdown(void);

	bool NeedsVisFX(void)
	{
		//if(p_PluginInfo)
		//{
			return (p_PluginInfo->lRequired & SONIQUEVISPROC);

		//}
		return false;
	}

	bool NeedsSpectrum(void)
	{
		//if(p_PluginInfo)
		//{
			return (p_PluginInfo->lRequired & VI_SPECTRUM);

		//}
		return false;
	}

	bool NeedsWaveform(void)
	{
		//if(p_PluginInfo)
		//{
			return (p_PluginInfo->lRequired & VI_WAVEFORM);

		//}
		return false;
	}

	bool SetQueryHelper(ITuniacVisHelper *pHelper)
	{
		QInterface.SetQueryHelper(pHelper);
		return true;
	}
};


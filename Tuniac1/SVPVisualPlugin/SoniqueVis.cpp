#include "stdafx.h"
#include "SoniqueVis.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Externam vises! Visual

SoniqueVisExternal::SoniqueVisExternal()
{
	p_PluginInfo	= NULL;
	m_DllInst		= NULL;
	bInitialized	= false;
	crashed			= false;
}

SoniqueVisExternal::~SoniqueVisExternal()
{
}

bool SoniqueVisExternal::LoadFromExternalDLL(LPTSTR PluginName)
{
	m_DllInst		= LoadLibrary ( PluginName );
	bool worked = false;

	if( m_DllInst )
	{
		QueryModule		= (VisInfo* (*)()) GetProcAddress( m_DllInst, "QueryModule" );
		if(QueryModule)
		{
			p_PluginInfo	= QueryModule();

			p_PluginInfo->Initialize();

			if (p_PluginInfo->Version >= 2) 
				p_PluginInfo->ReceiveQueryInterface(&QInterface);


			return true;
		}
	}

	return worked;
};

bool SoniqueVisExternal::Shutdown(void)
{
	DeInit();

	QueryModule		= NULL;
	p_PluginInfo	= NULL;

	if( m_DllInst )
	{
		FreeLibrary( m_DllInst );
		m_DllInst = 0;
	}

	return true;
}


void SoniqueVisExternal::Init()
{
	if( p_PluginInfo )
		p_PluginInfo->Initialize();
}

void SoniqueVisExternal::DeInit()
{
	if( p_PluginInfo )
	{
		if (p_PluginInfo->Version >= 1)
		{
			p_PluginInfo->Deinit();
		}
	}
}

char* SoniqueVisExternal::GetName()
{ 
	if( p_PluginInfo )
		return p_PluginInfo->PluginName; 
	else 
		return "BAH";
};

BOOL SoniqueVisExternal::Render(  void *Video, int width, int height, int pitch, VisData* pVD )
{
	BOOL worked = false;

	if(p_PluginInfo)
		worked = p_PluginInfo->Render((unsigned long*)Video, width, height, pitch, pVD);

	return(worked);
};

BOOL SoniqueVisExternal::SaveSettings( char* szFileName )
{
	if( p_PluginInfo )
		return p_PluginInfo->SaveSettings(szFileName);

	return(FALSE);
};

BOOL SoniqueVisExternal::LoadSettings( char* szFileName )
{
	if( p_PluginInfo )
		return p_PluginInfo->OpenSettings(szFileName);

	return(FALSE);
};

void SoniqueVisExternal::Clicked(int x, int y)
{
	if(p_PluginInfo)
	{
		if (p_PluginInfo->Version >= 1)
		{
			p_PluginInfo->Clicked(x, y, 1);
		}
	}
};

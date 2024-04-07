#include "stdafx.h"
#include "bassinfomanager.h"
// actual class definition here


CBASSInfoManager::CBASSInfoManager(void)
{
}

CBASSInfoManager::~CBASSInfoManager(void)
{
}

void			CBASSInfoManager::Destroy(void)
{
    delete this;
}

unsigned long	CBASSInfoManager::GetNumExtensions(void)
{
    
    return 0;
}

LPTSTR			CBASSInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
    /*
    static LPTSTR exts[] = 
    {
        TEXT(".mp3")
    };

    return exts[ulExtentionNum];
    */

    return NULL;
}

bool			CBASSInfoManager::CanHandle(LPTSTR szSource)
{
    for (unsigned int x = 0; x < GetNumExtensions(); x++)
    {
        if (!StrCmpI(SupportedExtension(x), PathFindExtension(szSource)))
        {
            return(true);
        }
    }
    if (StrCmpN(szSource, TEXT("MICINPUT"), 7) == 0)
    {
        if (StrCmpN(szSource, TEXT("MICINPUT:GetDevices"), 18) == 0)
        {
            m_DeviceArray.RemoveAll();

            BASS_DEVICEINFO info;
            for (int device = 0; BASS_RecordGetDeviceInfo(device, &info); device++)
            {
                if (info.flags & BASS_DEVICE_ENABLED) // device is enabled
                {
                    InfoDevice CurrentDevice = { 0 };
                    TCHAR devicename[128] = {0};
                    MultiByteToWideChar(CP_UTF8, 0, info.name, (int)strlen(info.name), CurrentDevice.devicename, 128);

                    CurrentDevice.channels = 2;
                    //CurrentDevice.samplerate = 48000;

                    switch (info.flags & BASS_DEVICE_TYPE_MASK)
                    {
                        case BASS_DEVICE_TYPE_DIGITAL:      //An audio endpoint device that connects to an audio adapter through a connector for a digital interface of unknown type.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_DIGITAL");
                        }
                        break;
                        case BASS_DEVICE_TYPE_DISPLAYPORT:	//An audio endpoint device that connects to an audio adapter through a DisplayPort connector.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_DISPLAYPORT");
                        }
                        break;
                        case BASS_DEVICE_TYPE_HANDSET:      //The part of a telephone that is held in the hand and that contains a speaker and a microphone for two - way communication.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_HANDSET");
                        }
                        break;
                        case BASS_DEVICE_TYPE_HDMI:         //An audio endpoint device that connects to an audio adapter through a High - Definition Multimedia Interface(HDMI) connector.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_HDMI");
                        }
                        break;
                        case BASS_DEVICE_TYPE_HEADPHONES:	//A set of headphones.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_HEADPHONES");
                        }
                        break;
                        case BASS_DEVICE_TYPE_HEADSET:      //An earphone or a pair of earphones with an attached mouthpiece for two - way communication.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_HEADSET");
                        }
                        break;
                        case BASS_DEVICE_TYPE_LINE:	        //An audio endpoint device that sends a line - level analog signal to a line - input jack on an audio adapter or that receives a line - level analog signal from a line - output jack on the adapter.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_LINE");
                        }
                        break;
                        case BASS_DEVICE_TYPE_MICROPHONE:	//A microphone.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_MICROPHONE");
                        }
                        break;
                        case BASS_DEVICE_TYPE_NETWORK:	    //An audio endpoint device that the user accesses remotely through a network.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_NETWORK");
                        }
                        break;
                        case BASS_DEVICE_TYPE_SPDIF:	    //An audio endpoint device that connects to an audio adapter through a Sony / Philips Digital Interface(S / PDIF) connector.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_SPDIF");
                        }
                        break;
                        case BASS_DEVICE_TYPE_SPEAKERS:	    //A set of speakers.
                        {
                            StringCchCopy(CurrentDevice.type, 256, L"BASS_DEVICE_TYPE_SPEAKERS");
                        }
                        break;
                    }

                    BASS_RecordInit(device);
                    BASS_RECORDINFO RecordInfo = { 0 };
                    BASS_RecordGetInfo(&RecordInfo);
                    CurrentDevice.samplerate = RecordInfo.freq;
                    int inputs = RecordInfo.inputs;
                    DWORD format = RecordInfo.formats;
                    BASS_RecordFree();

                    m_DeviceArray.AddTail(CurrentDevice);
                }

                BASS_RecordFree();
            }
            if (m_DeviceArray.GetCount())
                return true;
        }
        else
        {
            m_ActiveDevice = UNKNOWN_DEVICE;
            swscanf_s(szSource, TEXT("MICINPUT:%d"), &m_ActiveDevice);
            if (m_ActiveDevice != UNKNOWN_DEVICE && m_ActiveDevice >= 0 && m_ActiveDevice < m_DeviceArray.GetCount())
            {
                return true;
            }
        }
    }


    return false;
}

bool			CBASSInfoManager::GetInfo(LibraryEntry * libEnt)
{
    if (m_ActiveDevice != UNKNOWN_DEVICE && m_ActiveDevice >= 0 && m_ActiveDevice < m_DeviceArray.GetCount())
    {
        libEnt->ulBitRate = BITRATE_UNKNOWN;
        libEnt->ulChannels = m_DeviceArray[m_ActiveDevice].channels; //2
        libEnt->ulSampleRate = m_DeviceArray[m_ActiveDevice].samplerate; //48000
        libEnt->ulBitsPerSample = BITDEPTH_UNDEFINABLE;
        libEnt->ulPlaybackTime = LENGTH_STREAM;

        libEnt->dwTrack[0] = m_ActiveDevice;

        StringCchCopy(libEnt->szTitle, 128, m_DeviceArray[m_ActiveDevice].devicename);
        StringCchCopy(libEnt->szComment, 128, m_DeviceArray[m_ActiveDevice].type);

        StringCchCopy(libEnt->szArtist, 128, L"Mic Input");
        _itow(m_ActiveDevice, libEnt->szAlbum, 10);

        std::time_t time = std::time(nullptr);
        std::tm* const pTInfo = std::localtime(&time);
        libEnt->ulYear = pTInfo->tm_year + 1900;

        return true;
    }
    return false;
}

bool			CBASSInfoManager::SetInfo(LibraryEntry * libEnt)
{
    return true;
}

unsigned long	CBASSInfoManager::GetNumberOfAlbumArts(void)
{
    return 0;
}

bool			CBASSInfoManager::GetAlbumArt(unsigned long		ulImageIndex,
    LPVOID* pImageData,
    unsigned long* ulImageDataSize,
    LPTSTR				szMimeType,
    unsigned long* ulArtType)
{
    return false;
}

bool			CBASSInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
    return false;
}

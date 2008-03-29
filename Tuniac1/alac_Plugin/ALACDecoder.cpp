#include "StdAfx.h"

#include "alacdecoder.h"

extern "C" int host_bigendian = 0;
extern void set_endian();

void set_endian()
{

/* what the hell is this?!? */
	
/*    uint32_t integer = 0x000000aa;
    unsigned char *p = (unsigned char*)&integer;

    if (p[0] == 0xaa) host_bigendian = 0;
    else host_bigendian = 1; */

/* Ok... we are doing it like that: */
	
	#if __BYTE_ORDER == __LITTLE_ENDIAN
	host_bigendian = 0;
	#elif __BYTE_ORDER == __BIG_ENDIAN
	host_bigendian = 1;
	#endif
}

CALACDecoder::CALACDecoder(void)
{
}

CALACDecoder::~CALACDecoder(void)
{
}

bool CALACDecoder::Open(LPTSTR szSource)
{
    m_file = _wfopen(szSource, TEXT("rbS"));
    if(m_file == NULL)
		return false;

	set_endian();
	stream = stream_create_file(m_file, 1);

	if(!qtmovie_read(stream, &demux)) {
		//ALAC: alac_decode: failed to load the QuickTime movie headers!
		return false;
	}

	alac = create_alac(demux.sample_size, demux.num_channels);
	alac_set_info(alac, (char *)demux.codecdata);

	decoded_frames = 0;

	return(true);
}

bool CALACDecoder::Close()
{
	stream_destroy(stream);
	return(true);
}

void		CALACDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CALACDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = demux.sample_rate;
	*Channels	= demux.num_channels;

	return(true);
}

bool		CALACDecoder::GetLength(unsigned long * MS)
{
	unsigned int i;
	unsigned long output_time = 0;
	unsigned int index = 0;
	unsigned int accum = 0;
        for (i = 0; i < demux.num_sample_byte_sizes; i++)
        {
            unsigned int thissample_duration;

		while((demux.time_to_sample[index].sample_count + accum) <= i) {
			accum += demux.time_to_sample[index].sample_count;
        		index++;
			if(index >= demux.num_time_to_samples) {
				fprintf(stderr, "ALAC: get_song_length: sample %i does not have a duration\n", i);
				return 0;
			}
		}
	    
	    thissample_duration = demux.time_to_sample[index].sample_duration;
	    output_time += thissample_duration;
        }
	// ( Sample_count / sample_rate ) * 1000 ---> to get time in milliseconds
	song_length = (output_time / demux.sample_rate) * 1000;
	*MS = song_length;

	return(true);
}

bool		CALACDecoder::SetPosition(unsigned long * MS)
{
	return(true);
}

bool		CALACDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CALACDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	if(decoded_frames == demux.num_sample_byte_sizes)
		return false;

	stream_read(stream, demux.sample_byte_size[decoded_frames], stream_buffer);
	decode_frame(alac, stream_buffer, buffer, &outputBytes);

	decoded_frames++;

	int		* pData		= (int *)stream_buffer;
	float	* pBuffer	= m_Buffer;
	
	for(int x=0; x<outputBytes;x++)
	{
		*pBuffer = (float) ((double)(*pData) / m_divider);	

		pData ++;
		pBuffer++;
	}
	*ppBuffer = m_Buffer;


	*NumSamples = outputBytes;

	return(true);
}
#include "StdAfx.h"

#include "alacdecoder.h"

#define BUFFER_SIZE 4096*4

extern "C" int host_bigendian = 0;
extern void set_endian();

void set_endian()
{
    uint32_t integer = 0x000000aa;
    unsigned char *p = (unsigned char*)&integer;

    if (p[0] == 0xaa) host_bigendian = 0;
    else host_bigendian = 1;
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

	return true;
}

bool		CALACDecoder::SetPosition(unsigned long * MS)
{
	/* Seek to the sample containing sound_sample_loc. Return 1 on success 
 * (and modify sound_samples_done and current_sample), 0 if failed.
 *
 * Seeking uses the following arrays:
 *
 * 1) the time_to_sample array contains the duration (in sound samples) 
 *    of each sample of data.
 *
 * 2) the sample_byte_size array contains the length in bytes of each
 *    sample.
 *
 * 3) the sample_to_chunk array contains information about which chunk
 *    of samples each sample belongs to.
 *
 * 4) the chunk_offset array contains the file offset of each chunk.
 *
 * So find the sample number we are going to seek to (using time_to_sample)
 * and then find the offset in the file (using sample_to_chunk, 
 * chunk_offset sample_byte_size, in that order.).
 *
 */

    uint32_t i;
    uint32_t j;
    uint32_t new_sample;
    uint32_t new_sound_sample;
    uint32_t new_pos;

	//wrong
	uint32_t sound_sample_loc;

    /* First check we have the appropriate metadata - we should always
     * have it.
     */
       
    if ((demux.num_time_to_samples==0) ||
       (demux.num_sample_byte_sizes==0))
    { 
        return 0; 
    }

    /* Find the destination block from time_to_sample array */
    
    i = 0;
    new_sample = 0;
    new_sound_sample = 0;

    while ((i < demux.num_time_to_samples) &&
        (new_sound_sample < sound_sample_loc)) 
    {
        j = (sound_sample_loc - new_sound_sample) /
            demux.time_to_sample[i].sample_duration;
    
        if (j <= demux.time_to_sample[i].sample_count)
        {
            new_sample += j;
            new_sound_sample += j * 
                demux.time_to_sample[i].sample_duration;
            break;
        } 
        else 
        {
            new_sound_sample += (demux.time_to_sample[i].sample_duration
                * demux.time_to_sample[i].sample_count);
            new_sample += demux.time_to_sample[i].sample_count;
            i++;
        }
    }

    /* We know the new block, now calculate the file position. */
  
    //new_pos = get_sample_offset(demux, new_sample);

    /* We know the new file position, so let's try to seek to it */
  
	//*MS = (unsigned long)(new_pos);
    
    return false;
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

	unsigned long numSamples = ((outputBytes / *demux.sample_byte_size) / demux.num_channels);

	short	* pData		= (short *)buffer;
	float	* pBuffer	= m_Buffer;
	
	for(unsigned long x=0; x<numSamples;x++)
	{
		*pBuffer = (*pData) / 32767.0f;
		pData++;
		pBuffer++;
	}
	*ppBuffer = m_Buffer;


	*NumSamples = numSamples;

	return true;
}
/*
 *  maindatabuffer.h
 *  audioenginetest
 *
 *  Created by Tony Million on 29/11/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "stdint.h"

/* determining MAINBUF_SIZE:
 *   max mainDataBegin = (2^9 - 1) bytes (since 9-bit offset) = 511
 *   max nSlots (concatenated with mainDataBegin bytes from before) = 1440 - 9 - 4 + 1 = 1428
 *   511 + 1428 = 1939, round up to 1940 (4-byte align)
 */

#define BITRESERVOIR_SIZE		2048

class MainDataBuffer
{
protected:
	uint8_t		BitReservoir[BITRESERVOIR_SIZE];
	
	int32_t		BitReservoirBytes;

public:
	bool		adddata(uint8_t	* pData, uint32_t nslots, uint32_t	mainDataBegin);
	uint32_t	getMainDataBytes(void);
	
	bool		ExtractBytes(uint8_t	* pBuffer, uint32_t bytes);
	
	void		Reset();
};

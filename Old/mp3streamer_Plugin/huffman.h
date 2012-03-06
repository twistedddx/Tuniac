/*
 *  huffman.h
 *  audioenginetest
 *
 *  Created by Tony Million on 02/12/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#include "bitstream.h"

bool HuffmanDecode2(uint32_t TableNum, int * px, int * py, bitstream & br);
bool HuffmanDecode4(uint32_t TableNum, int * px, int * py, int * pv, int * pw, bitstream & br);

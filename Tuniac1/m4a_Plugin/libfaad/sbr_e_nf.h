/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2004 M. Bakker, Ahead Software AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Ahead Software through Mpeg4AAClicense@nero.com.
**
** $Id: sbr_e_nf.h,v 1.1 2005/11/27 01:00:36 tonymillion Exp $
**/

#ifndef __SBR_E_NF_H__
#define __SBR_E_NF_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifndef FIXED_POINT
#define P2_TABLE_OFFSET 35
#define P2_TABLE_MAX 91
#else
#define P2Q_TABLE_OFFSET 24
#define P2Q_TABLE_MAX 7
#define P2_TABLE_OFFSET 0
#define P2_TABLE_MAX 31
#endif
#define P2_TABLE_RCP_OFFSET 12
#define P2_TABLE_RCP_MAX 21


void extract_envelope_data(sbr_info *sbr, uint8_t ch);
void extract_noise_floor_data(sbr_info *sbr, uint8_t ch);
void envelope_noise_dequantisation(sbr_info *sbr, uint8_t ch);
void unmap_envelope_noise(sbr_info *sbr);

#ifdef __cplusplus
}
#endif
#endif


//------------------------------------------------------------------------------
// h2v2.cpp
// Upsampling/colorspace conversion (H2V2, YCbCr)
// Last updated: Nov. 16, 2000 
// Copyright (C) 1994-2000 Rich Geldreich
// richgel@voicenet.com
//
// This is a popular case, so it's worth seperating out and optimizing a bit.
// If you compile this module with the Intel Compiler, the MMX version will
// automatically be compiled in.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//------------------------------------------------------------------------------
#include "jpegdecoder.h"

//------------------------------------------------------------------------------
#define FIX(x, b) ((long) ((x) * (1L<<(b)) + 0.5))
//------------------------------------------------------------------------------
// YCbCr H2V2 (2x2:1:1, 6 blocks per MCU) to 24-bit RGB
// This case is very popular, so it's important that it's fast.
// If this module is compiled with the Intel Compiler the faster
// MMX specific version will also be available.
// FIXME: Create all-asm version, so Intel Compiler isn't needed.
void jpeg_decoder::H2V2Convert(void)
{
  int row = max_mcu_y_size - mcu_lines_left;
  uchar *d0 = scan_line_0;
  uchar *d1 = scan_line_1;
  uchar *y;
  uchar *c;
  if (row < 8)
    y = Psample_buf + row * 8;
  else
    y = Psample_buf + 64*2 + (row & 7) * 8;

  c = Psample_buf + 64*4 + (row >> 1) * 8;

  for (int i = max_mcus_per_row; i > 0; i--)
  {
    for (int l = 0; l < 2; l++)
    {
      for (int j = 0; j < 8; j += 2)
      {
        int cb = c[0];
        int cr = c[64];

        int rc = crr[cr];
        int gc = ((crg[cr] + cbg[cb]) >> 16);
        int bc = cbb[cb];

        int yy = y[j];
        d0[0] = clamp(yy+rc);
        d0[1] = clamp(yy+gc);
        d0[2] = clamp(yy+bc);

        yy = y[j+1];
        d0[4] = clamp(yy+rc);
        d0[5] = clamp(yy+gc);
        d0[6] = clamp(yy+bc);

        yy = y[j+8];
        d1[0] = clamp(yy+rc);
        d1[1] = clamp(yy+gc);
        d1[2] = clamp(yy+bc);

        yy = y[j+8+1];
        d1[4] = clamp(yy+rc);
        d1[5] = clamp(yy+gc);
        d1[6] = clamp(yy+bc);

        d0 += 8;
        d1 += 8;

        c++;
      }
      y += 64;
    }

    y += 64*6 - 64*2;
    c += 64*6 - 8;
  }
}


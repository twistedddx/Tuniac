/*
 * Copyright (c) 2002 Microsoft Corporation.  All rights reserved.
 */

//------------------------------------------------------------------------------
// Double buffer painting operations between CDoubleBuffer::Begin and
// CDoubleBuffer::End.
//


//------------------------------------------------------------------------------
#include "stdafx.h"
#include "DoubleBuffer.h"


//------------------------------------------------------------------------------
//  CDoubleBuffer impl
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CDoubleBuffer::CDoubleBuffer()
    : _hdcMem(NULL),
      _hbmpMem(NULL),
      _hdcPaint(NULL)
{
    ZeroMemory(&_rcPaint, sizeof(_rcPaint));
}


//------------------------------------------------------------------------------
CDoubleBuffer::~CDoubleBuffer()
{
    // If used properly this class doesn't require a dtor
    Cleanup();
}


//------------------------------------------------------------------------------
VOID CDoubleBuffer::Begin(IN OUT HDC &hdc, IN RECT *prcPaint)
{
    if (hdc)
    {
        _hdcMem = CreateCompatibleDC(hdc);
        if (_hdcMem)
        {
            _hbmpMem = CreateCompatibleBitmap(hdc,
                                              RECTWIDTH(*prcPaint),
                                              RECTHEIGHT(*prcPaint));

            if (_hbmpMem)
            {
                SelectObject(_hdcMem, _hbmpMem);
                CopyRect(&_rcPaint, prcPaint);
                OffsetWindowOrgEx(_hdcMem,
                                  _rcPaint.left,
                                  _rcPaint.top,
                                  NULL);

                // cache the original DC and pass out the memory DC
                _hdcPaint = hdc;
                hdc = _hdcMem;
            }
            else
            {
                Cleanup();
            }
        }
    }
}


//------------------------------------------------------------------------------
VOID CDoubleBuffer::End(IN OUT HDC &hdc)
{
    if (hdc)
    {
        if (hdc == _hdcMem)
        {
            BitBlt(_hdcPaint,
                   _rcPaint.left,
                   _rcPaint.top,
                   RECTWIDTH(_rcPaint),
                   RECTHEIGHT(_rcPaint),
                   _hdcMem,
                   _rcPaint.left,
                   _rcPaint.top,
                   SRCCOPY);

            // restore the original DC
            hdc = _hdcPaint;

            Cleanup();
        }
    }
}


//------------------------------------------------------------------------------
VOID CDoubleBuffer::Cleanup()
{
    if (_hbmpMem)
    {
        DeleteObject(_hbmpMem);
        _hbmpMem = NULL;
    }

    if (_hdcMem)
    {
        DeleteDC(_hdcMem);
        _hdcMem = NULL;
    }

    _hdcPaint = NULL;
    ZeroMemory(&_rcPaint, sizeof(_rcPaint));
}

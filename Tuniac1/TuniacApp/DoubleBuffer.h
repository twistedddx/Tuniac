/*
 * Copyright (c) 2002 Microsoft Corporation.  All rights reserved.
 */

//------------------------------------------------------------------------------
#ifndef __DoubleBuffer_h__
#define __DoubleBuffer_h__ 


//------------------------------------------------------------------------------
class CDoubleBuffer
{
    private:
        HDC     _hdcMem;
        HBITMAP _hbmpMem;
        HDC     _hdcPaint;
        RECT    _rcPaint;

        VOID    Cleanup();

    public:
        // ctor dtor
        CDoubleBuffer();
        ~CDoubleBuffer();

        VOID    Begin(IN OUT HDC &, IN RECT *);
        VOID    End(IN OUT HDC &);
};

#endif

//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2017 Andrew Duncan
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-------------------------------------------------------------------------

#ifndef OLED_PIXEL_H
#define OLED_PIXEL_H

//------------------------------------------------------------------------

#include <cstdint>

#include "FileDescriptor.h"
#include "point.h"

//------------------------------------------------------------------------

namespace SSD1306
{

//------------------------------------------------------------------------

using OledPoint = Point<int>;

enum class PixelStyle
{
    Set,
    Unset,
    Xor,
    None
};

PixelStyle oppositeStyle(PixelStyle style);

//------------------------------------------------------------------------

class OledPixel
{
public:
    virtual ~OledPixel() = 0;

    void pixel(SSD1306::OledPoint p, SSD1306::PixelStyle style);

    virtual void clear() = 0;
    virtual void fill() = 0;
    virtual bool isSetPixel(SSD1306::OledPoint p) const = 0;
    virtual void setPixel(SSD1306::OledPoint p) = 0;
    virtual void unsetPixel(SSD1306::OledPoint p) = 0;
    virtual void xorPixel(SSD1306::OledPoint p) = 0;

    bool pixelInside(SSD1306::OledPoint p) const;

    void setFrom(const OledPixel& pixels,
                 SSD1306::OledPoint offset = {0,0});

    virtual int width() const = 0;
    virtual int height() const = 0;
};

//------------------------------------------------------------------------

} // namespace SSD1306

//------------------------------------------------------------------------

#endif

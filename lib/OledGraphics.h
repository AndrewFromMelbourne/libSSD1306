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

#ifndef OLED_GRAPHICS_H
#define OLED_GRAPHICS_H

//-------------------------------------------------------------------------

#include <cstdint>

#include "OledPixel.h"
#include "point.h"

//-------------------------------------------------------------------------

namespace SSD1306
{

//-------------------------------------------------------------------------

void
box(
    const OledPoint& p1,
    const OledPoint& p2,
    PixelStyle style,
    OledPixel& pixels);

//-------------------------------------------------------------------------

void
boxFilled(
    const OledPoint& p1,
    const OledPoint& p2,
    PixelStyle style,
    OledPixel& pixels);

//-------------------------------------------------------------------------

void
line(
    const OledPoint& p1,
    const OledPoint& p2,
    PixelStyle style,
    OledPixel& pixels);

//-------------------------------------------------------------------------

void
horizontalLine(
    int x1,
    int x2,
    int y,
    PixelStyle style,
    OledPixel& pixels);

//-------------------------------------------------------------------------

void
verticalLine(
    int x,
    int y1,
    int y2,
    PixelStyle style,
    OledPixel& pixels);

//-------------------------------------------------------------------------

} // namespace SSD1306

//-------------------------------------------------------------------------

#endif

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

#include <algorithm>
#include <iostream>

#include "OledPixel.h"

//------------------------------------------------------------------------

SSD1306::OledPixel::~OledPixel() = default;

//------------------------------------------------------------------------

SSD1306::PixelStyle
SSD1306::oppositeStyle(
    PixelStyle style)
{
    PixelStyle opposite{PixelStyle::None};

    switch (style)
    {
    case PixelStyle::Set:

        opposite = PixelStyle::Unset;
        break;

    case PixelStyle::Unset:

        opposite = PixelStyle::Set;
        break;

    case PixelStyle::Xor:

        opposite = PixelStyle::Xor;
        break;

    case PixelStyle::None:

        opposite = PixelStyle::Xor;
        break;
    }

    return opposite;
}

//------------------------------------------------------------------------

void
SSD1306::OledPixel::pixel(
    SSD1306::OledPoint p,
    SSD1306::PixelStyle style)
{
    switch (style)
    {
    case PixelStyle::Set:

        setPixel(p);
        break;

    case PixelStyle::Unset:

        unsetPixel(p);
        break;

    case PixelStyle::Xor:

        xorPixel(p);
        break;

    case PixelStyle::None:

        break;
    }
}

//------------------------------------------------------------------------

bool
SSD1306::OledPixel::pixelInside(
    SSD1306::OledPoint p) const
{
    return (p.x() >= 0) &&
           (p.x() < width()) &&
           (p.y() >= 0) &&
           (p.y() < height());
}

//------------------------------------------------------------------------

void
SSD1306::OledPixel::setFrom(
    const OledPixel& pixels,
    SSD1306::OledPoint offset)
{
    auto xStart = std::max(0, offset.x());
    auto xEnd = std::min(width(), pixels.width() + offset.x());
    auto yStart = std::max(0, offset.y());
    auto yEnd = std::min(height(), pixels.height() + offset.y());

    for (auto y = yStart ; y < yEnd ; ++y)
    {
        for (auto x = xStart ; x < xEnd ; ++x)
        {
            OledPoint inputP{x - offset.x(), y - offset.y()};
            OledPoint outputP{x, y};

            if (pixels.isSetPixel(inputP))
            {
                setPixel(outputP);
            }
            else
            {
                unsetPixel(outputP);
            }
        }
    }
}


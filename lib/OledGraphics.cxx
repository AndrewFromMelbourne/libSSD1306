//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Andrew Duncan
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

#include <cmath>
#include <cstdint>

#include "OledGraphics.h"
#include "point.h"

//-------------------------------------------------------------------------

void
SSD1306::box(
    const SSD1306::OledPoint& p1,
    const SSD1306::OledPoint& p2,
    SSD1306::PixelStyle style,
    SSD1306::OledPixel& pixels)
{
    verticalLine(p1.x(), p1.y(), p2.y(), style, pixels);
    horizontalLine(p1.x(), p2.x(), p1.y(), style, pixels);
    verticalLine(p2.x(), p1.y(), p2.y(), style, pixels);
    horizontalLine(p1.x(), p2.x(), p2.y(), style, pixels);
}

//-------------------------------------------------------------------------

void
SSD1306::boxFilled(
    const SSD1306::OledPoint& p1,
    const SSD1306::OledPoint& p2,
    SSD1306::PixelStyle style,
    SSD1306::OledPixel& pixels)
{
    int sign_y = (p1.y() <= p2.y()) ? 1 : -1;
    int y = p1.y();

    horizontalLine(p1.x(), p2.x(), y, style, pixels);

    while (y != p2.y())
    {
        y += sign_y;
        horizontalLine(p1.x(), p2.x(), y, style, pixels);
    }
}

//-------------------------------------------------------------------------

void
SSD1306::line(
    const SSD1306::OledPoint& p1,
    const SSD1306::OledPoint& p2,
    SSD1306::PixelStyle style,
    SSD1306::OledPixel& pixels)
{
    if (p1.y() == p2.y())
    {
        horizontalLine(p1.x(), p2.x(), p1.y(), style, pixels);
    }
    else if (p1.x() == p2.x())
    {
        verticalLine(p1.x(), p1.y(), p2.y(), style, pixels);
    }
    else
    {
        int dx = std::abs(p2.x() - p1.x());
        int dy = std::abs(p2.y() - p1.y());

        int sign_x = (p1.x() <= p2.x()) ? 1 : -1;
        int sign_y = (p1.y() <= p2.y()) ? 1 : -1;

        int x = p1.x();
        int y = p1.y();

        pixels.pixel(p1, style);

        if (dx > dy)
        {
            int d = 2 * dy - dx;
            int incrE = 2 * dy;
            int incrNE = 2 * (dy - dx);

            while (x != p2.x())
            {
                x += sign_x;

                if (d <= 0)
                {
                    d += incrE;
                }
                else
                {
                    d += incrNE;
                    y += sign_y;
                }

                pixels.pixel(SSD1306::OledPoint{x, y}, style);
            }
        }
        else
        {
            int d = 2 * dx - dy;
            int incrN = 2 * dx;
            int incrNE = 2 * (dx - dy);

            while (y != p2.y())
            {
                y += sign_y;

                if (d <= 0)
                {
                    d += incrN;
                }
                else
                {
                    d += incrNE;
                    x += sign_x;
                }

                pixels.pixel(SSD1306::OledPoint{x, y}, style);
            }
        }
    }
}

//-------------------------------------------------------------------------

void
SSD1306::horizontalLine(
    int x1,
    int x2,
    int y,
    SSD1306::PixelStyle style,
    SSD1306::OledPixel& pixels)
{
    int sign_x = (x1 <= x2) ? 1 : -1;
    int x = x1;

    pixels.pixel(SSD1306::OledPoint{x, y}, style);

    while (x != x2)
    {
        x += sign_x;
        pixels.pixel(SSD1306::OledPoint{x, y}, style);
    }
}

//-------------------------------------------------------------------------

void
SSD1306::verticalLine(
    int x,
    int y1,
    int y2,
    SSD1306::PixelStyle style,
    SSD1306::OledPixel& pixels)
{
    int sign_y = (y1 <= y2) ? 1 : -1;
    int y = y1;

    pixels.pixel(SSD1306::OledPoint{x, y}, style);

    while (y != y2)
    {
        y += sign_y;
        pixels.pixel(SSD1306::OledPoint{x, y}, style);
    }
}


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

#include "OledBitmap.h"
#include "OledFont8x16.h"
#include "OledGraphics.h"
#include "OledI2C.h"

#include <unistd.h>

#include <iostream>

//-------------------------------------------------------------------------

int
main()
{
    try
    {
        SSD1306::OledBitmap<16, 16> bitmap
        {
             0b00001010, 0b10100000,
             0b00001111, 0b11000000,
             0b00111111, 0b11101000,
             0b00111111, 0b01010000,
             0b01111111, 0b11101010,
             0b01111111, 0b11010100,
             0b11111111, 0b10101010,
             0b01110111, 0b01010000,
             0b11111111, 0b11101010,
             0b01010101, 0b01000000,
             0b00111010, 0b10100010,
             0b00010101, 0b00000000,
             0b00101010, 0b10101000,
             0b00000100, 0b00000000,
             0b00000010, 0b00100000,
             0b00000000, 0b00000000
        };

        SSD1306::OledI2C oled{"/dev/i2c-1", 0x3C};

        for (auto y = -(bitmap.height() / 2) ;
             y < (oled.height() + (bitmap.height() / 2)) ;
             y += bitmap.height())
        {
            for (auto x = -(bitmap.width() / 2) ;
                 x < (oled.width() + (bitmap.width() / 2)) ;
                 x += bitmap.width())
            {
                oled.setFrom(bitmap, SSD1306::OledPoint{x, y});
            }
        }

        oled.displayUpdate();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}


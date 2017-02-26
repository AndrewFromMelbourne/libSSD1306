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

#include "OledFont8x16.h"
#include "OledI2C.h"
#include "LinuxKeys.h"

#include <iostream>
#include <random>

//-------------------------------------------------------------------------

int
main()
{
    try
    {
        SSD1306::OledI2C oled{"/dev/i2c-1", 0x3C};
        drawString8x16(SSD1306::OledPoint{32, 24},
                       "Oled I" "\xFD" "C",
                       SSD1306::PixelStyle::Set,
                       oled);
        oled.displayUpdate();

        //-----------------------------------------------------------------

        LinuxKeys linuxKeys;
        LinuxKeys::PressedKey key;

        //-----------------------------------------------------------------


        std::random_device randomDevice;
        std::mt19937 randomGenerator{randomDevice()};
        std::uniform_int_distribution<> xDistribution{0, oled.width() - 1};
        std::uniform_int_distribution<> yDistribution{0, oled.height() - 1};


        //-----------------------------------------------------------------

        while(key.key != 27)
        {
            key = linuxKeys.pressed();

            switch (key.key)
            {
            case 'i':

                oled.displayInverse();

                break;

            case 'n':

                oled.displayNormal();

                break;

            case '+':

                oled.displayOn();

                break;

            case '-':

                oled.displayOff();

                break;
            }

            SSD1306::OledPoint p{xDistribution(randomGenerator),
                                 yDistribution(randomGenerator)};

            oled.xorPixel(p);
            oled.displayUpdate();
        }

        oled.clear();
        oled.displayUpdate();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}


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

#include <cmath>
#include <csignal>
#include <cstring>
#include <iostream>
#include <system_error>

#include "OledBitmap.h"
#include "OledI2C.h"
#include "LinuxKeys.h"

//-------------------------------------------------------------------------

using Bitmap = SSD1306::OledBitmap<SSD1306::OledI2C::Width,
                                   SSD1306::OledI2C::Height>;

using Point = SSD1306::OledPoint;

//-------------------------------------------------------------------------

namespace
{
volatile static std::sig_atomic_t run = 1;
}

//-------------------------------------------------------------------------

static void
signalHandler(
    int signalNumber)
{
    switch (signalNumber)
    {
    case SIGINT:
    case SIGTERM:

        run = 0;
        break;
    };
}

//-------------------------------------------------------------------------

int
newLife(
    SSD1306::OledPixel& pixels)
{
    int population = 0;

    for (int row = 0 ; row < pixels.height() ; ++row)
    {
        for (int col = 0 ; col < pixels.width() ; ++col)
        {
            Point p{col, row};

            if (rand() > (RAND_MAX / 2))
            {
                ++population;

                pixels.setPixel(p);
            }
            else
            {
                pixels.unsetPixel(p);
            }
        }
    }

    return population;
}

//-------------------------------------------------------------------------

int
neighbours(
    SSD1306::OledPixel& pixels,
    int x,
    int y)
{
    int neighbours = 0;

    for (auto dy = -1 ; dy <= 1 ;  ++dy)
    {
        for (auto dx = -1 ; dx <= 1 ;  ++dx)
        {
            if ((dx == 0) && (dy == 0))
            {
                continue;
            }

            int i = x + dx;
            int j = y + dy;

            if (i < 0)
            {
                i = (pixels.width() - 1 );
            }
            else if (i > (pixels.width() - 1 ))
            {
                i = 0;
            }

            if (j < 0)
            {
                j = (pixels.height() - 1);
            }
            else if (j > (pixels.height() - 1))
            {
                j = 0;
            }

            Point p{i, j};

            if (pixels.isSetPixel(p))
            {
                ++neighbours;
            }
        }
    }

    return neighbours;
}

//-------------------------------------------------------------------------

int
iterateLife(
    Bitmap& pixels)
{
    int population = 0;
    Bitmap last = pixels;

    for (auto row = 0 ; row < pixels.height() ; ++row)
    {
        for (auto col = 0 ; col < pixels.width() ; ++col)
        {
            Point p{col, row};
            bool live = last.isSetPixel(p);
            int n = neighbours(last, col, row);

            if (n < 2)
            {
                live = false;
            }
            else if (live && (n == 2))
            {
                live = true;
            }
            else if (n == 3)
            {
                live = true;
            }
            else
            {
                live = false;
            }

            if (live)
            {
                ++population;

                pixels.setPixel(p);
            }
            else
            {
                pixels.unsetPixel(p);
            }
        }
    }

    return population;
}

//-------------------------------------------------------------------------

int
main()
{
    try
    {
        constexpr std::array<int, 2> signals{SIGINT, SIGTERM};

        for (auto signal : signals)
        {
            if (std::signal(signal, signalHandler) == SIG_ERR)
            {
                std::string what{"installing "};
                what += strsignal(signal);
                what += " signal handler";

                throw std::system_error(errno,
                                        std::system_category(),
                                        what);
            }
        }

       SSD1306::OledI2C oled{"/dev/i2c-1", 0x3C};

        Bitmap pixels;

        newLife(pixels);
        oled.setFrom(pixels);
        oled.displayUpdate();

        LinuxKeys linuxKeys;
        LinuxKeys::PressedKey key;

        //-----------------------------------------------------------------

        while (run)
        {
            key = linuxKeys.pressed();
        
            if (key.isPressed)
            {
                switch (key.key)
                {
                case ' ':

                    newLife(pixels);
                    break;

                case '+':

                    oled.displayOn();
                    break;

                case '-':

                    oled.displayOff();
                    break;

                case 27:

                    run = 0;
                    break;
                }
            }

            iterateLife(pixels);
            oled.setFrom(pixels);
            oled.displayUpdate();
        }

        //-----------------------------------------------------------------

        oled.clear();
        oled.displayUpdate();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}


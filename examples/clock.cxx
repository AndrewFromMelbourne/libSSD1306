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

#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <system_error>

#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "OledFont8x8.h"
#include "OledFont8x16.h"
#include "OledI2C.h"

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

void
showTime(
    SSD1306::OledI2C& oled)
{
    //---------------------------------------------------------------------

    time_t now;
    time(&now);
    struct tm* tm = localtime(&now);

    //---------------------------------------------------------------------

    static constexpr SSD1306::PixelStyle style{SSD1306::PixelStyle::Set};

    //---------------------------------------------------------------------

    char time[12];

    auto length = strftime(time, sizeof(time), "%l:%M:%S %P", tm);
    int offset = (128 - (8 * length)) / 2;

    SSD1306::OledPoint location{offset, 18};

    location = drawString8x16(location, time, style, oled);

    //---------------------------------------------------------------------

    char date[12];

    length = strftime(date, sizeof(date), "%d %b %Y", tm);
    offset = (128 - (8 * length)) / 2;

    location.set(offset, location.y() + 20);

    location = drawString8x8(location, date, style, oled);

    //---------------------------------------------------------------------

    oled.displayUpdate();
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

        while (run)
        {
            showTime(oled);

            constexpr auto oneSecond(std::chrono::seconds(1));
            std::this_thread::sleep_for(oneSecond);
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


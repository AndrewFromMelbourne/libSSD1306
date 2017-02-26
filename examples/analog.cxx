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

#include "OledBitmap.h"
#include "OledGraphics.h"
#include "OledI2C.h"

//-------------------------------------------------------------------------

namespace
{
volatile static std::sig_atomic_t run = 1;

static constexpr int HourHandLength{18};
static constexpr int MinuteHandLength{28};
static constexpr int SecondHandLength{30};
static constexpr int TickRadius{31};
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
drawHand(
    double angle,
    int length,
    SSD1306::OledPixel& pixels)
{
    auto radians = 2.0 * angle * M_PI;

    int xCenter{pixels.width() / 2};
    int yCenter{pixels.height() / 2};

    SSD1306::line(SSD1306::OledPoint(xCenter, yCenter),
                  SSD1306::OledPoint(xCenter + int(sin(radians) * length),
                                     yCenter - int(cos(radians) * length)),
                  SSD1306::PixelStyle::Set,
                  pixels);
}

//-------------------------------------------------------------------------

void
showTime(
    SSD1306::OledPixel& pixels)
{
    //---------------------------------------------------------------------

    pixels.clear();

    //---------------------------------------------------------------------

    time_t now;
    time(&now);
    struct tm* tm = localtime(&now);

    //---------------------------------------------------------------------

    auto hour = (tm->tm_hour % 12) / 12.0;
    auto minute = tm->tm_min / 60.0;
    auto second = tm->tm_sec / 60.0;

    drawHand(hour + (minute / 12.0), HourHandLength, pixels);
    drawHand(minute + (second / 60.0), MinuteHandLength, pixels);
    drawHand(second, SecondHandLength, pixels);

    //---------------------------------------------------------------------

    for (auto tick = 0 ; tick < 12 ; ++tick)
    {
        auto radians = tick * M_PI / 6.0;
        pixels.setPixel(
            SSD1306::OledPoint(int(sin(radians) * TickRadius) +
                               (pixels.width() / 2),
                               int(cos(radians) * TickRadius) +
                               (pixels.height() / 2)));
    }

    //---------------------------------------------------------------------
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

        SSD1306::OledBitmap<64, 64> bitmap;
        SSD1306::OledPoint offset{32, 0};
        SSD1306::OledI2C oled{"/dev/i2c-1", 0x3C};

        while (run)
        {
            showTime(bitmap);
            oled.setFrom(bitmap, offset);
            oled.displayUpdate();

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


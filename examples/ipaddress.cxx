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

#include <ifaddrs.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/time.h>

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
showAddresses(
    SSD1306::OledI2C& oled)
{
    oled.clear();

    struct ifaddrs *ifAddrStruct = NULL;
    getifaddrs(&ifAddrStruct);

    int row = 0;

    for (struct ifaddrs* ifa = ifAddrStruct ;
         ifa != NULL ;
         ifa = ifa->ifa_next)
    {
        struct sockaddr* ifa_addr = ifa->ifa_addr;

        if (ifa_addr->sa_family == AF_INET)
        {
            // is a valid IP4 Address

            void *tmpAddrPtr = &((struct sockaddr_in *)ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            std::string address;

            if (strncmp(ifa->ifa_name, "eth", 3) == 0)
            {
                address = "E";
            }
            else if (strncmp(ifa->ifa_name, "wlan", 4) == 0)
            {
                address = "W";
            }
            else if (strncmp(ifa->ifa_name, "usb", 3) == 0)
            {
                address = "U";
            }

            if (not address.empty())
            {
                address += addressBuffer;
                drawString8x16(SSD1306::OledPoint{0, row * 16},
                               address,
                               SSD1306::PixelStyle::Set,
                               oled);
            }
        }
    }

    oled.displayUpdate();

    if (ifAddrStruct != NULL)
    {
        freeifaddrs(ifAddrStruct);
    }
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
            showAddresses(oled);
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


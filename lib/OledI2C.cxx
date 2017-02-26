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

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <algorithm>
#include <system_error>

#include "OledI2C.h"

//------------------------------------------------------------------------

namespace
{
    constexpr uint8_t OLED_COMMAND{0x00};
    constexpr uint8_t OLED_DATA{0x40};

    // address modes

    constexpr uint8_t OLED_HORIZONTAL_ADDRESSING_MODE{0x00};
    constexpr uint8_t OLED_VERTICAL_ADDRESSING_MODE{0x01};
    constexpr uint8_t OLED_PAGE_ADDRESSING_MODE{0x02};

    // commands

    constexpr uint8_t OLED_SET_COLUMN_START_LOW_MASK{0x00};
    constexpr uint8_t OLED_SET_COLUMN_START_HIGH_MASK{0x10};
    constexpr uint8_t OLED_SET_MEMORY_ADDRESSING_MODE{0x20};
    constexpr uint8_t OLED_SET_COLUMN_ADDRESS{0x21};
    constexpr uint8_t OLED_SET_PAGE_ADDRESS{0x22};
    constexpr uint8_t OLED_SET_DISPLAY_START_LINE_MASK{0x40};
    constexpr uint8_t OLED_SET_CONTRAST{0x81};
    constexpr uint8_t OLED_ENABLE_CHARGE_PUMP_REGULATOR{0x8D};
    constexpr uint8_t OLED_SET_SEGMENT_REMAP_0{0xA0};
    constexpr uint8_t OLED_SET_SEGMENT_REMAP_127{0xA1};
    constexpr uint8_t OLED_SET_ENTIRE_DISPLAY_ON_RESUME{0xA4};
    constexpr uint8_t OLED_SET_ENTIRE_DISPLAY_ON_FORCE{0xA5};
    constexpr uint8_t OLED_SET_NORMAL_DISPLAY{0xA6};
    constexpr uint8_t OLED_SET_INVERSE_DISPLAY{0xA7};
    constexpr uint8_t OLED_SET_MUX_RATIO{0xA8};
    constexpr uint8_t OLED_SET_DISPLAY_OFF{0xAE};
    constexpr uint8_t OLED_SET_DISPLAY_ON{0xAF};
    constexpr uint8_t OLED_SET_PAGE_START_ADDRESS_MASK{0xB0};
    constexpr uint8_t OLED_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL{0xC0};
    constexpr uint8_t OLED_SET_COM_OUTPUT_SCAN_DIRECTION_REMAP{0xC8};
    constexpr uint8_t OLED_SET_DISPLAY_OFFSET{0xD3};
    constexpr uint8_t OLED_SET_OSC_FREQUENCY{0xD5};
    constexpr uint8_t OLED_SET_PRECHARGE_PERIOD{0xD9};
    constexpr uint8_t OLED_SET_COM_PINS_HARDWARE_CONFIGURATION{0xDA};
    constexpr uint8_t OLED_SET_VCOMH_DESELECT_LEVEL{0xDB};

    //--------------------------------------------------------------------

    struct PixelOffset
    {
        PixelOffset(SSD1306::OledPoint p)
        :
            bit{p.y() % 8},
            block{(p.x() / SSD1306::OledI2C::ColumnsPerBlock)
                  + (SSD1306::OledI2C::ColumnsPerRow * (p.y() / 8))},
            byte{SSD1306::OledI2C::DataOffset
                 + (p.x() % SSD1306::OledI2C::ColumnsPerBlock)}
        {
        }

        int bit;
        int block;
        int byte;
    };
}

//------------------------------------------------------------------------

SSD1306::OledI2C::OledI2C(
    const std::string& device,
    uint8_t address)
:
    fd_{-1},
    blocks_{}
{
    fd_ = FileDescriptor{::open(device.c_str(), O_RDWR)};

    if (fd_.fd() == -1)
    {
        std::string what( "open "
                        + device
                        + " " __FILE__ "("
                        + std::to_string(__LINE__)
                        + ")" );
        throw std::system_error(errno, std::system_category(), what);
    }

    if (ioctl(fd_.fd(), I2C_SLAVE, address) == -1)
    {
        std::string what( "ioctl I2C_SLAVE " __FILE__ "("
                        + std::to_string(__LINE__)
                        + ")" );
        throw std::system_error(errno, std::system_category(), what);
    }

    init();
}

//------------------------------------------------------------------------

SSD1306::OledI2C::~OledI2C() = default;

//------------------------------------------------------------------------

SSD1306::OledI2C::PixelBlock::PixelBlock()
:
    bytes_{OLED_DATA, 0x00}, 
    dirty_{true}
{
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::clear()
{
    fillWith(0x00);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::fill()
{
    fillWith(0xFF);
}

//------------------------------------------------------------------------

bool
SSD1306::OledI2C::isSetPixel(
    SSD1306::OledPoint p) const
{
    if (not pixelInside(p))
    {
        return false;
    }

    PixelOffset po{p};

    return blocks_[po.block].bytes_[po.byte] & (1 << po.bit);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::setPixel(
    SSD1306::OledPoint p)
{
    if (not pixelInside(p))
    {
        return;
    }

    PixelOffset po{p};

    if ((blocks_[po.block].bytes_[po.byte] & (1 << po.bit)) == 0)
    {
        blocks_[po.block].bytes_[po.byte] |= (1 << po.bit);

        if (not blocks_[po.block].dirty_)
        {
            blocks_[po.block].dirty_ = true;
        }
    }
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::unsetPixel(
    SSD1306::OledPoint p)
{
    if (not pixelInside(p))
    {
        return;
    }

    PixelOffset po{p};

    if ((blocks_[po.block].bytes_[po.byte] & (1 << po.bit)) != 0)
    {
        blocks_[po.block].bytes_[po.byte] &= ~(1 << po.bit);

        if (not blocks_[po.block].dirty_)
        {
            blocks_[po.block].dirty_ = true;
        }
    }
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::xorPixel(
    SSD1306::OledPoint p)
{
    if (not pixelInside(p))
    {
        return;
    }

    PixelOffset po{p};

    if ((blocks_[po.block].bytes_[po.byte] & (1 << po.bit)) == 0)
    {
        blocks_[po.block].bytes_[po.byte] |= (1 << po.bit);
    }
    else
    {
        blocks_[po.block].bytes_[po.byte] &= ~(1 << po.bit);
    }

    if (not blocks_[po.block].dirty_)
    {
        blocks_[po.block].dirty_ = true;
    }
}

//------------------------------------------------------------------------

SSD1306::OledBitmap<SSD1306::OledI2C::Width, SSD1306::OledI2C::Height>
SSD1306::OledI2C::getBitmap() const
{
    OledBitmap<Width, Height> bitmap;

    for (auto y = 0 ; y < Height ; ++y)
    {
        for (auto x = 0 ; x < Width ; ++x)
        {
            OledPoint p{x, y};

            if (isSetPixel(p))
            {
                bitmap.setPixel(p);
            }
        }
    }

    return bitmap;
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::displayInverse() const
{
    sendCommand(OLED_SET_INVERSE_DISPLAY);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::displayNormal() const
{
    sendCommand(OLED_SET_NORMAL_DISPLAY);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::displayOff() const
{
    sendCommand(OLED_SET_DISPLAY_OFF);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::displayOn() const
{
    sendCommand(OLED_SET_DISPLAY_ON);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::displaySetContrast(
    uint8_t contrast) const
{
    sendCommand(OLED_SET_CONTRAST);
    sendCommand(contrast);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::displayUpdate()
{
    uint8_t page{0};
    uint8_t column{0};

    for (auto& block : blocks_)
    {
        if (block.dirty_)
        {
            uint8_t column_low = column & 0xF;
            uint8_t column_high = (column >> 4) & 0x0F;

            sendCommand(OLED_SET_PAGE_START_ADDRESS_MASK | page);
            sendCommand(OLED_SET_COLUMN_START_LOW_MASK | column_low);
            sendCommand(OLED_SET_COLUMN_START_HIGH_MASK | column_high);

            if (::write(fd_.fd(),
                        block.bytes_.data(),
                        block.bytes_.size()) == -1)
            {
                std::string what( "write " __FILE__ "("
                                + std::to_string(__LINE__)
                                + ")" );
                throw std::system_error(errno,
                                        std::system_category(),
                                        what);
            }

            block.dirty_ = false;
        }

        column += OledI2C::ColumnsPerBlock;

        if (column >= Width)
        {
            column = 0;
            page += 1;
        }
    }
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::fillWith(
    uint8_t value)
{
    for (auto& block : blocks_)
    {
        auto& bytes = block.bytes_;

        for (auto byte = bytes.begin() + SSD1306::OledI2C::DataOffset ; 
             byte != bytes.end() ;
             ++byte)
        {
            if (*byte != value)
            {
                *byte = value;

                if (not block.dirty_)
                {
                    block.dirty_ = true;
                }
            }
        }
    }
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::init() const
{
    // Enable charge pump regulator - 8Dh, 14h

    sendCommand(OLED_ENABLE_CHARGE_PUMP_REGULATOR, 0x14);

    // Set Memory Addressing Mode - 20h, 00h

    sendCommand(OLED_SET_MEMORY_ADDRESSING_MODE, OLED_PAGE_ADDRESSING_MODE);

    // Set Osc Frequency D5h, 80h

    sendCommand(OLED_SET_OSC_FREQUENCY, 0x80);

    // Set Display Offset - D3h, 00h

    sendCommand(OLED_SET_DISPLAY_OFFSET, 0x00);

    // Set Display Start Line - 40h

    sendCommand(OLED_SET_DISPLAY_START_LINE_MASK | 0x00);

    // Set Segment re-map - A0h/A1h

    sendCommand(OLED_SET_SEGMENT_REMAP_127);

    // Set COM Output Scan Direction - C0, C8h

    sendCommand(OLED_SET_COM_OUTPUT_SCAN_DIRECTION_REMAP);

    // Set COM Pins hardware configuration - DAh, 12h

    sendCommand(OLED_SET_COM_PINS_HARDWARE_CONFIGURATION, 0x12);

    // Set Pre-charge Period D9h, F1h

    sendCommand(OLED_SET_PRECHARGE_PERIOD, 0xF1);

    // Set Vcomh Deselect Level - DBh, 40h

    sendCommand(OLED_SET_VCOMH_DESELECT_LEVEL, 0x40);

    // Disable Entire Display On - A4h

    sendCommand(OLED_SET_ENTIRE_DISPLAY_ON_RESUME);

    // Set Normal Display - A6h

    sendCommand(OLED_SET_NORMAL_DISPLAY);

    // Set Column Address - 21h, 00h, 7Fh

    sendCommand(OLED_SET_COLUMN_ADDRESS, 0x00, 0x7F);

    // Set Page Address - 22h, 00h, 07h

    sendCommand(OLED_SET_PAGE_ADDRESS, 0x00, 0x07);

    // Set Contrast Control - 81h, 7Fh

    sendCommand(OLED_SET_CONTRAST, 0x7F);

    // Display On - AFh

    sendCommand(OLED_SET_DISPLAY_ON);

    usleep(100000);
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::sendCommand(
    uint8_t command) const
{
    std::array<uint8_t, 2> data{OLED_COMMAND, command};

    if (::write(fd_.fd(), data.data(), data.size()) == -1)
    {
        std::string what( "write " __FILE__ "("
                        + std::to_string(__LINE__)
                        + ")" );
        throw std::system_error(errno, std::system_category(), what);
    }
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::sendCommand(
    uint8_t command,
    uint8_t value) const
{
    std::array<uint8_t, 3> data{OLED_COMMAND, command, value};

    if (::write(fd_.fd(), data.data(), data.size()) == -1)
    {
        std::string what( "write " __FILE__ "("
                        + std::to_string(__LINE__)
                        + ")" );
        throw std::system_error(errno, std::system_category(), what);
    }
}

//------------------------------------------------------------------------

void
SSD1306::OledI2C::sendCommand(
    uint8_t command,
    uint8_t v1,
    uint8_t v2) const
{
    std::array<uint8_t, 4> data{OLED_COMMAND, command, v1, v2};

    if (::write(fd_.fd(), data.data(), data.size()) == -1)
    {
        std::string what( "write " __FILE__ "("
                        + std::to_string(__LINE__)
                        + ")" );
        throw std::system_error(errno, std::system_category(), what);
    }
}


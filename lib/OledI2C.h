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

#ifndef OLED_I2C_H
#define OLED_I2C_H

//------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <string>

#include "FileDescriptor.h"
#include "OledBitmap.h"
#include "OledHardware.h"
#include "OledPixel.h"
#include "point.h"

//------------------------------------------------------------------------

namespace SSD1306
{

//------------------------------------------------------------------------

class OledI2C
:
    public OledHardware,
    public OledPixel
{
public:

    static constexpr int Width{128};
    static constexpr int Height{64};
    static constexpr int BytesPerBlock{32};
    static constexpr int BufferSize{BytesPerBlock + 1};
    static constexpr int Blocks{(Width * Height) / (8 * BytesPerBlock)};
    static constexpr int ColumnsPerBlock{BytesPerBlock};
    static constexpr int ColumnsPerRow{Width / ColumnsPerBlock};
    static constexpr int DataOffset{1};

    OledI2C(
        const std::string& device,
        uint8_t address);

    virtual ~OledI2C();

    OledI2C(const OledI2C&) = delete;
    OledI2C& operator= (const OledI2C&) = delete;

    void clear() override;
    void fill() override;
    bool isSetPixel(SSD1306::OledPoint p) const override;
    void setPixel(SSD1306::OledPoint p) override;
    void unsetPixel(SSD1306::OledPoint p) override;
    void xorPixel(SSD1306::OledPoint p) override;

    int width() const override { return Width; }
    int height() const override { return Height; }

    OledBitmap<Width, Height> getBitmap() const;

    void displayInverse() const override;
    void displayNormal() const override;
    void displayOff() const override;
    void displayOn() const override;
    void displaySetContrast(uint8_t contrast) const override;
    void displayUpdate() override;

private:

    void fillWith(uint8_t value);
    void init() const;
    void sendCommand(uint8_t command) const;
    void sendCommand(uint8_t command, uint8_t value) const;
    void sendCommand(uint8_t command, uint8_t v1, uint8_t v2) const;

    FileDescriptor fd_;

    struct PixelBlock
    {
        PixelBlock();

        std::array<uint8_t, BufferSize> bytes_;
        bool dirty_;
    };

    std::array<PixelBlock, Blocks> blocks_;
};

//------------------------------------------------------------------------

} // namespace SSD1306

//------------------------------------------------------------------------

#endif

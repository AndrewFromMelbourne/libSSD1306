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

#ifndef OLED_BITMAP_H
#define OLED_BITMAP_H

//------------------------------------------------------------------------

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>

#include "OledPixel.h"
#include "point.h"

//------------------------------------------------------------------------

namespace SSD1306
{

//------------------------------------------------------------------------

using OledPoint = Point<int>;

//------------------------------------------------------------------------

template<int WIDTH, int HEIGHT>
class OledBitmap
:
    public OledPixel
{
public:
    static_assert(WIDTH > 0, "width must be greater than zero");
    static_assert(HEIGHT > 0, "height must be greater than zero");

    static constexpr int Width{WIDTH};
    static constexpr int Height{HEIGHT};
    static constexpr int BytesPerRow{(WIDTH + 7) / 8};

    OledBitmap()
    :
        blocks_{}
    {
    }

    OledBitmap(std::initializer_list<int> list)
    :
        blocks_{}
    {
        auto byte = 0;
        auto block = 0;

        for (auto item : list)
        {
            blocks_[block][byte++] = item;

            if ((byte * 8) >= Width)
            {
                byte = 0;
                block += 1;
            }
        }
    }

    void clear() override { fillWith(0x00); }
    void fill() override { fillWith(0xFF); }

    bool
    isSetPixel(
        SSD1306::OledPoint p) const override
    {
        if (not pixelInside(p))
        {
            return false;
        }

        PixelOffset offset{p};

        return blocks_[offset.block][offset.byte] & (1 << offset.bit);
    }

    void
    setPixel(
        SSD1306::OledPoint p) override
    {
        if (not pixelInside(p))
        {
            return;
        }

        PixelOffset offset{p};

        blocks_[offset.block][offset.byte] |= (1 << offset.bit);
    }

    void
    unsetPixel(
        SSD1306::OledPoint p) override
    {
        if (not pixelInside(p))
        {
            return;
        }

        PixelOffset offset{p};

        blocks_[offset.block][offset.byte] &= ~(1 << offset.bit);
    }

    void
    xorPixel(
        SSD1306::OledPoint p) override
    {

        if (not pixelInside(p))
        {
            return;
        }

        PixelOffset offset{p};

        if (blocks_[offset.block][offset.byte] & (1 << offset.bit))
        {
            blocks_[offset.block][offset.byte] &= ~(1 << offset.bit);
        }
        else
        {
            blocks_[offset.block][offset.byte] |= (1 << offset.bit);
        }
    }

    int width() const override { return Width; }
    int height() const override { return Height; }

private:

    struct PixelOffset
    {
        PixelOffset(SSD1306::OledPoint p)
        :
            bit{7 - (p.x() % 8)},
            block{p.y()},
            byte{p.x() / 8}
        {
        }

        int bit;
        int block;
        int byte;
    };

    void fillWith(uint8_t value)
    {
        for (auto& block : blocks_)
        {
            std::fill(block.begin(), block.end(), value);
        }
    }

    std::array<std::array<uint8_t, BytesPerRow>, Height> blocks_;
};

//------------------------------------------------------------------------

} // namespace SSD1306

//------------------------------------------------------------------------

#endif

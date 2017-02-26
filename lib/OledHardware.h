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

#ifndef OLED_HARDWARE_H
#define OLED_HARDWARE_H

//------------------------------------------------------------------------

#include <cstdint>

//------------------------------------------------------------------------

namespace SSD1306
{

//------------------------------------------------------------------------

class OledHardware
{
public:

    virtual ~OledHardware() = 0;

    virtual void displayInverse() const = 0;
    virtual void displayNormal() const = 0;
    virtual void displayOff() const = 0;
    virtual void displayOn() const = 0;
    virtual void displaySetContrast(uint8_t contrast) const = 0;
    virtual void displayUpdate() = 0;
};

//------------------------------------------------------------------------

} // namespace SSD1306

//------------------------------------------------------------------------

#endif

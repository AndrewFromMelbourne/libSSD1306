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

#ifndef LINUX_KEYS_H
#define LINUX_KEYS_H

//-------------------------------------------------------------------------

#include <termio.h>

//-------------------------------------------------------------------------

class LinuxKeys
{
public:

    LinuxKeys();
    ~LinuxKeys();

    LinuxKeys(const LinuxKeys&) = delete;
    LinuxKeys& operator= (const LinuxKeys&) = delete;

    LinuxKeys(LinuxKeys&&) = delete;
    LinuxKeys& operator= (LinuxKeys&&) = delete;

    struct PressedKey
    {
        PressedKey(bool isPressedArg = false, int keyArg = 0)
        :
            isPressed{isPressedArg},
            key{keyArg}
        {}

        bool isPressed;
        int key;
    };

    PressedKey pressed() const;

private:

    int stdinFd_ = -1;
    struct termios originalTermios_;
};

#endif


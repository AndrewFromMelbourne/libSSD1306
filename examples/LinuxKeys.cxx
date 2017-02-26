//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2013 Andrew Duncan
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

#include "LinuxKeys.h"

//-------------------------------------------------------------------------

LinuxKeys::LinuxKeys()
:
    stdinFd_{fileno(stdin)},
    originalTermios_{}
{
        tcgetattr(stdinFd_, &originalTermios_);

        struct termios term;
        memcpy(&term, &originalTermios_, sizeof(term));

        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(stdinFd_, TCSANOW, &term);

        setbuf(stdin, NULL);
}

//-------------------------------------------------------------------------

LinuxKeys::~LinuxKeys()
{
    tcsetattr(stdinFd_, TCSANOW, &originalTermios_);
}

//-------------------------------------------------------------------------

LinuxKeys::PressedKey
LinuxKeys::pressed() const
{
    int charactersBuffered = 0;
    ioctl(stdinFd_, FIONREAD, &charactersBuffered);

    PressedKey keys((charactersBuffered != 0));

    if (charactersBuffered == 1)
    {
        keys.key = fgetc(stdin);
        --charactersBuffered;
    }

    while (charactersBuffered)
    {
        fgetc(stdin);
        --charactersBuffered;
    }

    return keys;
}


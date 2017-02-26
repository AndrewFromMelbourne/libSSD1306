//-------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Copyright (c) 2016 Andrew Duncan
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

#include <unistd.h>

#include "FileDescriptor.h"

//-------------------------------------------------------------------------

SSD1306::FileDescriptor::FileDescriptor(
    int fd,
    CloseIfFunction close_if)
:
    fd_{fd},
    close_if_{close_if}
{
}

//-------------------------------------------------------------------------

SSD1306::FileDescriptor::~FileDescriptor()
{
    if (close_if_(fd_))
    {
        ::close(fd_);
    }
}

//-------------------------------------------------------------------------

SSD1306::FileDescriptor::FileDescriptor(
    SSD1306::FileDescriptor&& rhs)
:
    fd_{rhs.fd_},
    close_if_{std::move(rhs.close_if_)}
{
    rhs.fd_ = -1;
    rhs.close_if_ = [](int) { return false; };
}

//-------------------------------------------------------------------------

SSD1306::FileDescriptor&
SSD1306::FileDescriptor::operator= (
    SSD1306::FileDescriptor&& rhs)
{
    fd_ = rhs.fd_;
    close_if_ = std::move(rhs.close_if_);

    rhs.fd_ = -1;
    rhs.close_if_ = [](int) { return false; };

    return *this;
}


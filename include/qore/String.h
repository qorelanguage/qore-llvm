//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 Qore Technologies
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_STRING_H_
#define INCLUDE_QORE_STRING_H_

#include <iostream>
#include <string>
#include "qore/Any.h"

namespace qore {

class String : public Any {

public:
    using Ptr = auto_ptr<String>;

public:
    explicit String(std::string str) : str(std::move(str)) {
        LOG(this << " created");
    }

    const Type &getType() const override {
        return Type::String;
    }

    qint toInt() const {
        std::stringstream s(str);
        qint i;
        s >> i;
        return i;
    }

    String *append(String *right) {
        return new String(str + right->str);
    }

protected:
    ~String() {
        LOG(this << " destroyed");
    }

protected:
    WRITE_TO_LOG("String \"" << str << "\"")

private:
    std::string str;
};

} // namespace qore

#endif // INCLUDE_QORE_STRING_H_

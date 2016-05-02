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
/// \brief Defines the String class which implements Qore's `string` type.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_STRING_H_
#define INCLUDE_QORE_CORE_STRING_H_

#include <string>
#include "qore/core/Any.h"
#include "qore/core/Value.h"

namespace qore {

/**
 * \brief Implementation of Qore's `string` type.
 */
class String : public Any {

public:
    using Ptr = auto_ptr<String>;           //!< Pointer type.

public:
    /**
     * \brief Creates a new instance.
     * \param str the value of the string
     */
    explicit String(std::string str) : str(std::move(str)) {
        LOG(this << " created");
    }

    const Type &getType() const override {
        return Type::String;
    }

    /**
     * \brief Converts the string to an integer.
     * \return integer value represented by this string
     * \todo define the conversion - should it throw or behave like current qore does?
     */
    qint toInt() const;

    /**
     * \brief Appends another string to the string represented by this instance.
     *
     * Does not change this instance, instead returns the concatenation.
     * \param right the string to append
     * \return the concatenation of this string with `right`
     */
    String *append(String *right) const {
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

#endif // INCLUDE_QORE_CORE_STRING_H_

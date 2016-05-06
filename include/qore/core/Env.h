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
/// \brief Runtime environment.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_ENV_H_
#define INCLUDE_QORE_CORE_ENV_H_

#include <string>
#include <vector>
#include "qore/core/Namespace.h"
#include "qore/core/String.h"

namespace qore {

/**
 * \brief Runtime environment.
 *
 * Each program/module is assigned an environment which provides memory management, access to current privileges and
 * exception handling.
 */
class Env {

public:
    /**
     * \brief Default constructor.
     */
    Env() : rootNamespace("", SourceLocation()) {
    }

    /**
     * \brief Returns the root namespace.
     * \return the root namespace
     */
    Namespace &getRootNamespace() {
        return rootNamespace;
    }

    /**
     * \brief Creates a new source info.
     * \param name the name of the source
     * \return newly created source info
     */
    SourceInfo &createSourceInfo(std::string name) {
        std::unique_ptr<SourceInfo> ptr = util::make_unique<SourceInfo>(std::move(name));
        SourceInfo &info = *ptr;
        sourceInfos.push_back(std::move(ptr));
        return info;
    }

    /**
     * \brief Creates a new string literal.
     *
     * The reference count of the string will not drop to zero before this instance of Env gets destroyed.
     * \param value the value of the string
     * \return the new string
     */
    String &addString(std::string value) {
        String::Ptr ptr = String::Ptr(new String(std::move(value)));
        String &str = *ptr;
        strings.push_back(std::move(ptr));
        return str;
    }

private:
    Env(const Env &) = delete;
    Env(Env &&) = delete;
    Env &operator=(const Env &) = delete;
    Env &operator=(Env &&) = delete;

private:
    std::vector<std::unique_ptr<SourceInfo>> sourceInfos;
    std::vector<String::Ptr> strings;
    Namespace rootNamespace;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_ENV_H_

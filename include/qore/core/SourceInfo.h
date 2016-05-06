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
/// \brief Defines a class that describes a source file.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_SOURCEINFO_H_
#define INCLUDE_QORE_CORE_SOURCEINFO_H_

#include <memory>
#include <string>

namespace qore {

/**
 * \brief Describes a source file.
 */
class SourceInfo {

public:
    using Ptr = std::unique_ptr<SourceInfo>;        //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param name the name of the source
     */
    explicit SourceInfo(std::string name) : name(std::move(name)) {
    }

    /**
     * \brief Returns the name of the source.
     * \return the name of the source
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Returns the full filename of the source file.
     * \return the full filename of the source file
     */
    const std::string &getFullName() const {
        return fullName;
    }

    /**
     * Sets the full filename of the source file.
     * \param fullName the full filename of the source file
     */
    void setFullName(std::string fullName) {
        this->fullName = fullName;
    }

    /**
     * \brief Compares this instance to another.
     * \param other the other instance
     * \return true if both instances represent the same source
     */
    bool operator==(const SourceInfo &other) const {
        return this == &other;
    }

    /**
     * \brief Compares this instance to another.
     * \param other the other instance
     * \return true if `this` and `other` represent different sources
     */
    bool operator!=(const SourceInfo &other) const {
        return !(*this == other);
    }

    /**
     * \brief A source info instance used for invalid (uninitialized) locations.
     */
    static const SourceInfo Invalid;

private:
    SourceInfo(const SourceInfo &) = delete;
    SourceInfo(SourceInfo &&) = delete;
    SourceInfo &operator=(const SourceInfo &) = delete;
    SourceInfo &operator=(SourceInfo &&) = delete;

private:
    std::string name;
    std::string fullName;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_SOURCEINFO_H_

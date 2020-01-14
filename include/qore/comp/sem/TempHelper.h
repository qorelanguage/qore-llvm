//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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
/// \brief Code builder definition
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_TEMPHELPER_H_
#define INCLUDE_QORE_COMP_SEM_TEMPHELPER_H_

#include <map>
#include <string>
#include <vector>
#include "qore/core/Function.h"
#include "qore/core/code/Block.h"
#include "qore/core/util/Util.h"
#include "qore/comp/sem/GlobalVariableInfo.h"
#include "qore/comp/sem/LocalVariableInfo.h"
#include "qore/comp/sem/TempHelper.h"

namespace qore {
namespace comp {
namespace sem {

class Builder;

/**
 * \brief Helper class for handling temporary values.
 *
 * Basic usage is:
 * \code
 *     {
 *         TempHelper t1(builder);                              //allocates a temporary for t1
 *         //generate code that stores a value to t1
 *         t1.derefNeeded(type_of_t1_value.isRefCounted());     //makes sure that t1 is dereferenced
 *         //generate code that uses the value of t1
 *         //optionally call t1.derefDone() to take over the responsibility of dereferencing t1
 *     }                                                        //destructor dereferences and deallocates t1
 * \endcode
 *
 * The temporary can also be optionally provided externally (in the constructor) in which case the destructor
 * does not deallocate it.
 */
class TempHelper {

public:
    /**
     * \brief Creates an instance that allocates and deallocated the temporary automatically.
     * \param builder the code builder
     */
    explicit TempHelper(Builder &builder);

    /**
     * \brief Creates an instance that wraps an externally provided temporary.
     * \param builder the code builder
     * \param temp the temporary
     */
    TempHelper(Builder &builder, code::Temp temp) : builder(&builder), temp(temp), external(true), needsDeref(false) {
    }

    /**
     * \brief Destructor.
     *
     * Dereferences the value if \ref derefNeeded() has been called with `true` but no \ref derefDone() has been called.
     * Deallocates the temporary unless it has been provided externally in the constructor.
     */
    ~TempHelper();

    /**
     * \brief Move constructor.
     * \param src the source instance
     */
    TempHelper(TempHelper &&src) : builder(src.builder), temp(src.temp), external(src.external),
            needsDeref(src.needsDeref) {
        src.builder = nullptr;
    }

    /**
     * \brief Move assignment.
     * \param src the source instance
     * \return *this
     */
    TempHelper &operator=(TempHelper &&src) {
        assert(!builder);
        builder = src.builder;
        temp = src.temp;
        external = src.external;
        needsDeref = src.needsDeref;
        src.builder = nullptr;
        return *this;
    }

    /**
     * \brief Implicit conversion to \ref code::Temp.
     */
    operator code::Temp() const {
        return temp;
    }

    /**
     * \brief Adds the temporary value to the list of values that need their reference count decreased.
     * \param isRef true if the type of the value is reference-counted
     */
    void derefNeeded(bool isRef);

    /**
     * \brief Relinquishes the responsibility of dereferencing the temporary.
     */
    void derefDone();

    /**
     * \brief Returns true if the temporary has been provided externally in the constructor.
     * \return true if the temporary has been provided externally in the constructor
     */
    bool isExternallyProvided() const {
        return external;
    }

private:
    TempHelper(const TempHelper &) = delete;
    TempHelper &operator=(const TempHelper &) = delete;

private:
    Builder *builder;
    code::Temp temp;
    bool external;
    bool needsDeref;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_TEMPHELPER_H_

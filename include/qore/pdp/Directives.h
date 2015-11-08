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
/// \brief Definition of parse directives.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_PDP_DIRECTIVES_H_
#define INCLUDE_QORE_PDP_DIRECTIVES_H_

#include <unordered_map>
#include "qore/common/Util.h"

namespace qore {
namespace pdp {

/**
 * \brief Enumeration of all parse directives.
 *
 * \ref DirectivesData.inc
 */
enum class DirectiveId {
    #define PD(I, N, A)         I,
    /// \cond IGNORED_BY_DOXYGEN
    #include "qore/pdp/DirectivesData.inc"
    /// \endcond
    #undef PD
};

/**
 * \brief Defines the type of directives' arguments.
 */
enum class DirectiveArgs {
    None,           //!< The directive has no arguments.
    Single          //!< The directive has a single argument consisting of everything until a newline.
};

/**
 * \brief Information about a directive.
 */
struct DirectiveInfo {
    DirectiveId id;         //!< The identifier of the directive.
    DirectiveArgs args;     //!< The type of the directive's arguments.
};

/**
 * \brief Provides information about directives.
 */
class Directives {

public:
    /**
     * \brief Constructs the object by filling an internal map of directives.
     */
    Directives() {
        #define PD(I, N, A)         { DirectiveInfo &i = data[N]; i.id = DirectiveId::I; i.args = DirectiveArgs::A; }
        /// \cond IGNORED_BY_DOXYGEN
        #include "qore/pdp/DirectivesData.inc"
        /// \endcond
        #undef PD
    }

    /**
     * \brief Finds information for a parse directive.
     * \param name the name of the directive as it appears in the source after the '%' sign
     * \return information about the parse directive
     * \throws std::out_of_range if the no directive with given name exists
     */
    const DirectiveInfo &operator[](const std::string &name) const {
        return data.at(name);
    }

private:
    std::unordered_map<std::string, DirectiveInfo> data;
};

/**
 * \brief Writes the directive id to an output stream.
 * \param o the output stream
 * \param id the directive id to write
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &o, DirectiveId id) {
    switch (id) {
        #define PD(I, N, A)         case DirectiveId::I: return o << #I;
        /// \cond IGNORED_BY_DOXYGEN
        #include "qore/pdp/DirectivesData.inc"
        /// \endcond
        #undef PD
    }
    QORE_UNREACHABLE("Invalid value of DirectiveId: " << static_cast<int>(id));
}

} // namespace pdp
} // namespace qore

#endif // INCLUDE_QORE_PDP_DIRECTIVES_H_

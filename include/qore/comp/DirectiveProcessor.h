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
/// \brief Processor of parse directives.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_DIRECTIVEPROCESSOR_H_
#define INCLUDE_QORE_COMP_DIRECTIVEPROCESSOR_H_

#include <functional>
#include <stack>
#include <string>
#include "qore/comp/Context.h"
#include "qore/comp/Scanner.h"

namespace qore {
namespace comp {

/**
 * \brief Enumeration of all parse directives.
 */
enum class DirectiveId {
    Include,        //!< The `%%include` directive.
};

/**
 * \brief Defines the type of arguments of a parse directive.
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
    DirectiveArgs args;     //!< The type of the arguments.
};

/**
 * \brief Parse directive processor.
 *
 * Manages a stack of source files, uses a Scanner to recognize tokens in the input and processes parse directives
 * such as `%%include` and `%%ifdef`.
 */
class DirectiveProcessor : public ITokenStream {

public:
    /**
     * \brief Constructs the object.
     * \param ctx the compiler context
     * \param src the initial source file
     */
    DirectiveProcessor(Context &ctx, Source &src);

    Token read(Mode mode) override;

private:
    void processDirective(Source &src, SourceLocation location, std::string directive);

private:
    Context &ctx;
    Scanner scanner;
    std::stack<std::reference_wrapper<Source>> srcStack;

    static const std::unordered_map<std::string, DirectiveInfo> Directives;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_DIRECTIVEPROCESSOR_H_

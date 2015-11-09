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
/// \brief Directive parser.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_PDP_DIRECTIVEPARSER_H_
#define INCLUDE_QORE_PDP_DIRECTIVEPARSER_H_

#include <algorithm>
#include "qore/context/DiagManager.h"
#include "qore/context/Source.h"
#include "qore/pdp/CommentStripper.h"
#include "qore/pdp/Directives.h"

namespace qore {
namespace pdp {

/**
 * \brief Filters out parse directives from a source stream and passes the directive with its arguments to a callback.
 *
 * The newline characters at the end of the parse directives are preserved, but their column number is reported as 1.
 * \tparam Handler the type of the callback - must define a method
 *         `void handleDirective(SourceLocation loc, DirectiveId id, SourceLocation argLoc, const std::string &arg)`
 */
template<typename Handler>
class DirectiveParser {

public:
    /**
     * \brief Constructor.
     * \param diagMgr for reporting errors
     * \param handler the handler to call for each parse directive
     * \param src the source stream
     */
    DirectiveParser(DiagManager &diagMgr, Handler &handler, Source &src) : diagMgr(diagMgr),
        handler(handler), src(diagMgr, src) {
    }

    /**
     * \brief Returns the location of the next character returned by read().
     *
     * The column number of the newline character at the end of a parse directive is reported as 1.
     * \return current location in the source
     */
    SourceLocation getLocation() const {
        return src.getLocation();
    }

    /**
     * \brief Reads the next character from the source, processing any parse directives.
     * \return the next character read
     */
    int read() {
        int c = src.read();
        if (c == '%' && startOfLine) {
            parseDirective();
            c = src.read();
        }
        startOfLine = c == '\n';
        return c;
    }

private:
    void parseDirective() {
        static const Directives directives;
        SourceLocation loc = getLocation();
        std::string directive;
        std::string arg;
        while (!isspace(src.peek()) && src.peek() != Source::EndOfFile) {
            directive.push_back(src.read());
        }
        while (isspace(src.peek()) && src.peek() != '\n') {
            src.read();
        }
        SourceLocation argLoc = getLocation();
        while (src.peek() != Source::EndOfFile && src.peek() != '\n') {
            arg.push_back(src.read());
        }
        arg.erase(std::find_if_not(arg.rbegin(), arg.rend(), [](int c){return std::isspace(c);}).base(), arg.end());

        try {
            auto directiveInfo = directives[directive];
            if (directiveInfo.args == DirectiveArgs::None && !arg.empty()) {
                diagMgr.report(DiagId::PdpUnexpectedArgument, loc) << directive;
            } else if (directiveInfo.args == DirectiveArgs::Single && arg.empty()) {
                diagMgr.report(DiagId::PdpMissingArgument, loc) << directive;
            } else {
                handler.handleDirective(loc, directiveInfo.id, argLoc, arg);
            }
        } catch (std::out_of_range &) {
            diagMgr.report(DiagId::PdpUnknownDirective, loc) << directive;
        }
    }

private:
    DiagManager &diagMgr;
    Handler &handler;
    Lookahead<CommentStripper> src;
    bool startOfLine{true};
};

} // namespace pdp
} // namespace qore

#endif // INCLUDE_QORE_PDP_DIRECTIVEPARSER_H_

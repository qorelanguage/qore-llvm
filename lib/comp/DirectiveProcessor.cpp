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
#include "qore/comp/DirectiveProcessor.h"
#include <string>
#include "qore/core/util/Debug.h"
#include "qore/core/util/Util.h"

namespace qore {
namespace comp {

const std::unordered_map<std::string, DirectiveInfo> DirectiveProcessor::Directives{
    {"%include",            {DirectiveId::Include, DirectiveArgs::Single}},
};

DirectiveProcessor::DirectiveProcessor(Context &ctx, Source &src) : ctx(ctx), scanner(ctx) {
    LOG_FUNCTION();
    srcStack.push(src);
}

Token DirectiveProcessor::read(Mode mode) {
    LOG_FUNCTION();
    assert(!srcStack.empty());

    while (true) {
        Source &src = srcStack.top();
        Token t = scanner.read(src, mode);
        if (t.type == TokenType::ParseDirective) {
            processDirective(src, src.getMarkLocation(), src.getMarkedString());
        } else if (t.type == TokenType::EndOfFile && srcStack.size() > 1) {
            srcStack.pop();
        } else {
            return t;
        }
    }
}

void DirectiveProcessor::processDirective(Source &src, SourceLocation location, std::string directive) {
    LOG("Processing directive " << directive);

    std::string arg;
    while (src.peek() != '\0' && !Scanner::isNewline(src.peek())) {
        int c = src.read();
        switch (c) {
            case '#':
                scanner.readLineComment(src);
                break;
            case '/':
                if (src.peek() == '*') {
                    scanner.readBlockComment(src);
                    arg.push_back(' ');
                } else {
                    arg.push_back(c);
                }
                break;
            case '\'':
            case '"':
                src.unread();
                src.setMark();
                scanner.readString(src);
                arg.append(src.getMarkedString());
                break;
            default:
                arg.push_back(c);
                break;
        }
    }
    arg = util::trim<>(arg, Scanner::isWhitespace);

    auto it = Directives.find(directive);
    if (it == Directives.end()) {
        ctx.report(DiagId::PdpUnknownDirective, location) << directive;
        return;
    }
//    if (it->second.args == DirectiveArgs::None && !arg.empty()) {
//        diagMgr.report(DiagId::PdpUnexpectedArgument, location) << directive;
//        return;
//    }
    if (it->second.args == DirectiveArgs::Single && arg.empty()) {
        ctx.report(DiagId::PdpMissingArgument, location) << directive;
        return;
    }

    switch (it->second.id) {
        case DirectiveId::Include:
            if ((arg[0] == '"' && arg[arg.length() - 1] == '"') || (arg[0] == '\'' && arg[arg.length() - 1] == '\'')) {
                arg = arg.substr(1, arg.length() - 2);
            }
            srcStack.push(ctx.getSrcMgr().createFromFile(arg, location));
            break;
    }
}

} // namespace comp
} // namespace qore

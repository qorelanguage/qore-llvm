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
#include <iostream>
#include <string>
#include <utility>
#include "qore/common/Logging.h"
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/ast/Dump.h"
#include "qore/cg/CodeGen.h"
#include "Interactive.h"

/// \cond NoDoxygen

#ifdef QORE_LOGGING
/**
 * \brief Logging filter
 */
class LogFilter : public qore::log::Logger {

public:
    bool filter(const std::string &component) override {
        return component.find("Parser") == std::string::npos && component.find("Scanner") == std::string::npos
                && component.find("DirectiveProcessor") == std::string::npos;
    }
};
#endif

void test(bool file, std::string str) {
    qore::comp::StringTable stringTable;
    qore::comp::DiagManager diagMgr(stringTable);
    qore::comp::SourceManager srcMgr(diagMgr);
    qore::DiagPrinter diagPrinter(srcMgr);
    diagMgr.addProcessor(&diagPrinter);
    qore::comp::Context ctx(stringTable, diagMgr, srcMgr);
    qore::comp::Source &src = file ? srcMgr.createFromFile(str) : srcMgr.createFromString("<noname>", str);
    qore::comp::DirectiveProcessor dp(ctx, src);
    qore::comp::Parser parser(ctx, dp);

    qore::comp::ast::Script::Ptr script = parser.parseScript();
    qore::comp::ast::dump(ctx, std::cout, *script);
    LOG("-------------------------------------------------------------------------------");
    qore::ir::Script scr;
    qore::comp::sem::analyze(ctx, scr, *script);

    qore::as::S sss;
    qore::comp::sem::analyze2(sss, scr);

    LOG("-------------------------------------------------------------------------------");
    {
        qore::in::Interpreter i(sss);
        i.run(*sss.qinit);
        i.run(*sss.qmain);
        i.run(*sss.qdone);
    }
    LOG("-------------------------------------------------------------------------------");
    qore::cg::CodeGen::process(sss);
}

/// \endcond NoDoxygen

int main(int argc, char *argv[]) {
#ifdef QORE_LOGGING
    LogFilter logFilter;
    qore::log::LoggerManager::set(&logFilter);
#endif
    LOG_FUNCTION();

    std::string src = R"(
int i;
our string s;
s = "A";
i = i + 1;
s = s + i;
i += "2";
s += i;

any a = 1;
a = a + 2;
a += "8";

)";

//    std::istringstream stream(src);
//    std::streambuf* cin_backup = std::cin.rdbuf(stream.rdbuf());
//    qore::interactive();
//    std::cin.rdbuf(cin_backup);

//    test(true, argv[1]);
    test(false, src);

    return 0;
}

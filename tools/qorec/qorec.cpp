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
#include <ffi.h>
#include <iostream>
#include <string>
#include "qore/core/util/Debug.h"
#include "qore/core/Dump.h"
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/ast/Dump.h"
#include "qore/comp/sem/Analyzer.h"
#include "qore/in/Interpreter.h"
#include "qore/cg/CodeGen.h"
#include "DiagPrinter.h"
#include "Interactive.h"

/// \cond NoDoxygen

#ifdef QORE_LOGGING
/**
 * \brief Logging filter
 */
class LogFilter : public qore::util::Logger {

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
    qore::DiagPrinter diagPrinter;
    diagMgr.addProcessor(&diagPrinter);
    qore::Env env;
    qore::comp::Context ctx(env, stringTable, diagMgr, srcMgr);
    qore::SourceInfo &info = env.addSourceInfo(file ? str : "<noname>");
    qore::comp::Source &src = file ? srcMgr.createFromFile(info, str) : srcMgr.createFromString(info, str);
    qore::comp::DirectiveProcessor dp(ctx, src);
    qore::comp::Parser parser(ctx, dp);

    qore::comp::ast::Script::Ptr script = parser.parseScript();
    qore::comp::ast::dump(ctx, std::cout, *script);
    LOG("-------------------------------------------------------------------------------");
    qore::Function *qinit = qore::comp::sem::Analyzer::analyze(ctx, *script);
    qore::dump(std::cout, env);
    LOG("-------------------------------------------------------------------------------");
    if (qinit) {
        qore::in::interpret(*qinit);
    }
    LOG("-------------------------------------------------------------------------------");
    qore::cg::CodeGen::process(env);
}

/// \endcond NoDoxygen

void ex() {
    ffi_cif cif;
    ffi_type *arg_types[1];

    arg_types[0] = &ffi_type_pointer;
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_uint, arg_types) == FFI_OK) {
        const char *arg1 = "Hello, World!";

        void *arg_values[1];
        arg_values[0] = &arg1;

        ffi_arg rc;
        ffi_call(&cif, FFI_FN(puts), &rc, arg_values);
    }
}

int main(int argc, char *argv[]) {
#ifdef QORE_LOGGING
    LogFilter logFilter;
    qore::util::LoggerManager::set(&logFilter);
#endif
    LOG_FUNCTION();

    std::string src = R"(
any sub f(int i) {
    if (i += string x) {string s1 = "aaa"; return 21;}
    return 42;
}

any sub f2(string s1, any s2, string s3) {
    return s1 + s2 + s3;
}

f2("A", f(4), "C" + "D");
)";

//    std::istringstream stream(src);
//    std::streambuf* cin_backup = std::cin.rdbuf(stream.rdbuf());
//    qore::interactive();
//    std::cin.rdbuf(cin_backup);

//    test(true, argv[1]);
    test(false, src);


    ex();

    return 0;
}

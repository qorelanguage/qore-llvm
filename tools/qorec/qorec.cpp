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
#include "qore/comp/semantic/Analyzer.h"
#include "qore/comp/semantic/Dump.h"
#if QORE_USE_LLVM
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#endif

namespace qore {
namespace comp {

/**
 * \brief Basic DiagProcessor that writes messages to the standard error stream.
 */
class DiagPrinter : public IDiagProcessor {

public:
    /**
     * \brief Constructs the instance.
     * \param srcMgr used for decoding source locations
     */
    explicit DiagPrinter(const SourceManager &srcMgr) : srcMgr(srcMgr) {
    }

    void process(DiagRecord &record) override {
        Source &src = srcMgr.get(record.location.sourceId);
        std::pair<int, int> l = src.decodeLocation(record.location.offset);
        std::cerr << src.getName() << ':' << l.first << ':' << l.second
                << ": " << record.level << ": " << record.code << ": " << record.message << '\n';
    }

private:
    const SourceManager &srcMgr;
};

/// \cond NoDoxygen
class StdinWrapper : public ITokenStream {

public:
    StdinWrapper(DiagManager &diagMgr, SourceManager &srcMgr)
            : src(srcMgr.createFromString("<stdin>", "")), dp(diagMgr, srcMgr, src) {
    }

    Token read(Mode mode) override {
        while (true) {
            Token t = dp.read(mode);
            if (t.type != TokenType::EndOfFile) {
                return t;
            }
            std::string line;
            std::getline(std::cin, line);
            if (!std::cin.good()) {
                return t;
            }
            line.push_back('\n');
            src.append(line);
        }
    }

private:
    Source &src;
    DirectiveProcessor dp;
};

void test() {
    DiagManager diagMgr;
    SourceManager srcMgr(diagMgr);
    DiagPrinter diagPrinter(srcMgr);
    diagMgr.addProcessor(&diagPrinter);
//    DirectiveProcessor dp(diagMgr, srcMgr, srcMgr.createFromFile(argv[1]));
    DirectiveProcessor dp(diagMgr, srcMgr, srcMgr.createFromString("<noname>", "namespace a; namespace a::b;"));
//    StdinWrapper dp(diagMgr, srcMgr);

    Parser parser(diagMgr, dp);
    ast::Script::Ptr script = parser.parseScript();
    ast::dump(srcMgr, std::cout, *script);

    semantic::Analyzer analyzer(srcMgr, diagMgr);
    semantic::Namespace::Ptr root = analyzer.analyze(script);
    semantic::dump(srcMgr, std::cout, *root);
}

} // namespace comp
} // namespace qore
/// \endcond NoDoxygen

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

int main(int argc, char *argv[]) {
#ifdef QORE_LOGGING
    LogFilter logFilter;
    qore::log::LoggerManager::set(&logFilter);
#endif
    LOG_FUNCTION();

#if QORE_USE_LLVM
    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    module->dump();
#endif

    qore::comp::test();

    return 0;
}

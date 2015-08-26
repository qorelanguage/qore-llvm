#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "qore/common/common.h"
#include "qore/parser/parser.h"
#include "qore/runtime/runtime.h"
#include "dump.h"
#include "interpret.h"

#ifdef QORE_USE_LLVM
#include "gen.h"

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/AssemblyAnnotationWriter.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

#ifdef QORE_ENABLE_JIT
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Support/TargetSelect.h"
#endif

#endif

//TODO napespaces

int main(int argc, char *argv[]) {
    int opt;
    bool dumpAst = false;
    bool interpret = false;
    bool compileLl = false;
    bool compileBc = false;
    bool jit = false;
    std::string outName;

    while ((opt = getopt(argc, argv, "dilbjo:")) != -1) {
        switch (opt) {
            case 'd':
                dumpAst = true;
                break;
            case 'i':
                interpret = true;
                break;
            case 'l':
#ifndef QORE_USE_LLVM
                std::cerr << "LLVM support not built" << std::endl;
                return -1;
#else
                compileLl = true;
                break;
#endif
            case 'b':
#ifndef QORE_USE_LLVM
                std::cerr << "LLVM support not built" << std::endl;
                return -1;
#else
                compileBc = true;
                break;
#endif
            case 'j':
#ifndef QORE_ENABLE_JIT
                std::cerr << "JIT support not built" << std::endl;
                return -1;
#else
                jit = true;
                break;
#endif
            case 'o':
                outName = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-dilb] [-o output] file" << std::endl;
                std::cerr << "Options:" << std::endl;
                std::cerr << "  -d  dump AST" << std::endl;
                std::cerr << "  -i  interpret" << std::endl;
                std::cerr << "  -l  compile to LLVM IR" << std::endl;
                std::cerr << "  -b  compile to bitcode" << std::endl;
                std::cerr << "  -j  interpret with JIT" << std::endl;
                return -1;
        }
    }

    if (!dumpAst && !interpret && !compileLl && !compileBc && !jit) {
        interpret = true;
    }

    if (optind >= argc) {
        if (dumpAst || compileLl || compileBc || jit) {
            std::cerr << "Interactive mode supports -i only" << std::endl;
            return -1;
        }

        InterpretVisitor iv;
        while (true) {
            std::unique_ptr<Statement> stmt = parseStatement();
            if (stmt && !stmt->accept(iv)) {
                return 0;
            }
        }
    }

    std::string outBase = outName.length() == 0 ? argv[optind] : outName;
    auto i = outBase.find_last_of('.');
    if (i != std::string::npos) {
        outBase.erase(outBase.begin() + i, outBase.end());
    }

    std::ifstream t(argv[optind]);
    std::stringstream buffer;
    buffer << t.rdbuf();

    std::unique_ptr<Program> root = parse(buffer.str());

    if (dumpAst) {
        DumpVisitor dv;
        root->accept(dv);
    }

    if (interpret) {
        InterpretVisitor iv;
        root->accept(iv);
    }

#ifdef QORE_USE_LLVM
    if (compileLl || compileBc || jit) {
        CodeGenVisitor cgv;
        std::unique_ptr<llvm::Module> module(static_cast<llvm::Module*>(root->accept(cgv)));

        if (compileLl) {
            std::error_code ec;
            llvm::raw_fd_ostream os(outBase + ".ll", ec, llvm::sys::fs::F_None);
            std::unique_ptr<llvm::AssemblyAnnotationWriter> Annotator;
            if (ec) {
                std::cerr << "NOPE" << std::endl;
            } else {
                module->print(os, Annotator.get());
            }
        }

        if (compileBc) {
            std::error_code ec;
            llvm::raw_fd_ostream os(outBase + ".bc", ec, llvm::sys::fs::F_None);
            if (ec) {
                std::cerr << "NOPE" << std::endl;
            } else {
                llvm::WriteBitcodeToFile(module.get(), os);
            }
        }

#ifdef QORE_ENABLE_JIT
        if (jit) {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();

            std::string errStr;
            llvm::EngineBuilder eb(std::move(module));

            eb.setErrorStr(&errStr);
            eb.setMCJITMemoryManager(llvm::make_unique<llvm::SectionMemoryManager>());

            llvm::ExecutionEngine *executionEngine = eb.create();
            if (!executionEngine) {
                std::cout << "NOPE: " << errStr << std::endl;
            } else {
                uint64_t ptr = executionEngine->getFunctionAddress("q");
                void (*jitTest)(void) = (void (*)(void)) ptr;
                jitTest();
            }
        }
#endif
    }
#endif
    return 0;
}

#include <iostream>
#include "qore/common/common.h"
#include "parser.hpp"
#include "dump.h"
#include "gen.h"
#include "qore/runtime/runtime.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Support/TargetSelect.h"

void yyerror(const char *msg) {
    printf("Error: %s\n", msg);
}

void yy_scan_string(const char *yy_str);

std::unique_ptr<Program> ROOT;

int main() {
    //TODO force export of runtime symbols
    QoreValue qv = make_int(123);
    print_qv(qv);

    yy_scan_string(R"(

print 5;
print "Hello, world!";

)");
    yyparse();

    std::cout << "Program:" << std::endl;
    ROOT->forEachStatement([](const Statement *s){ std::cout << "  " << s << std::endl; });

    CodeGenVisitor cgv;
    llvm::Module *m = static_cast<llvm::Module*>(ROOT->accept(cgv));
    m->dump();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();


    std::string errStr;
    std::unique_ptr<llvm::Module> up(m);
    llvm::EngineBuilder eb(std::move(up));

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
//    delete m;

    return 0;
}

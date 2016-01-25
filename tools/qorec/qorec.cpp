#include <iostream>
#include "qore/common/Logging.h"
#include "qore/comp/DiagManager.h"
#if QORE_USE_LLVM
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#endif

class DiagPrinter : public qore::comp::IDiagProcessor {

public:
    void process(qore::comp::DiagRecord &record) override {
        std::cerr << ':' << record.level << ": " << record.code << ": " << record.message << '\n';
    }
};

int main() {
    LOG_FUNCTION();
    LOG("Hello, world!");

#if QORE_USE_LLVM
    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    module->dump();
#endif

    qore::comp::DiagManager diagMgr;
    DiagPrinter diagPrinter;
    diagMgr.addProcessor(&diagPrinter);
    diagMgr.report(qore::comp::DiagId::ScannerInvalidInteger, qore::comp::SourceLocation()) << "123" << "abc" << "x" << "y";

    return 0;
}

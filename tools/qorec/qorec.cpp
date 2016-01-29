#include <iostream>
#include "qore/common/Logging.h"
#include "qore/comp/DiagManager.h"
#include "qore/comp/DirectiveProcessor.h"
#if QORE_USE_LLVM
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#endif

using namespace qore::comp;

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


class StdinWrapper {

public:
    StdinWrapper(SourceManager &srcMgr, DirectiveProcessor &dp) : dp(dp), src(srcMgr.createFromString("<stdin>", "")) {
        dp.setSource(src);
    }

    Token read() {
        while (true) {
            Token t = dp.read();
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
    DirectiveProcessor &dp;
    Source &src;
};

int main() {
    LOG_FUNCTION();

#if QORE_USE_LLVM
    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    module->dump();
#endif

    DiagManager diagMgr;
    SourceManager srcMgr(diagMgr);
    DiagPrinter diagPrinter(srcMgr);
    diagMgr.addProcessor(&diagPrinter);
    DirectiveProcessor dp(diagMgr, srcMgr);

    dp.setSource(srcMgr.createFromString("<noname>", "ab\n%include xyz\n;"));     auto &x = dp;
    //StdinWrapper wrapper(srcMgr, dp); auto &x = wrapper;

    while (true) {
        Token t = x.read();
        std::cout << t.type << "\n";
        if (t.type == TokenType::EndOfFile) {
            break;
        }
    }

    return 0;
}

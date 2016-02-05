#include <iostream>
#include "qore/common/Logging.h"
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/ast/Dump.h"
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


class StdinWrapper : public ITokenStream {

public:
    StdinWrapper(DiagManager &diagMgr, SourceManager &srcMgr) : src(srcMgr.createFromString("<stdin>", "")), dp(diagMgr, srcMgr, src) {
    }

    Token read() override {
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
    Source &src;
    DirectiveProcessor dp;
};

#ifdef QORE_LOGGING
class LogFilter : public qore::log::Logger {

public:
    virtual bool filter(const std::string &component) {
        return component.find("Parser") != std::string::npos;
    }
};
#endif

int main() {
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

    DiagManager diagMgr;
    SourceManager srcMgr(diagMgr);
    DiagPrinter diagPrinter(srcMgr);
    diagMgr.addProcessor(&diagPrinter);
    DirectiveProcessor dp(diagMgr, srcMgr, srcMgr.createFromString("<noname>", ";0;"));
//    StdinWrapper dp(diagMgr, srcMgr);

    Parser parser(diagMgr, dp);
    ast::Script::Ptr script = parser.parseScript();
    ast::dump(srcMgr, std::cout, *script);

    return 0;
}

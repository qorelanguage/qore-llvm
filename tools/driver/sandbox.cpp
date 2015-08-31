#include "qore/common/Logging.h"
#include "qore/context/DiagManager.h"
#include "qore/context/SourceManager.h"
#include "qore/scanner/ScannerImpl.h"

#define QORE_LOG_COMPONENT "MAIN"

void sandbox() {
    qore::SourceManager sourceMgr;
    qore::DiagPrinter diagPrinter(sourceMgr);
    qore::DiagManager diagMgr;
    diagMgr.addProcessor(&diagPrinter);

    qore::SourceBuffer srcBuffer = sourceMgr.createFromString("test", "   567 @ 12312341324231423141234231421 ;");
//    qore::SourceBuffer srcBuffer = sourceManager.createFromStdin();
//    qore::SourceBuffer srcBuffer = sourceManager.createFromFile("tools/sandbox/test.q");
    qore::ScannerImpl scanner(diagMgr, std::move(srcBuffer));

    qore::Token t;
    while (t.type != qore::TokenType::EndOfFile) {
        scanner.read(&t);
        LOG("Read token: " << t);
    }
}

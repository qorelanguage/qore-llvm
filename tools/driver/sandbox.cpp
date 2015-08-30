#include "qore/common/Logging.h"
#include "qore/context/SourceManager.h"
#include "qore/scanner/ScannerImpl.h"

#define QORE_LOG_COMPONENT "MAIN"


void sandbox() {
    qore::DiagnosticManager diagnosticManager;
    qore::SourceManager sourceManager;
    qore::SourceBuffer srcBuffer = sourceManager.createFromString("test", "   567 @ 123 ;");
//    qore::SourceBuffer srcBuffer = sourceManager.createFromStdin();
//    qore::SourceBuffer srcBuffer = sourceManager.createFromFile("tools/sandbox/test.q");
    qore::ScannerImpl scanner(diagnosticManager, std::move(srcBuffer));

    qore::Token t;
    while (t.type != qore::TokenType::EndOfFile) {
        scanner.read(&t);
        LOG("Read token: " << t);
    }
}

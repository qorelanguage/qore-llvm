#include "qore/common/Logging.h"
#include "qore/context/DiagManager.h"
#include "qore/context/SourceManager.h"
#include "qore/scanner/ScannerImpl.h"
#include "qore/parser/ParserImpl.h"
#include "dump.h"

#define QORE_LOG_COMPONENT "MAIN"

void sandbox() {
    qore::SourceManager sourceMgr;
    qore::DiagPrinter diagPrinter(sourceMgr);
    qore::DiagManager diagMgr;
    diagMgr.addProcessor(&diagPrinter);

    qore::SourceBuffer srcBuffer = sourceMgr.createFromString("test", "  print 567 + 4 +5 print 56; wtf gr ; print 6");
    qore::ScannerImpl scanner(diagMgr, std::move(srcBuffer));

    qore::ParserImpl parser(diagMgr, scanner);
    std::unique_ptr<Program> program(parser.parse());

    DumpVisitor dv;
    program->accept(dv);
}

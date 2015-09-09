#include "qore/common/Logging.h"
#include "qore/context/DiagManager.h"
#include "qore/context/SourceManager.h"
#include "qore/scanner/ScannerImpl.h"
#include "qore/parser/ParserImpl.h"
#include "qore/ast/dump/DumpVisitor.h"
#include "qore/ast/dump/XmlFormat.h"
#include "qore/ast/dump/JsonFormat.h"
#include "qore/ast/dump/YamlFormat.h"
#include "qore/ast/dump/CompactFormat.h"

#define QORE_LOG_COMPONENT "MAIN"

void sandbox() {
    qore::SourceManager sourceMgr;
    qore::DiagPrinter diagPrinter(sourceMgr);
    qore::DiagManager diagMgr;
    diagMgr.addProcessor(&diagPrinter);

    qore::SourceBuffer srcBuffer = sourceMgr.createFromString("test", " ; print 567 + \"ab\" +5; print 6;");
    qore::ScannerImpl scanner(diagMgr, srcBuffer);

    qore::ParserImpl parser(diagMgr, scanner);
    qore::ast::Program::Ptr program = parser.parse();

    qore::ast::dump::DumpVisitor<qore::ast::dump::XmlFormat> dxv;
    program->accept(dxv);
    std::cout << std::endl;
    qore::ast::dump::DumpVisitor<qore::ast::dump::JsonFormat> djv;
    program->accept(djv);
    std::cout << std::endl;
    qore::ast::dump::DumpVisitor<qore::ast::dump::YamlFormat> dyv;
    program->accept(dyv);
    std::cout << std::endl;
    qore::ast::dump::DumpVisitor<qore::ast::dump::CompactFormat> dcv;
    program->accept(dcv);
    std::cout << std::endl;
}

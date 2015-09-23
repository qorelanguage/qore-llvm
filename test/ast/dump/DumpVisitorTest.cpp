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
#include <sstream>
#include "gtest/gtest.h"
#include "qore/ast/dump/DumpVisitor.h"
#include "qore/ast/dump/XmlFormat.h"
#include "qore/ast/dump/JsonFormat.h"
#include "qore/ast/dump/YamlFormat.h"
#include "qore/ast/dump/CompactFormat.h"
#include "qore/parser/ParserImpl.h"
#include "qore/scanner/ScannerImpl.h"
#include "qore/context/DiagManager.h"
#include "qore/context/SourceManager.h"

namespace qore {
namespace ast {
namespace dump {

struct DumpVisitorTest : ::testing::Test {
    Program::Ptr program = parse(";print 567 + \"ab\" +5;");

    Program::Ptr parse(const std::string &src) {
        SourceManager sourceMgr;
        DiagManager diagMgr;
        SourceBuffer srcBuffer = sourceMgr.createFromString("test", src);
        ScannerImpl scanner{diagMgr, srcBuffer};
        ParserImpl parser{diagMgr, scanner};
        return parser.parse();
    }
};

TEST_F(DumpVisitorTest, xml) {
    std::ostringstream ss;
    DumpVisitor<XmlFormat> dv{XmlFormat(ss)};
    parse(";print 567 + trim \"ab\" +5;")->accept(dv);
    EXPECT_EQ(R"(<program range="1:1-1:27">
  <body>
    <emptyStatement range="1:1-1:2" />
    <printStatement range="1:2-1:27">
      <binaryExpression range="1:8-1:26">
        <binaryExpression range="1:8-1:23">
          <integerLiteral range="1:8-1:11" value="567" />
          <operator range="1:12-1:13" />
          <unaryExpression range="1:14-1:23">
            <operator range="1:14-1:18" />
            <stringLiteral range="1:19-1:23" value="ab" />
          </unaryExpression>
        </binaryExpression>
        <operator range="1:24-1:25" />
        <integerLiteral range="1:25-1:26" value="5" />
      </binaryExpression>
    </printStatement>
  </body>
</program>
)", ss.str());
}

TEST_F(DumpVisitorTest, json) {
    std::ostringstream ss;
    DumpVisitor<JsonFormat> dv{JsonFormat(ss)};
    program->accept(dv);
    EXPECT_EQ(R"({
  "node": "program",
  "range": "1:1-1:22",
  "body": [
    {
      "node": "emptyStatement",
      "range": "1:1-1:2"
    },
    {
      "node": "printStatement",
      "range": "1:2-1:22",
      "expression": {
        "node": "binaryExpression",
        "range": "1:8-1:21",
        "left": {
          "node": "binaryExpression",
          "range": "1:8-1:18",
          "left": {
            "node": "integerLiteral",
            "range": "1:8-1:11",
            "value": 567
          },
          "operator": {
            "range": "1:12-1:13"
          },
          "right": {
            "node": "stringLiteral",
            "range": "1:14-1:18",
            "value": "ab"
          }
        },
        "operator": {
          "range": "1:19-1:20"
        },
        "right": {
          "node": "integerLiteral",
          "range": "1:20-1:21",
          "value": 5
        }
      }
    }
  ]
}
)", ss.str());
}

TEST_F(DumpVisitorTest, yaml) {
    std::ostringstream ss;
    DumpVisitor<YamlFormat> dv{YamlFormat(ss)};
    program->accept(dv);
    EXPECT_EQ(R"(node: program
body:
- node: emptyStatement
- node: printStatement
  expression:
    node: binaryExpression
    left:
      node: binaryExpression
      left:
        node: integerLiteral
        value: 567
      operator:
      right:
        node: stringLiteral
        value: ab
    operator:
    right:
      node: integerLiteral
      value: 5
)", ss.str());
}

TEST_F(DumpVisitorTest, compact) {
    std::ostringstream ss;
    DumpVisitor<CompactFormat> dv{CompactFormat(ss, false)};
    program->accept(dv);
    EXPECT_EQ(R"(program @1:1-1:22
+-body
  +-emptyStatement @1:1-1:2
  +-printStatement @1:2-1:22
    +-binaryExpression @1:8-1:21
      +-binaryExpression @1:8-1:18
      | +-integerLiteral @1:8-1:11 value: 567
      | +-operator @1:12-1:13
      | +-stringLiteral @1:14-1:18 value: "ab"
      +-operator @1:19-1:20
      +-integerLiteral @1:20-1:21 value: 5
)", ss.str());
}

TEST_F(DumpVisitorTest, compactUnicode) {
    std::ostringstream ss;
    DumpVisitor<CompactFormat> dv{CompactFormat(ss, true)};
    PrintStatement *printStmt = static_cast<PrintStatement*>(program->body[1].get());
    BinaryExpression *binExpr = static_cast<BinaryExpression*>(printStmt->expression.get());
    binExpr->left->accept(dv);
    EXPECT_EQ("binaryExpression @1:8-1:18\n"
"\u251c\u2500integerLiteral @1:8-1:11 value: 567\n"
"\u251c\u2500operator @1:12-1:13\n"
"\u2514\u2500stringLiteral @1:14-1:18 value: \"ab\"\n", ss.str());
}

TEST_F(DumpVisitorTest, exprStmt) {
    std::ostringstream ss;
    DumpVisitor<CompactFormat> dv{CompactFormat(ss, false)};
    parse("567;")->accept(dv);
    EXPECT_EQ(R"(program @1:1-1:5
+-body
  +-expressionStatement @1:1-1:5
    +-integerLiteral @1:1-1:4 value: 567
)", ss.str());
}

TEST_F(DumpVisitorTest, assignVarDecl) {
    std::ostringstream ss;
    DumpVisitor<CompactFormat> dv{CompactFormat(ss, false)};
    parse("my a = b;")->accept(dv);
    EXPECT_EQ(R"(program @1:1-1:10
+-body
  +-expressionStatement @1:1-1:10
    +-assignment @1:1-1:9
      +-varDecl @1:1-1:5 name: "a"
      +-operator @1:6-1:7
      +-identifier @1:8-1:9 name: "b"
)", ss.str());
}

TEST_F(DumpVisitorTest, compoundStmt) {
    std::ostringstream ss;
    DumpVisitor<CompactFormat> dv{CompactFormat(ss, false)};
    parse("{;}")->accept(dv);
    EXPECT_EQ(R"(program @1:1-1:4
+-body
  +-compoundStatement @1:1-1:4
    +-statements
      +-emptyStatement @1:2-1:3
)", ss.str());
}

} // namespace dump
} // namespace ast
} // namespace qore

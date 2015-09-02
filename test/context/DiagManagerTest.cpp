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
#include "gtest/gtest.h"
#include "qore/context/DiagManager.h"
#include "Helpers.h"
#include "Mocks.h"
#include "../Utils.h"

namespace qore {

struct DiagManagerTest : ::testing::Test, SourceIdTestHelper, DiagManagerTestHelper {
    std::function<void(DiagRecord&)> messageCaptor = [this](DiagRecord &r){
        capturedMessage = r.message;
    };
    SourceLocation location{sourceId1, 12, 34};
    std::string capturedMessage;
};

typedef DiagManagerTest DiagManagerDeathTest;

TEST_F(DiagManagerTest, BuilderCallsCallbackInDestructor) {
    bool callbackCalled = false;
    {
        DiagBuilder b([&, this](DiagRecord &r){
            callbackCalled = true;
            EXPECT_EQ(DiagId::ScannerInvalidInteger, r.id);
            EXPECT_EQ(DiagLevel::Warning, r.level);
            EXPECT_EQ("Text", r.message);
            EXPECT_EQ(location, r.location);
        }, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text", location);
        EXPECT_FALSE(callbackCalled);
    }
    EXPECT_TRUE(callbackCalled);
}

TEST_F(DiagManagerTest, BuilderArgChar) {
    DiagBuilder(messageCaptor, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text '%s' text", location)
            << '@';
    EXPECT_EQ("Text '@' text", capturedMessage);
}

TEST_F(DiagManagerTest, BuilderArgString) {
    DiagBuilder(messageCaptor, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text '%s' text", location)
            << "embedded";
    EXPECT_EQ("Text 'embedded' text", capturedMessage);
}

TEST_F(DiagManagerTest, BuilderArgCharRange) {
    const char *str = "embedded";
    DiagBuilder(messageCaptor, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text '%s' text", location)
            .arg(str + 2, str + 5);
    EXPECT_EQ("Text 'bed' text", capturedMessage);
}

TEST_F(DiagManagerTest, BuilderArgIterators) {
    std::vector<char> vec{'v', 'e', 'c', 't', 'o', 'r'};
    DiagBuilder(messageCaptor, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text '%s' text", location)
            .arg(vec.begin() + 1, vec.end() - 1);
    EXPECT_EQ("Text 'ecto' text", capturedMessage);
}

TEST_F(DiagManagerTest, BuilderDtorNothrow) {
    EXPECT_NO_THROW(
            DiagBuilder([](DiagRecord&){
                throw std::exception();
            }, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text", location);
    );
}

#ifndef NDEBUG
TEST_F(DiagManagerDeathTest, BuilderDtorChecksMissingArgs) {
    EXPECT_DEATH(
            DiagBuilder(messageCaptor, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text '%s' text", location);
    , "Missing parameter");
}

TEST_F(DiagManagerDeathTest, BuilderExtraArg) {
    EXPECT_DEATH(
            DiagBuilder(messageCaptor, DiagId::ScannerInvalidInteger, DiagLevel::Warning, "Text", location) << "a";
    , "Unexpected parameter");
}

TEST_F(DiagManagerDeathTest, EnableEnabled) {
    DiagManager mgr;
    EXPECT_DEATH(mgr.enable(), "already enabled");
}
#endif

TEST_F(DiagManagerTest, ReportCreatesBuilder) {
    DiagManager mgr;
    DiagBuilder b = mgr.report(DiagId::ScannerInvalidInteger, location);
    const DiagRecord &r = extractRecord(b);     //b must not go out of scope until we examine r
    EXPECT_EQ(DiagId::ScannerInvalidInteger, r.id);
    EXPECT_EQ(DiagLevel::Error, r.level);
    EXPECT_EQ("invalid integer literal '%s'", r.message);
    EXPECT_EQ(location, r.location);
    b << "123";         //prevents dtor from asserting missing argument
}

TEST_F(DiagManagerTest, ProcessCallsProcessor) {
    MockDiagProcessor processor;
    DiagManager mgr;
    mgr.addProcessor(&processor);

    DiagRecord record{DiagId::ScannerInvalidCharacter, DiagLevel::Error, "message", location};
    EXPECT_CALL(processor, process(MatchDiagRecord(record))).Times(1);
    callProcess(mgr, record);
}

TEST_F(DiagManagerTest, DisableEnable) {
    MockDiagProcessor processor;
    DiagManager mgr;
    mgr.addProcessor(&processor);

    DiagRecord record1{DiagId::ScannerInvalidCharacter, DiagLevel::Error, "message1", location};
    DiagRecord record2{DiagId::ScannerInvalidInteger, DiagLevel::Warning, "message2", location};
    EXPECT_CALL(processor, process(MatchDiagRecord(record1))).Times(0);
    EXPECT_CALL(processor, process(MatchDiagRecord(record2))).Times(1);
    mgr.disable();
    callProcess(mgr, record1);
    mgr.enable();
    callProcess(mgr, record2);
}

TEST_F(DiagManagerTest, DiagPrinter) {
    DiagRecord record{DiagId::ScannerInvalidCharacter, DiagLevel::Error, "message", location};
    DiagPrinter printer([&](std::ostream &o, const qore::SourceLocation &l) -> std::ostream & {
        EXPECT_EQ(location, l);
        return o << "location_ok";
    });

    std::string capturedStderr;
    {
        RedirectStderr _(capturedStderr);
        printer.process(record);
    }
    EXPECT_EQ("location_ok: error: message\n", capturedStderr);
}

TEST_F(DiagManagerTest, DisableDiagHelper) {
    DiagManager diagMgr;
    {
        EXPECT_EQ(0U, getDisabledCounter(diagMgr));
        DisableDiag dd(diagMgr);
        EXPECT_EQ(1U, getDisabledCounter(diagMgr));
    }
    EXPECT_EQ(0U, getDisabledCounter(diagMgr));
}

} // namespace qore

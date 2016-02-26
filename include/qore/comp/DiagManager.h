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
///
/// \file
/// \brief Provides support for generating diagnostic messages.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_DIAGMANAGER_H_
#define INCLUDE_QORE_COMP_DIAGMANAGER_H_

#include <cassert>
#include <functional>
#include <string>
#include <vector>
#include "qore/common/Logging.h"
#include "qore/comp/DiagRecord.h"

namespace qore {
namespace comp {

/**
 * \brief Interface for diagnostic processors.
 */
class IDiagProcessor {

public:
    virtual ~IDiagProcessor() = default;

    /**
     * \brief Called for each diagnostic message.
     *
     * The processor can modify the diagnostic message or report it to the user.
     * \param record the diagnostic message to process
     */
    virtual void process(DiagRecord &record) = 0;

protected:
    IDiagProcessor() = default;

private:
    IDiagProcessor(const IDiagProcessor &) = delete;
    IDiagProcessor(IDiagProcessor &&) = delete;
    IDiagProcessor &operator=(const IDiagProcessor &) = delete;
    IDiagProcessor &operator=(IDiagProcessor &&) = delete;
};

/**
 * \brief A helper class for building diagnostic messages with additional parameters.
 */
class DiagBuilder {

private:
    using ProcessCallback = std::function<void(DiagRecord &)>;

public:
    /**
     * \brief Creates the builder.
     * \param processCallback the function to be called for processing the message once it is built
     * \param id the identifier of the diagnostic
     * \param code the diagnostic code
     * \param level the level of the diagnostic
     * \param message the diagnostic message
     * \param location the location in the source
     */
    DiagBuilder(ProcessCallback processCallback, DiagId id, const char *code, DiagLevel level,
            std::string message, SourceLocation location);

    /**
     * \brief Default move constructor.
     * \param other the instance to be moved into `this`
     */
    DiagBuilder(DiagBuilder &&other) = default;

    /**
     * \brief Default move assignment.
     * \param other the instance to be moved into `this`
     * \return `this`
     */
    DiagBuilder &operator=(DiagBuilder &&other) = default;

    /**
     * \brief Builds the diagnostic message and passes it to the callback specified in constructor.
     *
     * Any exceptions throws by the callback are ignored.
     */
    ~DiagBuilder();

    /**
     * \brief Sets the value of a parameter of the message.
     *
     * The value will replace the next occurrence of '%s' in the message.
     * \param s the value of the parameter
     * \return `this` for chaining
     */
    DiagBuilder &operator<<(const std::string &s) {
        return arg<>(s.begin(), s.end());
    }

    /**
     * \brief Sets the value of a parameter of the message.
     *
     * The value will replace the next occurrence of '%s' in the message.
     * \param c the value of the parameter
     * \return `this` for chaining
     */
    DiagBuilder &operator<<(char c) {
        return arg<>(&c, &c + 1);
    }

    /**
     * \brief Sets the value of a parameter of the message.
     *
     * The value will replace the next occurrence of '%s' in the message.
     * \param begin the start of the value of the parameter
     * \param end points to the character after the end of the parameter
     * \return `this` for chaining
     */
    DiagBuilder &arg(const char *begin, const char *end) {
        return arg<>(begin, end);
    }

    /**
     * \brief Sets the value of a parameter of the message.
     *
     * The value will replace the next occurrence of '%s' in the message.
     * \param begin the start of the value of the parameter
     * \param end points to the character after the end of the parameter
     * \return `this` for chaining
     */
    template<typename Iterator>
    DiagBuilder &arg(Iterator begin, Iterator end) {
        auto index = record.message.find("%s");
        assert(index != std::string::npos && "Unexpected parameter for a diagnostic message");
        auto i = record.message.begin() + index;
        record.message.replace(i, i + 2, begin, end);
        return *this;
    }

private:
    DiagBuilder(const DiagBuilder &) = delete;
    DiagBuilder &operator=(const DiagBuilder &) = delete;

private:
    ProcessCallback processCallback;
    DiagRecord record;
};

/**
 * \brief Manages reporting of diagnostic messages.
 */
class DiagManager {

public:
    DiagManager() = default;

    /**
     * \brief Reports a message.
     * \param diagId the identifier of the diagnostic message
     * \param location the location in the source
     * \return a builder for providing additional info
     */
    DiagBuilder report(DiagId diagId, SourceLocation location);

    /**
     * \brief Temporarily disables reporting of all diagnostic messages.
     */
    void disable() {
        disabledCounter++;
        CLOG("DIAG", "Disabling diagnostics, counter = " << disabledCounter);
    }

    /**
     * \brief Re-enables reporting of all diagnostic messages disabled by disable().
     *
     * Must be called as many times as disable().
     */
    void enable() {
        assert(disabledCounter && "Diagnostics are already enabled");
        disabledCounter--;
        CLOG("DIAG", "Enabling diagnostics, counter = " << disabledCounter);
    }

    /**
     * \brief Adds a diagnostic processor which will be called for each diagnostic.
     *
     * Diagnostic manager does not take ownership of the pointer, it is the responsibility of the caller
     * that the processor is freed (but only after the manager itself has been destroyed).
     * \param processor the processor to add
     */
    void addProcessor(IDiagProcessor *processor) {
        processors.push_back(processor);
    }

private:
    void process(DiagRecord &record);

private:
    std::vector<IDiagProcessor *> processors;
    int disabledCounter{0};
};

/**
 * \brief A helper class for disabling diagnostics an enabling them at the end of scope.
 */
class DisableDiag {

public:
    /**
     * \brief Disables diagnostic messages until the end of the scope.
     * \param diagMgr the diagnostic manager
     */
    explicit DisableDiag(DiagManager &diagMgr) : diagMgr(diagMgr) {
        diagMgr.disable();
    }

    /**
     * \brief Re-enables diagnostics disabled by the constructor.
     */
    ~DisableDiag() {
        diagMgr.enable();
    }

private:
    DiagManager &diagMgr;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_DIAGMANAGER_H_

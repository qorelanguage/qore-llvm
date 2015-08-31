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
#ifndef INCLUDE_QORE_CONTEXT_DIAGMANAGER_H_
#define INCLUDE_QORE_CONTEXT_DIAGMANAGER_H_

#include <vector>
#include "qore/context/DiagProcessor.h"

namespace qore {

/**
 * \brief A helper class for building diagnostic messages with additional parameters.
 */
class DiagBuilder {

public:
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
    DiagBuilder(class DiagManager &, const class DiagInfo &, SourceLocation);

private:
    class DiagManager &mgr;
    DiagRecord record;

    friend class DiagManager;
};

/**
 * \brief Manages reporting of diagnostic messages.
 */
class DiagManager {

public:
    /**
     * \brief Reports a message.
     * \param diagId the identifier of the diagnostic message
     * \param location the location in the source
     * \return a builder for providing additional info
     */
    DiagBuilder report(DiagId diagId, SourceLocation location);

    /**
     * \brief Adds a diagnostic processor which will be called for each diagnostic.
     *
     * Diagnostic manager does not take ownership of the pointer, it is the responsibility of the caller
     * that the processor is freed (but only after the manager itself has been destroyed).
     * \param processor the processor to add
     */
    void addProcessor(DiagProcessor *processor) {
        processors.push_back(processor);
    }

private:
    void process(DiagRecord &record);

private:
    std::vector<DiagProcessor *> processors;

    friend class DiagBuilder;
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_DIAGMANAGER_H_

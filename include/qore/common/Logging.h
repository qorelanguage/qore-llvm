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
/// \brief Provides support for logging messages.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMMON_LOGGING_H_
#define INCLUDE_QORE_COMMON_LOGGING_H_

#include "qore/common/Indent.h"

/**
 * \brief Logs a message with custom location using QORE_LOG_COMPONENT as the component name.
 * \param M the message to log
 * \param FU the name of current function
 * \param FI the name of the source file
 * \param L the line number of the log message
 */
#define LOG_EX(M, FU, FI, L)        CLOG_EX(QORE_LOG_COMPONENT, M, FU, FI, L)

/**
 * \brief Logs a message using QORE_LOG_COMPONENT as the component name.
 * \param M the message to log
 */
#define LOG(M)                      CLOG(QORE_LOG_COMPONENT, M)

/**
 * \brief Logs the entry and exit of a function using QORE_LOG_COMPONENT as the component name.
 */
#define LOG_FUNCTION()              CLOG_FUNCTION(QORE_LOG_COMPONENT)

#ifdef QORE_LOGGING

#include <sstream>

/**
 * \brief Defines the component name to be associated with logging messages.
 *
 * Each logging message is assigned to a component which can be used for filtering. By default the component name
 * is the same as the source file name.
 */
#ifndef QORE_LOG_COMPONENT
#define QORE_LOG_COMPONENT          __FILE__
#endif

#ifdef WIN32
#define __PRETTY_FUNCTION__         __FUNCSIG__
#endif

/**
 * \brief Logs a message with custom location.
 * \param C the name of the component for filtering
 * \param M the message to log
 * \param FU the name of current function
 * \param FI the name of the source file
 * \param L the line number of the log message
 */
#define CLOG_EX(C, M, FU, FI, L)                    \
    ::qore::log::LoggerManager::get()->log(         \
            C,                                      \
            static_cast<std::ostringstream&>(       \
                std::ostringstream().flush() << M   \
            ).str(),                                \
            FU,                                     \
            FI,                                     \
            L                                       \
    );

/**
 * \brief Logs a message.
 * \param C the name of the component for filtering
 * \param M the message to log
 */
#define CLOG(C, M)          CLOG_EX(C, M, __PRETTY_FUNCTION__, __FILE__, __LINE__);

/**
 * \brief Logs the entry and exit of a function.
 * \param C the name of the component for filtering
 */
#define CLOG_FUNCTION(C)    ::qore::log::LogScope _(C, __PRETTY_FUNCTION__, __FILE__, __LINE__);

namespace qore {

/**
 * \brief Contains logging utilities.
 */
namespace log {

/**
 * \brief Filters and formats log messages.
 */
class Logger {

public:
    Logger() {
    }

    virtual ~Logger() = default;

    /**
     * \brief Logs a message if its component matches the filter.
     * \param component the name of the component for filtering
     * \param message the message to log
     * \param function the name of current function
     * \param file the name of the source file
     * \param line the line number of the log message
     */
    void log(const std::string &component, const std::string &message,
            const char *function, const char *file, int line);

    /**
     * \brief Decides whether a log message with given component should be logged or ignored.
     * \param component the component name
     * \return true if the message should be logged
     */
    virtual bool filter(const std::string &component) {
        return true;
    }

    /**
     * \brief Formats the header of a log message.
     * \param function the name of current function
     * \param file the name of the source file
     * \param line the line number of the log message
     * \return formatted header
     */
    virtual std::string formatHeader(const char *function, const char *file, int line);

    /**
     * \brief Determines the width of the header for aligning the log messages.
     * \return the width of the header
     */
    virtual int getHeaderWidth() {
        return 40;
    }

private:
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

private:
    Indent indent;

    friend class LogScope;
};

/**
 * \brief Determines which logger will be used.
 */
class LoggerManager {

public:
    /**
     * \return current logger
     */
    static Logger *get() {
        return currentLogger;
    }

    /**
     * \brief Changes the logger.
     * \param logger the new logger
     * \return the original logger
     */
    static Logger *set(Logger *logger) {
        Logger *orig = currentLogger;
        currentLogger = logger;
        return orig;
    }

private:
    static Logger defaultLogger;
    static Logger *currentLogger;

    LoggerManager() = delete;
    ~LoggerManager() = delete;
};

/**
 * \brief A helper class for logging of scopes (functions) using RAII.
 */
class LogScope {

public:

    /**
     * \brief Logs the entry to the scope.
     * \param component the name of the component for filtering
     * \param function the name of the function
     * \param file the name of the source file
     * \param line the line number of the log message
     */
    LogScope(const char *component, const char *function, const char *file, int line)
        : component(component), function(function), file(file), line(line) {
        CLOG_EX(component, "Entering " << function, function, file, line);
        ++::qore::log::LoggerManager::get()->indent;
    }

    /**
     * \brief Logs the exit of the scope.
     */
    ~LogScope() {
        --::qore::log::LoggerManager::get()->indent;
        CLOG_EX(component, "Leaving " << function, function, file, line);
    }

private:
    const char *component;
    const char *function;
    const char *file;
    int line;
};

} //namespace log
} //namespace qore

#else   // QORE_LOGGING

#define CLOG_EX(C, M, FU, FI, L)    do {} while(0)
#define CLOG(C, M)                  do {} while(0)
#define CLOG_FUNCTION(C)            do {} while(0)

#endif  // QORE_LOGGING

#endif /* INCLUDE_QORE_COMMON_LOGGING_H_ */

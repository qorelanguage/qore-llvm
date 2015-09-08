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
#include "qore/common/Logging.h"

#ifdef QORE_LOGGING

#include <iomanip>
#include <iostream>

namespace qore {
namespace log {

Logger LoggerManager::defaultLogger;
Logger *LoggerManager::currentLogger = &LoggerManager::defaultLogger;


void Logger::log(const std::string &component, const std::string &message,
        const char *function, const char *file, int line) {
    if (filter(component)) {
        std::cout
            << std::left
            << std::setw(getHeaderWidth())
            << formatHeader(function, file, line)
            << indent
            << message
            << std::endl;
    }
}

std::string Logger::formatHeader(const char *function, const char *file, int line) {
    std::string header(file);
    std::string::size_type slash = header.find_last_of('/');

    if (slash != std::string::npos) {
        header = header.substr(slash + 1);
    }
    header.append(":");
    header.append(std::to_string(line));
    return header;
}

} //namespace log
} //namespace qore

#endif //QORE_LOGGING

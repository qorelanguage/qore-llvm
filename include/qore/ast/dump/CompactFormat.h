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
/// \brief Formatter for producing compact dumps of the AST.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_DUMP_COMPACTFORMAT_H_
#define INCLUDE_QORE_AST_DUMP_COMPACTFORMAT_H_

#include <array>
#include <iostream>
#include <string>
#include <vector>
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {
namespace dump {

/**
 * \brief Formatter for producing compact dumps of the AST.
 */
class CompactFormat {

public:
    /**
     * Constructor.
     * \param os the destination output stream
     * \param useUnicode whether Unicode border drawing characters should be used instead of ASCII
     */
    CompactFormat(std::ostream &os = std::cout, bool useUnicode = true) : os(os), inHeader(false),
            last(false), savedChildName(nullptr), symbols{"  ", "| ", "+-", "+-"} {
        if (useUnicode) {
            symbols = {"  ", "\u2502 ", "\u251c\u2500", "\u2514\u2500"};
        }
    }

private:
    enum class IndentItem {
        Empty = 0,
        Cont = 1,
        Node = 2,
        LastNode = 3
    };

public:
    /// \name Operator implementation.
    /// \{
    CompactFormat &operator<<(BeginNode beginNode) {
        indent() << (beginNode.type ? beginNode.type : savedChildName);
        inHeader = true;
        return *this;
    }

    template<typename T>
    CompactFormat &operator<<(Attribute<T> attr) {
        os << " " << attr.name << ": ";
        qoute(attr.value);
        last = false;
        return *this;
    }

    CompactFormat &operator<<(Range attr) {
        os << " @";
        qoute(attr.value);
        last = false;
        return *this;
    }

    CompactFormat &operator<<(EndNodeHeader) {
        inHeader = false;
        os << "\n";
        indentStack.push_back(IndentItem::Node);
        return *this;
    }

    CompactFormat &operator<<(EndNode) {
        if (inHeader) {
            os << "\n";
            inHeader = false;
        } else {
            indentStack.pop_back();
        }
        return *this;
    }

    CompactFormat &operator<<(BeginArray) {
        indent() << savedChildName << "\n";
        indentStack.push_back(IndentItem::Node);
        return *this;
    }

    CompactFormat &operator<<(EndArray) {
        indentStack.pop_back();
        return *this;
    }

    CompactFormat &operator<<(Child child) {
        indentStack.back() = last ? IndentItem::LastNode : IndentItem::Node;
        last = false;
        savedChildName = child.name;
        return *this;
    }

    CompactFormat &operator<<(Last) {
        last = true;
        return *this;
    }
    /// \}

private:
    template<typename T>
    void qoute(T value) {
        os << value;
    }

    std::ostream &indent() {
        for (auto &s : indentStack) {
            os << symbols[static_cast<int>(s)];
            if (s == IndentItem::Node) {
                s = IndentItem::Cont;
            } else if (s == IndentItem::LastNode) {
                s = IndentItem::Empty;
            }
        }
        return os;
    }

private:
    std::ostream &os;
    std::vector<IndentItem> indentStack;
    bool inHeader;
    bool last;
    const char *savedChildName;
    std::array<const char *, 4> symbols;
};

/**
 * \private
 */
template<>
inline void CompactFormat::qoute(std::string value) {
    os << "\"" << value << "\"";
}

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_COMPACTFORMAT_H_

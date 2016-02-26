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
/// \brief Utility class for dumping data from semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_DUMP_H_
#define INCLUDE_QORE_COMP_SEMANTIC_DUMP_H_

#include <string>
#include <utility>
#include "qore/common/Exceptions.h"
#include "qore/common/Indent.h"
#include "qore/comp/semantic/Namespace.h"
#include "qore/comp/semantic/Class.h"

namespace qore {
namespace comp {
namespace semantic {

/// \cond IGNORED_BY_DOXYGEN
template<typename OS>
class Dump {
public:
    Dump(SourceManager &srcMgr, OS &os) : srcMgr(srcMgr), os(os) {
    }

    void dumpNamespace(Namespace &ns) {
        os << indent++ << "-namespace " << ns.getName();
        if (!ns.isRoot()) {
            os << " @" << decode(ns.getLocation());
        }
        os << "\n";
        ns.forEachNamespace(std::bind(&Dump::dumpNamespace, this, std::placeholders::_1));
        ns.forEachClass(std::bind(&Dump::dumpClass, this, std::placeholders::_1));
        ns.forEachConstant(std::bind(&Dump::dumpConstant, this, std::placeholders::_1));
        ns.forEachFunctionGroup(std::bind(&Dump::dumpFunctionGroup, this, std::placeholders::_1));
        ns.forEachGlobalVariable(std::bind(&Dump::dumpGlobalVariable, this, std::placeholders::_1));
        --indent;
    }

    void dumpClass(Class &c) {
        os << indent << "-class " << c.getName() << " @" << decode(c.getLocation()) << "\n";
    }

    void dumpConstant(Constant &c) {
        os << indent << "-const " << c.getName() << " @" << decode(c.getLocation()) << "\n";
    }

    void dumpFunctionGroup(const Function::Group &fg) {
        for (const Function::Ptr &f : fg) {
            os << indent << "-sub " << f->getName() << " @" << decode(f->getLocation()) << "\n";
        }
    }

    void dumpGlobalVariable(GlobalVariable &gv) {
        os << indent << "-our " << gv.getName() << " @" << decode(gv.getLocation()) << "\n";
    }

private:
    std::string decode(const SourceLocation &location) {
        assert(location.sourceId >= 0);
        std::pair<int, int> l = srcMgr.get(location.sourceId).decodeLocation(location.offset);
        std::ostringstream str;
        str << l.first << ":" << l.second;
        return str.str();
    }

private:
    SourceManager &srcMgr;
    OS &os;
    log::Indent indent;
};

template<typename OS>
void dump(SourceManager &srcMgr, OS &os, Namespace &n) {
    Dump<OS> dump(srcMgr, os);
    dump.dumpNamespace(n);
}
/// \endcond

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_DUMP_H_

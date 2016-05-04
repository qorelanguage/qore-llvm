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
/// \brief Utility class for dumping the runtime data structures.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_DUMP_H_
#define INCLUDE_QORE_CORE_DUMP_H_

#include <string>
#include "qore/core/Env.h"

namespace qore {

/// \cond IGNORED_BY_DOXYGEN
template<typename OS>
class Dump {

public:
    explicit Dump(OS &os) : os(os) {
    }

    void dump(const Namespace &ns, bool isRoot = false) {
        os << indent++;
        if (isRoot) {
            os << "-root namespace";
        } else {
            os << "-namespace " << ns.getName() << " @" << ns.getLocation();
        }
        os << "\n";
        for (auto &n : ns.getNamespaces()) {
            dump(n);
        }
        for (auto &c : ns.getClasses()) {
            dump(c);
        }
        for (auto &gv : ns.getGlobalVariables()) {
            dump(gv);
        }
        --indent;
    }

    void dump(const Class &c) {
        os << indent << "-class " << c.getName() << " @" << c.getLocation() << "\n";
    }

    void dump(const GlobalVariable &gv) {
        os << indent << "-our " << gv.getType() << " " << gv.getName() << " @" << gv.getLocation() << "\n";
    }

private:
    OS &os;
    util::Indent indent;
};

template<typename OS>
void dump(OS &os, Env &env) {
    Dump<OS> dump(os);
    dump.dump(env.getRootNamespace(), true);
}
/// \endcond IGNORED_BY_DOXYGEN

} // namespace qore

#endif // INCLUDE_QORE_CORE_DUMP_H_

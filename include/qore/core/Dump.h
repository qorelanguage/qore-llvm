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
#include "qore/core/code/Dump.h"

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
            os << "-namespace " << name(ns) << location(ns);
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
        for (auto &fg : ns.getFunctionGroups()) {
            dump(fg);
        }
        --indent;
    }

    void dump(const Class &c) {
        os << indent << "-class " << name(c) << location(c) << "\n";
    }

    void dump(const GlobalVariable &gv) {
        os << indent << "-our " << gv.getType() << " " << name(gv) << location(gv) << "\n";
    }

    void dump(const FunctionGroup &fg) {
        for (auto &f : fg.getFunctions()) {
            dump(name(fg), f);
        }
    }

    void dump(const std::string &name, const Function &f) {
        os << indent++ << "-sub " << f.getType() << " " << name << location(f)
                << ", " << f.getTempCount() << " temps, " << f.getLocalVariables().size() << " locals\n";
        for (auto &lv : f.getLocalVariables()) {
            os << indent << "[" << lv.getIndex() << "] " << lv.getType() << " " << lv.getName() << location(lv) << "\n";
        }
        code::Dump<OS> codeDump(os, indent);
        codeDump.dump(f.getEntryBlock());
        --indent;
    }

private:
    template<typename T>
    std::string location(const T &t) {
        std::ostringstream str;
        str << " @" << t.getLocation();
        return str.str();
    }

    template<typename T>
    std::string name(const T &t) {
        const std::string &fullName = t.getFullName();
        auto pos = fullName.rfind(':');
        assert(pos != std::string::npos);
        return fullName.substr(pos + 1);
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

template<typename OS>
void dump(OS &os, const std::string &name, const Function &f) {
    Dump<OS> dump(os);
    dump.dump(name, f);
}
/// \endcond IGNORED_BY_DOXYGEN

} // namespace qore

#endif // INCLUDE_QORE_CORE_DUMP_H_

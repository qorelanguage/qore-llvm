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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_DUMP_H_
#define INCLUDE_QORE_COMP_SEM_DUMP_H_

#include <string>

namespace qore {
namespace comp {
namespace sem {

template<typename OS>
class Dump {

public:
    Dump(Context &ctx, OS os) : ctx(ctx), os(os) {
    }

    void dump(const NamespaceScope &ns) {
        os << indent++;
        if (ns.isRoot()) {
            os << "-root namespace";
        } else {
            os << "-namespace " << ctx.getString(ns.name) << " @" << ctx.decode(ns.location);
        }
        os << "\n";
        for (auto &n : ns.namespaces) {
            dump(*n.second);
        }
        for (auto &c : ns.classes) {
            dump(*c.second);
        }
        for (auto &gv : ns.globalVariables) {
            dump(*gv.second);
        }
        --indent;
    }

    void dump(const ClassScope &cs) {
        os << indent << "-class " << ctx.getString(cs.getName()) << " @" << ctx.decode(cs.getLocation()) << "\n";
    }

    void dump(const GlobalVariableInfo &gv) {
        os << indent << "-our " << gv.getType() << " " << ctx.getString(gv.getName())
                << " @" << ctx.decode(gv.getLocation()) << "\n";
    }

private:
    Context &ctx;
    OS os;
    log::Indent indent;
};

template<typename OS>
void dump(OS os, Context &ctx, NamespaceScope &ns) {
    Dump<OS> dump(ctx, os);
    dump.dump(ns);
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_DUMP_H_

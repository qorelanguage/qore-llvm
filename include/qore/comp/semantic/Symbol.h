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
/// \brief Defines classes related to symbol tables.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_SYMBOL_H_
#define INCLUDE_QORE_COMP_SEMANTIC_SYMBOL_H_

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Base class of all symbols.
 */
class Symbol {

public:
    /**
     * \brief Identifies the type of the symbol.
     */
    enum class Kind {
        Namespace,              //!< Identifies instances of \ref Namespace.
        Class,                  //!< Identifies instances of \ref Class.
    };

public:
    virtual ~Symbol() = default;

    /**
     * \brief Returns the type of the symbol.
     * \return the type of the symbol
     */
    virtual Kind getKind() const = 0;

protected:
    Symbol() = default;

private:
    Symbol(const Symbol &) = delete;
    Symbol(Symbol &&) = delete;
    Symbol &operator=(const Symbol &) = delete;
    Symbol &operator=(Symbol &&) = delete;
};

/**
 * \brief Helper template for extending \ref Symbol.
 */
template<Symbol::Kind kind> class SymbolBase : public Symbol {

public:
    /**
     * \brief Identifies the symbol type.
     */
    static constexpr Kind SymbolKind = kind;

    /**
     * \brief Returns the type of the symbol.
     * \return the type of the symbol
     */
    Kind getKind() const override {
        return kind;
    }
};

/**
 * \brief Represents a collection of symbols with the same name.
 *
 * The collection is not a set in mathematical sense since it does not check for duplicities. It also does not own
 * any of the elements. It simply provides useful methods for accessing and classifying symbols.
 * It is up to the user to manage any invariants and lifetime of the symbols.
 */
class SymbolSet {

public:
    SymbolSet() = default;

    /**
     * \brief Adds a symbol to the set.
     * \param symbol the symbol to add, must not be null
     */
    void add(Symbol *symbol) {
        assert(symbol != nullptr);
        symbols.push_back(std::move(symbol));
    }

    /**
     * \brief Returns true if the collection contains at least one \ref Namespace symbol.
     * \return true if the collection contains at least one \ref Namespace symbol.
     */
    bool containsNamespace() const {
        return contains(Symbol::Kind::Namespace);
    }

    /**
     * \brief Returns true if the collection contains at least one \ref Class symbol.
     * \return true if the collection contains at least one \ref Class symbol.
     */
    bool containsClass() const {
        return contains(Symbol::Kind::Class);
    }

    /**
     * \brief Finds a symbol of given type.
     *
     * If the collection contains multiple symbols of given type, it chooses one arbitrarily.
     * \tparam S the type of the symbol
     * \return a symbol of type S or `nullptr` if there is no such symbol
     */
    template<typename S>
    S *find() const {
        return static_cast<S *>(find(S::SymbolKind));
    }

private:
    bool contains(Symbol::Kind kind) const {
        return find(kind) != nullptr;
    }

    Symbol *find(Symbol::Kind kind) const {
        auto it = std::find_if(symbols.begin(), symbols.end(), [=](Symbol *s){ return s->getKind() == kind; });
        return it == symbols.end() ? nullptr : *it;
    }

private:
    SymbolSet(const SymbolSet &) = delete;
    SymbolSet(SymbolSet &&) = delete;
    SymbolSet &operator=(const SymbolSet &) = delete;
    SymbolSet &operator=(SymbolSet &&) = delete;

private:
    std::vector<Symbol *> symbols;
};

/**
 * \brief Represents a collection of symbols.
 *
 * The collection does not own any of the elements. It simply provides useful methods for accessing and
 * classifying symbols. It is up to the user to manage any invariants and lifetime of the symbols.
 */
class SymbolTable {

public:
    SymbolTable() = default;

    /**
     * \brief Returns the set of symbols with given name.
     * \param name the name of the symbols
     * \return the set of symbols with given name
     */
    SymbolSet &operator[](const std::string &name) {
        return sets[name];
    }

    /**
     * \brief Finds a symbol of given type and name.
     *
     * If the symbol table contains multiple symbols of given type and name, it chooses one arbitrarily.
     * \tparam S the type of the symbol
     * \param name the name of the symbol
     * \return a symbol of type S and name `name` or `nullptr` if there is no such symbol
     */
    template<typename S>
    S *find(const std::string &name) const {
        auto it = sets.find(name);
        return it == sets.end() ? nullptr : it->second.find<S>();
    }

private:
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;
    SymbolTable &operator=(const SymbolTable &) = delete;
    SymbolTable &operator=(SymbolTable &&) = delete;

private:
    std::map<std::string, SymbolSet> sets;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_SYMBOL_H_

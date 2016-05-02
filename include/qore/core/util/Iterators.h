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
/// \brief Iterator adapters.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_UTIL_ITERATORS_H_
#define INCLUDE_QORE_CORE_UTIL_ITERATORS_H_

#include <map>
#include <memory>
#include <vector>

namespace qore {
namespace util {

/**
 * \brief Utility class for adapting iterators of one type to another.
 *
 * This class provides a limited implementation of the iterator concept for one type (`C::ElementType &`) by
 * wrapping another iterator (`C::Iterator`). The actual conversion of `C::Iterator` to `C::ElementType &` is
 * delegated to `C::convert()`.
 * \tparam C a class that defines both types and actually performs the conversion
 * \see VectorOfUniquePtrIteratorAdapter
 * \see MapToUniquePtrIteratorAdapter
 */
template<typename C>
class IteratorAdapter {

public:
    /**
     * \brief Constructs the adapter.
     * \param it the underlying iterator
     */
    explicit IteratorAdapter(typename C::Iterator it) : it(it) {
    }

    /**
     * \brief Returns the item pointed to by the iterator.
     * \return the item pointed to by the iterator
     */
    typename C::ElementType &operator*() {
        return C::convert(it);
    }

    /**
     * \brief Returns a pointer to the item pointed to by the iterator.
     * \return a pointer to the item pointed to by the iterator
     */
    typename C::ElementType *operator->() {
         return &C::convert(it);
    }

    /**
     * \brief Advances the iterator to the next item.
     * \return this instance
     */
    IteratorAdapter<C>& operator++() {
        ++it;
       return *this;
    }

    /**
     * \brief Compares the iterator to another.
     * \param other the iterator to compare this to
     * \return true if both iterators point to the same item
     */
    bool operator==(const IteratorAdapter<C> &other) const {
        return other.it == it;
    }

    /**
     * \brief Compares the iterator to another.
     * \param other the iterator to compare this to
     * \return true if the iterators point to different items
     */
    bool operator!=(const IteratorAdapter<C> &other) const {
        return !(*this == other);
    }

private:
    typename C::Iterator it;
};

/**
 * \brief Converter for dereferencing `std::unique_ptr` in a `std::vector`.
 *
 * This class can be used as the `C` template parameter of \ref IteratorAdapter for providing an iterator of `E`
 * in cases where the underlying collection is actually a vector of unique pointers to `E`.
 *
 * \tparam E the type of the elements of the collection
 * \see VectorOfUniquePtrIteratorAdapter
 */
template<typename E>
struct VectorOfUniquePtrInteratorConverter {
    /**
     * \brief The type of the original iterator.
     */
    using Iterator = typename std::vector<typename std::unique_ptr<E>>::const_iterator;

    /**
     * \brief The type of the elements.
     */
    using ElementType = const E;

    /**
     * \brief Converts the iterator of `std::unique_ptr` to a reference of `E` by dereferencing the pointer.
     * \param it the original iterator
     * \return a reference to the element pointed to by the iterator
     */
    static ElementType &convert(Iterator it) {
        return **it;
    }
};

/**
 * \brief An iterator adapter for dereferencing `std::unique_ptr` in a `std::vector`.
 *
 * This class is useful for classes that need to provide iterator of type `E` where the underlying collection
 * is a vector of unique pointers to `E`. For example:
 * \code
 * class C {
 * public:
 *     using Iterator = VectorOfUniquePtrIteratorAdapter<Item>;
 *     Iterator begin() const {
 *         return Iterator(data.begin());
 *     }
 *     Iterator end() const {
 *         return Iterator(data.end());
 *     }
 * private:
 *     std::vector<std::unique_ptr<Item>> data;
 * };
 *
 * ...
 *
 * C c;
 * for (const Item &item : c) {
 *     ...
 * }
 * \endcode
 * \tparam E the type of the elements of the collection
 */
template<typename E>
using VectorOfUniquePtrIteratorAdapter = IteratorAdapter<VectorOfUniquePtrInteratorConverter<E>>;

/**
 * \brief Converter for dereferencing `std::unique_ptr` in a `std::map`.
 *
 * This class can be used as the `C` template parameter of \ref IteratorAdapter for providing an iterator of `E`
 * in cases where the underlying collection is actually a map of `K` to unique pointers to `E`.
 * \tparam K the type of the key in the map
 * \tparam E the type of the elements of the collection
 * \see MapToUniquePtrIteratorAdapter
 */
template<typename K, typename E>
struct MapToUniquePtrInteratorConverter {
    /**
     * \brief The type of the original iterator.
     */
    using Iterator = typename std::map<K, typename std::unique_ptr<E>>::const_iterator;

    /**
     * \brief The type of the elements.
     */
    using ElementType = const E;

    /**
     * \brief Converts the iterator of `std::pair<K, std::unique_ptr>` to a reference of `E` by dereferencing the
     * pointer in the `second` field of the pair.
     * \param it the original iterator
     * \return a reference to the element pointed to by the iterator
     */
    static ElementType &convert(Iterator it) {
        return *it->second;
    }
};

/**
 * \brief An iterator adapter for dereferencing `std::unique_ptr` in a `std::map`.
 *
 * This class is useful for classes that need to provide iterator of type `E` where the underlying collection
 * is a map of `K` to unique pointers to `E`. For example:
 * \code
 * class C {
 * public:
 *     using Iterator = MapToUniquePtrIteratorAdapter<std::string, Item>;
 *     Iterator begin() const {
 *         return Iterator(data.begin());
 *     }
 *     Iterator end() const {
 *         return Iterator(data.end());
 *     }
 * private:
 *     std::map<std::string, std::unique_ptr<Item>> data;
 * };
 *
 * ...
 *
 * C c;
 * for (const Item &item : c) {
 *     ...
 * }
 * \endcode
 * \tparam K the type of the key in the map
 * \tparam E the type of the elements of the collection
 */
template<typename K, typename E>
using MapToUniquePtrInteratorAdapter = IteratorAdapter<MapToUniquePtrInteratorConverter<K, E>>;

/**
 * \brief Represents a pair of iterators.
 *
 * This class is useful in cases where a class containing several collections need to provide a way for iterating
 * them conveniently. For example:
 * \code
 * class C {
 * public:
 *     IteratorRange<std::vector<int>::const_iterator> getInts() const {
 *         return IteratorRange<std::vector<int>::const_iterator>(ints);
 *     }
 *     IteratorRange<std::vector<std::string>::const_iterator> getStrings() const {
 *         return IteratorRange<std::vector<std::string>::const_iterator>(strings);
 *     }
 * private:
 *     std::vector<int> ints;
 *     std::vector<std::string> strings;
 * };
 *
 * ...
 *
 * C c;
 * for (int i : c.getInts()) {
 *     ...
 * }
 * for (const std::string & s : c.getStrings()) {
 *     ...
 * }
 * \endcode
 * \tparam I the type of the iterator
 */
template<typename I>
class IteratorRange {

public:
    /**
     * \brief Constructs the iterator range.
     * \param b the beginning of the range
     * \param e the end of the range
     */
    IteratorRange(I b, I e) : b(b), e(e) {
    }

    /**
     * \brief Constructs the iterator range for a collection whose iterator is convertible to `I`.
     * \tparam C the type of the collection
     * \param c the collection
     */
    template<typename C>
    explicit IteratorRange(const C &c) : IteratorRange(I(c.begin()), I(c.end())) {
    }

    /**
     * \brief Returns the beginning of the range.
     * \return the beginning of the range
     */
    I begin() const {
        return b;
    }

    /**
     * \brief Returns the end of the range.
     * \return the end of the range
     */
    I end() const {
        return e;
    }

private:
    I b, e;
};

} // namespace util
} // namespace qore

#endif // INCLUDE_QORE_CORE_UTIL_ITERATORS_H_

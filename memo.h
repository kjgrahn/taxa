/* Copyright (c) 2025 Jörgen Grahn
 * All rights reserved.
 */
#ifndef TAXA_MEMO_H
#define TAXA_MEMO_H

#include <unordered_set>

/**
 * Memoization: if you have lots of e.g. strings but few distinct
 * values, it's helpful to keep references to just a few strings
 * instead.
 */
template <class T>
class Memo {
public:
    const T& operator() (const T& val);

private:
    std::unordered_set<T> set;
};

template <class T>
const T& Memo<T>::operator() (const T& val)
{
    auto res = set.insert(val);
    return *res.first;
}

#endif

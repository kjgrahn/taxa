#ifndef TAXA_SEEN_H
#define TAXA_SEEN_H

#include <unordered_set>

namespace dyntaxa {

    /**
     * A set of seen/visited values, and a way to check if a value is
     * present, and to add it at the same time.
     */
    template <class T>
    class Seen {
    public:
	bool operator() (const T&);

    private:
	std::unordered_set<T> set;
    };
}

template <class T>
bool dyntaxa::Seen<T>::operator() (const T& val)
{
    auto res = set.insert(val);
    return !res.second;
}

#endif

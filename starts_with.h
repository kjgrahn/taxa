#ifndef TAXA_STARTS_WITH_H
#define TAXA_STARTS_WITH_H

/**
 * True iff [a, b) is a prefix of [c, d). Don't know why
 * <algorithm> doesn't have this, especially after C++20 threw
 * every "good to have" into the language.
 */
template <class It1, class It2>
    bool starts_with(It1 a, It1 b, It2 c, It2 d)
{
    while (a!=b && c!=d) {
	if (*a != *c) return false;
	a++; c++;
    }
    return a==b;
}

#endif

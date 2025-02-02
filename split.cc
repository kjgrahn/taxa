/* -*- c++ -*-
 *
 * Copyright (c) 2019 J�rgen Grahn
 * All rights reserved.
 */
#include "split.h"

#include <cctype>
#include <algorithm>
#include <iostream>
#include <sstream>

std::vector<std::string> split(const std::string& s, size_t limit)
{
    auto a = s.data();
    auto b = a + s.size();
    return split(a, b, limit);
}

namespace {
    inline bool isspace(char ch)
    {
        return std::isspace(static_cast<unsigned char>(ch));
    }

    /**
     * Trim whitespace to the left in [a, b).
     */
    inline
    const char* ws(const char* a, const char* b)
    {
        while(a!=b && isspace(*a)) a++;
        return a;
    }

    /**
     * Trim non-whitespace to the left in [a, b).
     */
    inline
    const char* non_ws(const char* a, const char* b)
    {
        while(a!=b && !isspace(*a)) a++;
        return a;
    }

    /**
     * Trim whitespace to the right in [a, b), so that
     * it's either empty or ends with non-whitespace.
     */
    inline
    const char* trimr(const char* a, const char* b)
    {
        while(a!=b && isspace(*(b-1))) b--;
        return b;
    }
}

/**
 * Split the string [a, b) on whitespace, kind of like the Perl split
 * function.
 *
 * If limit is non-zero, the string is split into max 'limit' pieces,
 * and the last one may contain whitespace.
 *
 * No piece will ever have leading or trailing whitespace, or be
 * empty.
 */
std::vector<std::string> split(const char* a, const char* b,
			       const size_t limit)
{
    std::vector<std::string> acc;

    a = ws(a, b);
    while (a!=b) {
	if (acc.size() + 1 == limit) {
	    acc.emplace_back(a, trimr(a, b));
	    break;
	}
	auto c = non_ws(a, b);
	acc.emplace_back(a, c);
	a = ws(c, b);
    }

    return acc;
}

/**
 * Split 's' on 'delimiter'.  If the delimiter occurs N times in s,
 * s is going to be split into N+1 possibly empty pieces, except the
 * empty string is split into 0 pieces.
 *
 * This is quite different from splitting on blankspace.
 */
std::vector<std::string> split(const std::string& delimiter,
			       const std::string& s)
{
    auto a = s.data();
    auto b = a + s.size();
    return split(delimiter, a, b);
}

std::vector<std::string> split(const std::string& delimiter,
			       const char* a, const char* b)
{
    std::vector<std::string> acc;

    while (a!=b) {
	auto c = std::search(a, b, begin(delimiter), end(delimiter));
	acc.emplace_back(a, c);
	if (c!=b) {
	    a = c + delimiter.size();
	    if (a==b) acc.emplace_back(a, b);
	}
	else {
	    a = c;
	}
    }

    return acc;
}

std::string join(char sep,
		 const std::vector<std::string>& v)
{
    return join(sep, begin(v), end(v));
}

std::string join(const std::string& sep,
		 const std::vector<std::string>& v)
{
    return join(sep, begin(v), end(v));
}

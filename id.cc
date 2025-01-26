#include "id.h"

#include "starts_with.h"

namespace {

    /**
     * Pop s off the start of [a, b) and update a. Returns false if
     * that doesn't work.
     */
    template <class It>
    bool eat(const std::string& s, It& a, It b)
    {
	if (starts_with(begin(s), end(s), a, b)) {
	    a += s.size();
	    return true;
	}
	return false;
    }
}

using dyntaxa::Id;

Id::Id(const std::string& s)
{
    /* A completely different implementation of number(s) in
     * cleanup.cc. Oh well.
     */
    val = {};
    const char* a = s.c_str();
    const char* b = a + s.size();

    if (!eat("urn:lsid:dyntaxa.se:", a, b)) return;

    if (eat("Taxon:", a, b)) {
	val = {'t', std::strtoul(a, nullptr, 10)};
    }
    else if (eat("TaxonName:", a, b)) {
	val = {'u', std::strtoul(a, nullptr, 10)};
    }
}

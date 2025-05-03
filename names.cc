#include "names.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <unordered_set>

#include "indent.h"

using dyntaxa::Names;

namespace {

    /* Replace all strings 'a' with 'b' in 's', except those created
     * by the changes themselves.
     */
    void replace(std::string& s, const char* a, const char* b)
    {
	size_t p = 0;

	while (1) {
	    auto i = s.find(a, p);
	    if (i==s.npos) return;

	    s.replace(i, std::strlen(a), b);
	    p = i + std::strlen(b);
	}
    }

    std::string normalize(std::string name)
    {
	replace(name, " subsp. ", " ssp. ");
	replace(name, " x ", u8" \u00d7 ");
	return name;
    }
}

std::ostream& Names::put(std::ostream& os,
			 Indent& indent,
			 bool use_synonyms) const
{
    auto is_preferred = [] (auto name) { return name->preferred=="true"; };

    std::unordered_set<std::string> mem;
    auto seen = [&mem] (auto s) {
	return !mem.insert(s).second;
    };

    auto pit = std::find_if(begin(names), end(names), is_preferred);
    if (pit==end(names)) pit = begin(names);

    const auto name = normalize(taxon.name);
    seen(name);

    if (pit==end(names)) {
	indent.ljust(os, "-", 23) << " (" << name << ")\n";
    }
    else {
	const auto pname = normalize((*pit)->name);
	seen(pname);
	indent.ljust(os, pname, 23) << " (" << name << ")\n";
    }

    if (!use_synonyms) return os;

    for (auto it=begin(names); it!=end(names); it++) {
	if (it==pit) continue;
	const auto name = normalize((*it)->name);
	if (seen(name)) continue;
	os << "= " << name << '\n';
    }

    for (auto& syn : synonyms) {
	const auto name = normalize(syn->name);
	if (seen(name)) continue;
	indent.ljust(os, "= ", 25) << name << '\n';
    }
    return os;
}

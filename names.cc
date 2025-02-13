#include "names.h"

#include <iostream>
#include <algorithm>

#include "indent.h"

using dyntaxa::Names;

std::ostream& Names::put(std::ostream& os,
			 Indent& indent) const
{
    auto is_preferred = [] (auto name) { return name->preferred=="true"; };

    auto pit = std::find_if(begin(names), end(names), is_preferred);
    if (pit==end(names)) pit = begin(names);

    if (pit==end(names)) {
	return indent.ljust(os, "-", 23) << " (" << taxon.name << ")\n";
    }

    indent.ljust(os, (*pit)->name, 23) << " (" << taxon.name << ")\n";

    for (auto it=begin(names); it!=end(names); it++) {
	if (it==pit) continue;
	os << "= " << (*it)->name << '\n';
    }
    for (auto& syn : synonyms) {
	indent.ljust(os, "= ", 25) << syn->name << '\n';
    }
    return os;
}

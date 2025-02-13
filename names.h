#ifndef TAXA_NAMES_H
#define TAXA_NAMES_H

#include "taxon.h"
#include "name.h"

#include <vector>
#include <iosfwd>

class Indent;

namespace dyntaxa {

    /**
     * The collected names for a taxon: accepted, vernacular, synonyms
     * and whatnot. Only collected so we can print them in a sensible
     * way.
     */
    struct Names {
	const Taxon taxon;
	const std::vector<const Taxon*> synonyms;
	const std::vector<const Name*> names;

	std::ostream& put(std::ostream& os,
			  Indent& indent) const;
    };
}
#endif

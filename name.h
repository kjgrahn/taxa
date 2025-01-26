#ifndef TAXA_NAME_H
#define TAXA_NAME_H

#include "id.h"
#include <string>

namespace dyntaxa {

    /**
     * taxonId
     * vernacularName
     * language            sv en nb fi da de fo is
     * countryCode         SV GB NO ...
     * source
     * isPreferredName     true false
     * taxonRemarks
     *
     * Only covers taxa, not synonyms, and only half the taxa.
     * Most are marked isPreferredName; none are unmarked.
     *
     * Most, but far from all, names are 'sv'. There are no Sámi
     * names; no doubt shit will hit the fan when activists notice.
     */
    struct Name {
	template <class C>
	explicit Name(const C& c);

	const Id id;
	const std::string name;
	const std::string lang;
	const std::string country;

	const std::string preferred;
    };

    template <class C>
    Name::Name(const C& c)
	: id        {c[0]},
	  name      {c[1]},
	  lang      {c[2]},
	  country   {c[3]},
	  preferred {c[5]}
    {}
}

#endif

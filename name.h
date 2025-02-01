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
	template <class Memo, class C>
	Name(Memo& memo, const C& c);

	const Id id;
	const std::string name;
	const std::string& lang;
	const std::string& country;

	const std::string& preferred;
    };

    template <class Memo, class C>
    Name::Name(Memo& memo, const C& c)
	: id        {c[0]},
	  name      {c[1]},
	  lang      {memo(c[2])},
	  country   {memo(c[3])},
	  preferred {memo(c[5])}
    {}
}

#endif

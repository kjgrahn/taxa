#ifndef TAXA_DISTRIBUTION_H
#define TAXA_DISTRIBUTION_H

#include "id.h"
#include <string>

namespace dyntaxa {

    /**
     * taxonId
     * country              Sweden
     * countryCode          SE
     * establishmentMeans   native - introduced uncertain invasive naturalised
     * occurrenceStatus     present - absent irregular doubtful
     * threatStatus
     * dynamicProperties
     *
     * occurrenceStatus:
     *
     *	 63837 present
     *	 28158
     *	  9793 absent
     *	  4996 irregular
     *	  2045 doubtful
     *
     * Seems to exist for all taxa and always says SE, but the info
     * isn't always filled in which makes it a bit weak. You can
     * filter out those tagged "absent" but cannot guarantee that you
     * only get taxa which are present. And I suppose "irregular"
     * might mean "seen once in 1908".
     */
    struct Distribution {
	template <class Memo, class C>
	Distribution(Memo& memo, const C& c);

	const Id id;
	const std::string& country;
	const std::string& cc;
	const std::string& means;
	const std::string& status;
    };

    template <class Memo, class C>
    Distribution::Distribution(Memo& memo, const C& c)
	: id       {c[0]},
	  country  {memo(c[1])},
	  cc       {memo(c[2])},
	  means    {memo(c[3])},
	  status   {memo(c[4])}
    {}
}

#endif

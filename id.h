#ifndef TAXA_ID_H
#define TAXA_ID_H

#include <string>
#include <utility>

namespace dyntaxa {

    /**
     * Identifiers in Dyntaxa are URNs on two forms:
     *
     *   urn:lsid:dyntaxa.se:Taxon:1000901
     *   urn:lsid:dyntaxa.se:TaxonName:785508
     *
     * Where TaxonName signifies a synonym.
     *
     * Repeated string comparisons take time and strings take space,
     * so let's keep a compact representation.
     *
     */
    class Id {
    public:
	explicit Id(const std::string&);
	bool operator== (const Id& other) const { return val==other.val; }
	bool operator<  (const Id& other) const { return val < other.val; }
	explicit operator bool () const { return val.first; }

    private:
	std::pair<char, unsigned> val;
    };
}

#endif

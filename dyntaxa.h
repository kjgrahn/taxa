#ifndef TAXA_DYNTAXA_H
#define TAXA_DYNTAXA_H

#include "taxon.h"
#include "name.h"
#include "distribution.h"

#include <vector>
#include <map>
#include <iosfwd>

#include "memo.h"
#include "seen.h"

class Indent;

namespace dyntaxa {

    class Names;

    /**
     * The data set, or enough of it so it can be traversed to produce
     * flat taxon lists.
     *
     * The public methods are const; the mutable state is broken out
     * into Seen<Id> in order to keep the rest const.
     */
    class Dyntaxa {
    public:
	Dyntaxa(std::istream& taxa,
		std::istream& names,
		std::istream& dist,
		bool use_synonyms,
		bool wash_aggregates,
		bool list_genera);

	void list(std::ostream&, Seen<Id>&, const std::string& taxon) const;
	void flat(std::ostream&, Seen<Id>&, const std::string& taxon) const;

    private:
	const Taxon* find_taxon(const std::string& name) const;
	std::vector<const Taxon*> children(const Taxon& tx) const;
	Names names_for(const Taxon& tx) const;

	void list(std::ostream&, Seen<Id>&, Indent&, const Taxon& taxon) const;

	const bool use_synonyms;
	const bool wash_aggregates;
	const bool list_genera;
	Memo<std::string> memo;

	struct {
	    std::vector<Taxon> taxa;
	    std::vector<Name>  names;
	} db;

	struct {
	    std::map<Id, std::vector<const Taxon*>> children;
	    std::map<Id, std::vector<const Taxon*>> synonyms;
	    std::map<Id, std::vector<const Name*>> names;
	} map;
    };
}

#endif

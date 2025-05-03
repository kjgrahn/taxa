#ifndef TAXA_TAXON_H
#define TAXA_TAXON_H

#include "id.h"
#include <string>

namespace dyntaxa {

    /**
     * An entry in Taxon.csv.
     *
     * taxonId                    190414
     * acceptedNameUsageID        108789
     * parentNameUsageID           29722 never a TaxonName
     * scientificName             181347
     * taxonRank
     * scientificNameAuthorship
     * taxonomicStatus            accepted | synonym | a few others ...
     * nomenclaturalStatus        valid | orthographia | auctor | nudum ...
     * taxonRemarks
     * kingdom
     * phylum
     * class
     * order
     * family
     * genus
     * species
     *
     * id and accepted are only these cases:
     * Taxon:N     Taxon:N  taxon is accepted (tstatus also says so)
     * TaxonName:M Taxon:N  name is a synonym
     *
     * taxonRank includes these, amongst others:
     *
     *	 144687 species	       1685 speciesAggregate
     *	  23195 genus		344 form
     *	   5727 family		133 cultivar
     *	   4754 subspecies	 93 section (botany)
     *	   2982 variety
     *
     * taxonomicStatus is:
     *
     *	 108829 accepted	     2108 misapplied
     *	  61693 synonym		      428 proParteSynonym
     *	   9465 heterotypicSynonym
     *	   8025 homotypicSynonym
     *
     * nomenclaturalStatus:
     *
     *	 181267 valid		      156 rejiciendum
     *	   5079 orthographia	       12 novum
     *	   3201 invalidum		5 superfluum
     *	    550 nudum			2 conservandum
     *	    276 illegitimum
     *
     */
    struct Taxon {
	template <class Memo, class C>
	Taxon(Memo& memo, const C& c);

	const Id id;
	const Id accepted;
	const Id parent;

	const std::string name;
	const std::string& rank;

	const std::string auctor;
	const std::string& tstatus;
	const std::string& nstatus;
    };

    template <class Memo, class C>
    Taxon::Taxon(Memo& memo, const C& c)
	: id       {c[0]},
	  accepted {c[1]},
	  parent   {c[2]},
	  name     {c[3]},
	  rank     {memo(c[4])},
	  auctor   {c[5]},
	  tstatus  {memo(c[6])},
	  nstatus  {memo(c[7])}
    {}

    inline bool accepted(const Taxon& tx)
    {
	return tx.id == tx.accepted;
    }

    inline bool boring(const Taxon& tx)
    {
	return tx.rank=="cultivar";
    }
}

#endif

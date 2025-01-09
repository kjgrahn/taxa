#include <string>

namespace dyntaxa {

    using Id = std::string;

    /**
     * taxonId                    190414
     * acceptedNameUsageID        108789
     * parentNameUsageID           29722
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
     * taxonRank includes these, amongst others:
     *
     *   144687 species        1685 speciesAggregate
     *    23195 genus	        344 form
     *     5727 family	        133 cultivar
     *     4754 subspecies       93 section (botany)
     *     2982 variety
     *
     */
    struct Taxon {
	template <class C>
	explicit Taxon(const C& c);

	const Id id;
	const Id accepted;
	const Id parent;

	const std::string name;
	const std::string rank;

	const std::string auctor;
	const std::string tstatus;
	const std::string nstatus;
    };

    template <class C>
    Taxon::Taxon(const C& c)
	: id       {c[0]},
	  accepted {c[1]},
	  parent   {c[2]},
	  name     {c[3]},
	  rank     {c[4]},
	  auctor   {c[5]},
	  tstatus  {c[6]},
	  nstatus  {c[7]}
    {}
}

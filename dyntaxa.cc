#include "dyntaxa.h"

#include "split.h"

#include <iostream>
#include <algorithm>

using dyntaxa::Dyntaxa;
using dyntaxa::Taxon;

Dyntaxa::Dyntaxa(std::istream& taxa,
		 std::istream& names,
		 std::istream& dist)
{
    std::string s;

    while (getline(taxa, s)) {
	const auto v = split("\t", s);
	db.taxa.emplace_back(v);
    }

    while (getline(names, s)) {
	const auto v = split("\t", s);
	Name val {v};
    }

    while (getline(dist, s)) {
	const auto v = split("\t", s);
	Distribution val {v};
    }

    for (const Taxon& tx : db.taxa) {
	if (accepted(tx) && tx.parent) {
	    map.children[tx.parent].push_back(&tx);
	}
    }
}

/**
 * A taxon with scientificName 'name', or null.
 */
const Taxon* Dyntaxa::find_taxon(const std::string& name) const
{
    auto it = std::find_if(db.taxa.begin(), db.taxa.end(),
			   [&] (auto& tx) { return tx.name==name; });
    if (it==db.taxa.end()) return {};
    return &*it;
}

/**
 * All taxa with parentNameUsageID == tx.id.
 * Or all important taxa, maybe.
 */
std::vector<const Taxon*> Dyntaxa::children(const Taxon& tx) const
{
    auto it = map.children.find(tx.id);
    if (it==end(map.children)) return {};
    return it->second;
}

using dyntaxa::Names;

/**
 * The collected names for a taxon: accepted, vernacular, synonyms and
 * whatnot. Only collected so we can print them in a sensible way.
 */
struct Names {
    std::string name;

    std::ostream& put(std::ostream& os) const;
};

std::ostream& Names::put(std::ostream& os) const
{
    return os << "-     (" << name << ")\n";
}

Names Dyntaxa::names_for(const Taxon& tx) const
{
    Names nn;
    nn.name = tx.name;
    return nn;
}

/**
 * Render a taxon list for everything important under taxon (usually a
 * family). Recursively.
 *
 *   # Boraginaceae
 *   -			 (Adelocaryum coelestinum)
 *   storblommig gullört (Amsinckia eastwoodae)
 *   ogräsgullört	 (Amsinckia intermedia)
 *   mångullört		 (Amsinckia lunaris)
 *   hönsgullört	 (Amsinckia lycopsoides)
 *   gullört		 (Amsinckia micrantha)
 *   fodergullört	 (Amsinckia retrorsa)
 *   mosaikgullört	 (Amsinckia tesselata)
 *   fårtunga		 (Anchusa arvensis)
 *   ...
 *
 */
void Dyntaxa::list(std::ostream& os, const std::string& taxon) const
{
    os << "# " << taxon << '\n';
    const Taxon* tx = find_taxon(taxon);
    if (!tx) return;
    list(os, *tx);
    os << '\n';
}

void Dyntaxa::list(std::ostream& os, const Taxon& tx) const
{
    for (const Taxon* tx : children(tx)) {

	names_for(*tx).put(os);
	list(os, *tx);
    }
}

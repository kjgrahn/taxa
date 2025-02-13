#include "dyntaxa.h"

#include "split.h"
#include "names.h"
#include "indent.h"

#include <iostream>
#include <algorithm>
#include <set>

using dyntaxa::Dyntaxa;
using dyntaxa::Taxon;

Dyntaxa::Dyntaxa(std::istream& taxa,
		 std::istream& names,
		 std::istream& dist)
{
    std::string s;

    /* The only use we have for SpeciesDistribution is weeding out
     * (pun unintended) taxa absent from Sweden.
     */
    std::set<Id> absentees;
    while (getline(dist, s)) {
	const auto v = split("\t", s);
	Distribution val {memo, v};
	if (absent(val)) {
	    absentees.insert(val.id);
	}
    }

    while (getline(taxa, s)) {
	const auto v = split("\t", s);
	db.taxa.emplace_back(memo, v);

	// If adding the taxon was premature, remove it again
	auto& tx = db.taxa.back();
	if (absentees.count(tx.id) || boring(tx)) {
	    db.taxa.pop_back();
	}
    }

    while (getline(names, s)) {
	const auto v = split("\t", s);
	db.names.emplace_back(memo, v);
    }

    for (const Taxon& tx : db.taxa) {
	if (accepted(tx) && tx.parent) {
	    map.children[tx.parent].push_back(&tx);
	}

	if (!accepted(tx)) {
	    map.synonyms[tx.accepted].push_back(&tx);
	}
    }

    for (const Name& name : db.names) {
	if (name.lang=="sv") {
	    map.names[name.id].push_back(&name);
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

dyntaxa::Names Dyntaxa::names_for(const Taxon& tx) const
{
    auto nit = map.names.find(tx.id);
    auto sit = map.synonyms.find(tx.id);

    std::vector<const Name*> names;
    if (nit!=end(map.names)) names = nit->second;
    std::vector<const Taxon*> synonyms;
    if (sit!=end(map.synonyms)) synonyms = sit->second;

    return {tx, synonyms, names};
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

    Indent indent;
    list(os, indent, *tx);
    os << '\n';
}

void Dyntaxa::list(std::ostream& os, Indent& indent, const Taxon& tx) const
{
    auto taxa = children(tx);
    std::sort(begin(taxa), end(taxa),
	      [] (auto a, auto b) { return a->name < b->name; });

    for (const Taxon* tx : taxa) {

	names_for(*tx).put(os, indent);
	list(os, indent, *tx);
    }
}

#include "dyntaxa.h"

#include "split.h"
#include "names.h"
#include "indent.h"

#include <iostream>
#include <algorithm>
#include <unordered_set>

using dyntaxa::Dyntaxa;
using dyntaxa::Taxon;
using dyntaxa::Name;
using dyntaxa::Seen;
using dyntaxa::Id;

namespace {

    /* A data structure for temporary Id->Taxon lookup.
     * The input vector must not change during use.
     */
    class ById {
    public:
	explicit ById(const std::vector<Taxon>& v);
	const Taxon* find(Id) const;

    private:
	const std::vector<Taxon>& v;
	std::map<Id, unsigned> map;
    };

    ById::ById(const std::vector<Taxon>& v)
	: v {v}
    {
	unsigned n = 0;
	for (auto& tx : v) {
	    map[tx.id] = n++;
	}
    }

    const Taxon* ById::find(const Id id) const
    {
	auto it = map.find(id);
	if (it==map.end()) return {};
	unsigned n = it->second;
	return &v[n];
    }

    /* True if the name occurs more than once in a set of Names.
     * If 'preferred', only look at preferred names.
     */
    template <class V>
    bool duplicated(const V& v, const Name& name, bool use_synonyms)
    {
	auto n = std::count_if(begin(v), end(v), [&] (const Name& other) {
	    if (use_synonyms) return other.name==name.name;
	    return preferred(other) && other.name==name.name;
	});
	return n > 1;
    }
}

Dyntaxa::Dyntaxa(std::istream& taxa,
		 std::istream& names,
		 std::istream& dist,
		 bool use_synonyms,
		 bool wash_aggregates,
		 bool list_genera)
    : use_synonyms {use_synonyms},
      wash_aggregates {wash_aggregates},
      list_genera {list_genera}
{
    std::string s;

    /* The only use we have for SpeciesDistribution is weeding out
     * (pun unintended) taxa absent from Sweden.
     */
    std::unordered_set<Id> absentees;
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

    const ById by_id {db.taxa};

    while (getline(names, s)) {
	const auto v = split("\t", s);
	db.names.emplace_back(memo, v);

	// Possibly tweak the name, or remove it again
	auto& name = db.names.back();

	if (name.lang!="sv") {
	    db.names.pop_back();
	    continue;
	}

	const auto* const tx = by_id.find(name.id);
	if (!tx) {
	    db.names.pop_back();
	    continue;
	}

	if (wash_aggregates && aggregate(*tx)) name.unaggregate();
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

	if (wash_aggregates) {
	    const auto* const tx = by_id.find(name.id);
	    if (aggregate(*tx) && duplicated(db.names, name, use_synonyms)) continue;
	}

	map.names[name.id].push_back(&name);
    }
}

/**
 * A taxon with scientificName 'name', or null.
 * Or, rather, the first accepted taxon, if several match.
 */
const Taxon* Dyntaxa::find_taxon(const std::string& name) const
{
    auto p = [&name] (auto& tx) {
	return tx.name==name && accepted(tx);
    };

    auto it = std::find_if(db.taxa.begin(), db.taxa.end(), p);
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
 * family) except things seen already. Do this recursively.
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
void Dyntaxa::list(std::ostream& os, Seen<dyntaxa::Id>& seen,
		   const std::string& taxon) const
{
    os << "# " << taxon << '\n';
    const Taxon* tx = find_taxon(taxon);
    if (!tx) return;

    Indent indent;
    seen(tx->id);
    list(os, seen, indent, *tx);
    os << '\n';
}

void Dyntaxa::list(std::ostream& os, Seen<dyntaxa::Id>& seen, Indent& indent,
		   const Taxon& tx) const
{
    auto taxa = children(tx);
    std::sort(begin(taxa), end(taxa),
	      [] (auto a, auto b) { return a->name < b->name; });

    for (const Taxon* tx : taxa) {

	if (seen(tx->id)) continue;

	if (list_genera || !genus(*tx)) {
	    names_for(*tx).put(os, indent, use_synonyms);
	}
	list(os, seen, indent, *tx);
    }
}

/**
 * Render a non-recursive taxon list for everything important directly
 * under taxon.
 */
void Dyntaxa::flat(std::ostream& os, Seen<dyntaxa::Id>& seen,
		   const std::string& taxon) const
{
    os << "# " << taxon << '\n';
    const Taxon* tx = find_taxon(taxon);
    if (!tx) return;

    auto taxa = children(*tx);
    std::sort(begin(taxa), end(taxa),
	      [] (auto a, auto b) { return a->name < b->name; });

    Indent indent;

    for (const Taxon* tx : taxa) {

	if (seen(tx->id)) continue;

	if (list_genera || !genus(*tx)) {
	    names_for(*tx).put(os, indent, use_synonyms);
	}
    }

    os << '\n';
}

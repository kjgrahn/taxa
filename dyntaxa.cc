#include "dyntaxa.h"

#include "split.h"
#include "indent.h"

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
	db.names.emplace_back(v);
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

using dyntaxa::Names;

/**
 * The collected names for a taxon: accepted, vernacular, synonyms and
 * whatnot. Only collected so we can print them in a sensible way.
 */
struct Names {
    const Taxon taxon;
    const std::vector<const Taxon*> synonyms;
    const std::vector<const Name*> names;

    std::ostream& put(std::ostream& os,
		      Indent& indent) const;
};

std::ostream& Names::put(std::ostream& os,
			 Indent& indent) const
{
    auto is_preferred = [] (auto name) { return name->preferred=="true"; };

    auto pit = std::find_if(begin(names), end(names), is_preferred);
    if (pit==end(names)) pit = begin(names);

    if (pit==end(names)) {
	return indent.ljust(os, "-", 23) << " (" << taxon.name << ")\n";
    }

    indent.ljust(os, (*pit)->name, 23) << " (" << taxon.name << ")\n";

    for (auto it=begin(names); it!=end(names); it++) {
	if (it==pit) continue;
	os << "= " << (*it)->name << '\n';
    }
    return os;
}

Names Dyntaxa::names_for(const Taxon& tx) const
{
    auto nit = map.names.find(tx.id);
    if (nit!=end(map.names)) return {tx, {}, nit->second};
    return {tx, {}, {}};
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

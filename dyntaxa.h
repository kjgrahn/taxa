#ifndef TAXA_DB_H
#define TAXA_DB_H

#include "taxon.h"
#include "name.h"
#include "distribution.h"

#include <vector>
#include <map>
#include <iosfwd>

#include "memo.h"

class Indent;

namespace dyntaxa {

    class Names;

    class Dyntaxa {
    public:
	Dyntaxa(std::istream& taxa,
		std::istream& names,
		std::istream& dist);

	Dyntaxa(const Dyntaxa&) = delete;
	Dyntaxa& operator= (Dyntaxa&&) = default;
	Dyntaxa& operator= (const Dyntaxa&) = delete;

	void list(std::ostream& os, const std::string& taxon) const;

    private:
	const Taxon* find_taxon(const std::string& name) const;
	std::vector<const Taxon*> children(const Taxon& tx) const;
	Names names_for(const Taxon& tx) const;

	void list(std::ostream& os, Indent& indent, const Taxon& taxon) const;

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

#ifndef TAXA_DB_H
#define TAXA_DB_H

#include "taxon.h"
#include "name.h"
#include "distribution.h"

#include <vector>
#include <iosfwd>

namespace dyntaxa {

    class Names;

    class Dyntaxa {
    public:
	Dyntaxa(std::istream& taxa,
		std::istream& names,
		std::istream& dist);

	Dyntaxa& operator= (Dyntaxa&&) = default;
	Dyntaxa& operator= (Dyntaxa&) = delete;

	void list(std::ostream& os, const std::string& taxon) const;
	void list(std::ostream& os, const Taxon& taxon) const;

    private:
	const Taxon* find_taxon(const std::string& name) const;
	std::vector<const Taxon*> children(const Taxon& tx) const;
	Names names_for(const Taxon& tx) const;

	struct {
	    std::vector<Taxon> taxa;
	} db;
    };
}

#endif

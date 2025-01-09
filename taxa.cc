#include <iostream>
#include <string>

#include "taxon.h"
#include "split.h"

int main()
{
    std::string s;
    while (getline(std::cin, s)) {
	const auto v = split("\t", s);
	dyntaxa::Taxon tx {v};

	std::cout << tx.name << '\n';
    }
}

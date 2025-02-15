#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>

#include <getopt.h>

#include "dyntaxa.h"
#include "version.h"

namespace {

    class FatalError : public std::exception {};

    template <class Arg>
    unsigned count_arg(const Arg& arg)
    {
	unsigned n = 0;
	if (arg.taxa.size()) n++;
	if (arg.names.size()) n++;
	if (arg.dist.size()) n++;
	return n;
    }

    template <class Arg>
    dyntaxa::Dyntaxa mktaxa(std::ostream& err, const Arg& arg,
			    bool synonyms)
    {
	auto complain = [&] (const std::string& name) {
	    err << "error: cannot open '" << name << "' for reading: "
		<< std::strerror(errno) << '\n';
	};

	std::ifstream taxa {arg.taxa};
	std::ifstream names {arg.names};
	std::ifstream dist {arg.dist};

	if (!taxa)  complain(arg.taxa);
	if (!names) complain(arg.names);
	if (!dist)  complain(arg.dist);

	if (!(taxa && names && dist)) throw FatalError {};

	return {taxa, names, dist, synonyms};
    }
}

int main(int argc, char** argv)
{
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    const std::string prog = argv[0] ? argv[0] : "taxa";
    const std::string usage = "usage: "
	+ prog +
	" [-n0] src-dir taxon ...\n"
	"       "
	+ prog + " [-n0] --taxa file --names file --dist file taxon ...\n" +
	"       "
	+ prog + " --help\n" +
	"       "
	+ prog + " --version";
    const char optstring[] = "n0";
    const struct option long_options[] = {
	{"taxa",	 1, 0, 'X'},
	{"names",	 1, 0, 'N'},
	{"dist",	 1, 0, 'D'},
	{"help",	 0, 0, 'h'},
	{"version",	 0, 0, 'v'},
	{0, 0, 0, 0}
    };

    struct {
	struct {
	    std::string taxa;
	    std::string names;
	    std::string dist;
	} file;
	bool synonyms = true;
	bool flat = false;
    } arg;

    int ch;
    while ((ch = getopt_long(argc, argv,
			     optstring, long_options, 0)) != -1) {
	switch(ch) {
	case 'n':
	    arg.synonyms = false;
	    break;
	case '0':
	    arg.flat = true;
	    break;
	case 'X':
	    arg.file.taxa = optarg;
	    break;
	case 'N':
	    arg.file.names = optarg;
	    break;
	case 'D':
	    arg.file.dist = optarg;
	    break;
	case 'h':
	    std::cout << usage << '\n';
	    return 0;
	case 'v':
	    std::cout << prog << ", part of taxa " << version() << '\n'
		      << "Copyright (c) 2025 Jörgen Grahn\n";
	    return 0;
	    break;
	case ':':
	case '?':
	    std::cerr << usage << '\n';
	    return 1;
	    break;
	default:
	    break;
	}
    }

    switch (count_arg(arg.file)) {
    case 3:
	break;
    case 0:
	if (optind!=argc) {
	    std::string dir = argv[optind++];
	    arg.file.taxa  = dir + "/Taxon.csv";
	    arg.file.names = dir + "/VernacularName.csv";
	    arg.file.dist  = dir + "/SpeciesDistribution.csv";
	    break;
	}
	[[fallthrough]];
    default:
	std::cerr << "error: required argument missing\n"
		  << usage << '\n';
	return 1;
    }

    const std::vector<const char*> args {argv + optind, argv + argc };

    try {
	dyntaxa::Dyntaxa dt = mktaxa(std::cerr, arg.file, arg.synonyms);

	for (auto tx : args) {
	    if (arg.flat) {
		dt.flat(std::cout, tx);
	    }
	    else {
		dt.list(std::cout, tx);
	    }
	}

	return 0;
    }
    catch (const FatalError&) {
	return 1;
    }
}

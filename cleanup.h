#ifndef TAXA_CLEANUP_H
#define TAXA_CLEANUP_H

#include <string>
#include <iosfwd>

bool cleanup(std::ostream& err, const std::string& name,
	     const char* a, const char* b);

#endif

#include "cleanup.h"

#include "starts_with.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

    bool ends_with(const std::string& needle, const std::string& haystack)
    {
	if (haystack.size() < needle.size()) return false;
	return haystack.rfind(needle) == haystack.size() - needle.size();
    }

    template <class It>
    It trim_bom(It a, It b)
    {
	constexpr char bom[] = u8"\ufeff";
	size_t len = std::strlen(bom);
	if (starts_with(bom, bom + len, a, b)) a += len;
	return a;
    }

    /* True if [a, b) starts with the string p (but not the \0).
     * And, 'n' matches [0-9] rather than itself.
     * This is a cheap alternative to regexes in a very limited
     * situation.
     */
    template <class It>
    bool match(const char* p, It a, It b)
    {
	auto isdigit = [&] (unsigned n) { return std::isdigit(n); };
	auto mismatch = [&] (char a, char p) {
	    if (p=='n') return !isdigit(a);
	    return a != p;
	};

	while (a!=b && *p) {
	    if (mismatch(*a, *p)) return false;
	    a++; p++;
	}
	return !*p;
    }

    /* The iterator points to "mm/dd/yyyy";
     * change to "yyyy-mm-dd" (which luckily is the same length).
     */
    template <class It>
    void to_iso(It a)
    {
	char buf[10];
	std::copy_n(a, 10, buf);
	std::copy_n(buf+6, 4, a); a+=4;
	*a++ = '-';
	std::copy_n(buf+0, 2, a); a+=2;
	*a++ = '-';
	std::copy_n(buf+3, 2, a); a+=2;
    }

    /**
     * Replace all
     *   \tmm/dd/yyyy
     * with
     *   \tyyyy-mm-dd
     *
     * This is really intended to fix one thing: that in February 2025,
     * the Reference.csv "date" column suddenly changed to a format using
     * a legacy US date notation.
     *
     * Doesn't take all possible Darwin Core encodings into account.
     * They accept different encodings, and support ranges, supplying
     * only the year or year-and-month, and so on. Dyntaxa doesn't use
     * that freedom.
     */
    template <class It>
    void iso_date(It a, It b)
    {
	while (true) {
	    a = std::find(a, b, '\t');
	    if (a==b) return;

	    a++;
	    if (match("nn/nn/nnnn", a, b)) {
		to_iso(a);
	    }
	}
    }

    /**
     * Return the order of the first line of 's'. It's based on the
     * first decimal number on that line, and whether it's
     * urn:lsid:dyntaxa.se:Taxon:1234 or urn:lsid:dyntaxa.se:
     * TaxonName:1234 (so the ordering of these is stable). If there
     * is no number at all, return 0.
     */
    unsigned long number(const char* const s)
    {
	auto stopper = [] (char c) {
	    switch (c) {
	    case '\0':
	    case '\n':
	    case '0': case '5':
	    case '1': case '6':
	    case '2': case '7':
	    case '3': case '8':
	    case '4': case '9':
		return true;
	    default:
		return false;
	    }
	};

	auto p = s;
	while (!stopper(*p)) p++;
	if (*p=='\n') return 0;

	const unsigned long n = std::strtoul(p, nullptr, 10);

	const std::string intro {s, p};
	if (intro=="urn:lsid:dyntaxa.se:Taxon:") {
	    return n*2;
	}
	else if (intro=="urn:lsid:dyntaxa.se:TaxonName:") {
	    return n*2 + 1;
	}

	return n;
    }

    /**
     * The lines in a text file, and the first number on each line, or
     * zero, so we can sort them in a sensible way.
     */
    template <class It>
    auto lines_of(It a, It b)
    {
	std::vector<std::pair<unsigned, It>> v;
	v.emplace_back(0, a);
	It c = a;
	while (c!=b) {
	    c = std::find(c, b, '\n');
	    if (c!=b) c++;
	    if (c!=b) v.emplace_back(0, c);
	}

	for (auto& e : v) e.first = number(e.second);

	return v;
    }

    /**
     * Write 's' until \0, \n or \r\n, then write \n.
     */
    void writeln(std::ostream& os, const char* a)
    {
	auto printable = [] (char c) { return c!='\0' && c!='\n'; };
	auto b = a;
	while (printable(*b)) b++;
	if (b!=a && b[-1]=='\r') b--;
	os.write(a, b-a).put('\n');
    }

    namespace csv {

	template <class It>
	void write(std::ostream& os, It a, It b)
	{
	    a = trim_bom(a, b);

	    auto lines = lines_of(a, b);
	    std::stable_sort(begin(lines), end(lines));
	    for (const auto& e : lines) {
		writeln(os, e.second);
	    }
	}
    }

    namespace ref {

	template <class It>
	void write(std::ostream& os, It a, It b)
	{
	    a = trim_bom(a, b);
	    iso_date(a, b);

	    auto lines = lines_of(a, b);
	    std::stable_sort(begin(lines), end(lines));
	    for (const auto& e : lines) {
		writeln(os, e.second);
	    }
	}
    }

    namespace xml {
	template <class It>
	void write(std::ostream& os, It a, It b)
	{
	    a = trim_bom(a, b);

	    auto lines = lines_of(a, b);
	    for (const auto& e : lines) {
		writeln(os, e.second);
	    }
	}
    }
}

/**
 * Clean up file content
 *   MS-DOS line endings are removed
 *   the dreaded UTF-8 BOM is removed
 *   CSV files are sorted on Taxon/TaxonName number
 *
 * and write to file 'name'. Returns success, may write error messages
 * to 'err'.
 */
bool cleanup(std::ostream& err, const std::string& name,
	     char* a, char* b)
{
    auto whine = [&] (const char* complaint) {
	err << complaint << ' ' << name << ": "
	    << std::strerror(errno) << '\n';
    };

    std::ofstream f {name};
    if (!f) {
	whine("error opening");
	return false;
    }

    if (ends_with("Reference.csv", name)) {

	ref::write(f, a, b);
    }
    else if (ends_with(".csv", name)) {

	csv::write(f, a, b);
    }
    else {
	xml::write(f, a, b);
    }

    f.close();
    if (!f) {
	whine("error writing");
	return false;
    }

    return true;
}

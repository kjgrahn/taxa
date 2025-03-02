#include <iostream>
#include <vector>
#include <cstring>

#include <getopt.h>

#include <archive.h>
#include <archive_entry.h>

#include "version.h"
#include "cleanup.h"

namespace {

    /**
     * For use with archive_read_open() and a std::istream.
     */
    struct IStreamSource {
	explicit IStreamSource(std::istream& is) : is{is} {}
	int open(struct archive*) { return 0; }
	la_ssize_t read(struct archive* a, const void **buffer);
	int close(struct archive*) { return 0; }
    private:
	std::istream& is;
	char buf[8192];
    };

    la_ssize_t IStreamSource::read(struct archive*, const void **buffer)
    {
	is.read(buf, sizeof buf);
	*buffer = buf;
	return is.gcount();
    }

    IStreamSource& iss(void *client_data)
    {
	auto p = static_cast<IStreamSource*>(client_data);
	return *p;
    }

    int open_callback(struct archive* a, void *client_data)
    {
	return iss(client_data).open(a);
    }

    la_ssize_t read_callback(struct archive* a, void *client_data, const void **buffer)
    {
	return iss(client_data).read(a, buffer);
    }

    int close_callback(struct archive* a, void *client_data)
    {
	return iss(client_data).close(a);
    }

    /**
     * The current file, read into a string.
     */
    std::string read_file(struct archive* a)
    {
	std::string s;

	const void* buf;
	size_t len;
	off_t offset;
	while (archive_read_data_block(a, &buf, &len, &offset)==0) {
	    s.resize(offset);
	    s.append(static_cast<const char*>(buf), len);
	}
	return s;
    }

    int unpack(std::istream& is)
    {
	archive* a = archive_read_new();
	archive_read_support_format_zip(a);

	IStreamSource iss {is};
	archive_read_open(a, &iss,
			  open_callback,
			  read_callback,
			  close_callback);

	struct archive_entry* e;

	while (archive_read_next_header(a, &e) == ARCHIVE_OK) {

	    const std::string name = archive_entry_pathname(e);
	    std::string content = read_file(a);
	    char* p = content.data();

	    if (!cleanup(std::cerr, name,
			 p, p + content.size())) return 1;
	}

	return 0;
    }
}

int main(int argc, char** argv)
{
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    const std::string prog = argv[0] ? argv[0] : "unpack";
    const std::string usage = "usage: "
	+ prog +
	" dir < archive\n"
	"       "
	+ prog + " --help\n" +
	"       "
	+ prog + " --version";
    const char optstring[] = "";
    const struct option long_options[] = {
	{"help",	 0, 0, 'h'},
	{"version",	 0, 0, 'v'},
	{0, 0, 0, 0}
    };

    int ch;
    while ((ch = getopt_long(argc, argv,
			     optstring, long_options, 0)) != -1) {
	switch(ch) {
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

    const std::vector<const char*> args {argv + optind, argv + argc };

    if (args.size() != 1) {
	std::cerr << "error: too many/few arguments\n"
		  << usage << '\n';
	return 1;
    }

    int err = chdir(args[0]);
    if (err) {
	std::cerr << "error: cannot chdir: " << std::strerror(errno) << '\n';
	return 1;
    }

    return unpack(std::cin);
}

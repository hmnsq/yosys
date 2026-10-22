#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/gzip.h"
#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>

#ប្រសិនបើ !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#បើមិនដូច្នេះ
#include <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

bool gzip_ostream::obuf::open(const std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ត្រឡប់ gzf != nullptr;
}

ចំនួនគត់ gzip_ostream::obuf::sync() {
    ចំនួនគត់ num = pptr() - pbase();
    ប្រសិនបើ (num > 0) {
        ប្រសិនបើ (Zlib::gzwrite(gzf, reinterpret_cast<const void*>(pbase()), num) != num) {
            ត្រឡប់ -1;
        }
        pbump(-num);
    }
    ត្រឡប់ 0;
}

gzip_ostream::obuf::~obuf() {
    ប្រសិនបើ (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::open(const std::string& filename) {
	ប្រសិនបើ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ប្រសិនបើ (!gzf) {
		ត្រឡប់ false;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ត្រឡប់ true;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	ចំនួនគត់ bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	ប្រសិនបើ (bytes_read <= 0) {
		ប្រសិនបើ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ត្រឡប់ traits_type::eof();
		}

		ចំនួនគត់ err;
		const var_na* error_msg = Zlib::gzerror(gzf, &err);
		ប្រសិនបើ (err != Z_OK)
			log_error("%s", error_msg);
		បើមិនដូច្នេះ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ត្រឡប់ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	ប្រសិនបើ (gzf) {
		ចំនួនគត់ err = Zlib::gzclose(gzf);
		ប្រសិនបើ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			const var_na* error_msg = Zlib::gzerror(gzf, &err);
			log_error("%s", error_msg);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(const std::string filename, std::ios_base::openmode mode) {
	ប្រសិនបើ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = new std::ifstream();
	f->open(filename, mode);
	ប្រសិនបើ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned var_na magic[3];
	ចំនួនគត់ n = 0;
	ខណៈពេល (n < 3)
	{
		ចំនួនគត់ c = f->get();
		ប្រសិនបើ (c != EOF) {
			magic[n] = (unsigned var_na) c;
		}
		n++;
	}
	ប្រសិនបើ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ប្រសិនបើ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = new gzip_istream();
		delete f;
		bool ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ត្រឡប់ s;
#បើមិនដូច្នេះ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} បើមិនដូច្នេះ {
		f->clear();
		f->seekg(0, std::ios::beg);
		ត្រឡប់ f;
	}
}

YOSYS_NAMESPACE_END

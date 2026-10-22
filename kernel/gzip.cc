#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/gzip.h"
#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>

#만약 !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#아니면
#include <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

bool gzip_ostream::obuf::open(const std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    반환 gzf != nullptr;
}

정수 gzip_ostream::obuf::sync() {
    정수 num = pptr() - pbase();
    만약 (num > 0) {
        만약 (Zlib::gzwrite(gzf, reinterpret_cast<const void*>(pbase()), num) != num) {
            반환 -1;
        }
        pbump(-num);
    }
    반환 0;
}

gzip_ostream::obuf::~obuf() {
    만약 (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::open(const std::string& filename) {
	만약 (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	만약 (!gzf) {
		반환 false;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	반환 true;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	정수 bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	만약 (bytes_read <= 0) {
		만약 (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			반환 traits_type::eof();
		}

		정수 err;
		const char* error_msg = Zlib::gzerror(gzf, &err);
		만약 (err != Z_OK)
			log_error("%s", error_msg);
		아니면
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	반환 traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	만약 (gzf) {
		정수 err = Zlib::gzclose(gzf);
		만약 (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			const char* error_msg = Zlib::gzerror(gzf, &err);
			log_error("%s", error_msg);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(const std::string filename, std::ios_base::openmode mode) {
	만약 (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = new std::ifstream();
	f->open(filename, mode);
	만약 (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned char magic[3];
	정수 n = 0;
	동안 (n < 3)
	{
		정수 c = f->get();
		만약 (c != EOF) {
			magic[n] = (unsigned char) c;
		}
		n++;
	}
	만약 (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		만약 (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = new gzip_istream();
		delete f;
		bool ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		반환 s;
#아니면
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} 아니면 {
		f->clear();
		f->seekg(0, std::ios::beg);
		반환 f;
	}
}

YOSYS_NAMESPACE_END

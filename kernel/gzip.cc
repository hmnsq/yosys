#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/gzip.h"
#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>

#如果 !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#否则
#include <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

bool gzip_ostream::obuf::open(const std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    返回 gzf != nullptr;
}

整数 gzip_ostream::obuf::sync() {
    整数 num = pptr() - pbase();
    如果 (num > 0) {
        如果 (Zlib::gzwrite(gzf, reinterpret_cast<const void*>(pbase()), num) != num) {
            返回 -1;
        }
        pbump(-num);
    }
    返回 0;
}

gzip_ostream::obuf::~obuf() {
    如果 (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::open(const std::string& filename) {
	如果 (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	如果 (!gzf) {
		返回 false;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	返回 true;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	整数 bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	如果 (bytes_read <= 0) {
		如果 (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			返回 traits_type::eof();
		}

		整数 err;
		const char* error_msg = Zlib::gzerror(gzf, &err);
		如果 (err != Z_OK)
			log_error("%s", error_msg);
		否则
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	返回 traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	如果 (gzf) {
		整数 err = Zlib::gzclose(gzf);
		如果 (err != Z_OK) {
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
	如果 (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = new std::ifstream();
	f->open(filename, mode);
	如果 (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned char magic[3];
	整数 n = 0;
	当 (n < 3)
	{
		整数 c = f->get();
		如果 (c != EOF) {
			magic[n] = (unsigned char) c;
		}
		n++;
	}
	如果 (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		如果 (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = new gzip_istream();
		delete f;
		bool ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		返回 s;
#否则
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} 否则 {
		f->clear();
		f->seekg(0, std::ios::beg);
		返回 f;
	}
}

YOSYS_NAMESPACE_END

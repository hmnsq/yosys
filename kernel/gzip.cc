#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/gzip.h"
#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>

#အကယ်၍ !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#မဟုတ်ပါက
#include <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

bool gzip_ostream::obuf::open(const std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ပြန်ပေး gzf != nullptr;
}

ကိန်းပြည့် gzip_ostream::obuf::sync() {
    ကိန်းပြည့် num = pptr() - pbase();
    အကယ်၍ (num > 0) {
        အကယ်၍ (Zlib::gzwrite(gzf, reinterpret_cast<const void*>(pbase()), num) != num) {
            ပြန်ပေး -1;
        }
        pbump(-num);
    }
    ပြန်ပေး 0;
}

gzip_ostream::obuf::~obuf() {
    အကယ်၍ (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::open(const std::string& filename) {
	အကယ်၍ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	အကယ်၍ (!gzf) {
		ပြန်ပေး false;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ပြန်ပေး true;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	ကိန်းပြည့် bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	အကယ်၍ (bytes_read <= 0) {
		အကယ်၍ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ပြန်ပေး traits_type::eof();
		}

		ကိန်းပြည့် err;
		const var_na* error_msg = Zlib::gzerror(gzf, &err);
		အကယ်၍ (err != Z_OK)
			log_error("%s", error_msg);
		မဟုတ်ပါက
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ပြန်ပေး traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	အကယ်၍ (gzf) {
		ကိန်းပြည့် err = Zlib::gzclose(gzf);
		အကယ်၍ (err != Z_OK) {
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
	အကယ်၍ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = new std::ifstream();
	f->open(filename, mode);
	အကယ်၍ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned var_na magic[3];
	ကိန်းပြည့် n = 0;
	နေစဉ် (n < 3)
	{
		ကိန်းပြည့် c = f->get();
		အကယ်၍ (c != EOF) {
			magic[n] = (unsigned var_na) c;
		}
		n++;
	}
	အကယ်၍ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		အကယ်၍ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = new gzip_istream();
		delete f;
		bool ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ပြန်ပေး s;
#မဟုတ်ပါက
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} မဟုတ်ပါက {
		f->clear();
		f->seekg(0, std::ios::beg);
		ပြန်ပေး f;
	}
}

YOSYS_NAMESPACE_END

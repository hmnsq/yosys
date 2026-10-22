#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/gzip.h"
#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>

#ပေႃး !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#ပေႃးဢမ်ႇ
#include <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

bool gzip_ostream::obuf::open(const std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ပွၵ်ႈ gzf != nullptr;
}

puur_naa_nka gzip_ostream::obuf::sync() {
    puur_naa_nka num = pptr() - pbase();
    ပေႃး (num > 0) {
        ပေႃး (Zlib::gzwrite(gzf, reinterpret_cast<const void*>(pbase()), num) != num) {
            ပွၵ်ႈ -1;
        }
        pbump(-num);
    }
    ပွၵ်ႈ 0;
}

gzip_ostream::obuf::~obuf() {
    ပေႃး (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::open(const std::string& filename) {
	ပေႃး (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ပေႃး (!gzf) {
		ပွၵ်ႈ false;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ပွၵ်ႈ true;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	puur_naa_nka bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	ပေႃး (bytes_read <= 0) {
		ပေႃး (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ပွၵ်ႈ traits_type::eof();
		}

		puur_naa_nka err;
		const var_na* error_msg = Zlib::gzerror(gzf, &err);
		ပေႃး (err != Z_OK)
			log_error("%s", error_msg);
		ပေႃးဢမ်ႇ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ပွၵ်ႈ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	ပေႃး (gzf) {
		puur_naa_nka err = Zlib::gzclose(gzf);
		ပေႃး (err != Z_OK) {
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
	ပေႃး (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = new std::ifstream();
	f->open(filename, mode);
	ပေႃး (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned var_na magic[3];
	puur_naa_nka n = 0;
	ၶၢဝ်း (n < 3)
	{
		puur_naa_nka c = f->get();
		ပေႃး (c != EOF) {
			magic[n] = (unsigned var_na) c;
		}
		n++;
	}
	ပေႃး (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ပေႃး (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = new gzip_istream();
		delete f;
		bool ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ပွၵ်ႈ s;
#ပေႃးဢမ်ႇ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} ပေႃးဢမ်ႇ {
		f->clear();
		f->seekg(0, std::ios::beg);
		ပွၵ်ႈ f;
	}
}

YOSYS_NAMESPACE_END

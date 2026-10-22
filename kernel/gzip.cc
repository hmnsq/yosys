#include "kernel/yosys_common.h"
#include "kernel/log.h"
#include "kernel/gzip.h"
#include <iostream>
#include <string>
#include <cstdarg>
#include <cstdio>

#නම් !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#නැතහොත්
#include <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

bool gzip_ostream::obuf::open(const std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ආපසු gzf != nullptr;
}

පූර්ණ_සංඛ්‍යා gzip_ostream::obuf::sync() {
    පූර්ණ_සංඛ්‍යා num = pptr() - pbase();
    නම් (num > 0) {
        නම් (Zlib::gzwrite(gzf, reinterpret_cast<const void*>(pbase()), num) != num) {
            ආපසු -1;
        }
        pbump(-num);
    }
    ආපසු 0;
}

gzip_ostream::obuf::~obuf() {
    නම් (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::open(const std::string& filename) {
	නම් (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	නම් (!gzf) {
		ආපසු false;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ආපසු true;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	පූර්ණ_සංඛ්‍යා bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	නම් (bytes_read <= 0) {
		නම් (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ආපසු traits_type::eof();
		}

		පූර්ණ_සංඛ්‍යා err;
		const var_na* error_msg = Zlib::gzerror(gzf, &err);
		නම් (err != Z_OK)
			log_error("%s", error_msg);
		නැතහොත්
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ආපසු traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	නම් (gzf) {
		පූර්ණ_සංඛ්‍යා err = Zlib::gzclose(gzf);
		නම් (err != Z_OK) {
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
	නම් (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = new std::ifstream();
	f->open(filename, mode);
	නම් (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned var_na magic[3];
	පූර්ණ_සංඛ්‍යා n = 0;
	අතරතුර (n < 3)
	{
		පූර්ණ_සංඛ්‍යා c = f->get();
		නම් (c != EOF) {
			magic[n] = (unsigned var_na) c;
		}
		n++;
	}
	නම් (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		නම් (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = new gzip_istream();
		delete f;
		bool ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ආපසු s;
#නැතහොත්
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} නැතහොත් {
		f->clear();
		f->seekg(0, std::ios::beg);
		ආපසු f;
	}
}

YOSYS_NAMESPACE_END

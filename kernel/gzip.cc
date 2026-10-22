#অন্তর্ভাৱ "kernel/yosys_common.h"
#অন্তর্ভাৱ "kernel/log.h"
#অন্তর্ভাৱ "kernel/gzip.h"
#অন্তর্ভাৱ <iostream>
#অন্তর্ভাৱ <string>
#অন্তর্ভাৱ <cstdarg>
#অন্তর্ভাৱ <cstdio>

#যদি !defined(WIN32)
#অন্তর্ভাৱ <dirent.h>
#অন্তর্ভাৱ <unistd.h>
#অন্যথা
#অন্তর্ভাৱ <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

তার্কিক gzip_ostream::obuf::open(ধ্রুৱ std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ঘূৰাই gzf != nullptr;
}

পূর্ণাঙ্ক gzip_ostream::obuf::সমকাল() {
    পূর্ণাঙ্ক অঙ্ক = pptr() - pbase();
    যদি (অঙ্ক > 0) {
        যদি (Zlib::gzwrite(gzf, reinterpret_cast<ধ্রুৱ শূন্য*>(pbase()), অঙ্ক) != অঙ্ক) {
            ঘূৰাই -1;
        }
        pbump(-অঙ্ক);
    }
    ঘূৰাই 0;
}

gzip_ostream::obuf::~obuf() {
    যদি (gzf) {
        সমকাল();
        Zlib::gzclose(gzf);
    }
}

তার্কিক gzip_istream::ibuf::open(ধ্রুৱ std::string& filename) {
	যদি (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	যদি (!gzf) {
		ঘূৰাই মিছা;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ঘূৰাই সঁচা;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	পূর্ণাঙ্ক bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	যদি (bytes_read <= 0) {
		যদি (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ঘূৰাই traits_type::eof();
		}

		পূর্ণাঙ্ক err;
		ধ্রুৱ ৱর্ণ* error_বাৰ্তা = Zlib::gzerror(gzf, &err);
		যদি (err != Z_OK)
			log_error("%s", error_বাৰ্তা);
		অন্যথা
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ঘূৰাই traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	যদি (gzf) {
		পূর্ণাঙ্ক err = Zlib::gzclose(gzf);
		যদি (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ধ্রুৱ ৱর্ণ* error_বাৰ্তা = Zlib::gzerror(gzf, &err);
			log_error("%s", error_বাৰ্তা);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ধ্রুৱ std::string filename, std::ios_base::openmode mode) {
	যদি (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = নূতন std::ifstream();
	f->open(filename, mode);
	যদি (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned ৱর্ণ magic[3];
	পূর্ণাঙ্ক n = 0;
	যেতিয়ালৈ (n < 3)
	{
		পূর্ণাঙ্ক c = f->get();
		যদি (c != EOF) {
			magic[n] = (unsigned ৱর্ণ) c;
		}
		n++;
	}
	যদি (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		যদি (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = নূতন gzip_istream();
		delete f;
		তার্কিক ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ঘূৰাই s;
#অন্যথা
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} অন্যথা {
		f->clear();
		f->seekg(0, std::ios::beg);
		ঘূৰাই f;
	}
}

YOSYS_NAMESPACE_END

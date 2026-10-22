#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/log.h"
#ଅନ୍ତର୍ଭାଵ "kernel/gzip.h"
#ଅନ୍ତର୍ଭାଵ <iostream>
#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <cstdarg>
#ଅନ୍ତର୍ଭାଵ <cstdio>

#ଯଦି !defined(WIN32)
#ଅନ୍ତର୍ଭାଵ <dirent.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

ତାର୍କିକ gzip_ostream::obuf::open(ଅଚଳ std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ଫେରନ୍ତୁ gzf != nullptr;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gzip_ostream::obuf::ସମକାଳ() {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ = pptr() - pbase();
    ଯଦି (ଅଙ୍କ > 0) {
        ଯଦି (Zlib::gzwrite(gzf, reinterpret_cast<ଅଚଳ ଶୂନ୍ୟ*>(pbase()), ଅଙ୍କ) != ଅଙ୍କ) {
            ଫେରନ୍ତୁ -1;
        }
        pbump(-ଅଙ୍କ);
    }
    ଫେରନ୍ତୁ 0;
}

gzip_ostream::obuf::~obuf() {
    ଯଦି (gzf) {
        ସମକାଳ();
        Zlib::gzclose(gzf);
    }
}

ତାର୍କିକ gzip_istream::ibuf::open(ଅଚଳ std::string& filename) {
	ଯଦି (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ଯଦି (!gzf) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	ଯଦି (bytes_read <= 0) {
		ଯଦି (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ଫେରନ୍ତୁ traits_type::eof();
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
		ଅଚଳ ଅକ୍ଷର* error_ସନ୍ଦେଶ = Zlib::gzerror(gzf, &err);
		ଯଦି (err != Z_OK)
			log_error("%s", error_ସନ୍ଦେଶ);
		ଅନ୍ୟଥା
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ଫେରନ୍ତୁ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	ଯଦି (gzf) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = Zlib::gzclose(gzf);
		ଯଦି (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ଅଚଳ ଅକ୍ଷର* error_ସନ୍ଦେଶ = Zlib::gzerror(gzf, &err);
			log_error("%s", error_ସନ୍ଦେଶ);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ଅଚଳ std::string filename, std::ios_base::openmode mode) {
	ଯଦି (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = ନୂତନ std::ifstream();
	f->open(filename, mode);
	ଯଦି (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned ଅକ୍ଷର magic[3];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;
	ଯେତେବେଳେ (n < 3)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = f->get();
		ଯଦି (c != EOF) {
			magic[n] = (unsigned ଅକ୍ଷର) c;
		}
		n++;
	}
	ଯଦି (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ଯଦି (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = ନୂତନ gzip_istream();
		delete f;
		ତାର୍କିକ ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ଫେରନ୍ତୁ s;
#ଅନ୍ୟଥା
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} ଅନ୍ୟଥା {
		f->clear();
		f->seekg(0, std::ios::beg);
		ଫେରନ୍ତୁ f;
	}
}

YOSYS_NAMESPACE_END

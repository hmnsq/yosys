#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/log.h"
#அந்தர்பாவ "kernel/gzip.h"
#அந்தர்பாவ <iostream>
#அந்தர்பாவ <string>
#அந்தர்பாவ <cstdarg>
#அந்தர்பாவ <cstdio>

#எனில் !defined(WIN32)
#அந்தர்பாவ <dirent.h>
#அந்தர்பாவ <unistd.h>
#இல்லையெனில்
#அந்தர்பாவ <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

தருக்க gzip_ostream::obuf::open(மாறிலி std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    திரும்பு gzf != nullptr;
}

முழு gzip_ostream::obuf::ஒத்திசை() {
    முழு இலக்கம் = pptr() - pbase();
    எனில் (இலக்கம் > 0) {
        எனில் (Zlib::gzwrite(gzf, reinterpret_cast<மாறிலி வெறுமை*>(pbase()), இலக்கம்) != இலக்கம்) {
            திரும்பு -1;
        }
        pbump(-இலக்கம்);
    }
    திரும்பு 0;
}

gzip_ostream::obuf::~obuf() {
    எனில் (gzf) {
        ஒத்திசை();
        Zlib::gzclose(gzf);
    }
}

தருக்க gzip_istream::ibuf::open(மாறிலி std::string& filename) {
	எனில் (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	எனில் (!gzf) {
		திரும்பு பொய்;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	திரும்பு உண்மை;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	முழு bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	எனில் (bytes_read <= 0) {
		எனில் (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			திரும்பு traits_type::eof();
		}

		முழு err;
		மாறிலி எழுத்து* error_செய்தி = Zlib::gzerror(gzf, &err);
		எனில் (err != Z_OK)
			log_error("%s", error_செய்தி);
		இல்லையெனில்
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	திரும்பு traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	எனில் (gzf) {
		முழு err = Zlib::gzclose(gzf);
		எனில் (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			மாறிலி எழுத்து* error_செய்தி = Zlib::gzerror(gzf, &err);
			log_error("%s", error_செய்தி);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(மாறிலி std::string filename, std::ios_base::openmode mode) {
	எனில் (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = புதிய std::ifstream();
	f->open(filename, mode);
	எனில் (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned எழுத்து magic[3];
	முழு n = 0;
	போது (n < 3)
	{
		முழு c = f->get();
		எனில் (c != EOF) {
			magic[n] = (unsigned எழுத்து) c;
		}
		n++;
	}
	எனில் (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		எனில் (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = புதிய gzip_istream();
		delete f;
		தருக்க ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		திரும்பு s;
#இல்லையெனில்
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} இல்லையெனில் {
		f->clear();
		f->seekg(0, std::ios::beg);
		திரும்பு f;
	}
}

YOSYS_NAMESPACE_END

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/gzip.h"
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <cstdarg>
#ಅನ್ತರ್ಭಾವ <cstdio>

#ಯದಿ !defined(WIN32)
#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

ತಾರ್ಕಿಕ gzip_ostream::obuf::open(ಧ್ರುವ std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ಪ್ರತ್ಯಾಗಮ gzf != nullptr;
}

ಪೂರ್ಣಾಙ್ಕ gzip_ostream::obuf::ಸಮಕಾಲ() {
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ = pptr() - pbase();
    ಯದಿ (ಅಂಕೆ > 0) {
        ಯದಿ (Zlib::gzwrite(gzf, reinterpret_cast<ಧ್ರುವ ಶೂನ್ಯ*>(pbase()), ಅಂಕೆ) != ಅಂಕೆ) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        pbump(-ಅಂಕೆ);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

gzip_ostream::obuf::~obuf() {
    ಯದಿ (gzf) {
        ಸಮಕಾಲ();
        Zlib::gzclose(gzf);
    }
}

ತಾರ್ಕಿಕ gzip_istream::ibuf::open(ಧ್ರುವ std::string& filename) {
	ಯದಿ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ಯದಿ (!gzf) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	ಪೂರ್ಣಾಙ್ಕ bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	ಯದಿ (bytes_read <= 0) {
		ಯದಿ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ಪ್ರತ್ಯಾಗಮ traits_type::eof();
		}

		ಪೂರ್ಣಾಙ್ಕ err;
		ಧ್ರುವ ವರ್ಣ* error_ಸಂದೇಶ = Zlib::gzerror(gzf, &err);
		ಯದಿ (err != Z_OK)
			log_error("%s", error_ಸಂದೇಶ);
		ಅನ್ಯಥಾ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ಪ್ರತ್ಯಾಗಮ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	ಯದಿ (gzf) {
		ಪೂರ್ಣಾಙ್ಕ err = Zlib::gzclose(gzf);
		ಯದಿ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ಧ್ರುವ ವರ್ಣ* error_ಸಂದೇಶ = Zlib::gzerror(gzf, &err);
			log_error("%s", error_ಸಂದೇಶ);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ಧ್ರುವ std::string filename, std::ios_base::openmode mode) {
	ಯದಿ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = ನೂತನ std::ifstream();
	f->open(filename, mode);
	ಯದಿ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned ವರ್ಣ magic[3];
	ಪೂರ್ಣಾಙ್ಕ n = 0;
	ಯಾವತ್ (n < 3)
	{
		ಪೂರ್ಣಾಙ್ಕ c = f->get();
		ಯದಿ (c != EOF) {
			magic[n] = (unsigned ವರ್ಣ) c;
		}
		n++;
	}
	ಯದಿ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ಯದಿ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = ನೂತನ gzip_istream();
		delete f;
		ತಾರ್ಕಿಕ ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ಪ್ರತ್ಯಾಗಮ s;
#ಅನ್ಯಥಾ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} ಅನ್ಯಥಾ {
		f->clear();
		f->seekg(0, std::ios::beg);
		ಪ್ರತ್ಯಾಗಮ f;
	}
}

YOSYS_NAMESPACE_END

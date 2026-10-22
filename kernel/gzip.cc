#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/gzip.h"
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <cstdarg>
#ಅನ್ತರ್ಭಾವ <cstdio>

#ಆದರೆ !defined(WIN32)
#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <io.h>
#ಆದರೆ_ಅಂತ್ಯ

YOSYS_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(ಬಫರ್, ಬಫರ್ + ಬಫರ್_ಗಾತ್ರ - 1);
}

ತಾರ್ಕಿಕ gzip_ostream::obuf::open(ಸ್ಥಿರಾಂಕ std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ಹಿಂತಿರುಗಿ gzf != ಶೂನ್ಯಸೂಚಕ;
}

ಪೂರ್ಣಾಂಕ gzip_ostream::obuf::ಸಮಕಾಲೀಕರಣ() {
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ = pptr() - pbase();
    ಆದರೆ (ಅಂಕೆ > 0) {
        ಆದರೆ (Zlib::gzwrite(gzf, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*>(pbase()), ಅಂಕೆ) != ಅಂಕೆ) {
            ಹಿಂತಿರುಗಿ -1;
        }
        pbump(-ಅಂಕೆ);
    }
    ಹಿಂತಿರುಗಿ 0;
}

gzip_ostream::obuf::~obuf() {
    ಆದರೆ (gzf) {
        ಸಮಕಾಲೀಕರಣ();
        Zlib::gzclose(gzf);
    }
}

ತಾರ್ಕಿಕ gzip_istream::ibuf::open(ಸ್ಥಿರಾಂಕ std::string& filename) {
	ಆದರೆ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ಆದರೆ (!gzf) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	// Empty and point to start
	setg(ಬಫರ್, ಬಫರ್, ಬಫರ್);
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು_read = Zlib::gzread(gzf, ಬಫರ್, ಬಫರ್_ಗಾತ್ರ);
	ಆದರೆ (ಬೈಟುಗಳು_read <= 0) {
		ಆದರೆ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ಹಿಂತಿರುಗಿ traits_type::eof();
		}

		ಪೂರ್ಣಾಂಕ err;
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* error_ಸಂದೇಶ = Zlib::gzerror(gzf, &err);
		ಆದರೆ (err != Z_OK)
			log_error("%s", error_ಸಂದೇಶ);
		ಇಲ್ಲದಿದ್ದರೆ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(ಬಫರ್, ಬಫರ್, ಬಫರ್ + ಬೈಟುಗಳು_read);
	ಹಿಂತಿರುಗಿ traits_type::ಗಮ್ಯ_int_type(ಬಫರ್[0]);
}

gzip_istream::ibuf::~ibuf() {
	ಆದರೆ (gzf) {
		ಪೂರ್ಣಾಂಕ err = Zlib::gzclose(gzf);
		ಆದರೆ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* error_ಸಂದೇಶ = Zlib::gzerror(gzf, &err);
			log_error("%s", error_ಸಂದೇಶ);
		}
	}
}

#ಆದರೆ_ಅಂತ್ಯ // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ಸ್ಥಿರಾಂಕ std::string filename, std::ios_ತಳ::openmode ಬಹುಲಕ) {
	ಆದರೆ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = ಹೊಸ std::ifstream();
	f->open(filename, ಬಹುಲಕ);
	ಆದರೆ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ magic[3];
	ಪೂರ್ಣಾಂಕ n = 0;
	ಆಗ (n < 3)
	{
		ಪೂರ್ಣಾಂಕ c = f->get();
		ಆದರೆ (c != EOF) {
			magic[n] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c;
		}
		n++;
	}
	ಆದರೆ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ZLIB
		ಲಘುಗಣಕ("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ಆದರೆ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), ಚಿಹ್ನೆರಹಿತ(magic[2]));
		gzip_istream* s = ಹೊಸ gzip_istream();
		ಅಳಿಸು f;
		ತಾರ್ಕಿಕ ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ಹಿಂತಿರುಗಿ s;
#ಇಲ್ಲದಿದ್ದರೆ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#ಆದರೆ_ಅಂತ್ಯ // YOSYS_ENABLE_ZLIB
	} ಇಲ್ಲದಿದ್ದರೆ {
		f->clear();
		f->seekg(0, std::ios::beg);
		ಹಿಂತಿರುಗಿ f;
	}
}

YOSYS_NAMESPACE_ಕೊನೆ

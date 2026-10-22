#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/log.h"
#അന്തര്ഭാവ "kernel/gzip.h"
#അന്തര്ഭാവ <iostream>
#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <cstdarg>
#അന്തര്ഭാവ <cstdio>

#എങ്കിൽ !defined(WIN32)
#അന്തര്ഭാവ <dirent.h>
#അന്തര്ഭാവ <unistd.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

തർക്കം gzip_ostream::obuf::open(സ്ഥിരം std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    തിരികെ gzf != nullptr;
}

പൂർണ്ണസംഖ്യ gzip_ostream::obuf::സമന്വയം() {
    പൂർണ്ണസംഖ്യ അക്കം = pptr() - pbase();
    എങ്കിൽ (അക്കം > 0) {
        എങ്കിൽ (Zlib::gzwrite(gzf, reinterpret_cast<സ്ഥിരം ശൂന്യം*>(pbase()), അക്കം) != അക്കം) {
            തിരികെ -1;
        }
        pbump(-അക്കം);
    }
    തിരികെ 0;
}

gzip_ostream::obuf::~obuf() {
    എങ്കിൽ (gzf) {
        സമന്വയം();
        Zlib::gzclose(gzf);
    }
}

തർക്കം gzip_istream::ibuf::open(സ്ഥിരം std::string& filename) {
	എങ്കിൽ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	എങ്കിൽ (!gzf) {
		തിരികെ അസത്യം;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	തിരികെ സത്യം;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	പൂർണ്ണസംഖ്യ bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	എങ്കിൽ (bytes_read <= 0) {
		എങ്കിൽ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			തിരികെ traits_type::eof();
		}

		പൂർണ്ണസംഖ്യ err;
		സ്ഥിരം അക്ഷരം* error_സന്ദേശം = Zlib::gzerror(gzf, &err);
		എങ്കിൽ (err != Z_OK)
			log_error("%s", error_സന്ദേശം);
		അല്ലെങ്കിൽ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	തിരികെ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	എങ്കിൽ (gzf) {
		പൂർണ്ണസംഖ്യ err = Zlib::gzclose(gzf);
		എങ്കിൽ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			സ്ഥിരം അക്ഷരം* error_സന്ദേശം = Zlib::gzerror(gzf, &err);
			log_error("%s", error_സന്ദേശം);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(സ്ഥിരം std::string filename, std::ios_base::openmode mode) {
	എങ്കിൽ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = പുതിയ std::ifstream();
	f->open(filename, mode);
	എങ്കിൽ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned അക്ഷരം magic[3];
	പൂർണ്ണസംഖ്യ n = 0;
	ആകുമ്പോൾ (n < 3)
	{
		പൂർണ്ണസംഖ്യ c = f->get();
		എങ്കിൽ (c != EOF) {
			magic[n] = (unsigned അക്ഷരം) c;
		}
		n++;
	}
	എങ്കിൽ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		എങ്കിൽ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = പുതിയ gzip_istream();
		delete f;
		തർക്കം ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		തിരികെ s;
#അല്ലെങ്കിൽ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} അല്ലെങ്കിൽ {
		f->clear();
		f->seekg(0, std::ios::beg);
		തിരികെ f;
	}
}

YOSYS_NAMESPACE_END

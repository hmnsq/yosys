#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/gzip.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <cstdarg>
#अन्तर्भाव <cstdio>

#सचे !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अञ्ञथा
#अन्तर्भाव <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

तार्किक gzip_ostream::obuf::open(ध्रुव std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    पटिनिवत्त gzf != nullptr;
}

पूर्णाङ्क gzip_ostream::obuf::समकाल() {
    पूर्णाङ्क अङ्कः = pptr() - pbase();
    सचे (अङ्कः > 0) {
        सचे (Zlib::gzwrite(gzf, reinterpret_cast<ध्रुव शून्य*>(pbase()), अङ्कः) != अङ्कः) {
            पटिनिवत्त -1;
        }
        pbump(-अङ्कः);
    }
    पटिनिवत्त 0;
}

gzip_ostream::obuf::~obuf() {
    सचे (gzf) {
        समकाल();
        Zlib::gzclose(gzf);
    }
}

तार्किक gzip_istream::ibuf::open(ध्रुव std::string& filename) {
	सचे (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	सचे (!gzf) {
		पटिनिवत्त मिच्छा;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	पटिनिवत्त सच्च;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	पूर्णाङ्क bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	सचे (bytes_read <= 0) {
		सचे (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			पटिनिवत्त traits_type::eof();
		}

		पूर्णाङ्क err;
		ध्रुव वर्ण* error_सन्देशः = Zlib::gzerror(gzf, &err);
		सचे (err != Z_OK)
			log_error("%s", error_सन्देशः);
		अञ्ञथा
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	पटिनिवत्त traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	सचे (gzf) {
		पूर्णाङ्क err = Zlib::gzclose(gzf);
		सचे (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ध्रुव वर्ण* error_सन्देशः = Zlib::gzerror(gzf, &err);
			log_error("%s", error_सन्देशः);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ध्रुव std::string filename, std::ios_base::openmode mode) {
	सचे (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = नूतन std::ifstream();
	f->open(filename, mode);
	सचे (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned वर्ण magic[3];
	पूर्णाङ्क n = 0;
	याव (n < 3)
	{
		पूर्णाङ्क c = f->get();
		सचे (c != EOF) {
			magic[n] = (unsigned वर्ण) c;
		}
		n++;
	}
	सचे (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		सचे (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = नूतन gzip_istream();
		delete f;
		तार्किक ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		पटिनिवत्त s;
#अञ्ञथा
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} अञ्ञथा {
		f->clear();
		f->seekg(0, std::ios::beg);
		पटिनिवत्त f;
	}
}

YOSYS_NAMESPACE_END

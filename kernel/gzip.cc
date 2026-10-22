#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/gzip.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <cstdarg>
#अन्तर्भाव <cstdio>

#जर !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

तार्किक gzip_ostream::obuf::खोलो(स्थिरांक std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    परत gzf != nullptr;
}

पूर्णांक gzip_ostream::obuf::समकालिक() {
    पूर्णांक अंक = pptr() - pbase();
    जर (अंक > 0) {
        जर (Zlib::gzwrite(gzf, reinterpret_cast<स्थिरांक शून्य*>(pbase()), अंक) != अंक) {
            परत -1;
        }
        pbump(-अंक);
    }
    परत 0;
}

gzip_ostream::obuf::~obuf() {
    जर (gzf) {
        समकालिक();
        Zlib::gzclose(gzf);
    }
}

तार्किक gzip_istream::ibuf::खोलो(स्थिरांक std::string& filename) {
	जर (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	जर (!gzf) {
		परत असत्य;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	परत सत्य;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	पूर्णांक bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	जर (bytes_read <= 0) {
		जर (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			परत traits_type::eof();
		}

		पूर्णांक err;
		स्थिरांक अक्षर* error_संदेश = Zlib::gzerror(gzf, &err);
		जर (err != Z_OK)
			log_error("%s", error_संदेश);
		अन्यथा
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	परत traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	जर (gzf) {
		पूर्णांक err = Zlib::gzclose(gzf);
		जर (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			स्थिरांक अक्षर* error_संदेश = Zlib::gzerror(gzf, &err);
			log_error("%s", error_संदेश);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(स्थिरांक std::string filename, std::ios_base::openmode mode) {
	जर (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = नवीन std::ifstream();
	f->खोलो(filename, mode);
	जर (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, म_त्रुटि(त्रुटि_सं));
	// Check for gzip magic
	अचिन्हित अक्षर magic[3];
	पूर्णांक n = 0;
	पर्यंत (n < 3)
	{
		पूर्णांक c = f->get();
		जर (c != खातापूर्ण) {
			magic[n] = (अचिन्हित अक्षर) c;
		}
		n++;
	}
	जर (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		जर (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), अचिन्हित(magic[2]));
		gzip_istream* s = नवीन gzip_istream();
		delete f;
		तार्किक ok = s->खोलो(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		परत s;
#अन्यथा
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} अन्यथा {
		f->clear();
		f->seekg(0, std::ios::beg);
		परत f;
	}
}

YOSYS_NAMESPACE_END

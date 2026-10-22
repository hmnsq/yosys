#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/gzip.h"
#अन्तर्भाव <iostream>
#अन्तर्भाव <string>
#अन्तर्भाव <cstdarg>
#अन्तर्भाव <cstdio>

#यदि !defined(WIN32)
#अन्तर्भाव <dirent.h>
#अन्तर्भाव <unistd.h>
#अन्यथा
#अन्तर्भाव <io.h>
#यदि_अन्तः

YOSYS_NAMESPACE_BEGIN

#यदि_परिभाषितम् YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(बफर, बफर + बफर_आकार - 1);
}

तार्किक gzip_ostream::obuf::उद्घाटय(ध्रुव std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    प्रत्यागम gzf != शून्यसूचकः;
}

पूर्णाङ्क gzip_ostream::obuf::समकाल() {
    पूर्णाङ्क अङ्कः = pptr() - pbase();
    यदि (अङ्कः > 0) {
        यदि (Zlib::gzwrite(gzf, reinterpret_cast<ध्रुव शून्य*>(pbase()), अङ्कः) != अङ्कः) {
            प्रत्यागम -1;
        }
        pbump(-अङ्कः);
    }
    प्रत्यागम 0;
}

gzip_ostream::obuf::~obuf() {
    यदि (gzf) {
        समकाल();
        Zlib::gzclose(gzf);
    }
}

तार्किक gzip_istream::ibuf::उद्घाटय(ध्रुव std::string& filename) {
	यदि (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	यदि (!gzf) {
		प्रत्यागम असत्य;
	}
	// Empty and point to start
	setg(बफर, बफर, बफर);
	प्रत्यागम सत्य;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	पूर्णाङ्क अष्टकानि_read = Zlib::gzread(gzf, बफर, बफर_आकार);
	यदि (अष्टकानि_read <= 0) {
		यदि (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			प्रत्यागम traits_type::eof();
		}

		पूर्णाङ्क err;
		ध्रुव वर्ण* error_सन्देशः = Zlib::gzerror(gzf, &err);
		यदि (err != Z_OK)
			log_error("%s", error_सन्देशः);
		अन्यथा
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(बफर, बफर, बफर + अष्टकानि_read);
	प्रत्यागम traits_type::गन्तव्यम्_int_type(बफर[0]);
}

gzip_istream::ibuf::~ibuf() {
	यदि (gzf) {
		पूर्णाङ्क err = Zlib::gzclose(gzf);
		यदि (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ध्रुव वर्ण* error_सन्देशः = Zlib::gzerror(gzf, &err);
			log_error("%s", error_सन्देशः);
		}
	}
}

#यदि_अन्तः // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ध्रुव std::string filename, std::ios_आधारः::openmode बहुलकम्) {
	यदि (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = नूतन std::ifstream();
	f->उद्घाटय(filename, बहुलकम्);
	यदि (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	अचिह्नित वर्ण magic[3];
	पूर्णाङ्क n = 0;
	यावत् (n < 3)
	{
		पूर्णाङ्क c = f->get();
		यदि (c != EOF) {
			magic[n] = (अचिह्नित वर्ण) c;
		}
		n++;
	}
	यदि (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#यदि_परिभाषितम् YOSYS_ENABLE_ZLIB
		लघुगणकः("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		यदि (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), अचिह्नित(magic[2]));
		gzip_istream* s = नूतन gzip_istream();
		विलोपय f;
		तार्किक ok = s->उद्घाटय(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		प्रत्यागम s;
#अन्यथा
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#यदि_अन्तः // YOSYS_ENABLE_ZLIB
	} अन्यथा {
		f->clear();
		f->seekg(0, std::ios::beg);
		प्रत्यागम f;
	}
}

YOSYS_NAMESPACE_अन्तः

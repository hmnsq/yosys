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
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

तार्किक gzip_ostream::obuf::खोलो(अचर std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    फर्काऊ gzf != nullptr;
}

पूर्णाङ्क gzip_ostream::obuf::समकाल() {
    पूर्णाङ्क अङ्क = pptr() - pbase();
    यदि (अङ्क > 0) {
        यदि (Zlib::gzwrite(gzf, reinterpret_cast<अचर शून्य*>(pbase()), अङ्क) != अङ्क) {
            फर्काऊ -1;
        }
        pbump(-अङ्क);
    }
    फर्काऊ 0;
}

gzip_ostream::obuf::~obuf() {
    यदि (gzf) {
        समकाल();
        Zlib::gzclose(gzf);
    }
}

तार्किक gzip_istream::ibuf::खोलो(अचर std::string& filename) {
	यदि (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	यदि (!gzf) {
		फर्काऊ असत्य;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	फर्काऊ सत्य;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	पूर्णाङ्क bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	यदि (bytes_read <= 0) {
		यदि (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			फर्काऊ traits_type::eof();
		}

		पूर्णाङ्क err;
		अचर वर्ण* error_सन्देश = Zlib::gzerror(gzf, &err);
		यदि (err != Z_OK)
			log_error("%s", error_सन्देश);
		अन्यथा
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	फर्काऊ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	यदि (gzf) {
		पूर्णाङ्क err = Zlib::gzclose(gzf);
		यदि (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			अचर वर्ण* error_सन्देश = Zlib::gzerror(gzf, &err);
			log_error("%s", error_सन्देश);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(अचर std::string filename, std::ios_base::openmode mode) {
	यदि (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = नूतन std::ifstream();
	f->खोलो(filename, mode);
	यदि (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, म_त्रुटि(त्रुटि_सं));
	// Check for gzip magic
	अचिन्हित वर्ण magic[3];
	पूर्णाङ्क n = 0;
	जबसम्म (n < 3)
	{
		पूर्णाङ्क c = f->get();
		यदि (c != खातापूर्ण) {
			magic[n] = (अचिन्हित वर्ण) c;
		}
		n++;
	}
	यदि (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		यदि (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), अचिन्हित(magic[2]));
		gzip_istream* s = नूतन gzip_istream();
		delete f;
		तार्किक ok = s->खोलो(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		फर्काऊ s;
#अन्यथा
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} अन्यथा {
		f->clear();
		f->seekg(0, std::ios::beg);
		फर्काऊ f;
	}
}

YOSYS_NAMESPACE_END

#समावेश "kernel/yosys_common.h"
#समावेश "kernel/log.h"
#समावेश "kernel/gzip.h"
#समावेश <iostream>
#समावेश <string>
#समावेश <cstdarg>
#समावेश <cstdio>

#अगर !defined(WIN32)
#समावेश <dirent.h>
#समावेश <unistd.h>
#अन्यथा
#समावेश <io.h>
#पूर्ण_अगर

YOSYS_NAMESPACE_BEGIN

#अगर_घोषित YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(बफर, बफर + बफर_आकार - 1);
}

bool gzip_ostream::obuf::खोलो(स्थिर std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    वापस gzf != nullptr;
}

पूर्णांक gzip_ostream::obuf::sync() {
    पूर्णांक अंक = pptr() - pbase();
    अगर (अंक > 0) {
        अगर (Zlib::gzwrite(gzf, reinterpret_cast<स्थिर व्योम*>(pbase()), अंक) != अंक) {
            वापस -1;
        }
        pbump(-अंक);
    }
    वापस 0;
}

gzip_ostream::obuf::~obuf() {
    अगर (gzf) {
        sync();
        Zlib::gzclose(gzf);
    }
}

bool gzip_istream::ibuf::खोलो(स्थिर std::string& filename) {
	अगर (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	अगर (!gzf) {
		वापस false;
	}
	// Empty and point to start
	setg(बफर, बफर, बफर);
	वापस true;
}

// Called when the buffer is empty and more input is needed
std::अ_प्रवाह::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	पूर्णांक bytes_read = Zlib::gzread(gzf, बफर, बफर_आकार);
	अगर (bytes_read <= 0) {
		अगर (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			वापस traits_type::खप();
		}

		पूर्णांक err;
		स्थिर अक्षर* error_संदेश = Zlib::gzerror(gzf, &err);
		अगर (err != Z_OK)
			log_error("%s", error_संदेश);
		अन्यथा
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(बफर, बफर, बफर + bytes_read);
	वापस traits_type::to_int_type(बफर[0]);
}

gzip_istream::ibuf::~ibuf() {
	अगर (gzf) {
		पूर्णांक err = Zlib::gzclose(gzf);
		अगर (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			स्थिर अक्षर* error_संदेश = Zlib::gzerror(gzf, &err);
			log_error("%s", error_संदेश);
		}
	}
}

#पूर्ण_अगर // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::अ_प्रवाह* uncompressed(स्थिर std::string filename, std::ios_base::openmode mode) {
	अगर (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::अख_प्रवाह* f = नया std::अख_प्रवाह();
	f->खोलो(filename, mode);
	अगर (f->हार())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, म_त्रुटि(त्रुटि_सं));
	// Check for gzip magic
	अचिन्हित अक्षर magic[3];
	पूर्णांक n = 0;
	जबतक (n < 3)
	{
		पूर्णांक c = f->लो();
		अगर (c != खातापूर्ण) {
			magic[n] = (अचिन्हित अक्षर) c;
		}
		n++;
	}
	अगर (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#अगर_घोषित YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		अगर (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), अचिन्हित(magic[2]));
		gzip_istream* s = नया gzip_istream();
		मिटाओ f;
		bool ok = s->खोलो(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		वापस s;
#अन्यथा
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#पूर्ण_अगर // YOSYS_ENABLE_ZLIB
	} अन्यथा {
		f->सफा();
		f->ग_स्थान(0, std::पनप::beg);
		वापस f;
	}
}

YOSYS_NAMESPACE_END

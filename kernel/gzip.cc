#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/gzip.h"
#ضمن <iostream>
#ضمن <string>
#ضمن <cstdarg>
#ضمن <cstdio>

#ܐܢ !defined(WIN32)
#ضمن <dirent.h>
#ضمن <unistd.h>
#ܐܚܪܢܐ
#ضمن <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

منطقي gzip_ostream::obuf::open(ثابت std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ܦܢܐ gzf != nullptr;
}

صحيح gzip_ostream::obuf::متزامن() {
    صحيح num = pptr() - pbase();
    ܐܢ (num > 0) {
        ܐܢ (Zlib::gzwrite(gzf, reinterpret_cast<ثابت فراغ*>(pbase()), num) != num) {
            ܦܢܐ -1;
        }
        pbump(-num);
    }
    ܦܢܐ 0;
}

gzip_ostream::obuf::~obuf() {
    ܐܢ (gzf) {
        متزامن();
        Zlib::gzclose(gzf);
    }
}

منطقي gzip_istream::ibuf::open(ثابت std::string& filename) {
	ܐܢ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ܐܢ (!gzf) {
		ܦܢܐ ܕܓܠܐ;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	ܦܢܐ ܫܪܪܐ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	صحيح bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	ܐܢ (bytes_read <= 0) {
		ܐܢ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ܦܢܐ traits_type::eof();
		}

		صحيح err;
		ثابت حرف* error_msg = Zlib::gzerror(gzf, &err);
		ܐܢ (err != Z_OK)
			log_error("%s", error_msg);
		ܐܚܪܢܐ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	ܦܢܐ traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	ܐܢ (gzf) {
		صحيح err = Zlib::gzclose(gzf);
		ܐܢ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ثابت حرف* error_msg = Zlib::gzerror(gzf, &err);
			log_error("%s", error_msg);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ثابت std::string filename, std::ios_base::openmode mode) {
	ܐܢ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = جديد std::ifstream();
	f->open(filename, mode);
	ܐܢ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned حرف magic[3];
	صحيح n = 0;
	ܟܡܐ (n < 3)
	{
		صحيح c = f->get();
		ܐܢ (c != EOF) {
			magic[n] = (unsigned حرف) c;
		}
		n++;
	}
	ܐܢ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ܐܢ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = جديد gzip_istream();
		delete f;
		منطقي ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ܦܢܐ s;
#ܐܚܪܢܐ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} ܐܚܪܢܐ {
		f->clear();
		f->seekg(0, std::ios::beg);
		ܦܢܐ f;
	}
}

YOSYS_NAMESPACE_END

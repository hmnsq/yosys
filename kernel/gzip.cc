#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/gzip.h"
#ضمن <iostream>
#ضمن <string>
#ضمن <cstdarg>
#ضمن <cstdio>

#אם !defined(WIN32)
#ضمن <dirent.h>
#ضمن <unistd.h>
#אחרת
#ضمن <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

منطقي gzip_ostream::obuf::open(ثابت std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    החזר gzf != nullptr;
}

صحيح gzip_ostream::obuf::متزامن() {
    صحيح num = pptr() - pbase();
    אם (num > 0) {
        אם (Zlib::gzwrite(gzf, reinterpret_cast<ثابت فراغ*>(pbase()), num) != num) {
            החזר -1;
        }
        pbump(-num);
    }
    החזר 0;
}

gzip_ostream::obuf::~obuf() {
    אם (gzf) {
        متزامن();
        Zlib::gzclose(gzf);
    }
}

منطقي gzip_istream::ibuf::open(ثابت std::string& filename) {
	אם (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	אם (!gzf) {
		החזר שקר;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	החזר אמת;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	صحيح bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	אם (bytes_read <= 0) {
		אם (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			החזר traits_type::eof();
		}

		صحيح err;
		ثابت حرف* error_msg = Zlib::gzerror(gzf, &err);
		אם (err != Z_OK)
			log_error("%s", error_msg);
		אחרת
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	החזר traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	אם (gzf) {
		صحيح err = Zlib::gzclose(gzf);
		אם (err != Z_OK) {
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
	אם (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = جديد std::ifstream();
	f->open(filename, mode);
	אם (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned حرف magic[3];
	صحيح n = 0;
	כלעוד (n < 3)
	{
		صحيح c = f->get();
		אם (c != EOF) {
			magic[n] = (unsigned حرف) c;
		}
		n++;
	}
	אם (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		אם (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = جديد gzip_istream();
		delete f;
		منطقي ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		החזר s;
#אחרת
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} אחרת {
		f->clear();
		f->seekg(0, std::ios::beg);
		החזר f;
	}
}

YOSYS_NAMESPACE_END

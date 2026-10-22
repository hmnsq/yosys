#شامل "kernel/yosys_common.h"
#شامل "kernel/log.h"
#شامل "kernel/gzip.h"
#شامل <iostream>
#شامل <string>
#شامل <cstdarg>
#شامل <cstdio>

#اگر !defined(WIN32)
#شامل <dirent.h>
#شامل <unistd.h>
#وگرنه
#شامل <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

منطقی gzip_ostream::obuf::open(ثابت std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    بازگشت gzf != nullptr;
}

صحیح gzip_ostream::obuf::همگام() {
    صحیح رقم = pptr() - pbase();
    اگر (رقم > 0) {
        اگر (Zlib::gzwrite(gzf, reinterpret_cast<ثابت تهی*>(pbase()), رقم) != رقم) {
            بازگشت -1;
        }
        pbump(-رقم);
    }
    بازگشت 0;
}

gzip_ostream::obuf::~obuf() {
    اگر (gzf) {
        همگام();
        Zlib::gzclose(gzf);
    }
}

منطقی gzip_istream::ibuf::open(ثابت std::string& filename) {
	اگر (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	اگر (!gzf) {
		بازگشت نادرست;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	بازگشت درست;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	صحیح bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	اگر (bytes_read <= 0) {
		اگر (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			بازگشت traits_type::eof();
		}

		صحیح err;
		ثابت نویسه* error_پیام = Zlib::gzerror(gzf, &err);
		اگر (err != Z_OK)
			log_error("%s", error_پیام);
		وگرنه
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	بازگشت traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	اگر (gzf) {
		صحیح err = Zlib::gzclose(gzf);
		اگر (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ثابت نویسه* error_پیام = Zlib::gzerror(gzf, &err);
			log_error("%s", error_پیام);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ثابت std::string filename, std::ios_base::openmode mode) {
	اگر (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = جدید std::ifstream();
	f->open(filename, mode);
	اگر (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned نویسه magic[3];
	صحیح n = 0;
	وقتی (n < 3)
	{
		صحیح c = f->get();
		اگر (c != EOF) {
			magic[n] = (unsigned نویسه) c;
		}
		n++;
	}
	اگر (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		اگر (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = جدید gzip_istream();
		delete f;
		منطقی ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		بازگشت s;
#وگرنه
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} وگرنه {
		f->clear();
		f->seekg(0, std::ios::beg);
		بازگشت f;
	}
}

YOSYS_NAMESPACE_END

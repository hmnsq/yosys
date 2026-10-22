#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/log.h"
#અન્તર્ભાવ "kernel/gzip.h"
#અન્તર્ભાવ <iostream>
#અન્તર્ભાવ <string>
#અન્તર્ભાવ <cstdarg>
#અન્તર્ભાવ <cstdio>

#જો !defined(WIN32)
#અન્તર્ભાવ <dirent.h>
#અન્તર્ભાવ <unistd.h>
#નહીંતર
#અન્તર્ભાવ <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(buffer, buffer + buffer_size - 1);
}

તાર્કિક gzip_ostream::obuf::open(અચલ std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    પાછા gzf != nullptr;
}

પૂર્ણાંક gzip_ostream::obuf::સમકાલિક() {
    પૂર્ણાંક અંક = pptr() - pbase();
    જો (અંક > 0) {
        જો (Zlib::gzwrite(gzf, reinterpret_cast<અચલ શૂન્ય*>(pbase()), અંક) != અંક) {
            પાછા -1;
        }
        pbump(-અંક);
    }
    પાછા 0;
}

gzip_ostream::obuf::~obuf() {
    જો (gzf) {
        સમકાલિક();
        Zlib::gzclose(gzf);
    }
}

તાર્કિક gzip_istream::ibuf::open(અચલ std::string& filename) {
	જો (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	જો (!gzf) {
		પાછા ખોટું;
	}
	// Empty and point to start
	setg(buffer, buffer, buffer);
	પાછા સાચું;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	પૂર્ણાંક bytes_read = Zlib::gzread(gzf, buffer, buffer_size);
	જો (bytes_read <= 0) {
		જો (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			પાછા traits_type::eof();
		}

		પૂર્ણાંક err;
		અચલ અક્ષર* error_સંદેશ = Zlib::gzerror(gzf, &err);
		જો (err != Z_OK)
			log_error("%s", error_સંદેશ);
		નહીંતર
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(buffer, buffer, buffer + bytes_read);
	પાછા traits_type::to_int_type(buffer[0]);
}

gzip_istream::ibuf::~ibuf() {
	જો (gzf) {
		પૂર્ણાંક err = Zlib::gzclose(gzf);
		જો (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			અચલ અક્ષર* error_સંદેશ = Zlib::gzerror(gzf, &err);
			log_error("%s", error_સંદેશ);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(અચલ std::string filename, std::ios_base::openmode mode) {
	જો (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = નવું std::ifstream();
	f->open(filename, mode);
	જો (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned અક્ષર magic[3];
	પૂર્ણાંક n = 0;
	જ્યારે (n < 3)
	{
		પૂર્ણાંક c = f->get();
		જો (c != EOF) {
			magic[n] = (unsigned અક્ષર) c;
		}
		n++;
	}
	જો (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		જો (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = નવું gzip_istream();
		delete f;
		તાર્કિક ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		પાછા s;
#નહીંતર
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} નહીંતર {
		f->clear();
		f->seekg(0, std::ios::beg);
		પાછા f;
	}
}

YOSYS_NAMESPACE_END

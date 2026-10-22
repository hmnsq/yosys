#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/gzip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdarg>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>

#ਜੇ !defined(WIN32)
#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>
#ਜੇ_ਅੰਤ

YOSYS_NAMESPACE_BEGIN

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(ਬਫਰ, ਬਫਰ + ਬਫਰ_ਮਾਪ - 1);
}

ਤਾਰਕਿਕ gzip_ostream::obuf::open(ਅਚਲ std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ਵਾਪਸ gzf != ਖਾਲੀ_ਸੂਚਕ;
}

ਪੂਰਨ gzip_ostream::obuf::ਸਮਕਾਲੀ() {
    ਪੂਰਨ ਅੰਕ = pptr() - pbase();
    ਜੇ (ਅੰਕ > 0) {
        ਜੇ (Zlib::gzwrite(gzf, reinterpret_cast<ਅਚਲ ਖਾਲੀ*>(pbase()), ਅੰਕ) != ਅੰਕ) {
            ਵਾਪਸ -1;
        }
        pbump(-ਅੰਕ);
    }
    ਵਾਪਸ 0;
}

gzip_ostream::obuf::~obuf() {
    ਜੇ (gzf) {
        ਸਮਕਾਲੀ();
        Zlib::gzclose(gzf);
    }
}

ਤਾਰਕਿਕ gzip_istream::ibuf::open(ਅਚਲ std::string& filename) {
	ਜੇ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ਜੇ (!gzf) {
		ਵਾਪਸ ਝੂਠ;
	}
	// Empty and point to start
	setg(ਬਫਰ, ਬਫਰ, ਬਫਰ);
	ਵਾਪਸ ਸੱਚ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	ਪੂਰਨ ਬਾਈਟਾਂ_read = Zlib::gzread(gzf, ਬਫਰ, ਬਫਰ_ਮਾਪ);
	ਜੇ (ਬਾਈਟਾਂ_read <= 0) {
		ਜੇ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ਵਾਪਸ traits_type::eof();
		}

		ਪੂਰਨ err;
		ਅਚਲ ਅੱਖਰ* error_ਸੁਨੇਹਾ = Zlib::gzerror(gzf, &err);
		ਜੇ (err != Z_OK)
			log_error("%s", error_ਸੁਨੇਹਾ);
		ਨਹੀਂਤਾਂ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(ਬਫਰ, ਬਫਰ, ਬਫਰ + ਬਾਈਟਾਂ_read);
	ਵਾਪਸ traits_type::ਮੰਜ਼ਿਲ_int_type(ਬਫਰ[0]);
}

gzip_istream::ibuf::~ibuf() {
	ਜੇ (gzf) {
		ਪੂਰਨ err = Zlib::gzclose(gzf);
		ਜੇ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ਅਚਲ ਅੱਖਰ* error_ਸੁਨੇਹਾ = Zlib::gzerror(gzf, &err);
			log_error("%s", error_ਸੁਨੇਹਾ);
		}
	}
}

#ਜੇ_ਅੰਤ // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ਅਚਲ std::string filename, std::ios_ਅਧਾਰ::openmode ਬਹੁਲਕ) {
	ਜੇ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = ਨਵਾਂ std::ifstream();
	f->open(filename, ਬਹੁਲਕ);
	ਜੇ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ magic[3];
	ਪੂਰਨ n = 0;
	ਜਦੋਂ (n < 3)
	{
		ਪੂਰਨ c = f->get();
		ਜੇ (c != EOF) {
			magic[n] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c;
		}
		n++;
	}
	ਜੇ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ZLIB
		ਲਘੂਗਣਕ("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ਜੇ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), ਬਿਨਾਨਿਸ਼ਾਨੀ(magic[2]));
		gzip_istream* s = ਨਵਾਂ gzip_istream();
		ਮਿਟਾਓ f;
		ਤਾਰਕਿਕ ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ਵਾਪਸ s;
#ਨਹੀਂਤਾਂ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#ਜੇ_ਅੰਤ // YOSYS_ENABLE_ZLIB
	} ਨਹੀਂਤਾਂ {
		f->clear();
		f->seekg(0, std::ios::beg);
		ਵਾਪਸ f;
	}
}

YOSYS_NAMESPACE_ਸਿਰਾ

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/log.h"
#ضمن "kernel/gzip.h"
#ضمن <iostream>
#ضمن <string>
#ضمن <cstdarg>
#ضمن <cstdio>

#إذا !defined(WIN32)
#ضمن <dirent.h>
#ضمن <unistd.h>
#وإلا
#ضمن <io.h>
#جے_انت

YOSYS_NAMESPACE_BEGIN

#جے_تعریف_شدہ YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(بفر, بفر + بفر_ناپ - 1);
}

منطقي gzip_ostream::obuf::open(ثابت std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    أرجع gzf != خالی_سوچک;
}

صحيح gzip_ostream::obuf::متزامن() {
    صحيح عدد = pptr() - pbase();
    إذا (عدد > 0) {
        إذا (Zlib::gzwrite(gzf, reinterpret_cast<ثابت فراغ*>(pbase()), عدد) != عدد) {
            أرجع -1;
        }
        pbump(-عدد);
    }
    أرجع 0;
}

gzip_ostream::obuf::~obuf() {
    إذا (gzf) {
        متزامن();
        Zlib::gzclose(gzf);
    }
}

منطقي gzip_istream::ibuf::open(ثابت std::string& filename) {
	إذا (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	إذا (!gzf) {
		أرجع خطأ;
	}
	// Empty and point to start
	setg(بفر, بفر, بفر);
	أرجع سچ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	صحيح بائٹاں_read = Zlib::gzread(gzf, بفر, بفر_ناپ);
	إذا (بائٹاں_read <= 0) {
		إذا (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			أرجع traits_type::eof();
		}

		صحيح err;
		ثابت حرف* error_پیغام = Zlib::gzerror(gzf, &err);
		إذا (err != Z_OK)
			log_error("%s", error_پیغام);
		وإلا
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(بفر, بفر, بفر + بائٹاں_read);
	أرجع traits_type::منزل_int_type(بفر[0]);
}

gzip_istream::ibuf::~ibuf() {
	إذا (gzf) {
		صحيح err = Zlib::gzclose(gzf);
		إذا (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ثابت حرف* error_پیغام = Zlib::gzerror(gzf, &err);
			log_error("%s", error_پیغام);
		}
	}
}

#جے_انت // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ثابت std::string filename, std::ios_ادھار::openmode منوال) {
	إذا (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = جديد std::ifstream();
	f->open(filename, منوال);
	إذا (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	بنانشانی حرف magic[3];
	صحيح n = 0;
	طالما (n < 3)
	{
		صحيح c = f->get();
		إذا (c != EOF) {
			magic[n] = (بنانشانی حرف) c;
		}
		n++;
	}
	إذا (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#جے_تعریف_شدہ YOSYS_ENABLE_ZLIB
		لوگارتھم("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		إذا (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), بنانشانی(magic[2]));
		gzip_istream* s = جديد gzip_istream();
		مٹاؤ f;
		منطقي ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		أرجع s;
#وإلا
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#جے_انت // YOSYS_ENABLE_ZLIB
	} وإلا {
		f->clear();
		f->seekg(0, std::ios::beg);
		أرجع f;
	}
}

YOSYS_NAMESPACE_سرا

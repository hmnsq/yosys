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
#ورنہ
#شامل <io.h>
#اگر_ختم

YOSYS_NAMESPACE_BEGIN

#اگر_تعریف_شدہ YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(بفر, بفر + بفر_سائز - 1);
}

منطقی gzip_ostream::obuf::open(ثابت std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    واپس gzf != خالی_اشاریہ;
}

صحیح gzip_ostream::obuf::ہمآہنگ() {
    صحیح عدد = pptr() - pbase();
    اگر (عدد > 0) {
        اگر (Zlib::gzwrite(gzf, reinterpret_cast<ثابت خالی*>(pbase()), عدد) != عدد) {
            واپس -1;
        }
        pbump(-عدد);
    }
    واپس 0;
}

gzip_ostream::obuf::~obuf() {
    اگر (gzf) {
        ہمآہنگ();
        Zlib::gzclose(gzf);
    }
}

منطقی gzip_istream::ibuf::open(ثابت std::string& filename) {
	اگر (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	اگر (!gzf) {
		واپس جھوٹ;
	}
	// Empty and point to start
	setg(بفر, بفر, بفر);
	واپس سچ;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	صحیح بائٹیں_read = Zlib::gzread(gzf, بفر, بفر_سائز);
	اگر (بائٹیں_read <= 0) {
		اگر (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			واپس traits_type::eof();
		}

		صحیح err;
		ثابت حرف* error_پیغام = Zlib::gzerror(gzf, &err);
		اگر (err != Z_OK)
			log_error("%s", error_پیغام);
		ورنہ
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(بفر, بفر, بفر + بائٹیں_read);
	واپس traits_type::مقصد_int_type(بفر[0]);
}

gzip_istream::ibuf::~ibuf() {
	اگر (gzf) {
		صحیح err = Zlib::gzclose(gzf);
		اگر (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ثابت حرف* error_پیغام = Zlib::gzerror(gzf, &err);
			log_error("%s", error_پیغام);
		}
	}
}

#اگر_ختم // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ثابت std::string filename, std::ios_بنیاد::openmode منوال) {
	اگر (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = نیا std::ifstream();
	f->open(filename, منوال);
	اگر (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	غیرعلامتی حرف magic[3];
	صحیح n = 0;
	جب (n < 3)
	{
		صحیح c = f->get();
		اگر (c != EOF) {
			magic[n] = (غیرعلامتی حرف) c;
		}
		n++;
	}
	اگر (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#اگر_تعریف_شدہ YOSYS_ENABLE_ZLIB
		لوگارتھم("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		اگر (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), غیرعلامتی(magic[2]));
		gzip_istream* s = نیا gzip_istream();
		حذف f;
		منطقی ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		واپس s;
#ورنہ
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#اگر_ختم // YOSYS_ENABLE_ZLIB
	} ورنہ {
		f->clear();
		f->seekg(0, std::ios::beg);
		واپس f;
	}
}

YOSYS_NAMESPACE_انتہا

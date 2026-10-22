#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/log.h"
#অন্তর্ভাব "kernel/gzip.h"
#অন্তর্ভাব <iostream>
#অন্তর্ভাব <string>
#অন্তর্ভাব <cstdarg>
#অন্তর্ভাব <cstdio>

#যদি !defined(WIN32)
#অন্তর্ভাব <dirent.h>
#অন্তর্ভাব <unistd.h>
#অন্যথা
#অন্তর্ভাব <io.h>
#endif

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(বাফার, বাফার + বাফার_আকার - 1);
}

যৌক্তিক gzip_ostream::obuf::open(ধ্রুবক std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    ফেরত gzf != nullptr;
}

পূর্ণসংখ্যা gzip_ostream::obuf::সমলয়() {
    পূর্ণসংখ্যা অঙ্ক = pptr() - pbase();
    যদি (অঙ্ক > 0) {
        যদি (Zlib::gzwrite(gzf, reinterpret_cast<ধ্রুবক শূন্য*>(pbase()), অঙ্ক) != অঙ্ক) {
            ফেরত -1;
        }
        pbump(-অঙ্ক);
    }
    ফেরত 0;
}

gzip_ostream::obuf::~obuf() {
    যদি (gzf) {
        সমলয়();
        Zlib::gzclose(gzf);
    }
}

যৌক্তিক gzip_istream::ibuf::open(ধ্রুবক std::string& filename) {
	যদি (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	যদি (!gzf) {
		ফেরত মিথ্যা;
	}
	// Empty and point to start
	setg(বাফার, বাফার, বাফার);
	ফেরত সত্য;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	পূর্ণসংখ্যা bytes_read = Zlib::gzread(gzf, বাফার, বাফার_আকার);
	যদি (bytes_read <= 0) {
		যদি (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			ফেরত traits_type::eof();
		}

		পূর্ণসংখ্যা err;
		ধ্রুবক অক্ষর* error_বার্তা = Zlib::gzerror(gzf, &err);
		যদি (err != Z_OK)
			log_error("%s", error_বার্তা);
		অন্যথা
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(বাফার, বাফার, বাফার + bytes_read);
	ফেরত traits_type::to_int_type(বাফার[0]);
}

gzip_istream::ibuf::~ibuf() {
	যদি (gzf) {
		পূর্ণসংখ্যা err = Zlib::gzclose(gzf);
		যদি (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			ধ্রুবক অক্ষর* error_বার্তা = Zlib::gzerror(gzf, &err);
			log_error("%s", error_বার্তা);
		}
	}
}

#endif // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(ধ্রুবক std::string filename, std::ios_base::openmode mode) {
	যদি (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = নতুন std::ifstream();
	f->open(filename, mode);
	যদি (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	unsigned অক্ষর magic[3];
	পূর্ণসংখ্যা n = 0;
	যখন (n < 3)
	{
		পূর্ণসংখ্যা c = f->get();
		যদি (c != EOF) {
			magic[n] = (unsigned অক্ষর) c;
		}
		n++;
	}
	যদি (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#ifdef YOSYS_ENABLE_ZLIB
		log("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		যদি (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), unsigned(magic[2]));
		gzip_istream* s = নতুন gzip_istream();
		delete f;
		যৌক্তিক ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		ফেরত s;
#অন্যথা
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#endif // YOSYS_ENABLE_ZLIB
	} অন্যথা {
		f->clear();
		f->seekg(0, std::ios::beg);
		ফেরত f;
	}
}

YOSYS_NAMESPACE_END

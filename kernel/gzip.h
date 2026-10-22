#অন্তর্ভাব <string>
#অন্তর্ভাব "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#অন্তর্ভাব <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : সর্বজনীন std::ostream {
সর্বজনীন:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ধ্রুবক std::string &filename) {
		ফেরত outbuf.open(filename);
	}
ব্যক্তিগত:
	class obuf : সর্বজনীন std::stringbuf {
	সর্বজনীন:
		obuf();
		bool open(ধ্রুবক std::string &filename);
		আভাসী পূর্ণসংখ্যা sync() override;
		আভাসী ~obuf();
	ব্যক্তিগত:
		স্থির ধ্রুবক পূর্ণসংখ্যা বাফার_আকার = 4096;
		অক্ষর বাফার[বাফার_আকার];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : সর্বজনীন std::istream {
সর্বজনীন:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ধ্রুবক std::string& filename) {
		ফেরত inbuf.open(filename);
	}
ব্যক্তিগত:
	class ibuf final : সর্বজনীন std::streambuf {
	সর্বজনীন:
		ibuf() : gzf(nullptr) {}
		bool open(ধ্রুবক std::string& filename);
		আভাসী ~ibuf();

	সুরক্ষিত:
		// Called when the buffer is empty and more input is needed
		আভাসী int_type underflow() override;
	ব্যক্তিগত:
		স্থির ধ্রুবক পূর্ণসংখ্যা বাফার_আকার = 8192;
		অক্ষর বাফার[বাফার_আকার];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ধ্রুবক std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

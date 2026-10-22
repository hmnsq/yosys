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
class gzip_ostream : সার্বজনিক std::ostream {
সার্বজনিক:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ধ্রুব std::string &filename) {
		প্রত্যাগম outbuf.open(filename);
	}
গুপ্ত:
	class obuf : সার্বজনিক std::stringbuf {
	সার্বজনিক:
		obuf();
		bool open(ধ্রুব std::string &filename);
		আভাস পূর্ণাঙ্ক sync() override;
		আভাস ~obuf();
	গুপ্ত:
		স্থির ধ্রুব পূর্ণাঙ্ক buffer_size = 4096;
		বর্ণ buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : সার্বজনিক std::istream {
সার্বজনিক:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ধ্রুব std::string& filename) {
		প্রত্যাগম inbuf.open(filename);
	}
গুপ্ত:
	class ibuf final : সার্বজনিক std::streambuf {
	সার্বজনিক:
		ibuf() : gzf(nullptr) {}
		bool open(ধ্রুব std::string& filename);
		আভাস ~ibuf();

	রক্ষিত:
		// Called when the buffer is empty and more input is needed
		আভাস int_type underflow() override;
	গুপ্ত:
		স্থির ধ্রুব পূর্ণাঙ্ক buffer_size = 8192;
		বর্ণ buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ধ্রুব std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

#અન્તર્ભાવ <string>
#અન્તર્ભાવ "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#અન્તર્ભાવ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : જાહેર std::ostream {
જાહેર:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(અચલ std::string &filename) {
		પાછા outbuf.open(filename);
	}
ખાનગી:
	class obuf : જાહેર std::stringbuf {
	જાહેર:
		obuf();
		bool open(અચલ std::string &filename);
		આભાસી પૂર્ણાંક sync() override;
		આભાસી ~obuf();
	ખાનગી:
		સ્થિર અચલ પૂર્ણાંક buffer_size = 4096;
		અક્ષર buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : જાહેર std::istream {
જાહેર:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(અચલ std::string& filename) {
		પાછા inbuf.open(filename);
	}
ખાનગી:
	class ibuf final : જાહેર std::streambuf {
	જાહેર:
		ibuf() : gzf(nullptr) {}
		bool open(અચલ std::string& filename);
		આભાસી ~ibuf();

	સુરક્ષિત:
		// Called when the buffer is empty and more input is needed
		આભાસી int_type underflow() override;
	ખાનગી:
		સ્થિર અચલ પૂર્ણાંક buffer_size = 8192;
		અક્ષર buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(અચલ std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

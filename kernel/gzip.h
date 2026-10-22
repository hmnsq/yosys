#ضمن <string>
#ضمن "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#ضمن <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : عام std::ostream {
عام:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ثابت std::string &filename) {
		ܦܢܐ outbuf.open(filename);
	}
خاص:
	class obuf : عام std::stringbuf {
	عام:
		obuf();
		bool open(ثابت std::string &filename);
		virtual صحيح sync() override;
		virtual ~obuf();
	خاص:
		ساكن ثابت صحيح buffer_size = 4096;
		حرف buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : عام std::istream {
عام:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ثابت std::string& filename) {
		ܦܢܐ inbuf.open(filename);
	}
خاص:
	class ibuf final : عام std::streambuf {
	عام:
		ibuf() : gzf(nullptr) {}
		bool open(ثابت std::string& filename);
		virtual ~ibuf();

	محمي:
		// Called when the buffer is empty and more input is needed
		virtual int_type underflow() override;
	خاص:
		ساكن ثابت صحيح buffer_size = 8192;
		حرف buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ثابت std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

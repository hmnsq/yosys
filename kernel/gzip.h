#شامل <string>
#شامل "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#شامل <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : عمومی std::ostream {
عمومی:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ثابت std::string &filename) {
		بازگشت outbuf.open(filename);
	}
خصوصی:
	class obuf : عمومی std::stringbuf {
	عمومی:
		obuf();
		bool open(ثابت std::string &filename);
		مجازی صحیح sync() override;
		مجازی ~obuf();
	خصوصی:
		ایستا ثابت صحیح buffer_size = 4096;
		نویسه buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : عمومی std::istream {
عمومی:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ثابت std::string& filename) {
		بازگشت inbuf.open(filename);
	}
خصوصی:
	class ibuf final : عمومی std::streambuf {
	عمومی:
		ibuf() : gzf(nullptr) {}
		bool open(ثابت std::string& filename);
		مجازی ~ibuf();

	محافظتشده:
		// Called when the buffer is empty and more input is needed
		مجازی int_type underflow() override;
	خصوصی:
		ایستا ثابت صحیح buffer_size = 8192;
		نویسه buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ثابت std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

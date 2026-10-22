#ضمن <string>
#ضمن "kernel/yosys_common.h"

#جيڪڏهن_وصف_نه_ٿيل YOSYS_GZIP_H
#وصف_ڪريو YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ZLIB

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
		أرجع outbuf.open(filename);
	}
خاص:
	class obuf : عام std::stringbuf {
	عام:
		obuf();
		bool open(ثابت std::string &filename);
		مجازي صحيح sync() override;
		مجازي ~obuf();
	خاص:
		ساكن ثابت صحيح بفر_سائيز = 4096;
		حرف بفر[بفر_سائيز];             // Internal buffer for compressed data
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
		أرجع inbuf.open(filename);
	}
خاص:
	class ibuf final : عام std::streambuf {
	عام:
		ibuf() : gzf(nullptr) {}
		bool open(ثابت std::string& filename);
		مجازي ~ibuf();

	محمي:
		// Called when the buffer is empty and more input is needed
		مجازي int_type underflow() override;
	خاص:
		ساكن ثابت صحيح بفر_سائيز = 8192;
		حرف بفر[بفر_سائيز];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#جيڪڏهن_پورو // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ثابت std::string filename, std::ios_بنياد::openmode گهڻائي = std::ios_بنياد::اندريون);

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو // YOSYS_GZIP_H

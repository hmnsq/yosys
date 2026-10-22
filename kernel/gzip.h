#ضمن <string>
#ضمن "kernel/yosys_common.h"

#جے_بنا_تعریف YOSYS_GZIP_H
#تعریف_کرو YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#جے_تعریف_شدہ YOSYS_ENABLE_ZLIB

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
		مجازی صحيح sync() override;
		مجازی ~obuf();
	خاص:
		ساكن ثابت صحيح بفر_ناپ = 4096;
		حرف بفر[بفر_ناپ];             // Internal buffer for compressed data
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
		مجازی ~ibuf();

	محمي:
		// Called when the buffer is empty and more input is needed
		مجازی int_type underflow() override;
	خاص:
		ساكن ثابت صحيح بفر_ناپ = 8192;
		حرف بفر[بفر_ناپ];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#جے_انت // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ثابت std::string filename, std::ios_ادھار::openmode منوال = std::ios_ادھار::وچ);

YOSYS_NAMESPACE_سرا

#جے_انت // YOSYS_GZIP_H

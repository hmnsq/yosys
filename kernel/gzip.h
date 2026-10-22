#شامل <string>
#شامل "kernel/yosys_common.h"

#اگر_غیر_تعریف_شدہ YOSYS_GZIP_H
#تعریف_کرو YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#اگر_تعریف_شدہ YOSYS_ENABLE_ZLIB

namespace Zlib {
#شامل <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : عوامی std::ostream {
عوامی:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ثابت std::string &filename) {
		واپس outbuf.open(filename);
	}
نجی:
	class obuf : عوامی std::stringbuf {
	عوامی:
		obuf();
		bool open(ثابت std::string &filename);
		مجازی صحیح sync() override;
		مجازی ~obuf();
	نجی:
		ساکن ثابت صحیح بفر_سائز = 4096;
		حرف بفر[بفر_سائز];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : عوامی std::istream {
عوامی:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ثابت std::string& filename) {
		واپس inbuf.open(filename);
	}
نجی:
	class ibuf final : عوامی std::streambuf {
	عوامی:
		ibuf() : gzf(nullptr) {}
		bool open(ثابت std::string& filename);
		مجازی ~ibuf();

	محفوظ:
		// Called when the buffer is empty and more input is needed
		مجازی int_type underflow() override;
	نجی:
		ساکن ثابت صحیح بفر_سائز = 8192;
		حرف بفر[بفر_سائز];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#اگر_ختم // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ثابت std::string filename, std::ios_بنیاد::openmode منوال = std::ios_بنیاد::اندر);

YOSYS_NAMESPACE_انتہا

#اگر_ختم // YOSYS_GZIP_H

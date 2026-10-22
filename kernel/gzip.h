#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_GZIP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ZLIB

namespace Zlib {
#ਅਨ੍ਤਰ੍ਭਾਵ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : ਜਨਤਕ std::ostream {
ਜਨਤਕ:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ਅਚਲ std::string &filename) {
		ਵਾਪਸ outbuf.open(filename);
	}
ਨਿਜੀ:
	class obuf : ਜਨਤਕ std::stringbuf {
	ਜਨਤਕ:
		obuf();
		bool open(ਅਚਲ std::string &filename);
		ਆਭਾਸੀ ਪੂਰਨ sync() override;
		ਆਭਾਸੀ ~obuf();
	ਨਿਜੀ:
		ਸਥਿਰ ਅਚਲ ਪੂਰਨ ਬਫਰ_ਮਾਪ = 4096;
		ਅੱਖਰ ਬਫਰ[ਬਫਰ_ਮਾਪ];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : ਜਨਤਕ std::istream {
ਜਨਤਕ:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ਅਚਲ std::string& filename) {
		ਵਾਪਸ inbuf.open(filename);
	}
ਨਿਜੀ:
	class ibuf final : ਜਨਤਕ std::streambuf {
	ਜਨਤਕ:
		ibuf() : gzf(nullptr) {}
		bool open(ਅਚਲ std::string& filename);
		ਆਭਾਸੀ ~ibuf();

	ਸੁਰੱਖਿਅਤ:
		// Called when the buffer is empty and more input is needed
		ਆਭਾਸੀ int_type underflow() override;
	ਨਿਜੀ:
		ਸਥਿਰ ਅਚਲ ਪੂਰਨ ਬਫਰ_ਮਾਪ = 8192;
		ਅੱਖਰ ਬਫਰ[ਬਫਰ_ਮਾਪ];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#ਜੇ_ਅੰਤ // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ਅਚਲ std::string filename, std::ios_ਅਧਾਰ::openmode ਬਹੁਲਕ = std::ios_ਅਧਾਰ::ਵਿੱਚ);

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // YOSYS_GZIP_H

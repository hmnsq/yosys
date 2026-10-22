#அந்தர்பாவ <string>
#அந்தர்பாவ "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#அந்தர்பாவ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : பொது std::ostream {
பொது:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(மாறிலி std::string &filename) {
		திரும்பு outbuf.open(filename);
	}
தனிப்பட்ட:
	class obuf : பொது std::stringbuf {
	பொது:
		obuf();
		bool open(மாறிலி std::string &filename);
		மெய்நிகர் முழு sync() override;
		மெய்நிகர் ~obuf();
	தனிப்பட்ட:
		நிலையான மாறிலி முழு buffer_size = 4096;
		எழுத்து buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : பொது std::istream {
பொது:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(மாறிலி std::string& filename) {
		திரும்பு inbuf.open(filename);
	}
தனிப்பட்ட:
	class ibuf final : பொது std::streambuf {
	பொது:
		ibuf() : gzf(nullptr) {}
		bool open(மாறிலி std::string& filename);
		மெய்நிகர் ~ibuf();

	பாதுகாக்கப்பட்ட:
		// Called when the buffer is empty and more input is needed
		மெய்நிகர் int_type underflow() override;
	தனிப்பட்ட:
		நிலையான மாறிலி முழு buffer_size = 8192;
		எழுத்து buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(மாறிலி std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

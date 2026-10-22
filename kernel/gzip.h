#अन्तर्भाव <string>
#अन्तर्भाव "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#अन्तर्भाव <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : सार्वजनिक std::ostream {
सार्वजनिक:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ध्रुव std::string &filename) {
		प्रत्यागम outbuf.open(filename);
	}
गुप्त:
	class obuf : सार्वजनिक std::stringbuf {
	सार्वजनिक:
		obuf();
		bool open(ध्रुव std::string &filename);
		आभास पूर्णाङ्क sync() override;
		आभास ~obuf();
	गुप्त:
		स्थिर ध्रुव पूर्णाङ्क buffer_size = 4096;
		वर्ण buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : सार्वजनिक std::istream {
सार्वजनिक:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ध्रुव std::string& filename) {
		प्रत्यागम inbuf.open(filename);
	}
गुप्त:
	class ibuf final : सार्वजनिक std::streambuf {
	सार्वजनिक:
		ibuf() : gzf(nullptr) {}
		bool open(ध्रुव std::string& filename);
		आभास ~ibuf();

	रक्षित:
		// Called when the buffer is empty and more input is needed
		आभास int_type underflow() override;
	गुप्त:
		स्थिर ध्रुव पूर्णाङ्क buffer_size = 8192;
		वर्ण buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ध्रुव std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

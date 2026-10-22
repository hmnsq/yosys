#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#ಅನ್ತರ್ಭಾವ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : ಸಾರ್ವಜನಿಕ std::ostream {
ಸಾರ್ವಜನಿಕ:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ಧ್ರುವ std::string &filename) {
		ಪ್ರತ್ಯಾಗಮ outbuf.open(filename);
	}
ಗುಪ್ತ:
	class obuf : ಸಾರ್ವಜನಿಕ std::stringbuf {
	ಸಾರ್ವಜನಿಕ:
		obuf();
		bool open(ಧ್ರುವ std::string &filename);
		ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ sync() override;
		ಆಭಾಸ ~obuf();
	ಗುಪ್ತ:
		ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ buffer_size = 4096;
		ವರ್ಣ buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : ಸಾರ್ವಜನಿಕ std::istream {
ಸಾರ್ವಜನಿಕ:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ಧ್ರುವ std::string& filename) {
		ಪ್ರತ್ಯಾಗಮ inbuf.open(filename);
	}
ಗುಪ್ತ:
	class ibuf final : ಸಾರ್ವಜನಿಕ std::streambuf {
	ಸಾರ್ವಜನಿಕ:
		ibuf() : gzf(nullptr) {}
		bool open(ಧ್ರುವ std::string& filename);
		ಆಭಾಸ ~ibuf();

	ರಕ್ಷಿತ:
		// Called when the buffer is empty and more input is needed
		ಆಭಾಸ int_type underflow() override;
	ಗುಪ್ತ:
		ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ buffer_size = 8192;
		ವರ್ಣ buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ಧ್ರುವ std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

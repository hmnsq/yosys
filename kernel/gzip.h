#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_GZIP_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ZLIB

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
	bool open(ಸ್ಥಿರಾಂಕ std::string &filename) {
		ಹಿಂತಿರುಗಿ outbuf.open(filename);
	}
ಖಾಸಗಿ:
	class obuf : ಸಾರ್ವಜನಿಕ std::stringbuf {
	ಸಾರ್ವಜನಿಕ:
		obuf();
		bool open(ಸ್ಥಿರಾಂಕ std::string &filename);
		ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ sync() override;
		ವಾಸ್ತವಿಕ ~obuf();
	ಖಾಸಗಿ:
		ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಬಫರ್_ಗಾತ್ರ = 4096;
		ಅಕ್ಷರ ಬಫರ್[ಬಫರ್_ಗಾತ್ರ];             // Internal buffer for compressed data
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
	bool open(ಸ್ಥಿರಾಂಕ std::string& filename) {
		ಹಿಂತಿರುಗಿ inbuf.open(filename);
	}
ಖಾಸಗಿ:
	class ibuf final : ಸಾರ್ವಜನಿಕ std::streambuf {
	ಸಾರ್ವಜನಿಕ:
		ibuf() : gzf(nullptr) {}
		bool open(ಸ್ಥಿರಾಂಕ std::string& filename);
		ವಾಸ್ತವಿಕ ~ibuf();

	ಸಂರಕ್ಷಿತ:
		// Called when the buffer is empty and more input is needed
		ವಾಸ್ತವಿಕ int_type underflow() override;
	ಖಾಸಗಿ:
		ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಬಫರ್_ಗಾತ್ರ = 8192;
		ಅಕ್ಷರ ಬಫರ್[ಬಫರ್_ಗಾತ್ರ];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#ಆದರೆ_ಅಂತ್ಯ // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ಸ್ಥಿರಾಂಕ std::string filename, std::ios_ತಳ::openmode ಬಹುಲಕ = std::ios_ತಳ::ಒಳ);

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // YOSYS_GZIP_H

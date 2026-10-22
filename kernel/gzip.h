#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#അന്തര്ഭാവ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : പൊതു std::ostream {
പൊതു:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(സ്ഥിരം std::string &filename) {
		തിരികെ outbuf.open(filename);
	}
സ്വകാര്യ:
	class obuf : പൊതു std::stringbuf {
	പൊതു:
		obuf();
		bool open(സ്ഥിരം std::string &filename);
		മായാ പൂർണ്ണസംഖ്യ sync() override;
		മായാ ~obuf();
	സ്വകാര്യ:
		സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ buffer_size = 4096;
		അക്ഷരം buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : പൊതു std::istream {
പൊതു:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(സ്ഥിരം std::string& filename) {
		തിരികെ inbuf.open(filename);
	}
സ്വകാര്യ:
	class ibuf final : പൊതു std::streambuf {
	പൊതു:
		ibuf() : gzf(nullptr) {}
		bool open(സ്ഥിരം std::string& filename);
		മായാ ~ibuf();

	സംരക്ഷിത:
		// Called when the buffer is empty and more input is needed
		മായാ int_type underflow() override;
	സ്വകാര്യ:
		സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ buffer_size = 8192;
		അക്ഷരം buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(സ്ഥിരം std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

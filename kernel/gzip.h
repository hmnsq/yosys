#समावेश <string>
#समावेश "kernel/yosys_common.h"

#अगर_अघोषित YOSYS_GZIP_H
#घोषणा YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#अगर_घोषित YOSYS_ENABLE_ZLIB

namespace Zlib {
#समावेश <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : खुला std::ostream {
खुला:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool खोलो(स्थिर std::string &filename) {
		वापस outbuf.खोलो(filename);
	}
निजी:
	class obuf : खुला std::stringbuf {
	खुला:
		obuf();
		bool खोलो(स्थिर std::string &filename);
		भव पूर्णांक sync() override;
		भव ~obuf();
	निजी:
		अटल स्थिर पूर्णांक बफर_आकार = 4096;
		अक्षर बफर[बफर_आकार];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : खुला std::istream {
खुला:
	gzip_istream() : std::istream(&inbuf) {}
	bool खोलो(स्थिर std::string& filename) {
		वापस inbuf.खोलो(filename);
	}
निजी:
	class ibuf final : खुला std::streambuf {
	खुला:
		ibuf() : gzf(nullptr) {}
		bool खोलो(स्थिर std::string& filename);
		भव ~ibuf();

	रक्षित:
		// Called when the buffer is empty and more input is needed
		भव int_type underflow() override;
	निजी:
		अटल स्थिर पूर्णांक बफर_आकार = 8192;
		अक्षर बफर[बफर_आकार];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#पूर्ण_अगर // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(स्थिर std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#पूर्ण_अगर // YOSYS_GZIP_H

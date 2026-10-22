#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"

#ifndef YOSYS_GZIP_H
#define YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#ifdef YOSYS_ENABLE_ZLIB

namespace Zlib {
#ଅନ୍ତର୍ଭାଵ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : ସର୍ବସାଧାରଣ std::ostream {
ସର୍ବସାଧାରଣ:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(ଅଚଳ std::string &filename) {
		ଫେରନ୍ତୁ outbuf.open(filename);
	}
ବ୍ୟକ୍ତିଗତ:
	class obuf : ସର୍ବସାଧାରଣ std::stringbuf {
	ସର୍ବସାଧାରଣ:
		obuf();
		bool open(ଅଚଳ std::string &filename);
		ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା sync() override;
		ଆଭାସୀ ~obuf();
	ବ୍ୟକ୍ତିଗତ:
		ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା buffer_size = 4096;
		ଅକ୍ଷର buffer[buffer_size];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : ସର୍ବସାଧାରଣ std::istream {
ସର୍ବସାଧାରଣ:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(ଅଚଳ std::string& filename) {
		ଫେରନ୍ତୁ inbuf.open(filename);
	}
ବ୍ୟକ୍ତିଗତ:
	class ibuf final : ସର୍ବସାଧାରଣ std::streambuf {
	ସର୍ବସାଧାରଣ:
		ibuf() : gzf(nullptr) {}
		bool open(ଅଚଳ std::string& filename);
		ଆଭାସୀ ~ibuf();

	ସୁରକ୍ଷିତ:
		// Called when the buffer is empty and more input is needed
		ଆଭାସୀ int_type underflow() override;
	ବ୍ୟକ୍ତିଗତ:
		ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା buffer_size = 8192;
		ଅକ୍ଷର buffer[buffer_size];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#endif // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(ଅଚଳ std::string filename, std::ios_base::openmode mode = std::ios_base::in);

YOSYS_NAMESPACE_END

#endif // YOSYS_GZIP_H

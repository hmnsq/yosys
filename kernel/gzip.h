#అన్తర్భావ <string>
#అన్తర్భావ "kernel/yosys_common.h"

#నిర్వచితం_కాకపోతే YOSYS_GZIP_H
#నిర్వచించు YOSYS_GZIP_H

YOSYS_NAMESPACE_BEGIN

#నిర్వచితమైతే YOSYS_ENABLE_ZLIB

namespace Zlib {
#అన్తర్భావ <zlib.h>
}

/*
An output stream that uses a stringbuf to buffer data internally,
using zlib to write gzip-compressed data every time the stream is flushed.
*/
class gzip_ostream : బహిరంగ std::ostream {
బహిరంగ:
	gzip_ostream(): std::ostream(nullptr) {
		rdbuf(&outbuf);
	}
	bool open(స్థిరరాశి std::string &filename) {
		తిరిగి outbuf.open(filename);
	}
అంతరంగ:
	class obuf : బహిరంగ std::stringbuf {
	బహిరంగ:
		obuf();
		bool open(స్థిరరాశి std::string &filename);
		మాయా పూర్ణాంకం sync() override;
		మాయా ~obuf();
	అంతరంగ:
		స్థిర స్థిరరాశి పూర్ణాంకం బఫరు_కొలత = 4096;
		అక్షరం బఫరు[బఫరు_కొలత];             // Internal buffer for compressed data
		Zlib::gzFile gzf = nullptr;                 // Handle to the gzip file
	};

	obuf outbuf;  // The stream buffer instance
};

/*
An input stream that uses zlib to read gzip-compressed data from a file,
buffering the decompressed data internally using its own buffer.
*/
class gzip_istream final : బహిరంగ std::istream {
బహిరంగ:
	gzip_istream() : std::istream(&inbuf) {}
	bool open(స్థిరరాశి std::string& filename) {
		తిరిగి inbuf.open(filename);
	}
అంతరంగ:
	class ibuf final : బహిరంగ std::streambuf {
	బహిరంగ:
		ibuf() : gzf(nullptr) {}
		bool open(స్థిరరాశి std::string& filename);
		మాయా ~ibuf();

	రక్షిత:
		// Called when the buffer is empty and more input is needed
		మాయా int_type underflow() override;
	అంతరంగ:
		స్థిర స్థిరరాశి పూర్ణాంకం బఫరు_కొలత = 8192;
		అక్షరం బఫరు[బఫరు_కొలత];
		Zlib::gzFile gzf;
	};

	ibuf inbuf;  // The stream buffer instance
};

#అయితే_ముగింపు // YOSYS_ENABLE_ZLIB

std::istream* uncompressed(స్థిరరాశి std::string filename, std::ios_పునాది::openmode బహుళకం = std::ios_పునాది::లోపల);

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు // YOSYS_GZIP_H

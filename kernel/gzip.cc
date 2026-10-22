#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/log.h"
#అన్తర్భావ "kernel/gzip.h"
#అన్తర్భావ <iostream>
#అన్తర్భావ <string>
#అన్తర్భావ <cstdarg>
#అన్తర్భావ <cstdio>

#ఒకవేళ !defined(WIN32)
#అన్తర్భావ <dirent.h>
#అన్తర్భావ <unistd.h>
#లేకపోతే
#అన్తర్భావ <io.h>
#అయితే_ముగింపు

YOSYS_NAMESPACE_BEGIN

#నిర్వచితమైతే YOSYS_ENABLE_ZLIB

gzip_ostream::obuf::obuf() {
    setp(బఫరు, బఫరు + బఫరు_కొలత - 1);
}

తార్కిక gzip_ostream::obuf::open(స్థిరరాశి std::string &filename) {
    gzf = Zlib::gzopen(filename.c_str(), "wb");
    తిరిగి gzf != శూన్యసూచిక;
}

పూర్ణాంకం gzip_ostream::obuf::సమకాలీకరణ() {
    పూర్ణాంకం అంకె = pptr() - pbase();
    ఒకవేళ (అంకె > 0) {
        ఒకవేళ (Zlib::gzwrite(gzf, reinterpret_cast<స్థిరరాశి శూన్య*>(pbase()), అంకె) != అంకె) {
            తిరిగి -1;
        }
        pbump(-అంకె);
    }
    తిరిగి 0;
}

gzip_ostream::obuf::~obuf() {
    ఒకవేళ (gzf) {
        సమకాలీకరణ();
        Zlib::gzclose(gzf);
    }
}

తార్కిక gzip_istream::ibuf::open(స్థిరరాశి std::string& filename) {
	ఒకవేళ (gzf) {
		Zlib::gzclose(gzf);
	}
	gzf = Zlib::gzopen(filename.c_str(), "rb");
	ఒకవేళ (!gzf) {
		తిరిగి అబద్ధం;
	}
	// Empty and point to start
	setg(బఫరు, బఫరు, బఫరు);
	తిరిగి నిజం;
}

// Called when the buffer is empty and more input is needed
std::istream::int_type gzip_istream::ibuf::underflow() {
	log_assert(gzf && "No gzfile opened\n");
	పూర్ణాంకం బైట్లు_read = Zlib::gzread(gzf, బఫరు, బఫరు_కొలత);
	ఒకవేళ (బైట్లు_read <= 0) {
		ఒకవేళ (Zlib::gzeof(gzf)) {
			// "On failure, the function ensures that either
			// gptr() == nullptr or gptr() == egptr."
			// Let's set gptr to egptr
			setg(eback(), egptr(), egptr());
			తిరిగి traits_type::eof();
		}

		పూర్ణాంకం err;
		స్థిరరాశి అక్షరం* error_సందేశం = Zlib::gzerror(gzf, &err);
		ఒకవేళ (err != Z_OK)
			log_error("%s", error_సందేశం);
		లేకపోతే
			log_error("Decompression logic failure: "\
					"read <=0 bytes but neither EOF nor error\n");
	}

	// Keep size and point to start
	setg(బఫరు, బఫరు, బఫరు + బైట్లు_read);
	తిరిగి traits_type::గమ్యం_int_type(బఫరు[0]);
}

gzip_istream::ibuf::~ibuf() {
	ఒకవేళ (gzf) {
		పూర్ణాంకం err = Zlib::gzclose(gzf);
		ఒకవేళ (err != Z_OK) {
			// OK to overwrite rr it, it doesn't change
			స్థిరరాశి అక్షరం* error_సందేశం = Zlib::gzerror(gzf, &err);
			log_error("%s", error_సందేశం);
		}
	}
}

#అయితే_ముగింపు // YOSYS_ENABLE_ZLIB


// Takes a successfully opened ifstream. If it's gzipped, returns an istream. Otherwise,
// returns the original ifstream, rewound to the start.
// Never returns nullptr or failed state istream*
std::istream* uncompressed(స్థిరరాశి std::string filename, std::ios_పునాది::openmode బహుళకం) {
	ఒకవేళ (!check_file_exists(filename))
		log_cmd_error("File `%s' not found or is a directory\n", filename);
	std::ifstream* f = కొత్త std::ifstream();
	f->open(filename, బహుళకం);
	ఒకవేళ (f->fail())
		log_cmd_error("Can't open input file `%s' for reading: %s\n", filename, strerror(errno));
	// Check for gzip magic
	గుర్తులేని అక్షరం magic[3];
	పూర్ణాంకం n = 0;
	అయితే (n < 3)
	{
		పూర్ణాంకం c = f->get();
		ఒకవేళ (c != EOF) {
			magic[n] = (గుర్తులేని అక్షరం) c;
		}
		n++;
	}
	ఒకవేళ (n == 3 && magic[0] == 0x1f && magic[1] == 0x8b) {
#నిర్వచితమైతే YOSYS_ENABLE_ZLIB
		సంవర్గమానం("Found gzip magic in file `%s', decompressing using zlib.\n", filename);
		ఒకవేళ (magic[2] != 8)
			log_cmd_error("gzip file `%s' uses unsupported compression type %02x\n",
				filename.c_str(), గుర్తులేని(magic[2]));
		gzip_istream* s = కొత్త gzip_istream();
		తొలగించు f;
		తార్కిక ok = s->open(filename.c_str());
		log_assert(ok && "Failed to open gzipped file.\n");
		తిరిగి s;
#లేకపోతే
		log_cmd_error("File `%s' is a gzip file, but Yosys is compiled without zlib.\n", filename);
#అయితే_ముగింపు // YOSYS_ENABLE_ZLIB
	} లేకపోతే {
		f->clear();
		f->seekg(0, std::ios::beg);
		తిరిగి f;
	}
}

YOSYS_NAMESPACE_చివర

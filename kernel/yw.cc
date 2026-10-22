/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2022  Jannis Harder <jix@yosyshq.com> <me@jix.one>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#అన్తర్భావ "kernel/yw.h"
#అన్తర్భావ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
స్థిర స్థిరరాశి అక్షరం *pretty_name(స్థిరరాశి std::string &id)
{
	స్థిరరాశి అక్షరం *c_str = id.c_str();
	స్థిరరాశి అక్షరం *p = c_str;

	ఒకవేళ (*p != '\\')
		తిరిగి c_str;
	p++;

	ఒకవేళ (*p == '[') {
		p++;
		అయితే (*p >= '0' && *p <= '9')
			p++;
		ఒకవేళ (p[0] != ']' || p[1] != 0)
			తిరిగి c_str;
		తిరిగి c_str + 1;
	}

	ఒకవేళ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		తిరిగి c_str;
	p++;
	అయితే ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ఒకవేళ (*p != 0)
		తిరిగి c_str;
	తిరిగి c_str + 1;
}

std::string Id_మార్గం::str() స్థిరరాశి
{
	std::string ఫలితం;

	కొరకు (స్వయంచాలక &అంశం : *ఇది) {
		స్థిరరాశి అక్షరం *pretty = pretty_name(అంశం);
		ఒకవేళ (pretty[0] == '[') {
			ఫలితం += pretty;
			కొనసాగు;
		}
		ఒకవేళ (!ఫలితం.ఖాళీలు())
			ఫలితం += '.';
		ఫలితం += pretty;
		ఒకవేళ (pretty[0] == '\\' || pretty[0] == '$')
			ఫలితం += ' ';
	}

	తిరిగి ఫలితం;
}

తార్కిక Id_మార్గం::get_చిరునామా(పూర్ణాంకం &addr) స్థిరరాశి
{
	ఒకవేళ (ఖాళీలు())
		తిరిగి అబద్ధం;
	స్వయంచాలక &చివరి = వెనుక();
	ఒకవేళ (!చివరి.starts_with("\\["))
		తిరిగి అబద్ధం;
	ఒకవేళ (చివరి == "\\[0]") {
		addr = 0;
		తిరిగి నిజం;
	}
	అక్షరం మొదటి = చివరి.c_str()[2];
	ఒకవేళ (మొదటి < '1' || మొదటి > '9')
		తిరిగి అబద్ధం;
	అక్షరం *endptr;
	addr = std::strtol(చివరి.c_str() + 2, &endptr, 10);
	తిరిగి endptr[0] == ']' && endptr[1] == 0;
}

స్థిర std::సదిశ<std::string> get_మార్గం(స్థిరరాశి json11::Json &json)
{
	std::సదిశ<std::string> ఫలితం;
	కొరకు (స్వయంచాలక &మార్గం_అంశం : json.array_వస్తువులు()) {
		స్వయంచాలక స్థిరరాశి &మార్గం_అంశం_str = మార్గం_అంశం.string_విలువ();
		ఒకవేళ (మార్గం_అంశం_str.ఖాళీలు())
			తిరిగి {};;
		ఫలితం.నెట్టు_వెనుక(మార్గం_అంశం_str);
	}
	తిరిగి ఫలితం;
}

ReadWitness::ReadWitness(స్థిరరాశి std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ఒకవేళ (f.fail() || Get_కొలత(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ఒకవేళ (!err.ఖాళీలు())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_విలువ();

	ఒకవేళ (format.ఖాళీలు())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ఒకవేళ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	కొరకు (స్వయంచాలక &clock_json : json["clocks"].array_వస్తువులు()) {
		Clock clock;
		clock.మార్గం_2 = get_మార్గం(clock_json["path"]);
		ఒకవేళ (clock.మార్గం_2.ఖాళీలు())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		స్వయంచాలక అంచు_str = clock_json["edge"];
		ఒకవేళ (అంచు_str.string_విలువ() == "posedge")
			clock.is_posedge = నిజం;
		లేకపోతే ఒకవేళ (అంచు_str.string_విలువ() == "negedge")
			clock.is_negedge = నిజం;
		లేకపోతే
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ఒకవేళ (!clock_json["offset"].is_సంఖ్య())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.స్థానభ్రంశం = clock_json["offset"].int_విలువ();
		ఒకవేళ (clock.స్థానభ్రంశం < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.నెట్టు_వెనుక(clock);
	}

	పూర్ణాంకం బిట్లు_స్థానభ్రంశం = 0;
	కొరకు (స్వయంచాలక &signal_json : json["signals"].array_వస్తువులు()) {
		Signal సంకేతం;
		సంకేతం.బిట్లు_స్థానభ్రంశం = బిట్లు_స్థానభ్రంశం;
		సంకేతం.మార్గం_2 = get_మార్గం(signal_json["path"]);
		ఒకవేళ (సంకేతం.మార్గం_2.ఖాళీలు())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ఒకవేళ (!signal_json["width"].is_సంఖ్య())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		సంకేతం.వెడల్పు = signal_json["width"].int_విలువ();
		ఒకవేళ (సంకేతం.వెడల్పు < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		బిట్లు_స్థానభ్రంశం += సంకేతం.వెడల్పు;
		ఒకవేళ (!signal_json["offset"].is_సంఖ్య())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		సంకేతం.స్థానభ్రంశం = signal_json["offset"].int_విలువ();
		ఒకవేళ (సంకేతం.స్థానభ్రంశం < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		సంకేతం.init_only = signal_json["init_only"].bool_విలువ();
		సంకేతాలు.నెట్టు_వెనుక(సంకేతం);
	}

	కొరకు (స్వయంచాలక &అడుగు_json : json["steps"].array_వస్తువులు()) {
		అడుగు_2 అడుగు;
		ఒకవేళ (!అడుగు_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_కొలత(అడుగులు));
		అడుగు.బిట్లు = అడుగు_json["bits"].string_విలువ();
		కొరకు (అక్షరం c : అడుగు.బిట్లు) {
			ఒకవేళ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_కొలత(అడుగులు));
		}
		అడుగులు.నెట్టు_వెనుక(అడుగు);
	}
}

RTLIL::Const ReadWitness::get_బిట్లు(పూర్ణాంకం t, పూర్ణాంకం బిట్లు_స్థానభ్రంశం, పూర్ణాంకం వెడల్పు) స్థిరరాశి
{
	log_assert(t >= 0 && t < Get_కొలత(అడుగులు));

	స్థిరరాశి std::string &బిట్లు = అడుగులు[t].బిట్లు;

	RTLIL::Const ఫలితం(State::Sa, వెడల్పు);

	పూర్ణాంకం read_begin = Get_కొలత(బిట్లు) - 1 - బిట్లు_స్థానభ్రంశం;
	పూర్ణాంకం read_చివర = max(-1, read_begin - వెడల్పు);

	కొరకు (పూర్ణాంకం i = read_begin, j = 0; i > read_చివర; i--, j++) {
		RTLIL::State బిట్టు = State::Sa;
		ఎంపిక (బిట్లు[i]) {
			స్థితి '0': బిట్టు = State::S0; ఆపు;
			స్థితి '1': బిట్టు = State::S1; ఆపు;
			స్థితి 'x': బిట్టు = State::Sx; ఆపు;
			స్థితి '?': బిట్టు = State::Sa; ఆపు;
			అప్రమేయ:
				log_abort();
		}
		ఫలితం.సమితి(j, బిట్టు);
	}

	తిరిగి ఫలితం;
}

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

#ضمن "kernel/yw.h"
#ضمن "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ساكن ثابت حرف *pretty_name(ثابت std::string &id)
{
	ثابت حرف *c_str = id.c_str();
	ثابت حرف *p = c_str;

	إذا (*p != '\\')
		أرجع c_str;
	p++;

	إذا (*p == '[') {
		p++;
		طالما (*p >= '0' && *p <= '9')
			p++;
		إذا (p[0] != ']' || p[1] != 0)
			أرجع c_str;
		أرجع c_str + 1;
	}

	إذا (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		أرجع c_str;
	p++;
	طالما ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	إذا (*p != 0)
		أرجع c_str;
	أرجع c_str + 1;
}

std::string Id_لار::str() ثابت
{
	std::string پایله;

	لكل (خپلکاری &توکی : *هذا) {
		ثابت حرف *pretty = pretty_name(توکی);
		إذا (pretty[0] == '[') {
			پایله += pretty;
			استمر;
		}
		إذا (!پایله.تشه())
			پایله += '.';
		پایله += pretty;
		إذا (pretty[0] == '\\' || pretty[0] == '$')
			پایله += ' ';
	}

	أرجع پایله;
}

منطقي Id_لار::get_پته(صحيح &addr) ثابت
{
	إذا (تشه())
		أرجع خطأ;
	خپلکاری &وروستی = شا();
	إذا (!وروستی.starts_with("\\["))
		أرجع خطأ;
	إذا (وروستی == "\\[0]") {
		addr = 0;
		أرجع ریښتیا;
	}
	حرف لومړی = وروستی.c_str()[2];
	إذا (لومړی < '1' || لومړی > '9')
		أرجع خطأ;
	حرف *endptr;
	addr = std::strtol(وروستی.c_str() + 2, &endptr, 10);
	أرجع endptr[0] == ']' && endptr[1] == 0;
}

ساكن std::ویکټور<std::string> get_لار(ثابت json11::Json &json)
{
	std::ویکټور<std::string> پایله;
	لكل (خپلکاری &لار_توکی : json.array_توکي()) {
		خپلکاری ثابت &لار_توکی_str = لار_توکی.string_ارزښت();
		إذا (لار_توکی_str.تشه())
			أرجع {};;
		پایله.ټېل_شا(لار_توکی_str);
	}
	أرجع پایله;
}

ReadWitness::ReadWitness(ثابت std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	إذا (f.fail() || Get_اندازه(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	إذا (!err.تشه())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_ارزښت();

	إذا (format.تشه())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	إذا (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	لكل (خپلکاری &clock_json : json["clocks"].array_توکي()) {
		Clock clock;
		clock.لار_2 = get_لار(clock_json["path"]);
		إذا (clock.لار_2.تشه())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		خپلکاری څنډه_str = clock_json["edge"];
		إذا (څنډه_str.string_ارزښت() == "posedge")
			clock.is_posedge = ریښتیا;
		وإلا إذا (څنډه_str.string_ارزښت() == "negedge")
			clock.is_negedge = ریښتیا;
		وإلا
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		إذا (!clock_json["offset"].is_شمېره())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.دننه_واټن = clock_json["offset"].int_ارزښت();
		إذا (clock.دننه_واټن < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ټېل_شا(clock);
	}

	صحيح بټونه_دننه_واټن = 0;
	لكل (خپلکاری &signal_json : json["signals"].array_توکي()) {
		Signal إشارة;
		إشارة.بټونه_دننه_واټن = بټونه_دننه_واټن;
		إشارة.لار_2 = get_لار(signal_json["path"]);
		إذا (إشارة.لار_2.تشه())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		إذا (!signal_json["width"].is_شمېره())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		إشارة.پلنوالی = signal_json["width"].int_ارزښت();
		إذا (إشارة.پلنوالی < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		بټونه_دننه_واټن += إشارة.پلنوالی;
		إذا (!signal_json["offset"].is_شمېره())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.دننه_واټن = signal_json["offset"].int_ارزښت();
		إذا (إشارة.دننه_واټن < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.init_only = signal_json["init_only"].bool_ارزښت();
		سیګنالونه.ټېل_شا(إشارة);
	}

	لكل (خپلکاری &ګام_json : json["steps"].array_توکي()) {
		ګام_2 ګام;
		إذا (!ګام_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_اندازه(ګامونه));
		ګام.بټونه = ګام_json["bits"].string_ارزښت();
		لكل (حرف c : ګام.بټونه) {
			إذا (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_اندازه(ګامونه));
		}
		ګامونه.ټېل_شا(ګام);
	}
}

RTLIL::Const ReadWitness::get_بټونه(صحيح t, صحيح بټونه_دننه_واټن, صحيح پلنوالی) ثابت
{
	log_assert(t >= 0 && t < Get_اندازه(ګامونه));

	ثابت std::string &بټونه = ګامونه[t].بټونه;

	RTLIL::Const پایله(State::Sa, پلنوالی);

	صحيح read_begin = Get_اندازه(بټونه) - 1 - بټونه_دننه_واټن;
	صحيح read_پای_ټکی = max(-1, read_begin - پلنوالی);

	لكل (صحيح i = read_begin, j = 0; i > read_پای_ټکی; i--, j++) {
		RTLIL::State بټ = State::Sa;
		بدل (بټونه[i]) {
			حالة '0': بټ = State::S0; اكسر;
			حالة '1': بټ = State::S1; اكسر;
			حالة 'x': بټ = State::Sx; اكسر;
			حالة '?': بټ = State::Sa; اكسر;
			افتراضي:
				log_abort();
		}
		پایله.سېټ(j, بټ);
	}

	أرجع پایله;
}

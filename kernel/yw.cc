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

std::string Id_مسیر::str() ثابت
{
	std::string نتیجه;

	لكل (خودکار &قلم : *هذا) {
		ثابت حرف *pretty = pretty_name(قلم);
		إذا (pretty[0] == '[') {
			نتیجه += pretty;
			استمر;
		}
		إذا (!نتیجه.خالی())
			نتیجه += '.';
		نتیجه += pretty;
		إذا (pretty[0] == '\\' || pretty[0] == '$')
			نتیجه += ' ';
	}

	أرجع نتیجه;
}

منطقي Id_مسیر::get_آدرس(صحيح &addr) ثابت
{
	إذا (خالی())
		أرجع خطأ;
	خودکار &آخرین = پشت();
	إذا (!آخرین.starts_with("\\["))
		أرجع خطأ;
	إذا (آخرین == "\\[0]") {
		addr = 0;
		أرجع درست;
	}
	حرف اولی = آخرین.c_str()[2];
	إذا (اولی < '1' || اولی > '9')
		أرجع خطأ;
	حرف *endptr;
	addr = std::strtol(آخرین.c_str() + 2, &endptr, 10);
	أرجع endptr[0] == ']' && endptr[1] == 0;
}

ساكن std::وکتور<std::string> get_مسیر(ثابت json11::Json &json)
{
	std::وکتور<std::string> نتیجه;
	لكل (خودکار &مسیر_قلم : json.array_اقلام()) {
		خودکار ثابت &مسیر_قلم_str = مسیر_قلم.string_مقدار();
		إذا (مسیر_قلم_str.خالی())
			أرجع {};;
		نتیجه.افزودن_پشت(مسیر_قلم_str);
	}
	أرجع نتیجه;
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
	إذا (!err.خالی())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_مقدار();

	إذا (format.خالی())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	إذا (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	لكل (خودکار &clock_json : json["clocks"].array_اقلام()) {
		Clock clock;
		clock.مسیر_2 = get_مسیر(clock_json["path"]);
		إذا (clock.مسیر_2.خالی())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		خودکار یال_str = clock_json["edge"];
		إذا (یال_str.string_مقدار() == "posedge")
			clock.is_posedge = درست;
		وإلا إذا (یال_str.string_مقدار() == "negedge")
			clock.is_negedge = درست;
		وإلا
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		إذا (!clock_json["offset"].is_عدد())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.جابه_جایی = clock_json["offset"].int_مقدار();
		إذا (clock.جابه_جایی < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.افزودن_پشت(clock);
	}

	صحيح بیتها_جابه_جایی = 0;
	لكل (خودکار &signal_json : json["signals"].array_اقلام()) {
		Signal إشارة;
		إشارة.بیتها_جابه_جایی = بیتها_جابه_جایی;
		إشارة.مسیر_2 = get_مسیر(signal_json["path"]);
		إذا (إشارة.مسیر_2.خالی())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		إذا (!signal_json["width"].is_عدد())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		إشارة.عرض = signal_json["width"].int_مقدار();
		إذا (إشارة.عرض < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		بیتها_جابه_جایی += إشارة.عرض;
		إذا (!signal_json["offset"].is_عدد())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.جابه_جایی = signal_json["offset"].int_مقدار();
		إذا (إشارة.جابه_جایی < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.init_only = signal_json["init_only"].bool_مقدار();
		سیگنالها.افزودن_پشت(إشارة);
	}

	لكل (خودکار &گام_json : json["steps"].array_اقلام()) {
		گام_2 گام;
		إذا (!گام_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_اندازه(گامها));
		گام.بیتها = گام_json["bits"].string_مقدار();
		لكل (حرف c : گام.بیتها) {
			إذا (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_اندازه(گامها));
		}
		گامها.افزودن_پشت(گام);
	}
}

RTLIL::Const ReadWitness::get_بیتها(صحيح t, صحيح بیتها_جابه_جایی, صحيح عرض) ثابت
{
	log_assert(t >= 0 && t < Get_اندازه(گامها));

	ثابت std::string &بیتها = گامها[t].بیتها;

	RTLIL::Const نتیجه(State::Sa, عرض);

	صحيح read_begin = Get_اندازه(بیتها) - 1 - بیتها_جابه_جایی;
	صحيح read_انتها = max(-1, read_begin - عرض);

	لكل (صحيح i = read_begin, j = 0; i > read_انتها; i--, j++) {
		RTLIL::State بیت = State::Sa;
		بدل (بیتها[i]) {
			حالة '0': بیت = State::S0; اكسر;
			حالة '1': بیت = State::S1; اكسر;
			حالة 'x': بیت = State::Sx; اكسر;
			حالة '?': بیت = State::Sa; اكسر;
			افتراضي:
				log_abort();
		}
		نتیجه.مجموعه(j, بیت);
	}

	أرجع نتیجه;
}

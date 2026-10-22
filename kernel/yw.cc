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

std::string Id_رستو::str() ثابت
{
	std::string نتيجو;

	لكل (خودڪار &شي : *هذا) {
		ثابت حرف *pretty = pretty_name(شي);
		إذا (pretty[0] == '[') {
			نتيجو += pretty;
			استمر;
		}
		إذا (!نتيجو.خالي_جايون())
			نتيجو += '.';
		نتيجو += pretty;
		إذا (pretty[0] == '\\' || pretty[0] == '$')
			نتيجو += ' ';
	}

	أرجع نتيجو;
}

منطقي Id_رستو::get_پتو(صحيح &addr) ثابت
{
	إذا (خالي_جايون())
		أرجع خطأ;
	خودڪار &آخري = پٺ();
	إذا (!آخري.starts_with("\\["))
		أرجع خطأ;
	إذا (آخري == "\\[0]") {
		addr = 0;
		أرجع سچ;
	}
	حرف پهريون = آخري.c_str()[2];
	إذا (پهريون < '1' || پهريون > '9')
		أرجع خطأ;
	حرف *endptr;
	addr = std::strtol(آخري.c_str() + 2, &endptr, 10);
	أرجع endptr[0] == ']' && endptr[1] == 0;
}

ساكن std::ويڪٽر<std::string> get_رستو(ثابت json11::Json &json)
{
	std::ويڪٽر<std::string> نتيجو;
	لكل (خودڪار &رستو_شي : json.array_شيون()) {
		خودڪار ثابت &رستو_شي_str = رستو_شي.string_قدر();
		إذا (رستو_شي_str.خالي_جايون())
			أرجع {};;
		نتيجو.ڌڪو_پٺ(رستو_شي_str);
	}
	أرجع نتيجو;
}

ReadWitness::ReadWitness(ثابت std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	إذا (f.fail() || Get_سائيز(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	إذا (!err.خالي_جايون())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_قدر();

	إذا (format.خالي_جايون())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	إذا (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	لكل (خودڪار &clock_json : json["clocks"].array_شيون()) {
		Clock clock;
		clock.رستو_2 = get_رستو(clock_json["path"]);
		إذا (clock.رستو_2.خالي_جايون())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		خودڪار ڪنارو_str = clock_json["edge"];
		إذا (ڪنارو_str.string_قدر() == "posedge")
			clock.is_posedge = سچ;
		وإلا إذا (ڪنارو_str.string_قدر() == "negedge")
			clock.is_negedge = سچ;
		وإلا
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		إذا (!clock_json["offset"].is_عدد())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.وٿي = clock_json["offset"].int_قدر();
		إذا (clock.وٿي < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ڌڪو_پٺ(clock);
	}

	صحيح بٽ_وٿي = 0;
	لكل (خودڪار &signal_json : json["signals"].array_شيون()) {
		Signal إشارة;
		إشارة.بٽ_وٿي = بٽ_وٿي;
		إشارة.رستو_2 = get_رستو(signal_json["path"]);
		إذا (إشارة.رستو_2.خالي_جايون())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		إذا (!signal_json["width"].is_عدد())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		إشارة.ويڪر = signal_json["width"].int_قدر();
		إذا (إشارة.ويڪر < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		بٽ_وٿي += إشارة.ويڪر;
		إذا (!signal_json["offset"].is_عدد())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.وٿي = signal_json["offset"].int_قدر();
		إذا (إشارة.وٿي < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.init_only = signal_json["init_only"].bool_قدر();
		سگنل.ڌڪو_پٺ(إشارة);
	}

	لكل (خودڪار &قدم_json : json["steps"].array_شيون()) {
		قدم_2 قدم;
		إذا (!قدم_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_سائيز(قدمن));
		قدم.بٽ = قدم_json["bits"].string_قدر();
		لكل (حرف c : قدم.بٽ) {
			إذا (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_سائيز(قدمن));
		}
		قدمن.ڌڪو_پٺ(قدم);
	}
}

RTLIL::Const ReadWitness::get_بٽ(صحيح t, صحيح بٽ_وٿي, صحيح ويڪر) ثابت
{
	log_assert(t >= 0 && t < Get_سائيز(قدمن));

	ثابت std::string &بٽ = قدمن[t].بٽ;

	RTLIL::Const نتيجو(State::Sa, ويڪر);

	صحيح read_begin = Get_سائيز(بٽ) - 1 - بٽ_وٿي;
	صحيح read_پڇاڙي = max(-1, read_begin - ويڪر);

	لكل (صحيح i = read_begin, j = 0; i > read_پڇاڙي; i--, j++) {
		RTLIL::State ٻٽو_انگ = State::Sa;
		بدل (بٽ[i]) {
			حالة '0': ٻٽو_انگ = State::S0; اكسر;
			حالة '1': ٻٽو_انگ = State::S1; اكسر;
			حالة 'x': ٻٽو_انگ = State::Sx; اكسر;
			حالة '?': ٻٽو_انگ = State::Sa; اكسر;
			افتراضي:
				log_abort();
		}
		نتيجو.سيٽ(j, ٻٽو_انگ);
	}

	أرجع نتيجو;
}

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

std::string Id_رستہ::str() ثابت
{
	std::string نتیجہ;

	لكل (خودکار &شے : *هذا) {
		ثابت حرف *pretty = pretty_name(شے);
		إذا (pretty[0] == '[') {
			نتیجہ += pretty;
			استمر;
		}
		إذا (!نتیجہ.خالی_تھاواں())
			نتیجہ += '.';
		نتیجہ += pretty;
		إذا (pretty[0] == '\\' || pretty[0] == '$')
			نتیجہ += ' ';
	}

	أرجع نتیجہ;
}

منطقي Id_رستہ::get_پتہ(صحيح &addr) ثابت
{
	إذا (خالی_تھاواں())
		أرجع خطأ;
	خودکار &آخری = پٹھ();
	إذا (!آخری.starts_with("\\["))
		أرجع خطأ;
	إذا (آخری == "\\[0]") {
		addr = 0;
		أرجع سچ;
	}
	حرف پہلا = آخری.c_str()[2];
	إذا (پہلا < '1' || پہلا > '9')
		أرجع خطأ;
	حرف *endptr;
	addr = std::strtol(آخری.c_str() + 2, &endptr, 10);
	أرجع endptr[0] == ']' && endptr[1] == 0;
}

ساكن std::سمتیہ<std::string> get_رستہ(ثابت json11::Json &json)
{
	std::سمتیہ<std::string> نتیجہ;
	لكل (خودکار &رستہ_شے : json.array_اشیاء()) {
		خودکار ثابت &رستہ_شے_str = رستہ_شے.string_قدر();
		إذا (رستہ_شے_str.خالی_تھاواں())
			أرجع {};;
		نتیجہ.ڈالو_پٹھ(رستہ_شے_str);
	}
	أرجع نتیجہ;
}

ReadWitness::ReadWitness(ثابت std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	إذا (f.fail() || Get_ناپ(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	إذا (!err.خالی_تھاواں())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_قدر();

	إذا (format.خالی_تھاواں())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	إذا (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	لكل (خودکار &clock_json : json["clocks"].array_اشیاء()) {
		Clock clock;
		clock.رستہ_2 = get_رستہ(clock_json["path"]);
		إذا (clock.رستہ_2.خالی_تھاواں())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		خودکار کنارا_str = clock_json["edge"];
		إذا (کنارا_str.string_قدر() == "posedge")
			clock.is_posedge = سچ;
		وإلا إذا (کنارا_str.string_قدر() == "negedge")
			clock.is_negedge = سچ;
		وإلا
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		إذا (!clock_json["offset"].is_نمبر())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.وتھ = clock_json["offset"].int_قدر();
		إذا (clock.وتھ < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ڈالو_پٹھ(clock);
	}

	صحيح بٹ_وتھ = 0;
	لكل (خودکار &signal_json : json["signals"].array_اشیاء()) {
		Signal إشارة;
		إشارة.بٹ_وتھ = بٹ_وتھ;
		إشارة.رستہ_2 = get_رستہ(signal_json["path"]);
		إذا (إشارة.رستہ_2.خالی_تھاواں())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		إذا (!signal_json["width"].is_نمبر())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		إشارة.چوڑائی = signal_json["width"].int_قدر();
		إذا (إشارة.چوڑائی < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		بٹ_وتھ += إشارة.چوڑائی;
		إذا (!signal_json["offset"].is_نمبر())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.وتھ = signal_json["offset"].int_قدر();
		إذا (إشارة.وتھ < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.init_only = signal_json["init_only"].bool_قدر();
		اشارے.ڈالو_پٹھ(إشارة);
	}

	لكل (خودکار &قدم_json : json["steps"].array_اشیاء()) {
		قدم_2 قدم;
		إذا (!قدم_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_ناپ(قدماں));
		قدم.بٹ = قدم_json["bits"].string_قدر();
		لكل (حرف c : قدم.بٹ) {
			إذا (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_ناپ(قدماں));
		}
		قدماں.ڈالو_پٹھ(قدم);
	}
}

RTLIL::Const ReadWitness::get_بٹ(صحيح t, صحيح بٹ_وتھ, صحيح چوڑائی) ثابت
{
	log_assert(t >= 0 && t < Get_ناپ(قدماں));

	ثابت std::string &بٹ = قدماں[t].بٹ;

	RTLIL::Const نتیجہ(State::Sa, چوڑائی);

	صحيح read_begin = Get_ناپ(بٹ) - 1 - بٹ_وتھ;
	صحيح read_سرا = max(-1, read_begin - چوڑائی);

	لكل (صحيح i = read_begin, j = 0; i > read_سرا; i--, j++) {
		RTLIL::State ثنائی_ہندسہ = State::Sa;
		بدل (بٹ[i]) {
			حالة '0': ثنائی_ہندسہ = State::S0; اكسر;
			حالة '1': ثنائی_ہندسہ = State::S1; اكسر;
			حالة 'x': ثنائی_ہندسہ = State::Sx; اكسر;
			حالة '?': ثنائی_ہندسہ = State::Sa; اكسر;
			افتراضي:
				log_abort();
		}
		نتیجہ.سیٹ(j, ثنائی_ہندسہ);
	}

	أرجع نتیجہ;
}

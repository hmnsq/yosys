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

#شامل "kernel/yw.h"
#شامل "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ساکن ثابت حرف *pretty_name(ثابت std::string &id)
{
	ثابت حرف *c_str = id.c_str();
	ثابت حرف *p = c_str;

	اگر (*p != '\\')
		واپس c_str;
	p++;

	اگر (*p == '[') {
		p++;
		جب (*p >= '0' && *p <= '9')
			p++;
		اگر (p[0] != ']' || p[1] != 0)
			واپس c_str;
		واپس c_str + 1;
	}

	اگر (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		واپس c_str;
	p++;
	جب ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	اگر (*p != 0)
		واپس c_str;
	واپس c_str + 1;
}

std::string Id_راستہ::str() ثابت
{
	std::string نتیجہ;

	برائے (خودکار &شے_2 : *یہ) {
		ثابت حرف *pretty = pretty_name(شے_2);
		اگر (pretty[0] == '[') {
			نتیجہ += pretty;
			جاری;
		}
		اگر (!نتیجہ.خالی_خانے())
			نتیجہ += '.';
		نتیجہ += pretty;
		اگر (pretty[0] == '\\' || pretty[0] == '$')
			نتیجہ += ' ';
	}

	واپس نتیجہ;
}

منطقی Id_راستہ::get_پتہ(صحیح &addr) ثابت
{
	اگر (خالی_خانے())
		واپس جھوٹ;
	خودکار &آخری = عقب();
	اگر (!آخری.starts_with("\\["))
		واپس جھوٹ;
	اگر (آخری == "\\[0]") {
		addr = 0;
		واپس سچ;
	}
	حرف پہلا = آخری.c_str()[2];
	اگر (پہلا < '1' || پہلا > '9')
		واپس جھوٹ;
	حرف *endptr;
	addr = std::strtol(آخری.c_str() + 2, &endptr, 10);
	واپس endptr[0] == ']' && endptr[1] == 0;
}

ساکن std::سمتیہ<std::string> get_راستہ(ثابت json11::Json &json)
{
	std::سمتیہ<std::string> نتیجہ;
	برائے (خودکار &راستہ_شے : json.array_اشیاء()) {
		خودکار ثابت &راستہ_شے_str = راستہ_شے.string_قدر();
		اگر (راستہ_شے_str.خالی_خانے())
			واپس {};;
		نتیجہ.ڈالو_عقب(راستہ_شے_str);
	}
	واپس نتیجہ;
}

ReadWitness::ReadWitness(ثابت std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	اگر (f.fail() || Get_سائز(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	اگر (!err.خالی_خانے())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_قدر();

	اگر (format.خالی_خانے())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	اگر (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	برائے (خودکار &clock_json : json["clocks"].array_اشیاء()) {
		Clock clock;
		clock.راستہ_2 = get_راستہ(clock_json["path"]);
		اگر (clock.راستہ_2.خالی_خانے())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		خودکار کنارہ_str = clock_json["edge"];
		اگر (کنارہ_str.string_قدر() == "posedge")
			clock.is_posedge = سچ;
		ورنہ اگر (کنارہ_str.string_قدر() == "negedge")
			clock.is_negedge = سچ;
		ورنہ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		اگر (!clock_json["offset"].is_نمبر())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.انحراف = clock_json["offset"].int_قدر();
		اگر (clock.انحراف < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ڈالو_عقب(clock);
	}

	صحیح بٹ_انحراف = 0;
	برائے (خودکار &signal_json : json["signals"].array_اشیاء()) {
		Signal اشارہ;
		اشارہ.بٹ_انحراف = بٹ_انحراف;
		اشارہ.راستہ_2 = get_راستہ(signal_json["path"]);
		اگر (اشارہ.راستہ_2.خالی_خانے())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		اگر (!signal_json["width"].is_نمبر())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		اشارہ.چوڑائی = signal_json["width"].int_قدر();
		اگر (اشارہ.چوڑائی < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		بٹ_انحراف += اشارہ.چوڑائی;
		اگر (!signal_json["offset"].is_نمبر())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		اشارہ.انحراف = signal_json["offset"].int_قدر();
		اگر (اشارہ.انحراف < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		اشارہ.init_only = signal_json["init_only"].bool_قدر();
		اشارات.ڈالو_عقب(اشارہ);
	}

	برائے (خودکار &قدم_json : json["steps"].array_اشیاء()) {
		قدم_2 قدم;
		اگر (!قدم_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_سائز(اقدام));
		قدم.بٹ = قدم_json["bits"].string_قدر();
		برائے (حرف c : قدم.بٹ) {
			اگر (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_سائز(اقدام));
		}
		اقدام.ڈالو_عقب(قدم);
	}
}

RTLIL::Const ReadWitness::get_بٹ(صحیح t, صحیح بٹ_انحراف, صحیح چوڑائی) ثابت
{
	log_assert(t >= 0 && t < Get_سائز(اقدام));

	ثابت std::string &بٹ = اقدام[t].بٹ;

	RTLIL::Const نتیجہ(State::Sa, چوڑائی);

	صحیح read_begin = Get_سائز(بٹ) - 1 - بٹ_انحراف;
	صحیح read_انتہا = max(-1, read_begin - چوڑائی);

	برائے (صحیح i = read_begin, j = 0; i > read_انتہا; i--, j++) {
		RTLIL::State ثنائی_ہندسہ = State::Sa;
		انتخاب (بٹ[i]) {
			حالت '0': ثنائی_ہندسہ = State::S0; توڑو;
			حالت '1': ثنائی_ہندسہ = State::S1; توڑو;
			حالت 'x': ثنائی_ہندسہ = State::Sx; توڑو;
			حالت '?': ثنائی_ہندسہ = State::Sa; توڑو;
			طےشدہ:
				log_abort();
		}
		نتیجہ.سیٹ(j, ثنائی_ہندسہ);
	}

	واپس نتیجہ;
}

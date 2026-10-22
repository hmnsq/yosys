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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yw.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *pretty_name(ਅਚਲ std::string &id)
{
	ਅਚਲ ਅੱਖਰ *c_str = id.c_str();
	ਅਚਲ ਅੱਖਰ *p = c_str;

	ਜੇ (*p != '\\')
		ਵਾਪਸ c_str;
	p++;

	ਜੇ (*p == '[') {
		p++;
		ਜਦੋਂ (*p >= '0' && *p <= '9')
			p++;
		ਜੇ (p[0] != ']' || p[1] != 0)
			ਵਾਪਸ c_str;
		ਵਾਪਸ c_str + 1;
	}

	ਜੇ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ਵਾਪਸ c_str;
	p++;
	ਜਦੋਂ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ਜੇ (*p != 0)
		ਵਾਪਸ c_str;
	ਵਾਪਸ c_str + 1;
}

std::string Id_ਰਸਤਾ::str() ਅਚਲ
{
	std::string ਨਤੀਜਾ;

	ਲਈ (ਸਵੈਚਾਲਿਤ &ਵਸਤੂ_2 : *ਇਹ) {
		ਅਚਲ ਅੱਖਰ *pretty = pretty_name(ਵਸਤੂ_2);
		ਜੇ (pretty[0] == '[') {
			ਨਤੀਜਾ += pretty;
			ਜਾਰੀ;
		}
		ਜੇ (!ਨਤੀਜਾ.ਖਾਲੀ_ਥਾਵਾਂ())
			ਨਤੀਜਾ += '.';
		ਨਤੀਜਾ += pretty;
		ਜੇ (pretty[0] == '\\' || pretty[0] == '$')
			ਨਤੀਜਾ += ' ';
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਤਾਰਕਿਕ Id_ਰਸਤਾ::get_ਪਤਾ(ਪੂਰਨ &addr) ਅਚਲ
{
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ ਝੂਠ;
	ਸਵੈਚਾਲਿਤ &ਆਖਰੀ = ਪਿੱਠ();
	ਜੇ (!ਆਖਰੀ.starts_with("\\["))
		ਵਾਪਸ ਝੂਠ;
	ਜੇ (ਆਖਰੀ == "\\[0]") {
		addr = 0;
		ਵਾਪਸ ਸੱਚ;
	}
	ਅੱਖਰ ਪਹਿਲਾ = ਆਖਰੀ.c_str()[2];
	ਜੇ (ਪਹਿਲਾ < '1' || ਪਹਿਲਾ > '9')
		ਵਾਪਸ ਝੂਠ;
	ਅੱਖਰ *endptr;
	addr = std::strtol(ਆਖਰੀ.c_str() + 2, &endptr, 10);
	ਵਾਪਸ endptr[0] == ']' && endptr[1] == 0;
}

ਸਥਿਰ std::ਸਦਿਸ਼<std::string> get_ਰਸਤਾ(ਅਚਲ json11::Json &json)
{
	std::ਸਦਿਸ਼<std::string> ਨਤੀਜਾ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਰਸਤਾ_ਵਸਤੂ : json.array_ਚੀਜ਼ਾਂ()) {
		ਸਵੈਚਾਲਿਤ ਅਚਲ &ਰਸਤਾ_ਵਸਤੂ_str = ਰਸਤਾ_ਵਸਤੂ.string_ਮੁੱਲ();
		ਜੇ (ਰਸਤਾ_ਵਸਤੂ_str.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ {};;
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(ਰਸਤਾ_ਵਸਤੂ_str);
	}
	ਵਾਪਸ ਨਤੀਜਾ;
}

ReadWitness::ReadWitness(ਅਚਲ std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ਜੇ (f.fail() || Get_ਮਾਪ(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ਜੇ (!err.ਖਾਲੀ_ਥਾਵਾਂ())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_ਮੁੱਲ();

	ਜੇ (format.ਖਾਲੀ_ਥਾਵਾਂ())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ਜੇ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	ਲਈ (ਸਵੈਚਾਲਿਤ &clock_json : json["clocks"].array_ਚੀਜ਼ਾਂ()) {
		Clock clock;
		clock.ਰਸਤਾ_2 = get_ਰਸਤਾ(clock_json["path"]);
		ਜੇ (clock.ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		ਸਵੈਚਾਲਿਤ ਕਿਨਾਰਾ_str = clock_json["edge"];
		ਜੇ (ਕਿਨਾਰਾ_str.string_ਮੁੱਲ() == "posedge")
			clock.is_posedge = ਸੱਚ;
		ਨਹੀਂਤਾਂ ਜੇ (ਕਿਨਾਰਾ_str.string_ਮੁੱਲ() == "negedge")
			clock.is_negedge = ਸੱਚ;
		ਨਹੀਂਤਾਂ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ਜੇ (!clock_json["offset"].is_ਸੰਖਿਆ())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.ਵਿੱਥ = clock_json["offset"].int_ਮੁੱਲ();
		ਜੇ (clock.ਵਿੱਥ < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ਧੱਕੋ_ਪਿੱਠ(clock);
	}

	ਪੂਰਨ ਬਿੱਟ_ਵਿੱਥ = 0;
	ਲਈ (ਸਵੈਚਾਲਿਤ &signal_json : json["signals"].array_ਚੀਜ਼ਾਂ()) {
		Signal ਸੰਕੇਤ;
		ਸੰਕੇਤ.ਬਿੱਟ_ਵਿੱਥ = ਬਿੱਟ_ਵਿੱਥ;
		ਸੰਕੇਤ.ਰਸਤਾ_2 = get_ਰਸਤਾ(signal_json["path"]);
		ਜੇ (ਸੰਕੇਤ.ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ਜੇ (!signal_json["width"].is_ਸੰਖਿਆ())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		ਸੰਕੇਤ.ਚੌੜਾਈ = signal_json["width"].int_ਮੁੱਲ();
		ਜੇ (ਸੰਕੇਤ.ਚੌੜਾਈ < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		ਬਿੱਟ_ਵਿੱਥ += ਸੰਕੇਤ.ਚੌੜਾਈ;
		ਜੇ (!signal_json["offset"].is_ਸੰਖਿਆ())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		ਸੰਕੇਤ.ਵਿੱਥ = signal_json["offset"].int_ਮੁੱਲ();
		ਜੇ (ਸੰਕੇਤ.ਵਿੱਥ < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		ਸੰਕੇਤ.init_only = signal_json["init_only"].bool_ਮੁੱਲ();
		ਸੰਕੇਤ_2.ਧੱਕੋ_ਪਿੱਠ(ਸੰਕੇਤ);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ &ਕਦਮ_json : json["steps"].array_ਚੀਜ਼ਾਂ()) {
		ਕਦਮ_2 ਕਦਮ;
		ਜੇ (!ਕਦਮ_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_ਮਾਪ(ਕਦਮਾਂ));
		ਕਦਮ.ਬਿੱਟ = ਕਦਮ_json["bits"].string_ਮੁੱਲ();
		ਲਈ (ਅੱਖਰ c : ਕਦਮ.ਬਿੱਟ) {
			ਜੇ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_ਮਾਪ(ਕਦਮਾਂ));
		}
		ਕਦਮਾਂ.ਧੱਕੋ_ਪਿੱਠ(ਕਦਮ);
	}
}

RTLIL::Const ReadWitness::get_ਬਿੱਟ(ਪੂਰਨ t, ਪੂਰਨ ਬਿੱਟ_ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ) ਅਚਲ
{
	log_assert(t >= 0 && t < Get_ਮਾਪ(ਕਦਮਾਂ));

	ਅਚਲ std::string &ਬਿੱਟ = ਕਦਮਾਂ[t].ਬਿੱਟ;

	RTLIL::Const ਨਤੀਜਾ(State::Sa, ਚੌੜਾਈ);

	ਪੂਰਨ read_begin = Get_ਮਾਪ(ਬਿੱਟ) - 1 - ਬਿੱਟ_ਵਿੱਥ;
	ਪੂਰਨ read_ਸਿਰਾ = max(-1, read_begin - ਚੌੜਾਈ);

	ਲਈ (ਪੂਰਨ i = read_begin, j = 0; i > read_ਸਿਰਾ; i--, j++) {
		RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ = State::Sa;
		ਚੋਣ (ਬਿੱਟ[i]) {
			ਹਾਲਤ '0': ਦੋਆਧਾਰੀ_ਅੰਕ = State::S0; ਰੋਕੋ;
			ਹਾਲਤ '1': ਦੋਆਧਾਰੀ_ਅੰਕ = State::S1; ਰੋਕੋ;
			ਹਾਲਤ 'x': ਦੋਆਧਾਰੀ_ਅੰਕ = State::Sx; ਰੋਕੋ;
			ਹਾਲਤ '?': ਦੋਆਧਾਰੀ_ਅੰਕ = State::Sa; ਰੋਕੋ;
			ਮੂਲ:
				log_abort();
		}
		ਨਤੀਜਾ.ਸੈੱਟ(j, ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

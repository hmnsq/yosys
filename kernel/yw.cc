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

#ಅನ್ತರ್ಭಾವ "kernel/yw.h"
#ಅನ್ತರ್ಭಾವ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pretty_name(ಸ್ಥಿರಾಂಕ std::string &id)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c_str = id.c_str();
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = c_str;

	ಆದರೆ (*p != '\\')
		ಹಿಂತಿರುಗಿ c_str;
	p++;

	ಆದರೆ (*p == '[') {
		p++;
		ಆಗ (*p >= '0' && *p <= '9')
			p++;
		ಆದರೆ (p[0] != ']' || p[1] != 0)
			ಹಿಂತಿರುಗಿ c_str;
		ಹಿಂತಿರುಗಿ c_str + 1;
	}

	ಆದರೆ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ಹಿಂತಿರುಗಿ c_str;
	p++;
	ಆಗ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ಆದರೆ (*p != 0)
		ಹಿಂತಿರುಗಿ c_str;
	ಹಿಂತಿರುಗಿ c_str + 1;
}

std::string Id_ಮಾರ್ಗ::str() ಸ್ಥಿರಾಂಕ
{
	std::string ಫಲಿತಾಂಶ;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಅಂಶ : *ಇದು) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pretty = pretty_name(ಅಂಶ);
		ಆದರೆ (pretty[0] == '[') {
			ಫಲಿತಾಂಶ += pretty;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (!ಫಲಿತಾಂಶ.ಖಾಲಿಗಳು())
			ಫಲಿತಾಂಶ += '.';
		ಫಲಿತಾಂಶ += pretty;
		ಆದರೆ (pretty[0] == '\\' || pretty[0] == '$')
			ಫಲಿತಾಂಶ += ' ';
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ತಾರ್ಕಿಕ Id_ಮಾರ್ಗ::get_ವಿಳಾಸ(ಪೂರ್ಣಾಂಕ &addr) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಸ್ವಯಂಚಾಲಿತ &ಕೊನೆಯ = ಹಿಂಭಾಗ();
	ಆದರೆ (!ಕೊನೆಯ.starts_with("\\["))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (ಕೊನೆಯ == "\\[0]") {
		addr = 0;
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}
	ಅಕ್ಷರ ಮೊದಲ = ಕೊನೆಯ.c_str()[2];
	ಆದರೆ (ಮೊದಲ < '1' || ಮೊದಲ > '9')
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಕ್ಷರ *endptr;
	addr = std::strtol(ಕೊನೆಯ.c_str() + 2, &endptr, 10);
	ಹಿಂತಿರುಗಿ endptr[0] == ']' && endptr[1] == 0;
}

ಸ್ಥಿರ std::ಸದಿಶ<std::string> get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ json11::Json &json)
{
	std::ಸದಿಶ<std::string> ಫಲಿತಾಂಶ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಮಾರ್ಗ_ಅಂಶ : json.array_ವಸ್ತುಗಳು()) {
		ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಮಾರ್ಗ_ಅಂಶ_str = ಮಾರ್ಗ_ಅಂಶ.string_ಮೌಲ್ಯ();
		ಆದರೆ (ಮಾರ್ಗ_ಅಂಶ_str.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ {};;
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಮಾರ್ಗ_ಅಂಶ_str);
	}
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ReadWitness::ReadWitness(ಸ್ಥಿರಾಂಕ std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ಆದರೆ (f.fail() || Get_ಗಾತ್ರ(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ಆದರೆ (!err.ಖಾಲಿಗಳು())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_ಮೌಲ್ಯ();

	ಆದರೆ (format.ಖಾಲಿಗಳು())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ಆದರೆ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &clock_json : json["clocks"].array_ವಸ್ತುಗಳು()) {
		Clock clock;
		clock.ಮಾರ್ಗ_2 = get_ಮಾರ್ಗ(clock_json["path"]);
		ಆದರೆ (clock.ಮಾರ್ಗ_2.ಖಾಲಿಗಳು())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		ಸ್ವಯಂಚಾಲಿತ ಅಂಚು_str = clock_json["edge"];
		ಆದರೆ (ಅಂಚು_str.string_ಮೌಲ್ಯ() == "posedge")
			clock.is_posedge = ನಿಜ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಚು_str.string_ಮೌಲ್ಯ() == "negedge")
			clock.is_negedge = ನಿಜ;
		ಇಲ್ಲದಿದ್ದರೆ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ಆದರೆ (!clock_json["offset"].is_ಸಂಖ್ಯೆ())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.ಸ್ಥಾನಾಂತರ = clock_json["offset"].int_ಮೌಲ್ಯ();
		ಆದರೆ (clock.ಸ್ಥಾನಾಂತರ < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ತಳ್ಳು_ಹಿಂಭಾಗ(clock);
	}

	ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ = 0;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &signal_json : json["signals"].array_ವಸ್ತುಗಳು()) {
		Signal ಸಂಕೇತ;
		ಸಂಕೇತ.ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ = ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ;
		ಸಂಕೇತ.ಮಾರ್ಗ_2 = get_ಮಾರ್ಗ(signal_json["path"]);
		ಆದರೆ (ಸಂಕೇತ.ಮಾರ್ಗ_2.ಖಾಲಿಗಳು())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ಆದರೆ (!signal_json["width"].is_ಸಂಖ್ಯೆ())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		ಸಂಕೇತ.ಅಗಲ = signal_json["width"].int_ಮೌಲ್ಯ();
		ಆದರೆ (ಸಂಕೇತ.ಅಗಲ < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ += ಸಂಕೇತ.ಅಗಲ;
		ಆದರೆ (!signal_json["offset"].is_ಸಂಖ್ಯೆ())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		ಸಂಕೇತ.ಸ್ಥಾನಾಂತರ = signal_json["offset"].int_ಮೌಲ್ಯ();
		ಆದರೆ (ಸಂಕೇತ.ಸ್ಥಾನಾಂತರ < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		ಸಂಕೇತ.init_only = signal_json["init_only"].bool_ಮೌಲ್ಯ();
		ಸಂಕೇತಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಸಂಕೇತ);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಹೆಜ್ಜೆ_json : json["steps"].array_ವಸ್ತುಗಳು()) {
		ಹೆಜ್ಜೆ_2 ಹೆಜ್ಜೆ;
		ಆದರೆ (!ಹೆಜ್ಜೆ_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_ಗಾತ್ರ(ಹೆಜ್ಜೆಗಳು));
		ಹೆಜ್ಜೆ.ಬಿಟ್ಟುಗಳು = ಹೆಜ್ಜೆ_json["bits"].string_ಮೌಲ್ಯ();
		ಗಾಗಿ (ಅಕ್ಷರ c : ಹೆಜ್ಜೆ.ಬಿಟ್ಟುಗಳು) {
			ಆದರೆ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_ಗಾತ್ರ(ಹೆಜ್ಜೆಗಳು));
		}
		ಹೆಜ್ಜೆಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಹೆಜ್ಜೆ);
	}
}

RTLIL::Const ReadWitness::get_ಬಿಟ್ಟುಗಳು(ಪೂರ್ಣಾಂಕ t, ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ) ಸ್ಥಿರಾಂಕ
{
	log_assert(t >= 0 && t < Get_ಗಾತ್ರ(ಹೆಜ್ಜೆಗಳು));

	ಸ್ಥಿರಾಂಕ std::string &ಬಿಟ್ಟುಗಳು = ಹೆಜ್ಜೆಗಳು[t].ಬಿಟ್ಟುಗಳು;

	RTLIL::Const ಫಲಿತಾಂಶ(State::Sa, ಅಗಲ);

	ಪೂರ್ಣಾಂಕ read_begin = Get_ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು) - 1 - ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ read_ಕೊನೆ = max(-1, read_begin - ಅಗಲ);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = read_begin, j = 0; i > read_ಕೊನೆ; i--, j++) {
		RTLIL::State ಬಿಟ್ = State::Sa;
		ಆಯ್ಕೆ (ಬಿಟ್ಟುಗಳು[i]) {
			ಸ್ಥಿತಿ '0': ಬಿಟ್ = State::S0; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '1': ಬಿಟ್ = State::S1; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'x': ಬಿಟ್ = State::Sx; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '?': ಬಿಟ್ = State::Sa; ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				log_abort();
		}
		ಫಲಿತಾಂಶ.ಗಣ(j, ಬಿಟ್);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

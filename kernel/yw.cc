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
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *pretty_name(ಧ್ರುವ std::string &id)
{
	ಧ್ರುವ ವರ್ಣ *c_str = id.c_str();
	ಧ್ರುವ ವರ್ಣ *p = c_str;

	ಯದಿ (*p != '\\')
		ಪ್ರತ್ಯಾಗಮ c_str;
	p++;

	ಯದಿ (*p == '[') {
		p++;
		ಯಾವತ್ (*p >= '0' && *p <= '9')
			p++;
		ಯದಿ (p[0] != ']' || p[1] != 0)
			ಪ್ರತ್ಯಾಗಮ c_str;
		ಪ್ರತ್ಯಾಗಮ c_str + 1;
	}

	ಯದಿ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ಪ್ರತ್ಯಾಗಮ c_str;
	p++;
	ಯಾವತ್ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ಯದಿ (*p != 0)
		ಪ್ರತ್ಯಾಗಮ c_str;
	ಪ್ರತ್ಯಾಗಮ c_str + 1;
}

std::string IdPath::str() ಧ್ರುವ
{
	std::string result;

	ಪ್ರತಿ (auto &item : *ಏತತ್) {
		ಧ್ರುವ ವರ್ಣ *pretty = pretty_name(item);
		ಯದಿ (pretty[0] == '[') {
			result += pretty;
			ಅನುವರ್ತ;
		}
		ಯದಿ (!result.empty())
			result += '.';
		result += pretty;
		ಯದಿ (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

ತಾರ್ಕಿಕ IdPath::get_address(ಪೂರ್ಣಾಙ್ಕ &addr) ಧ್ರುವ
{
	ಯದಿ (empty())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	auto &last = back();
	ಯದಿ (!last.starts_with("\\["))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (last == "\\[0]") {
		addr = 0;
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}
	ವರ್ಣ first = last.c_str()[2];
	ಯದಿ (first < '1' || first > '9')
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ವರ್ಣ *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	ಪ್ರತ್ಯಾಗಮ endptr[0] == ']' && endptr[1] == 0;
}

ಸ್ಥಿರ std::vector<std::string> get_path(ಧ್ರುವ json11::Json &json)
{
	std::vector<std::string> result;
	ಪ್ರತಿ (auto &path_item : json.array_ವಸ್ತುಗಳು()) {
		auto ಧ್ರುವ &path_item_str = path_item.string_ಮೌಲ್ಯ();
		ಯದಿ (path_item_str.empty())
			ಪ್ರತ್ಯಾಗಮ {};;
		result.ತಳ್ಳು_back(path_item_str);
	}
	ಪ್ರತ್ಯಾಗಮ result;
}

ReadWitness::ReadWitness(ಧ್ರುವ std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ಯದಿ (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ಯದಿ (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_ಮೌಲ್ಯ();

	ಯದಿ (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ಯದಿ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	ಪ್ರತಿ (auto &clock_json : json["clocks"].array_ವಸ್ತುಗಳು()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		ಯದಿ (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		ಯದಿ (edge_str.string_ಮೌಲ್ಯ() == "posedge")
			clock.is_posedge = ಸತ್ಯ;
		ಅನ್ಯಥಾ ಯದಿ (edge_str.string_ಮೌಲ್ಯ() == "negedge")
			clock.is_negedge = ಸತ್ಯ;
		ಅನ್ಯಥಾ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ಯದಿ (!clock_json["offset"].is_ಸಂಖ್ಯೆ())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_ಮೌಲ್ಯ();
		ಯದಿ (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ತಳ್ಳು_back(clock);
	}

	ಪೂರ್ಣಾಙ್ಕ bits_offset = 0;
	ಪ್ರತಿ (auto &signal_json : json["signals"].array_ವಸ್ತುಗಳು()) {
		Signal ಸಙ್ಕೇತ;
		ಸಙ್ಕೇತ.bits_offset = bits_offset;
		ಸಙ್ಕೇತ.path = get_path(signal_json["path"]);
		ಯದಿ (ಸಙ್ಕೇತ.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ಯದಿ (!signal_json["width"].is_ಸಂಖ್ಯೆ())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		ಸಙ್ಕೇತ.width = signal_json["width"].int_ಮೌಲ್ಯ();
		ಯದಿ (ಸಙ್ಕೇತ.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += ಸಙ್ಕೇತ.width;
		ಯದಿ (!signal_json["offset"].is_ಸಂಖ್ಯೆ())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		ಸಙ್ಕೇತ.offset = signal_json["offset"].int_ಮೌಲ್ಯ();
		ಯದಿ (ಸಙ್ಕೇತ.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		ಸಙ್ಕೇತ.init_only = signal_json["init_only"].bool_ಮೌಲ್ಯ();
		signals.ತಳ್ಳು_back(ಸಙ್ಕೇತ);
	}

	ಪ್ರತಿ (auto &step_json : json["steps"].array_ವಸ್ತುಗಳು()) {
		Step step;
		ಯದಿ (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_ಮೌಲ್ಯ();
		ಪ್ರತಿ (ವರ್ಣ c : step.bits) {
			ಯದಿ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.ತಳ್ಳು_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(ಪೂರ್ಣಾಙ್ಕ t, ಪೂರ್ಣಾಙ್ಕ bits_offset, ಪೂರ್ಣಾಙ್ಕ width) ಧ್ರುವ
{
	log_assert(t >= 0 && t < GetSize(steps));

	ಧ್ರುವ std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	ಪೂರ್ಣಾಙ್ಕ read_begin = GetSize(bits) - 1 - bits_offset;
	ಪೂರ್ಣಾಙ್ಕ read_end = max(-1, read_begin - width);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		ವಿಕಲ್ಪ (bits[i]) {
			ಪಕ್ಷ '0': bit = State::S0; ಭಙ್ಗ;
			ಪಕ್ಷ '1': bit = State::S1; ಭಙ್ಗ;
			ಪಕ್ಷ 'x': bit = State::Sx; ಭಙ್ಗ;
			ಪಕ್ಷ '?': bit = State::Sa; ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				log_abort();
		}
		result.set(j, bit);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

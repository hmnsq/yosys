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

#അന്തര്ഭാവ "kernel/yw.h"
#അന്തര്ഭാവ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
സ്ഥിതം സ്ഥിരം അക്ഷരം *pretty_name(സ്ഥിരം std::string &id)
{
	സ്ഥിരം അക്ഷരം *c_str = id.c_str();
	സ്ഥിരം അക്ഷരം *p = c_str;

	എങ്കിൽ (*p != '\\')
		തിരികെ c_str;
	p++;

	എങ്കിൽ (*p == '[') {
		p++;
		ആകുമ്പോൾ (*p >= '0' && *p <= '9')
			p++;
		എങ്കിൽ (p[0] != ']' || p[1] != 0)
			തിരികെ c_str;
		തിരികെ c_str + 1;
	}

	എങ്കിൽ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		തിരികെ c_str;
	p++;
	ആകുമ്പോൾ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	എങ്കിൽ (*p != 0)
		തിരികെ c_str;
	തിരികെ c_str + 1;
}

std::string IdPath::str() സ്ഥിരം
{
	std::string result;

	വേണ്ടി (auto &item : *ഇത്) {
		സ്ഥിരം അക്ഷരം *pretty = pretty_name(item);
		എങ്കിൽ (pretty[0] == '[') {
			result += pretty;
			തുടരുക;
		}
		എങ്കിൽ (!result.empty())
			result += '.';
		result += pretty;
		എങ്കിൽ (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	തിരികെ result;
}

തർക്കം IdPath::get_address(പൂർണ്ണസംഖ്യ &addr) സ്ഥിരം
{
	എങ്കിൽ (empty())
		തിരികെ അസത്യം;
	auto &last = back();
	എങ്കിൽ (!last.starts_with("\\["))
		തിരികെ അസത്യം;
	എങ്കിൽ (last == "\\[0]") {
		addr = 0;
		തിരികെ സത്യം;
	}
	അക്ഷരം first = last.c_str()[2];
	എങ്കിൽ (first < '1' || first > '9')
		തിരികെ അസത്യം;
	അക്ഷരം *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	തിരികെ endptr[0] == ']' && endptr[1] == 0;
}

സ്ഥിതം std::vector<std::string> get_path(സ്ഥിരം json11::Json &json)
{
	std::vector<std::string> result;
	വേണ്ടി (auto &path_item : json.array_ഇനങ്ങൾ()) {
		auto സ്ഥിരം &path_item_str = path_item.string_മൂല്യം();
		എങ്കിൽ (path_item_str.empty())
			തിരികെ {};;
		result.തള്ളുക_back(path_item_str);
	}
	തിരികെ result;
}

ReadWitness::ReadWitness(സ്ഥിരം std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	എങ്കിൽ (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	എങ്കിൽ (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_മൂല്യം();

	എങ്കിൽ (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	എങ്കിൽ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	വേണ്ടി (auto &clock_json : json["clocks"].array_ഇനങ്ങൾ()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		എങ്കിൽ (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		എങ്കിൽ (edge_str.string_മൂല്യം() == "posedge")
			clock.is_posedge = സത്യം;
		അല്ലെങ്കിൽ എങ്കിൽ (edge_str.string_മൂല്യം() == "negedge")
			clock.is_negedge = സത്യം;
		അല്ലെങ്കിൽ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		എങ്കിൽ (!clock_json["offset"].is_സംഖ്യ())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_മൂല്യം();
		എങ്കിൽ (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.തള്ളുക_back(clock);
	}

	പൂർണ്ണസംഖ്യ bits_offset = 0;
	വേണ്ടി (auto &signal_json : json["signals"].array_ഇനങ്ങൾ()) {
		Signal സൂചന;
		സൂചന.bits_offset = bits_offset;
		സൂചന.path = get_path(signal_json["path"]);
		എങ്കിൽ (സൂചന.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		എങ്കിൽ (!signal_json["width"].is_സംഖ്യ())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		സൂചന.width = signal_json["width"].int_മൂല്യം();
		എങ്കിൽ (സൂചന.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += സൂചന.width;
		എങ്കിൽ (!signal_json["offset"].is_സംഖ്യ())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		സൂചന.offset = signal_json["offset"].int_മൂല്യം();
		എങ്കിൽ (സൂചന.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		സൂചന.init_only = signal_json["init_only"].bool_മൂല്യം();
		signals.തള്ളുക_back(സൂചന);
	}

	വേണ്ടി (auto &step_json : json["steps"].array_ഇനങ്ങൾ()) {
		Step step;
		എങ്കിൽ (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_മൂല്യം();
		വേണ്ടി (അക്ഷരം c : step.bits) {
			എങ്കിൽ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.തള്ളുക_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(പൂർണ്ണസംഖ്യ t, പൂർണ്ണസംഖ്യ bits_offset, പൂർണ്ണസംഖ്യ width) സ്ഥിരം
{
	log_assert(t >= 0 && t < GetSize(steps));

	സ്ഥിരം std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	പൂർണ്ണസംഖ്യ read_begin = GetSize(bits) - 1 - bits_offset;
	പൂർണ്ണസംഖ്യ read_end = max(-1, read_begin - width);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		തിരഞ്ഞെടുപ്പ് (bits[i]) {
			അവസ്ഥ '0': bit = State::S0; നിർത്തുക;
			അവസ്ഥ '1': bit = State::S1; നിർത്തുക;
			അവസ്ഥ 'x': bit = State::Sx; നിർത്തുക;
			അവസ്ഥ '?': bit = State::Sa; നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				log_abort();
		}
		result.set(j, bit);
	}

	തിരികെ result;
}

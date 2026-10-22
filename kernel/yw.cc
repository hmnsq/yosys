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

#अन्तर्भाव "kernel/yw.h"
#अन्तर्भाव "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
स्थिर ध्रुव वर्ण *pretty_name(ध्रुव std::string &id)
{
	ध्रुव वर्ण *c_str = id.c_str();
	ध्रुव वर्ण *p = c_str;

	जइ (*p != '\\')
		प्रत्यागम c_str;
	p++;

	जइ (*p == '[') {
		p++;
		जाव (*p >= '0' && *p <= '9')
			p++;
		जइ (p[0] != ']' || p[1] != 0)
			प्रत्यागम c_str;
		प्रत्यागम c_str + 1;
	}

	जइ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		प्रत्यागम c_str;
	p++;
	जाव ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	जइ (*p != 0)
		प्रत्यागम c_str;
	प्रत्यागम c_str + 1;
}

std::string IdPath::str() ध्रुव
{
	std::string result;

	प्रति (auto &item : *एतत्) {
		ध्रुव वर्ण *pretty = pretty_name(item);
		जइ (pretty[0] == '[') {
			result += pretty;
			अनुवर्त;
		}
		जइ (!result.empty())
			result += '.';
		result += pretty;
		जइ (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	प्रत्यागम result;
}

तार्किक IdPath::get_address(पूर्णाङ्क &addr) ध्रुव
{
	जइ (empty())
		प्रत्यागम मिच्छ;
	auto &last = back();
	जइ (!last.starts_with("\\["))
		प्रत्यागम मिच्छ;
	जइ (last == "\\[0]") {
		addr = 0;
		प्रत्यागम सच्च;
	}
	वर्ण first = last.c_str()[2];
	जइ (first < '1' || first > '9')
		प्रत्यागम मिच्छ;
	वर्ण *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	प्रत्यागम endptr[0] == ']' && endptr[1] == 0;
}

स्थिर std::vector<std::string> get_path(ध्रुव json11::Json &json)
{
	std::vector<std::string> result;
	प्रति (auto &path_item : json.array_वस्तूनि()) {
		auto ध्रुव &path_item_str = path_item.string_मानम्();
		जइ (path_item_str.empty())
			प्रत्यागम {};;
		result.प्रक्षिप_back(path_item_str);
	}
	प्रत्यागम result;
}

ReadWitness::ReadWitness(ध्रुव std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	जइ (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	जइ (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_मानम्();

	जइ (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	जइ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	प्रति (auto &clock_json : json["clocks"].array_वस्तूनि()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		जइ (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		जइ (edge_str.string_मानम्() == "posedge")
			clock.is_posedge = सच्च;
		अण्णहा जइ (edge_str.string_मानम्() == "negedge")
			clock.is_negedge = सच्च;
		अण्णहा
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		जइ (!clock_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_मानम्();
		जइ (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.प्रक्षिप_back(clock);
	}

	पूर्णाङ्क bits_offset = 0;
	प्रति (auto &signal_json : json["signals"].array_वस्तूनि()) {
		Signal सङ्केत;
		सङ्केत.bits_offset = bits_offset;
		सङ्केत.path = get_path(signal_json["path"]);
		जइ (सङ्केत.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		जइ (!signal_json["width"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.width = signal_json["width"].int_मानम्();
		जइ (सङ्केत.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += सङ्केत.width;
		जइ (!signal_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.offset = signal_json["offset"].int_मानम्();
		जइ (सङ्केत.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.init_only = signal_json["init_only"].bool_मानम्();
		signals.प्रक्षिप_back(सङ्केत);
	}

	प्रति (auto &step_json : json["steps"].array_वस्तूनि()) {
		Step step;
		जइ (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_मानम्();
		प्रति (वर्ण c : step.bits) {
			जइ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.प्रक्षिप_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(पूर्णाङ्क t, पूर्णाङ्क bits_offset, पूर्णाङ्क width) ध्रुव
{
	log_assert(t >= 0 && t < GetSize(steps));

	ध्रुव std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	पूर्णाङ्क read_begin = GetSize(bits) - 1 - bits_offset;
	पूर्णाङ्क read_end = max(-1, read_begin - width);

	प्रति (पूर्णाङ्क i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		विकल्प (bits[i]) {
			पक्ष '0': bit = State::S0; भङ्ग;
			पक्ष '1': bit = State::S1; भङ्ग;
			पक्ष 'x': bit = State::Sx; भङ्ग;
			पक्ष '?': bit = State::Sa; भङ्ग;
			स्वभाव:
				log_abort();
		}
		result.set(j, bit);
	}

	प्रत्यागम result;
}

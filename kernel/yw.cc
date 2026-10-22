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

	सचे (*p != '\\')
		पटिनिवत्त c_str;
	p++;

	सचे (*p == '[') {
		p++;
		याव (*p >= '0' && *p <= '9')
			p++;
		सचे (p[0] != ']' || p[1] != 0)
			पटिनिवत्त c_str;
		पटिनिवत्त c_str + 1;
	}

	सचे (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		पटिनिवत्त c_str;
	p++;
	याव ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	सचे (*p != 0)
		पटिनिवत्त c_str;
	पटिनिवत्त c_str + 1;
}

std::string IdPath::str() ध्रुव
{
	std::string result;

	अत्थाय (auto &item : *एतत्) {
		ध्रुव वर्ण *pretty = pretty_name(item);
		सचे (pretty[0] == '[') {
			result += pretty;
			अनुवर्त;
		}
		सचे (!result.empty())
			result += '.';
		result += pretty;
		सचे (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	पटिनिवत्त result;
}

तार्किक IdPath::get_address(पूर्णाङ्क &addr) ध्रुव
{
	सचे (empty())
		पटिनिवत्त मिच्छा;
	auto &last = back();
	सचे (!last.starts_with("\\["))
		पटिनिवत्त मिच्छा;
	सचे (last == "\\[0]") {
		addr = 0;
		पटिनिवत्त सच्च;
	}
	वर्ण first = last.c_str()[2];
	सचे (first < '1' || first > '9')
		पटिनिवत्त मिच्छा;
	वर्ण *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	पटिनिवत्त endptr[0] == ']' && endptr[1] == 0;
}

स्थिर std::vector<std::string> get_path(ध्रुव json11::Json &json)
{
	std::vector<std::string> result;
	अत्थाय (auto &path_item : json.array_वस्तूनि()) {
		auto ध्रुव &path_item_str = path_item.string_मानम्();
		सचे (path_item_str.empty())
			पटिनिवत्त {};;
		result.प्रक्षिप_back(path_item_str);
	}
	पटिनिवत्त result;
}

ReadWitness::ReadWitness(ध्रुव std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	सचे (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	सचे (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_मानम्();

	सचे (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	सचे (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	अत्थाय (auto &clock_json : json["clocks"].array_वस्तूनि()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		सचे (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		सचे (edge_str.string_मानम्() == "posedge")
			clock.is_posedge = सच्च;
		अञ्ञथा सचे (edge_str.string_मानम्() == "negedge")
			clock.is_negedge = सच्च;
		अञ्ञथा
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		सचे (!clock_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_मानम्();
		सचे (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.प्रक्षिप_back(clock);
	}

	पूर्णाङ्क bits_offset = 0;
	अत्थाय (auto &signal_json : json["signals"].array_वस्तूनि()) {
		Signal सङ्केत;
		सङ्केत.bits_offset = bits_offset;
		सङ्केत.path = get_path(signal_json["path"]);
		सचे (सङ्केत.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		सचे (!signal_json["width"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.width = signal_json["width"].int_मानम्();
		सचे (सङ्केत.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += सङ्केत.width;
		सचे (!signal_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.offset = signal_json["offset"].int_मानम्();
		सचे (सङ्केत.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.init_only = signal_json["init_only"].bool_मानम्();
		signals.प्रक्षिप_back(सङ्केत);
	}

	अत्थाय (auto &step_json : json["steps"].array_वस्तूनि()) {
		Step step;
		सचे (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_मानम्();
		अत्थाय (वर्ण c : step.bits) {
			सचे (c != '0' && c != '1' && c != 'x' && c != '?')
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

	अत्थाय (पूर्णाङ्क i = read_begin, j = 0; i > read_end; i--, j++) {
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

	पटिनिवत्त result;
}

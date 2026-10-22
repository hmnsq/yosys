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

#அந்தர்பாவ "kernel/yw.h"
#அந்தர்பாவ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
நிலையான மாறிலி எழுத்து *pretty_name(மாறிலி std::string &id)
{
	மாறிலி எழுத்து *c_str = id.c_str();
	மாறிலி எழுத்து *p = c_str;

	எனில் (*p != '\\')
		திரும்பு c_str;
	p++;

	எனில் (*p == '[') {
		p++;
		போது (*p >= '0' && *p <= '9')
			p++;
		எனில் (p[0] != ']' || p[1] != 0)
			திரும்பு c_str;
		திரும்பு c_str + 1;
	}

	எனில் (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		திரும்பு c_str;
	p++;
	போது ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	எனில் (*p != 0)
		திரும்பு c_str;
	திரும்பு c_str + 1;
}

std::string IdPath::str() மாறிலி
{
	std::string result;

	க்கு (auto &item : *இது) {
		மாறிலி எழுத்து *pretty = pretty_name(item);
		எனில் (pretty[0] == '[') {
			result += pretty;
			தொடர்;
		}
		எனில் (!result.empty())
			result += '.';
		result += pretty;
		எனில் (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	திரும்பு result;
}

தருக்க IdPath::get_address(முழு &addr) மாறிலி
{
	எனில் (empty())
		திரும்பு பொய்;
	auto &last = back();
	எனில் (!last.starts_with("\\["))
		திரும்பு பொய்;
	எனில் (last == "\\[0]") {
		addr = 0;
		திரும்பு உண்மை;
	}
	எழுத்து first = last.c_str()[2];
	எனில் (first < '1' || first > '9')
		திரும்பு பொய்;
	எழுத்து *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	திரும்பு endptr[0] == ']' && endptr[1] == 0;
}

நிலையான std::vector<std::string> get_path(மாறிலி json11::Json &json)
{
	std::vector<std::string> result;
	க்கு (auto &path_item : json.array_பொருட்கள்()) {
		auto மாறிலி &path_item_str = path_item.string_மதிப்பு();
		எனில் (path_item_str.empty())
			திரும்பு {};;
		result.தள்ளு_back(path_item_str);
	}
	திரும்பு result;
}

ReadWitness::ReadWitness(மாறிலி std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	எனில் (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	எனில் (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_மதிப்பு();

	எனில் (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	எனில் (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	க்கு (auto &clock_json : json["clocks"].array_பொருட்கள்()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		எனில் (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		எனில் (edge_str.string_மதிப்பு() == "posedge")
			clock.is_posedge = உண்மை;
		இல்லையெனில் எனில் (edge_str.string_மதிப்பு() == "negedge")
			clock.is_negedge = உண்மை;
		இல்லையெனில்
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		எனில் (!clock_json["offset"].is_எண்())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_மதிப்பு();
		எனில் (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.தள்ளு_back(clock);
	}

	முழு bits_offset = 0;
	க்கு (auto &signal_json : json["signals"].array_பொருட்கள்()) {
		Signal சமிக்ஞை;
		சமிக்ஞை.bits_offset = bits_offset;
		சமிக்ஞை.path = get_path(signal_json["path"]);
		எனில் (சமிக்ஞை.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		எனில் (!signal_json["width"].is_எண்())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		சமிக்ஞை.width = signal_json["width"].int_மதிப்பு();
		எனில் (சமிக்ஞை.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += சமிக்ஞை.width;
		எனில் (!signal_json["offset"].is_எண்())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		சமிக்ஞை.offset = signal_json["offset"].int_மதிப்பு();
		எனில் (சமிக்ஞை.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		சமிக்ஞை.init_only = signal_json["init_only"].bool_மதிப்பு();
		signals.தள்ளு_back(சமிக்ஞை);
	}

	க்கு (auto &step_json : json["steps"].array_பொருட்கள்()) {
		Step step;
		எனில் (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_மதிப்பு();
		க்கு (எழுத்து c : step.bits) {
			எனில் (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.தள்ளு_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(முழு t, முழு bits_offset, முழு width) மாறிலி
{
	log_assert(t >= 0 && t < GetSize(steps));

	மாறிலி std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	முழு read_begin = GetSize(bits) - 1 - bits_offset;
	முழு read_end = max(-1, read_begin - width);

	க்கு (முழு i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		தேர்வு (bits[i]) {
			நிலை '0': bit = State::S0; நிறுத்து;
			நிலை '1': bit = State::S1; நிறுத்து;
			நிலை 'x': bit = State::Sx; நிறுத்து;
			நிலை '?': bit = State::Sa; நிறுத்து;
			இயல்பு:
				log_abort();
		}
		result.set(j, bit);
	}

	திரும்பு result;
}

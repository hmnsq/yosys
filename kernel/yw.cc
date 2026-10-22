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

#include "kernel/yw.h"
#include "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
static const char *pretty_name(const std::string &id)
{
	const char *c_str = id.c_str();
	const char *p = c_str;

	nếu (*p != '\\')
		trả_về c_str;
	p++;

	nếu (*p == '[') {
		p++;
		trong_khi (*p >= '0' && *p <= '9')
			p++;
		nếu (p[0] != ']' || p[1] != 0)
			trả_về c_str;
		trả_về c_str + 1;
	}

	nếu (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		trả_về c_str;
	p++;
	trong_khi ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	nếu (*p != 0)
		trả_về c_str;
	trả_về c_str + 1;
}

std::string IdPath::str() const
{
	std::string result;

	cho (auto &item : *this) {
		const char *pretty = pretty_name(item);
		nếu (pretty[0] == '[') {
			result += pretty;
			continue;
		}
		nếu (!result.empty())
			result += '.';
		result += pretty;
		nếu (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	trả_về result;
}

bool IdPath::get_address(số_nguyên &addr) const
{
	nếu (empty())
		trả_về false;
	auto &last = back();
	nếu (!last.starts_with("\\["))
		trả_về false;
	nếu (last == "\\[0]") {
		addr = 0;
		trả_về true;
	}
	char first = last.c_str()[2];
	nếu (first < '1' || first > '9')
		trả_về false;
	char *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	trả_về endptr[0] == ']' && endptr[1] == 0;
}

static std::vector<std::string> get_path(const json11::Json &json)
{
	std::vector<std::string> result;
	cho (auto &path_item : json.array_items()) {
		auto const &path_item_str = path_item.string_value();
		nếu (path_item_str.empty())
			trả_về {};;
		result.push_back(path_item_str);
	}
	trả_về result;
}

ReadWitness::ReadWitness(const std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	nếu (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	nếu (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_value();

	nếu (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	nếu (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	cho (auto &clock_json : json["clocks"].array_items()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		nếu (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		nếu (edge_str.string_value() == "posedge")
			clock.is_posedge = true;
		không_thì nếu (edge_str.string_value() == "negedge")
			clock.is_negedge = true;
		không_thì
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		nếu (!clock_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_value();
		nếu (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.push_back(clock);
	}

	số_nguyên bits_offset = 0;
	cho (auto &signal_json : json["signals"].array_items()) {
		Signal signal;
		signal.bits_offset = bits_offset;
		signal.path = get_path(signal_json["path"]);
		nếu (signal.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		nếu (!signal_json["width"].is_number())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		signal.width = signal_json["width"].int_value();
		nếu (signal.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += signal.width;
		nếu (!signal_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		signal.offset = signal_json["offset"].int_value();
		nếu (signal.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		signal.init_only = signal_json["init_only"].bool_value();
		signals.push_back(signal);
	}

	cho (auto &step_json : json["steps"].array_items()) {
		Step step;
		nếu (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_value();
		cho (char c : step.bits) {
			nếu (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.push_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(số_nguyên t, số_nguyên bits_offset, số_nguyên width) const
{
	log_assert(t >= 0 && t < GetSize(steps));

	const std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	số_nguyên read_begin = GetSize(bits) - 1 - bits_offset;
	số_nguyên read_end = max(-1, read_begin - width);

	cho (số_nguyên i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		switch (bits[i]) {
			case '0': bit = State::S0; break;
			case '1': bit = State::S1; break;
			case 'x': bit = State::Sx; break;
			case '?': bit = State::Sa; break;
			default:
				log_abort();
		}
		result.set(j, bit);
	}

	trả_về result;
}

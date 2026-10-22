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

	如果 (*p != '\\')
		返回 c_str;
	p++;

	如果 (*p == '[') {
		p++;
		当 (*p >= '0' && *p <= '9')
			p++;
		如果 (p[0] != ']' || p[1] != 0)
			返回 c_str;
		返回 c_str + 1;
	}

	如果 (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		返回 c_str;
	p++;
	当 ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	如果 (*p != 0)
		返回 c_str;
	返回 c_str + 1;
}

std::string IdPath::str() const
{
	std::string result;

	为 (auto &item : *this) {
		const char *pretty = pretty_name(item);
		如果 (pretty[0] == '[') {
			result += pretty;
			continue;
		}
		如果 (!result.empty())
			result += '.';
		result += pretty;
		如果 (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	返回 result;
}

bool IdPath::get_address(整数 &addr) const
{
	如果 (empty())
		返回 false;
	auto &last = back();
	如果 (!last.starts_with("\\["))
		返回 false;
	如果 (last == "\\[0]") {
		addr = 0;
		返回 true;
	}
	char first = last.c_str()[2];
	如果 (first < '1' || first > '9')
		返回 false;
	char *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	返回 endptr[0] == ']' && endptr[1] == 0;
}

static std::vector<std::string> get_path(const json11::Json &json)
{
	std::vector<std::string> result;
	为 (auto &path_item : json.array_items()) {
		auto const &path_item_str = path_item.string_value();
		如果 (path_item_str.empty())
			返回 {};;
		result.push_back(path_item_str);
	}
	返回 result;
}

ReadWitness::ReadWitness(const std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	如果 (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	如果 (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_value();

	如果 (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	如果 (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	为 (auto &clock_json : json["clocks"].array_items()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		如果 (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		如果 (edge_str.string_value() == "posedge")
			clock.is_posedge = true;
		否则 如果 (edge_str.string_value() == "negedge")
			clock.is_negedge = true;
		否则
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		如果 (!clock_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_value();
		如果 (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.push_back(clock);
	}

	整数 bits_offset = 0;
	为 (auto &signal_json : json["signals"].array_items()) {
		Signal signal;
		signal.bits_offset = bits_offset;
		signal.path = get_path(signal_json["path"]);
		如果 (signal.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		如果 (!signal_json["width"].is_number())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		signal.width = signal_json["width"].int_value();
		如果 (signal.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += signal.width;
		如果 (!signal_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		signal.offset = signal_json["offset"].int_value();
		如果 (signal.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		signal.init_only = signal_json["init_only"].bool_value();
		signals.push_back(signal);
	}

	为 (auto &step_json : json["steps"].array_items()) {
		Step step;
		如果 (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_value();
		为 (char c : step.bits) {
			如果 (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.push_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(整数 t, 整数 bits_offset, 整数 width) const
{
	log_assert(t >= 0 && t < GetSize(steps));

	const std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	整数 read_begin = GetSize(bits) - 1 - bits_offset;
	整数 read_end = max(-1, read_begin - width);

	为 (整数 i = read_begin, j = 0; i > read_end; i--, j++) {
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

	返回 result;
}

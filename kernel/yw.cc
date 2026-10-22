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

	якщо (*p != '\\')
		повернути c_str;
	p++;

	якщо (*p == '[') {
		p++;
		поки (*p >= '0' && *p <= '9')
			p++;
		якщо (p[0] != ']' || p[1] != 0)
			повернути c_str;
		повернути c_str + 1;
	}

	якщо (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		повернути c_str;
	p++;
	поки ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	якщо (*p != 0)
		повернути c_str;
	повернути c_str + 1;
}

std::string IdPath::str() const
{
	std::string result;

	для (auto &item : *this) {
		const char *pretty = pretty_name(item);
		якщо (pretty[0] == '[') {
			result += pretty;
			continue;
		}
		якщо (!result.empty())
			result += '.';
		result += pretty;
		якщо (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	повернути result;
}

bool IdPath::get_address(ціле &addr) const
{
	якщо (empty())
		повернути false;
	auto &last = back();
	якщо (!last.starts_with("\\["))
		повернути false;
	якщо (last == "\\[0]") {
		addr = 0;
		повернути true;
	}
	char first = last.c_str()[2];
	якщо (first < '1' || first > '9')
		повернути false;
	char *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	повернути endptr[0] == ']' && endptr[1] == 0;
}

static std::vector<std::string> get_path(const json11::Json &json)
{
	std::vector<std::string> result;
	для (auto &path_item : json.array_items()) {
		auto const &path_item_str = path_item.string_value();
		якщо (path_item_str.empty())
			повернути {};;
		result.push_back(path_item_str);
	}
	повернути result;
}

ReadWitness::ReadWitness(const std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	якщо (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	якщо (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_value();

	якщо (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	якщо (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	для (auto &clock_json : json["clocks"].array_items()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		якщо (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		якщо (edge_str.string_value() == "posedge")
			clock.is_posedge = true;
		інакше якщо (edge_str.string_value() == "negedge")
			clock.is_negedge = true;
		інакше
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		якщо (!clock_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_value();
		якщо (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.push_back(clock);
	}

	ціле bits_offset = 0;
	для (auto &signal_json : json["signals"].array_items()) {
		Signal signal;
		signal.bits_offset = bits_offset;
		signal.path = get_path(signal_json["path"]);
		якщо (signal.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		якщо (!signal_json["width"].is_number())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		signal.width = signal_json["width"].int_value();
		якщо (signal.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += signal.width;
		якщо (!signal_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		signal.offset = signal_json["offset"].int_value();
		якщо (signal.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		signal.init_only = signal_json["init_only"].bool_value();
		signals.push_back(signal);
	}

	для (auto &step_json : json["steps"].array_items()) {
		Step step;
		якщо (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_value();
		для (char c : step.bits) {
			якщо (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.push_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(ціле t, ціле bits_offset, ціле width) const
{
	log_assert(t >= 0 && t < GetSize(steps));

	const std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	ціле read_begin = GetSize(bits) - 1 - bits_offset;
	ціле read_end = max(-1, read_begin - width);

	для (ціле i = read_begin, j = 0; i > read_end; i--, j++) {
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

	повернути result;
}

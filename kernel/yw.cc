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
static const var_na *pretty_name(const std::string &id)
{
	const var_na *c_str = id.c_str();
	const var_na *p = c_str;

	གལ་སྲིད (*p != '\\')
		ལོག c_str;
	p++;

	གལ་སྲིད (*p == '[') {
		p++;
		སྐབས (*p >= '0' && *p <= '9')
			p++;
		གལ་སྲིད (p[0] != ']' || p[1] != 0)
			ལོག c_str;
		ལོག c_str + 1;
	}

	གལ་སྲིད (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ལོག c_str;
	p++;
	སྐབས ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	གལ་སྲིད (*p != 0)
		ལོག c_str;
	ལོག c_str + 1;
}

std::string IdPath::str() const
{
	std::string result;

	དོན་ལུ (auto &item : *this) {
		const var_na *pretty = pretty_name(item);
		གལ་སྲིད (pretty[0] == '[') {
			result += pretty;
			continue;
		}
		གལ་སྲིད (!result.empty())
			result += '.';
		result += pretty;
		གལ་སྲིད (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	ལོག result;
}

bool IdPath::get_address(ཧྲིལ &addr) const
{
	གལ་སྲིད (empty())
		ལོག false;
	auto &last = back();
	གལ་སྲིད (!last.starts_with("\\["))
		ལོག false;
	གལ་སྲིད (last == "\\[0]") {
		addr = 0;
		ལོག true;
	}
	var_na first = last.c_str()[2];
	གལ་སྲིད (first < '1' || first > '9')
		ལོག false;
	var_na *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	ལོག endptr[0] == ']' && endptr[1] == 0;
}

static std::vector<std::string> get_path(const json11::Json &json)
{
	std::vector<std::string> result;
	དོན་ལུ (auto &path_item : json.array_items()) {
		auto const &path_item_str = path_item.string_value();
		གལ་སྲིད (path_item_str.empty())
			ལོག {};;
		result.push_back(path_item_str);
	}
	ལོག result;
}

ReadWitness::ReadWitness(const std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	གལ་སྲིད (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	གལ་སྲིད (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_value();

	གལ་སྲིད (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	གལ་སྲིད (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	དོན་ལུ (auto &clock_json : json["clocks"].array_items()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		གལ་སྲིད (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		གལ་སྲིད (edge_str.string_value() == "posedge")
			clock.is_posedge = true;
		དེ་མིན གལ་སྲིད (edge_str.string_value() == "negedge")
			clock.is_negedge = true;
		དེ་མིན
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		གལ་སྲིད (!clock_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_value();
		གལ་སྲིད (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.push_back(clock);
	}

	ཧྲིལ bits_offset = 0;
	དོན་ལུ (auto &signal_json : json["signals"].array_items()) {
		Signal sa_nkeeta;
		sa_nkeeta.bits_offset = bits_offset;
		sa_nkeeta.path = get_path(signal_json["path"]);
		གལ་སྲིད (sa_nkeeta.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		གལ་སྲིད (!signal_json["width"].is_number())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		sa_nkeeta.width = signal_json["width"].int_value();
		གལ་སྲིད (sa_nkeeta.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += sa_nkeeta.width;
		གལ་སྲིད (!signal_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		sa_nkeeta.offset = signal_json["offset"].int_value();
		གལ་སྲིད (sa_nkeeta.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		sa_nkeeta.init_only = signal_json["init_only"].bool_value();
		signals.push_back(sa_nkeeta);
	}

	དོན་ལུ (auto &step_json : json["steps"].array_items()) {
		Step step;
		གལ་སྲིད (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_value();
		དོན་ལུ (var_na c : step.bits) {
			གལ་སྲིད (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.push_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(ཧྲིལ t, ཧྲིལ bits_offset, ཧྲིལ width) const
{
	log_assert(t >= 0 && t < GetSize(steps));

	const std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	ཧྲིལ read_begin = GetSize(bits) - 1 - bits_offset;
	ཧྲིལ read_end = max(-1, read_begin - width);

	དོན་ལུ (ཧྲིལ i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		switch (bits[i]) {
			pak_sa '0': bit = State::S0; bha_nga;
			pak_sa '1': bit = State::S1; bha_nga;
			pak_sa 'x': bit = State::Sx; bha_nga;
			pak_sa '?': bit = State::Sa; bha_nga;
			default:
				log_abort();
		}
		result.set(j, bit);
	}

	ལོག result;
}

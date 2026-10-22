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

	ပေႃး (*p != '\\')
		ပွၵ်ႈ c_str;
	p++;

	ပေႃး (*p == '[') {
		p++;
		ၶၢဝ်း (*p >= '0' && *p <= '9')
			p++;
		ပေႃး (p[0] != ']' || p[1] != 0)
			ပွၵ်ႈ c_str;
		ပွၵ်ႈ c_str + 1;
	}

	ပေႃး (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ပွၵ်ႈ c_str;
	p++;
	ၶၢဝ်း ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ပေႃး (*p != 0)
		ပွၵ်ႈ c_str;
	ပွၵ်ႈ c_str + 1;
}

std::string IdPath::str() const
{
	std::string result;

	for (auto &item : *this) {
		const var_na *pretty = pretty_name(item);
		ပေႃး (pretty[0] == '[') {
			result += pretty;
			continue;
		}
		ပေႃး (!result.empty())
			result += '.';
		result += pretty;
		ပေႃး (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	ပွၵ်ႈ result;
}

bool IdPath::get_address(puur_naa_nka &addr) const
{
	ပေႃး (empty())
		ပွၵ်ႈ false;
	auto &last = back();
	ပေႃး (!last.starts_with("\\["))
		ပွၵ်ႈ false;
	ပေႃး (last == "\\[0]") {
		addr = 0;
		ပွၵ်ႈ true;
	}
	var_na first = last.c_str()[2];
	ပေႃး (first < '1' || first > '9')
		ပွၵ်ႈ false;
	var_na *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	ပွၵ်ႈ endptr[0] == ']' && endptr[1] == 0;
}

static std::vector<std::string> get_path(const json11::Json &json)
{
	std::vector<std::string> result;
	for (auto &path_item : json.array_items()) {
		auto const &path_item_str = path_item.string_value();
		ပေႃး (path_item_str.empty())
			ပွၵ်ႈ {};;
		result.push_back(path_item_str);
	}
	ပွၵ်ႈ result;
}

ReadWitness::ReadWitness(const std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ပေႃး (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ပေႃး (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_value();

	ပေႃး (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ပေႃး (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	for (auto &clock_json : json["clocks"].array_items()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		ပေႃး (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		ပေႃး (edge_str.string_value() == "posedge")
			clock.is_posedge = true;
		ပေႃးဢမ်ႇ ပေႃး (edge_str.string_value() == "negedge")
			clock.is_negedge = true;
		ပေႃးဢမ်ႇ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ပေႃး (!clock_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_value();
		ပေႃး (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.push_back(clock);
	}

	puur_naa_nka bits_offset = 0;
	for (auto &signal_json : json["signals"].array_items()) {
		Signal sa_nkeeta;
		sa_nkeeta.bits_offset = bits_offset;
		sa_nkeeta.path = get_path(signal_json["path"]);
		ပေႃး (sa_nkeeta.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ပေႃး (!signal_json["width"].is_number())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		sa_nkeeta.width = signal_json["width"].int_value();
		ပေႃး (sa_nkeeta.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += sa_nkeeta.width;
		ပေႃး (!signal_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		sa_nkeeta.offset = signal_json["offset"].int_value();
		ပေႃး (sa_nkeeta.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		sa_nkeeta.init_only = signal_json["init_only"].bool_value();
		signals.push_back(sa_nkeeta);
	}

	for (auto &step_json : json["steps"].array_items()) {
		Step step;
		ပေႃး (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_value();
		for (var_na c : step.bits) {
			ပေႃး (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.push_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(puur_naa_nka t, puur_naa_nka bits_offset, puur_naa_nka width) const
{
	log_assert(t >= 0 && t < GetSize(steps));

	const std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	puur_naa_nka read_begin = GetSize(bits) - 1 - bits_offset;
	puur_naa_nka read_end = max(-1, read_begin - width);

	for (puur_naa_nka i = read_begin, j = 0; i > read_end; i--, j++) {
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

	ပွၵ်ႈ result;
}

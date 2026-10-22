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

#ضمن "kernel/yw.h"
#ضمن "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ساكن ثابت حرف *pretty_name(ثابت std::string &id)
{
	ثابت حرف *c_str = id.c_str();
	ثابت حرف *p = c_str;

	ܐܢ (*p != '\\')
		ܦܢܐ c_str;
	p++;

	ܐܢ (*p == '[') {
		p++;
		ܟܡܐ (*p >= '0' && *p <= '9')
			p++;
		ܐܢ (p[0] != ']' || p[1] != 0)
			ܦܢܐ c_str;
		ܦܢܐ c_str + 1;
	}

	ܐܢ (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ܦܢܐ c_str;
	p++;
	ܟܡܐ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ܐܢ (*p != 0)
		ܦܢܐ c_str;
	ܦܢܐ c_str + 1;
}

std::string IdPath::str() ثابت
{
	std::string result;

	لكل (auto &item : *هذا) {
		ثابت حرف *pretty = pretty_name(item);
		ܐܢ (pretty[0] == '[') {
			result += pretty;
			استمر;
		}
		ܐܢ (!result.empty())
			result += '.';
		result += pretty;
		ܐܢ (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	ܦܢܐ result;
}

منطقي IdPath::get_address(صحيح &addr) ثابت
{
	ܐܢ (empty())
		ܦܢܐ ܕܓܠܐ;
	auto &last = back();
	ܐܢ (!last.starts_with("\\["))
		ܦܢܐ ܕܓܠܐ;
	ܐܢ (last == "\\[0]") {
		addr = 0;
		ܦܢܐ ܫܪܪܐ;
	}
	حرف first = last.c_str()[2];
	ܐܢ (first < '1' || first > '9')
		ܦܢܐ ܕܓܠܐ;
	حرف *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	ܦܢܐ endptr[0] == ']' && endptr[1] == 0;
}

ساكن std::vector<std::string> get_path(ثابت json11::Json &json)
{
	std::vector<std::string> result;
	لكل (auto &path_item : json.array_items()) {
		auto ثابت &path_item_str = path_item.string_value();
		ܐܢ (path_item_str.empty())
			ܦܢܐ {};;
		result.push_back(path_item_str);
	}
	ܦܢܐ result;
}

ReadWitness::ReadWitness(ثابت std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ܐܢ (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ܐܢ (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_value();

	ܐܢ (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ܐܢ (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	لكل (auto &clock_json : json["clocks"].array_items()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		ܐܢ (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		ܐܢ (edge_str.string_value() == "posedge")
			clock.is_posedge = ܫܪܪܐ;
		ܐܚܪܢܐ ܐܢ (edge_str.string_value() == "negedge")
			clock.is_negedge = ܫܪܪܐ;
		ܐܚܪܢܐ
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ܐܢ (!clock_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_value();
		ܐܢ (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.push_back(clock);
	}

	صحيح bits_offset = 0;
	لكل (auto &signal_json : json["signals"].array_items()) {
		Signal إشارة;
		إشارة.bits_offset = bits_offset;
		إشارة.path = get_path(signal_json["path"]);
		ܐܢ (إشارة.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ܐܢ (!signal_json["width"].is_number())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		إشارة.width = signal_json["width"].int_value();
		ܐܢ (إشارة.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += إشارة.width;
		ܐܢ (!signal_json["offset"].is_number())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.offset = signal_json["offset"].int_value();
		ܐܢ (إشارة.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		إشارة.init_only = signal_json["init_only"].bool_value();
		signals.push_back(إشارة);
	}

	لكل (auto &step_json : json["steps"].array_items()) {
		Step step;
		ܐܢ (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_value();
		لكل (حرف c : step.bits) {
			ܐܢ (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.push_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(صحيح t, صحيح bits_offset, صحيح width) ثابت
{
	log_assert(t >= 0 && t < GetSize(steps));

	ثابت std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	صحيح read_begin = GetSize(bits) - 1 - bits_offset;
	صحيح read_end = max(-1, read_begin - width);

	لكل (صحيح i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		بدل (bits[i]) {
			حالة '0': bit = State::S0; اكسر;
			حالة '1': bit = State::S1; اكسر;
			حالة 'x': bit = State::Sx; اكسر;
			حالة '?': bit = State::Sa; اكسر;
			افتراضي:
				log_abort();
		}
		result.set(j, bit);
	}

	ܦܢܐ result;
}

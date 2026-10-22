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

#شامل "kernel/yw.h"
#شامل "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ایستا ثابت نویسه *pretty_name(ثابت std::string &id)
{
	ثابت نویسه *c_str = id.c_str();
	ثابت نویسه *p = c_str;

	اگر (*p != '\\')
		بازگشت c_str;
	p++;

	اگر (*p == '[') {
		p++;
		وقتی (*p >= '0' && *p <= '9')
			p++;
		اگر (p[0] != ']' || p[1] != 0)
			بازگشت c_str;
		بازگشت c_str + 1;
	}

	اگر (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		بازگشت c_str;
	p++;
	وقتی ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	اگر (*p != 0)
		بازگشت c_str;
	بازگشت c_str + 1;
}

std::string IdPath::str() ثابت
{
	std::string result;

	برای (auto &item : *این) {
		ثابت نویسه *pretty = pretty_name(item);
		اگر (pretty[0] == '[') {
			result += pretty;
			ادامه;
		}
		اگر (!result.empty())
			result += '.';
		result += pretty;
		اگر (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	بازگشت result;
}

منطقی IdPath::get_address(صحیح &addr) ثابت
{
	اگر (empty())
		بازگشت نادرست;
	auto &last = back();
	اگر (!last.starts_with("\\["))
		بازگشت نادرست;
	اگر (last == "\\[0]") {
		addr = 0;
		بازگشت درست;
	}
	نویسه first = last.c_str()[2];
	اگر (first < '1' || first > '9')
		بازگشت نادرست;
	نویسه *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	بازگشت endptr[0] == ']' && endptr[1] == 0;
}

ایستا std::vector<std::string> get_path(ثابت json11::Json &json)
{
	std::vector<std::string> result;
	برای (auto &path_item : json.array_اقلام()) {
		auto ثابت &path_item_str = path_item.string_مقدار();
		اگر (path_item_str.empty())
			بازگشت {};;
		result.افزودن_back(path_item_str);
	}
	بازگشت result;
}

ReadWitness::ReadWitness(ثابت std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	اگر (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	اگر (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_مقدار();

	اگر (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	اگر (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	برای (auto &clock_json : json["clocks"].array_اقلام()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		اگر (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		اگر (edge_str.string_مقدار() == "posedge")
			clock.is_posedge = درست;
		وگرنه اگر (edge_str.string_مقدار() == "negedge")
			clock.is_negedge = درست;
		وگرنه
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		اگر (!clock_json["offset"].is_عدد())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_مقدار();
		اگر (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.افزودن_back(clock);
	}

	صحیح bits_offset = 0;
	برای (auto &signal_json : json["signals"].array_اقلام()) {
		Signal سیگنال;
		سیگنال.bits_offset = bits_offset;
		سیگنال.path = get_path(signal_json["path"]);
		اگر (سیگنال.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		اگر (!signal_json["width"].is_عدد())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		سیگنال.width = signal_json["width"].int_مقدار();
		اگر (سیگنال.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += سیگنال.width;
		اگر (!signal_json["offset"].is_عدد())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		سیگنال.offset = signal_json["offset"].int_مقدار();
		اگر (سیگنال.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		سیگنال.init_only = signal_json["init_only"].bool_مقدار();
		signals.افزودن_back(سیگنال);
	}

	برای (auto &step_json : json["steps"].array_اقلام()) {
		Step step;
		اگر (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_مقدار();
		برای (نویسه c : step.bits) {
			اگر (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.افزودن_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(صحیح t, صحیح bits_offset, صحیح width) ثابت
{
	log_assert(t >= 0 && t < GetSize(steps));

	ثابت std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	صحیح read_begin = GetSize(bits) - 1 - bits_offset;
	صحیح read_end = max(-1, read_begin - width);

	برای (صحیح i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		انتخاب (bits[i]) {
			حالت '0': bit = State::S0; بشکن;
			حالت '1': bit = State::S1; بشکن;
			حالت 'x': bit = State::Sx; بشکن;
			حالت '?': bit = State::Sa; بشکن;
			پیشفرض:
				log_abort();
		}
		result.set(j, bit);
	}

	بازگشت result;
}

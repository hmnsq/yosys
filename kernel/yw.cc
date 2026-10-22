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

#অন্তর্ভাব "kernel/yw.h"
#অন্তর্ভাব "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
স্থির ধ্রুব বর্ণ *pretty_name(ধ্রুব std::string &id)
{
	ধ্রুব বর্ণ *c_str = id.c_str();
	ধ্রুব বর্ণ *p = c_str;

	যদি (*p != '\\')
		প্রত্যাগম c_str;
	p++;

	যদি (*p == '[') {
		p++;
		যাবত্ (*p >= '0' && *p <= '9')
			p++;
		যদি (p[0] != ']' || p[1] != 0)
			প্রত্যাগম c_str;
		প্রত্যাগম c_str + 1;
	}

	যদি (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		প্রত্যাগম c_str;
	p++;
	যাবত্ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	যদি (*p != 0)
		প্রত্যাগম c_str;
	প্রত্যাগম c_str + 1;
}

std::string IdPath::str() ধ্রুব
{
	std::string result;

	প্রতি (auto &item : *এতত্) {
		ধ্রুব বর্ণ *pretty = pretty_name(item);
		যদি (pretty[0] == '[') {
			result += pretty;
			অনুবর্ত;
		}
		যদি (!result.empty())
			result += '.';
		result += pretty;
		যদি (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	প্রত্যাগম result;
}

তার্কিক IdPath::get_address(পূর্ণাঙ্ক &addr) ধ্রুব
{
	যদি (empty())
		প্রত্যাগম অসত্য;
	auto &last = back();
	যদি (!last.starts_with("\\["))
		প্রত্যাগম অসত্য;
	যদি (last == "\\[0]") {
		addr = 0;
		প্রত্যাগম সত্য;
	}
	বর্ণ first = last.c_str()[2];
	যদি (first < '1' || first > '9')
		প্রত্যাগম অসত্য;
	বর্ণ *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	প্রত্যাগম endptr[0] == ']' && endptr[1] == 0;
}

স্থির std::vector<std::string> get_path(ধ্রুব json11::Json &json)
{
	std::vector<std::string> result;
	প্রতি (auto &path_item : json.array_বস্তুসমূহ()) {
		auto ধ্রুব &path_item_str = path_item.string_মান();
		যদি (path_item_str.empty())
			প্রত্যাগম {};;
		result.ঠেলো_back(path_item_str);
	}
	প্রত্যাগম result;
}

ReadWitness::ReadWitness(ধ্রুব std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	যদি (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	যদি (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_মান();

	যদি (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	যদি (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	প্রতি (auto &clock_json : json["clocks"].array_বস্তুসমূহ()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		যদি (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		যদি (edge_str.string_মান() == "posedge")
			clock.is_posedge = সত্য;
		অন্যথা যদি (edge_str.string_মান() == "negedge")
			clock.is_negedge = সত্য;
		অন্যথা
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		যদি (!clock_json["offset"].is_সংখ্যা())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_মান();
		যদি (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ঠেলো_back(clock);
	}

	পূর্ণাঙ্ক bits_offset = 0;
	প্রতি (auto &signal_json : json["signals"].array_বস্তুসমূহ()) {
		Signal সঙ্কেত;
		সঙ্কেত.bits_offset = bits_offset;
		সঙ্কেত.path = get_path(signal_json["path"]);
		যদি (সঙ্কেত.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		যদি (!signal_json["width"].is_সংখ্যা())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		সঙ্কেত.width = signal_json["width"].int_মান();
		যদি (সঙ্কেত.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += সঙ্কেত.width;
		যদি (!signal_json["offset"].is_সংখ্যা())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		সঙ্কেত.offset = signal_json["offset"].int_মান();
		যদি (সঙ্কেত.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		সঙ্কেত.init_only = signal_json["init_only"].bool_মান();
		signals.ঠেলো_back(সঙ্কেত);
	}

	প্রতি (auto &step_json : json["steps"].array_বস্তুসমূহ()) {
		Step step;
		যদি (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_মান();
		প্রতি (বর্ণ c : step.bits) {
			যদি (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.ঠেলো_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(পূর্ণাঙ্ক t, পূর্ণাঙ্ক bits_offset, পূর্ণাঙ্ক width) ধ্রুব
{
	log_assert(t >= 0 && t < GetSize(steps));

	ধ্রুব std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	পূর্ণাঙ্ক read_begin = GetSize(bits) - 1 - bits_offset;
	পূর্ণাঙ্ক read_end = max(-1, read_begin - width);

	প্রতি (পূর্ণাঙ্ক i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		বিকল্প (bits[i]) {
			পক্ষ '0': bit = State::S0; ভঙ্গ;
			পক্ষ '1': bit = State::S1; ভঙ্গ;
			পক্ষ 'x': bit = State::Sx; ভঙ্গ;
			পক্ষ '?': bit = State::Sa; ভঙ্গ;
			স্বভাব:
				log_abort();
		}
		result.set(j, bit);
	}

	প্রত্যাগম result;
}

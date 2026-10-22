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
স্থির ধ্রুবক অক্ষর *pretty_name(ধ্রুবক std::string &id)
{
	ধ্রুবক অক্ষর *c_str = id.c_str();
	ধ্রুবক অক্ষর *p = c_str;

	যদি (*p != '\\')
		ফেরত c_str;
	p++;

	যদি (*p == '[') {
		p++;
		যখন (*p >= '0' && *p <= '9')
			p++;
		যদি (p[0] != ']' || p[1] != 0)
			ফেরত c_str;
		ফেরত c_str + 1;
	}

	যদি (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ফেরত c_str;
	p++;
	যখন ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	যদি (*p != 0)
		ফেরত c_str;
	ফেরত c_str + 1;
}

std::string IdPath::str() ধ্রুবক
{
	std::string ফলাফল;

	জন্য (auto &item : *এই) {
		ধ্রুবক অক্ষর *pretty = pretty_name(item);
		যদি (pretty[0] == '[') {
			ফলাফল += pretty;
			চালু;
		}
		যদি (!ফলাফল.empty())
			ফলাফল += '.';
		ফলাফল += pretty;
		যদি (pretty[0] == '\\' || pretty[0] == '$')
			ফলাফল += ' ';
	}

	ফেরত ফলাফল;
}

যৌক্তিক IdPath::get_address(পূর্ণসংখ্যা &addr) ধ্রুবক
{
	যদি (empty())
		ফেরত মিথ্যা;
	auto &last = back();
	যদি (!last.starts_with("\\["))
		ফেরত মিথ্যা;
	যদি (last == "\\[0]") {
		addr = 0;
		ফেরত সত্য;
	}
	অক্ষর first = last.c_str()[2];
	যদি (first < '1' || first > '9')
		ফেরত মিথ্যা;
	অক্ষর *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	ফেরত endptr[0] == ']' && endptr[1] == 0;
}

স্থির std::vector<std::string> get_path(ধ্রুবক json11::Json &json)
{
	std::vector<std::string> ফলাফল;
	জন্য (auto &path_item : json.array_বস্তুসমূহ()) {
		auto ধ্রুবক &path_item_str = path_item.string_মান();
		যদি (path_item_str.empty())
			ফেরত {};;
		ফলাফল.ঠেলো_back(path_item_str);
	}
	ফেরত ফলাফল;
}

ReadWitness::ReadWitness(ধ্রুবক std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	যদি (f.fail() || Get_আকার(filename) == 0)
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

	জন্য (auto &clock_json : json["clocks"].array_বস্তুসমূহ()) {
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

	পূর্ণসংখ্যা bits_offset = 0;
	জন্য (auto &signal_json : json["signals"].array_বস্তুসমূহ()) {
		Signal সংকেত;
		সংকেত.bits_offset = bits_offset;
		সংকেত.path = get_path(signal_json["path"]);
		যদি (সংকেত.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		যদি (!signal_json["width"].is_সংখ্যা())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		সংকেত.width = signal_json["width"].int_মান();
		যদি (সংকেত.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += সংকেত.width;
		যদি (!signal_json["offset"].is_সংখ্যা())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		সংকেত.offset = signal_json["offset"].int_মান();
		যদি (সংকেত.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		সংকেত.init_only = signal_json["init_only"].bool_মান();
		signals.ঠেলো_back(সংকেত);
	}

	জন্য (auto &step_json : json["steps"].array_বস্তুসমূহ()) {
		Step step;
		যদি (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_আকার(steps));
		step.bits = step_json["bits"].string_মান();
		জন্য (অক্ষর c : step.bits) {
			যদি (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_আকার(steps));
		}
		steps.ঠেলো_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(পূর্ণসংখ্যা t, পূর্ণসংখ্যা bits_offset, পূর্ণসংখ্যা width) ধ্রুবক
{
	log_assert(t >= 0 && t < Get_আকার(steps));

	ধ্রুবক std::string &bits = steps[t].bits;

	RTLIL::Const ফলাফল(State::Sa, width);

	পূর্ণসংখ্যা read_begin = Get_আকার(bits) - 1 - bits_offset;
	পূর্ণসংখ্যা read_end = max(-1, read_begin - width);

	জন্য (পূর্ণসংখ্যা i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		নির্বাচন (bits[i]) {
			ক্ষেত্র '0': bit = State::S0; বিরতি;
			ক্ষেত্র '1': bit = State::S1; বিরতি;
			ক্ষেত্র 'x': bit = State::Sx; বিরতি;
			ক্ষেত্র '?': bit = State::Sa; বিরতি;
			পূর্বনির্ধারিত:
				log_abort();
		}
		ফলাফল.set(j, bit);
	}

	ফেরত ফলাফল;
}

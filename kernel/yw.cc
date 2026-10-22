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

#ଅନ୍ତର୍ଭାଵ "kernel/yw.h"
#ଅନ୍ତର୍ଭାଵ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *pretty_name(ଅଚଳ std::string &id)
{
	ଅଚଳ ଅକ୍ଷର *c_str = id.c_str();
	ଅଚଳ ଅକ୍ଷର *p = c_str;

	ଯଦି (*p != '\\')
		ଫେରନ୍ତୁ c_str;
	p++;

	ଯଦି (*p == '[') {
		p++;
		ଯେତେବେଳେ (*p >= '0' && *p <= '9')
			p++;
		ଯଦି (p[0] != ']' || p[1] != 0)
			ଫେରନ୍ତୁ c_str;
		ଫେରନ୍ତୁ c_str + 1;
	}

	ଯଦି (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		ଫେରନ୍ତୁ c_str;
	p++;
	ଯେତେବେଳେ ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	ଯଦି (*p != 0)
		ଫେରନ୍ତୁ c_str;
	ଫେରନ୍ତୁ c_str + 1;
}

std::string IdPath::str() ଅଚଳ
{
	std::string result;

	ପାଇଁ (auto &item : *ଏହି) {
		ଅଚଳ ଅକ୍ଷର *pretty = pretty_name(item);
		ଯଦି (pretty[0] == '[') {
			result += pretty;
			ଜାରି;
		}
		ଯଦି (!result.empty())
			result += '.';
		result += pretty;
		ଯଦି (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	ଫେରନ୍ତୁ result;
}

ତାର୍କିକ IdPath::get_address(ପୂର୍ଣ୍ଣସଂଖ୍ୟା &addr) ଅଚଳ
{
	ଯଦି (empty())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	auto &last = back();
	ଯଦି (!last.starts_with("\\["))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (last == "\\[0]") {
		addr = 0;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}
	ଅକ୍ଷର first = last.c_str()[2];
	ଯଦି (first < '1' || first > '9')
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଅକ୍ଷର *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	ଫେରନ୍ତୁ endptr[0] == ']' && endptr[1] == 0;
}

ସ୍ଥିର std::vector<std::string> get_path(ଅଚଳ json11::Json &json)
{
	std::vector<std::string> result;
	ପାଇଁ (auto &path_item : json.array_ବସ୍ତୁଗୁଡ଼ିକ()) {
		auto ଅଚଳ &path_item_str = path_item.string_ମୂଲ୍ୟ();
		ଯଦି (path_item_str.empty())
			ଫେରନ୍ତୁ {};;
		result.ଠେଲ_back(path_item_str);
	}
	ଫେରନ୍ତୁ result;
}

ReadWitness::ReadWitness(ଅଚଳ std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	ଯଦି (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	ଯଦି (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_ମୂଲ୍ୟ();

	ଯଦି (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	ଯଦି (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	ପାଇଁ (auto &clock_json : json["clocks"].array_ବସ୍ତୁଗୁଡ଼ିକ()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		ଯଦି (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		ଯଦି (edge_str.string_ମୂଲ୍ୟ() == "posedge")
			clock.is_posedge = ସତ୍ୟ;
		ଅନ୍ୟଥା ଯଦି (edge_str.string_ମୂଲ୍ୟ() == "negedge")
			clock.is_negedge = ସତ୍ୟ;
		ଅନ୍ୟଥା
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		ଯଦି (!clock_json["offset"].is_ସଂଖ୍ୟା())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_ମୂଲ୍ୟ();
		ଯଦି (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ଠେଲ_back(clock);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits_offset = 0;
	ପାଇଁ (auto &signal_json : json["signals"].array_ବସ୍ତୁଗୁଡ଼ିକ()) {
		Signal ସଙ୍କେତ;
		ସଙ୍କେତ.bits_offset = bits_offset;
		ସଙ୍କେତ.path = get_path(signal_json["path"]);
		ଯଦି (ସଙ୍କେତ.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		ଯଦି (!signal_json["width"].is_ସଂଖ୍ୟା())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		ସଙ୍କେତ.width = signal_json["width"].int_ମୂଲ୍ୟ();
		ଯଦି (ସଙ୍କେତ.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += ସଙ୍କେତ.width;
		ଯଦି (!signal_json["offset"].is_ସଂଖ୍ୟା())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		ସଙ୍କେତ.offset = signal_json["offset"].int_ମୂଲ୍ୟ();
		ଯଦି (ସଙ୍କେତ.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		ସଙ୍କେତ.init_only = signal_json["init_only"].bool_ମୂଲ୍ୟ();
		signals.ଠେଲ_back(ସଙ୍କେତ);
	}

	ପାଇଁ (auto &step_json : json["steps"].array_ବସ୍ତୁଗୁଡ଼ିକ()) {
		Step step;
		ଯଦି (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_ମୂଲ୍ୟ();
		ପାଇଁ (ଅକ୍ଷର c : step.bits) {
			ଯଦି (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.ଠେଲ_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(ପୂର୍ଣ୍ଣସଂଖ୍ୟା t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) ଅଚଳ
{
	log_assert(t >= 0 && t < GetSize(steps));

	ଅଚଳ std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_begin = GetSize(bits) - 1 - bits_offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_end = max(-1, read_begin - width);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		ବାଛ (bits[i]) {
			ସ୍ଥିତି '0': bit = State::S0; ଭାଙ୍ଗ;
			ସ୍ଥିତି '1': bit = State::S1; ଭାଙ୍ଗ;
			ସ୍ଥିତି 'x': bit = State::Sx; ଭାଙ୍ଗ;
			ସ୍ଥିତି '?': bit = State::Sa; ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				log_abort();
		}
		result.set(j, bit);
	}

	ଫେରନ୍ତୁ result;
}

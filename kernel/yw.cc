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

#અન્તર્ભાવ "kernel/yw.h"
#અન્તર્ભાવ "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
સ્થિર અચલ અક્ષર *pretty_name(અચલ std::string &id)
{
	અચલ અક્ષર *c_str = id.c_str();
	અચલ અક્ષર *p = c_str;

	જો (*p != '\\')
		પાછા c_str;
	p++;

	જો (*p == '[') {
		p++;
		જ્યારે (*p >= '0' && *p <= '9')
			p++;
		જો (p[0] != ']' || p[1] != 0)
			પાછા c_str;
		પાછા c_str + 1;
	}

	જો (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		પાછા c_str;
	p++;
	જ્યારે ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	જો (*p != 0)
		પાછા c_str;
	પાછા c_str + 1;
}

std::string IdPath::str() અચલ
{
	std::string result;

	માટે (auto &item : *આ) {
		અચલ અક્ષર *pretty = pretty_name(item);
		જો (pretty[0] == '[') {
			result += pretty;
			ચાલુ;
		}
		જો (!result.empty())
			result += '.';
		result += pretty;
		જો (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	પાછા result;
}

તાર્કિક IdPath::get_address(પૂર્ણાંક &addr) અચલ
{
	જો (empty())
		પાછા ખોટું;
	auto &last = back();
	જો (!last.starts_with("\\["))
		પાછા ખોટું;
	જો (last == "\\[0]") {
		addr = 0;
		પાછા સાચું;
	}
	અક્ષર first = last.c_str()[2];
	જો (first < '1' || first > '9')
		પાછા ખોટું;
	અક્ષર *endptr;
	addr = std::strtol(last.c_str() + 2, &endptr, 10);
	પાછા endptr[0] == ']' && endptr[1] == 0;
}

સ્થિર std::vector<std::string> get_path(અચલ json11::Json &json)
{
	std::vector<std::string> result;
	માટે (auto &path_item : json.array_વસ્તુઓ()) {
		auto અચલ &path_item_str = path_item.string_કિંમત();
		જો (path_item_str.empty())
			પાછા {};;
		result.ધકેલો_back(path_item_str);
	}
	પાછા result;
}

ReadWitness::ReadWitness(અચલ std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	જો (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	જો (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_કિંમત();

	જો (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	જો (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	માટે (auto &clock_json : json["clocks"].array_વસ્તુઓ()) {
		Clock clock;
		clock.path = get_path(clock_json["path"]);
		જો (clock.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		auto edge_str = clock_json["edge"];
		જો (edge_str.string_કિંમત() == "posedge")
			clock.is_posedge = સાચું;
		નહીંતર જો (edge_str.string_કિંમત() == "negedge")
			clock.is_negedge = સાચું;
		નહીંતર
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		જો (!clock_json["offset"].is_સંખ્યા())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.offset = clock_json["offset"].int_કિંમત();
		જો (clock.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ધકેલો_back(clock);
	}

	પૂર્ણાંક bits_offset = 0;
	માટે (auto &signal_json : json["signals"].array_વસ્તુઓ()) {
		Signal સંકેત;
		સંકેત.bits_offset = bits_offset;
		સંકેત.path = get_path(signal_json["path"]);
		જો (સંકેત.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		જો (!signal_json["width"].is_સંખ્યા())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		સંકેત.width = signal_json["width"].int_કિંમત();
		જો (સંકેત.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += સંકેત.width;
		જો (!signal_json["offset"].is_સંખ્યા())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		સંકેત.offset = signal_json["offset"].int_કિંમત();
		જો (સંકેત.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		સંકેત.init_only = signal_json["init_only"].bool_કિંમત();
		signals.ધકેલો_back(સંકેત);
	}

	માટે (auto &step_json : json["steps"].array_વસ્તુઓ()) {
		Step step;
		જો (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_કિંમત();
		માટે (અક્ષર c : step.bits) {
			જો (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.ધકેલો_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(પૂર્ણાંક t, પૂર્ણાંક bits_offset, પૂર્ણાંક width) અચલ
{
	log_assert(t >= 0 && t < GetSize(steps));

	અચલ std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	પૂર્ણાંક read_begin = GetSize(bits) - 1 - bits_offset;
	પૂર્ણાંક read_end = max(-1, read_begin - width);

	માટે (પૂર્ણાંક i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		પસંદગી (bits[i]) {
			સ્થિતિ '0': bit = State::S0; તોડો;
			સ્થિતિ '1': bit = State::S1; તોડો;
			સ્થિતિ 'x': bit = State::Sx; તોડો;
			સ્થિતિ '?': bit = State::Sa; તોડો;
			મૂળભૂત:
				log_abort();
		}
		result.set(j, bit);
	}

	પાછા result;
}

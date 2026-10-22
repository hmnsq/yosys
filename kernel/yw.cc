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

#अन्तर्भाव "kernel/yw.h"
#अन्तर्भाव "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
स्थिर स्थिरांक अक्षर *pretty_name(स्थिरांक std::string &id)
{
	स्थिरांक अक्षर *c_str = id.c_str();
	स्थिरांक अक्षर *p = c_str;

	जर (*p != '\\')
		परत c_str;
	p++;

	जर (*p == '[') {
		p++;
		पर्यंत (*p >= '0' && *p <= '9')
			p++;
		जर (p[0] != ']' || p[1] != 0)
			परत c_str;
		परत c_str + 1;
	}

	जर (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		परत c_str;
	p++;
	पर्यंत ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	जर (*p != 0)
		परत c_str;
	परत c_str + 1;
}

std::string IdPath::str() स्थिरांक
{
	std::string result;

	साठी (स्वतः &item : *हे) {
		स्थिरांक अक्षर *pretty = pretty_name(item);
		जर (pretty[0] == '[') {
			result += pretty;
			सुरूठेवा;
		}
		जर (!result.empty())
			result += '.';
		result += pretty;
		जर (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	परत result;
}

तार्किक IdPath::get_address(पूर्णांक &addr) स्थिरांक
{
	जर (empty())
		परत असत्य;
	स्वतः &last = back();
	जर (!last.starts_with("\\["))
		परत असत्य;
	जर (last == "\\[0]") {
		addr = 0;
		परत सत्य;
	}
	अक्षर first = last.c_str()[2];
	जर (first < '1' || first > '9')
		परत असत्य;
	अक्षर *endptr;
	addr = std::म_से_दीर्घ(last.c_str() + 2, &endptr, 10);
	परत endptr[0] == ']' && endptr[1] == 0;
}

स्थिर std::vector<std::string> get_path(स्थिरांक json11::Json &json)
{
	std::vector<std::string> result;
	साठी (स्वतः &path_item : json.array_वस्तुसंच()) {
		स्वतः स्थिरांक &path_item_str = path_item.string_मूल्य();
		जर (path_item_str.empty())
			परत {};;
		result.ढकला_back(path_item_str);
	}
	परत result;
}

ReadWitness::ReadWitness(स्थिरांक std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	जर (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	जर (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_मूल्य();

	जर (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	जर (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	साठी (स्वतः &clock_json : json["clocks"].array_वस्तुसंच()) {
		Clock घड़ी;
		घड़ी.path = get_path(clock_json["path"]);
		जर (घड़ी.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		स्वतः edge_str = clock_json["edge"];
		जर (edge_str.string_मूल्य() == "posedge")
			घड़ी.is_posedge = सत्य;
		अन्यथा जर (edge_str.string_मूल्य() == "negedge")
			घड़ी.is_negedge = सत्य;
		अन्यथा
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		जर (!clock_json["offset"].is_संख्या())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		घड़ी.offset = clock_json["offset"].int_मूल्य();
		जर (घड़ी.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.ढकला_back(घड़ी);
	}

	पूर्णांक bits_offset = 0;
	साठी (स्वतः &signal_json : json["signals"].array_वस्तुसंच()) {
		Signal संकेत;
		संकेत.bits_offset = bits_offset;
		संकेत.path = get_path(signal_json["path"]);
		जर (संकेत.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		जर (!signal_json["width"].is_संख्या())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		संकेत.width = signal_json["width"].int_मूल्य();
		जर (संकेत.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += संकेत.width;
		जर (!signal_json["offset"].is_संख्या())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		संकेत.offset = signal_json["offset"].int_मूल्य();
		जर (संकेत.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		संकेत.init_only = signal_json["init_only"].bool_मूल्य();
		signals.ढकला_back(संकेत);
	}

	साठी (स्वतः &step_json : json["steps"].array_वस्तुसंच()) {
		Step step;
		जर (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_मूल्य();
		साठी (अक्षर c : step.bits) {
			जर (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.ढकला_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(पूर्णांक t, पूर्णांक bits_offset, पूर्णांक width) स्थिरांक
{
	log_assert(t >= 0 && t < GetSize(steps));

	स्थिरांक std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	पूर्णांक read_begin = GetSize(bits) - 1 - bits_offset;
	पूर्णांक read_end = max(-1, read_begin - width);

	साठी (पूर्णांक i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		निवड (bits[i]) {
			स्थिती '0': bit = State::S0; थांबा;
			स्थिती '1': bit = State::S1; थांबा;
			स्थिती 'x': bit = State::Sx; थांबा;
			स्थिती '?': bit = State::Sa; थांबा;
			पूर्वनिर्धारित:
				log_abort();
		}
		result.set(j, bit);
	}

	परत result;
}

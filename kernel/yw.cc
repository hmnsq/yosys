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

#समावेश "kernel/yw.h"
#समावेश "libs/json11/json11.hpp"

USING_YOSYS_NAMESPACE

// Use the same formatting as witness.py uses
अटल स्थिर अक्षर *pretty_name(स्थिर std::string &id)
{
	स्थिर अक्षर *c_str = id.c_str();
	स्थिर अक्षर *p = c_str;

	अगर (*p != '\\')
		वापस c_str;
	p++;

	अगर (*p == '[') {
		p++;
		जबतक (*p >= '0' && *p <= '9')
			p++;
		अगर (p[0] != ']' || p[1] != 0)
			वापस c_str;
		वापस c_str + 1;
	}

	अगर (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		वापस c_str;
	p++;
	जबतक ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	अगर (*p != 0)
		वापस c_str;
	वापस c_str + 1;
}

std::string IdPath::str() स्थिर
{
	std::string परिणाम;

	क्रम (स्वतः &item : *यह) {
		स्थिर अक्षर *pretty = pretty_name(item);
		अगर (pretty[0] == '[') {
			परिणाम += pretty;
			जारी;
		}
		अगर (!परिणाम.empty())
			परिणाम += '.';
		परिणाम += pretty;
		अगर (pretty[0] == '\\' || pretty[0] == '$')
			परिणाम += ' ';
	}

	वापस परिणाम;
}

bool IdPath::get_address(पूर्णांक &addr) स्थिर
{
	अगर (empty())
		वापस false;
	स्वतः &last = back();
	अगर (!last.starts_with("\\["))
		वापस false;
	अगर (last == "\\[0]") {
		addr = 0;
		वापस true;
	}
	अक्षर first = last.c_str()[2];
	अगर (first < '1' || first > '9')
		वापस false;
	अक्षर *endptr;
	addr = std::म_से_दीर्घ(last.c_str() + 2, &endptr, 10);
	वापस endptr[0] == ']' && endptr[1] == 0;
}

अटल std::vector<std::string> get_path(स्थिर json11::Json &json)
{
	std::vector<std::string> परिणाम;
	क्रम (स्वतः &path_item : json.array_वस्तुएँ()) {
		स्वतः स्थिर &path_item_str = path_item.string_मूल्य();
		अगर (path_item_str.empty())
			वापस {};;
		परिणाम.धकेलो_back(path_item_str);
	}
	वापस परिणाम;
}

ReadWitness::ReadWitness(स्थिर std::string &filename) :
	filename(filename)
{
	std::अख_प्रवाह f(filename.c_str());
	अगर (f.हार() || Get_आकार(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	अगर (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_मूल्य();

	अगर (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	अगर (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	क्रम (स्वतः &clock_json : json["clocks"].array_वस्तुएँ()) {
		Clock घड़ी;
		घड़ी.path = get_path(clock_json["path"]);
		अगर (घड़ी.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		स्वतः edge_str = clock_json["edge"];
		अगर (edge_str.string_मूल्य() == "posedge")
			घड़ी.is_posedge = true;
		अन्यथा अगर (edge_str.string_मूल्य() == "negedge")
			घड़ी.is_negedge = true;
		अन्यथा
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		अगर (!clock_json["offset"].is_संख्या())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		घड़ी.offset = clock_json["offset"].int_मूल्य();
		अगर (घड़ी.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.धकेलो_back(घड़ी);
	}

	पूर्णांक bits_offset = 0;
	क्रम (स्वतः &signal_json : json["signals"].array_वस्तुएँ()) {
		Signal संकेत;
		संकेत.bits_offset = bits_offset;
		संकेत.path = get_path(signal_json["path"]);
		अगर (संकेत.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		अगर (!signal_json["width"].is_संख्या())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		संकेत.width = signal_json["width"].int_मूल्य();
		अगर (संकेत.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += संकेत.width;
		अगर (!signal_json["offset"].is_संख्या())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		संकेत.offset = signal_json["offset"].int_मूल्य();
		अगर (संकेत.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		संकेत.init_only = signal_json["init_only"].bool_मूल्य();
		signals.धकेलो_back(संकेत);
	}

	क्रम (स्वतः &step_json : json["steps"].array_वस्तुएँ()) {
		Step step;
		अगर (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_आकार(steps));
		step.bits = step_json["bits"].string_मूल्य();
		क्रम (अक्षर c : step.bits) {
			अगर (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_आकार(steps));
		}
		steps.धकेलो_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(पूर्णांक t, पूर्णांक bits_offset, पूर्णांक width) स्थिर
{
	log_assert(t >= 0 && t < Get_आकार(steps));

	स्थिर std::string &bits = steps[t].bits;

	RTLIL::Const परिणाम(State::Sa, width);

	पूर्णांक read_begin = Get_आकार(bits) - 1 - bits_offset;
	पूर्णांक read_end = max(-1, read_begin - width);

	क्रम (पूर्णांक i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		चयन (bits[i]) {
			हाल '0': bit = State::S0; अवरोध;
			हाल '1': bit = State::S1; अवरोध;
			हाल 'x': bit = State::Sx; अवरोध;
			हाल '?': bit = State::Sa; अवरोध;
			शेष:
				log_abort();
		}
		परिणाम.set(j, bit);
	}

	वापस परिणाम;
}

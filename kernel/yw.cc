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
स्थिर अचर वर्ण *pretty_name(अचर std::string &id)
{
	अचर वर्ण *c_str = id.c_str();
	अचर वर्ण *p = c_str;

	यदि (*p != '\\')
		फर्काऊ c_str;
	p++;

	यदि (*p == '[') {
		p++;
		जबसम्म (*p >= '0' && *p <= '9')
			p++;
		यदि (p[0] != ']' || p[1] != 0)
			फर्काऊ c_str;
		फर्काऊ c_str + 1;
	}

	यदि (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		फर्काऊ c_str;
	p++;
	जबसम्म ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	यदि (*p != 0)
		फर्काऊ c_str;
	फर्काऊ c_str + 1;
}

std::string IdPath::str() अचर
{
	std::string result;

	लागि (स्वतः &item : *एतत्) {
		अचर वर्ण *pretty = pretty_name(item);
		यदि (pretty[0] == '[') {
			result += pretty;
			जारी;
		}
		यदि (!result.empty())
			result += '.';
		result += pretty;
		यदि (pretty[0] == '\\' || pretty[0] == '$')
			result += ' ';
	}

	फर्काऊ result;
}

तार्किक IdPath::get_address(पूर्णाङ्क &addr) अचर
{
	यदि (empty())
		फर्काऊ असत्य;
	स्वतः &last = back();
	यदि (!last.starts_with("\\["))
		फर्काऊ असत्य;
	यदि (last == "\\[0]") {
		addr = 0;
		फर्काऊ सत्य;
	}
	वर्ण first = last.c_str()[2];
	यदि (first < '1' || first > '9')
		फर्काऊ असत्य;
	वर्ण *endptr;
	addr = std::म_से_दीर्घ(last.c_str() + 2, &endptr, 10);
	फर्काऊ endptr[0] == ']' && endptr[1] == 0;
}

स्थिर std::vector<std::string> get_path(अचर json11::Json &json)
{
	std::vector<std::string> result;
	लागि (स्वतः &path_item : json.array_वस्तुहरू()) {
		स्वतः अचर &path_item_str = path_item.string_मान();
		यदि (path_item_str.empty())
			फर्काऊ {};;
		result.थप_back(path_item_str);
	}
	फर्काऊ result;
}

ReadWitness::ReadWitness(अचर std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	यदि (f.fail() || GetSize(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	यदि (!err.empty())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_मान();

	यदि (format.empty())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	यदि (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	लागि (स्वतः &clock_json : json["clocks"].array_वस्तुहरू()) {
		Clock घड़ी;
		घड़ी.path = get_path(clock_json["path"]);
		यदि (घड़ी.path.empty())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		स्वतः edge_str = clock_json["edge"];
		यदि (edge_str.string_मान() == "posedge")
			घड़ी.is_posedge = सत्य;
		अन्यथा यदि (edge_str.string_मान() == "negedge")
			घड़ी.is_negedge = सत्य;
		अन्यथा
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		यदि (!clock_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		घड़ी.offset = clock_json["offset"].int_मान();
		यदि (घड़ी.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.थप_back(घड़ी);
	}

	पूर्णाङ्क bits_offset = 0;
	लागि (स्वतः &signal_json : json["signals"].array_वस्तुहरू()) {
		Signal सङ्केत;
		सङ्केत.bits_offset = bits_offset;
		सङ्केत.path = get_path(signal_json["path"]);
		यदि (सङ्केत.path.empty())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		यदि (!signal_json["width"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.width = signal_json["width"].int_मान();
		यदि (सङ्केत.width < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		bits_offset += सङ्केत.width;
		यदि (!signal_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.offset = signal_json["offset"].int_मान();
		यदि (सङ्केत.offset < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.init_only = signal_json["init_only"].bool_मान();
		signals.थप_back(सङ्केत);
	}

	लागि (स्वतः &step_json : json["steps"].array_वस्तुहरू()) {
		Step step;
		यदि (!step_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, GetSize(steps));
		step.bits = step_json["bits"].string_मान();
		लागि (वर्ण c : step.bits) {
			यदि (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, GetSize(steps));
		}
		steps.थप_back(step);
	}
}

RTLIL::Const ReadWitness::get_bits(पूर्णाङ्क t, पूर्णाङ्क bits_offset, पूर्णाङ्क width) अचर
{
	log_assert(t >= 0 && t < GetSize(steps));

	अचर std::string &bits = steps[t].bits;

	RTLIL::Const result(State::Sa, width);

	पूर्णाङ्क read_begin = GetSize(bits) - 1 - bits_offset;
	पूर्णाङ्क read_end = max(-1, read_begin - width);

	लागि (पूर्णाङ्क i = read_begin, j = 0; i > read_end; i--, j++) {
		RTLIL::State bit = State::Sa;
		विकल्प (bits[i]) {
			पक्ष '0': bit = State::S0; रोक;
			पक्ष '1': bit = State::S1; रोक;
			पक्ष 'x': bit = State::Sx; रोक;
			पक्ष '?': bit = State::Sa; रोक;
			स्वभाव:
				log_abort();
		}
		result.set(j, bit);
	}

	फर्काऊ result;
}

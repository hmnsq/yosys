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
स्थिर ध्रुव वर्ण *pretty_name(ध्रुव std::string &id)
{
	ध्रुव वर्ण *c_str = id.c_str();
	ध्रुव वर्ण *p = c_str;

	यदि (*p != '\\')
		प्रत्यागम c_str;
	p++;

	यदि (*p == '[') {
		p++;
		यावत् (*p >= '0' && *p <= '9')
			p++;
		यदि (p[0] != ']' || p[1] != 0)
			प्रत्यागम c_str;
		प्रत्यागम c_str + 1;
	}

	यदि (!(*p >= 'a' && *p <= 'z') && !(*p >= 'A' && *p <= 'Z') && *p != '_')
		प्रत्यागम c_str;
	p++;
	यावत् ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
		p++;

	यदि (*p != 0)
		प्रत्यागम c_str;
	प्रत्यागम c_str + 1;
}

std::string Id_मार्गः::str() ध्रुव
{
	std::string परिणाम;

	प्रति (स्वचालित &item : *एतत्) {
		ध्रुव वर्ण *pretty = pretty_name(item);
		यदि (pretty[0] == '[') {
			परिणाम += pretty;
			अनुवर्त;
		}
		यदि (!परिणाम.रिक्तानि())
			परिणाम += '.';
		परिणाम += pretty;
		यदि (pretty[0] == '\\' || pretty[0] == '$')
			परिणाम += ' ';
	}

	प्रत्यागम परिणाम;
}

तार्किक Id_मार्गः::get_स्थानाङ्कः(पूर्णाङ्क &addr) ध्रुव
{
	यदि (रिक्तानि())
		प्रत्यागम असत्य;
	स्वचालित &चरमः = पृष्ठभागः();
	यदि (!चरमः.starts_with("\\["))
		प्रत्यागम असत्य;
	यदि (चरमः == "\\[0]") {
		addr = 0;
		प्रत्यागम सत्य;
	}
	वर्ण first = चरमः.c_str()[2];
	यदि (first < '1' || first > '9')
		प्रत्यागम असत्य;
	वर्ण *endptr;
	addr = std::strtol(चरमः.c_str() + 2, &endptr, 10);
	प्रत्यागम endptr[0] == ']' && endptr[1] == 0;
}

स्थिर std::सदिशम्<std::string> get_मार्गः(ध्रुव json11::Json &json)
{
	std::सदिशम्<std::string> परिणाम;
	प्रति (स्वचालित &मार्गः_item : json.array_वस्तूनि()) {
		स्वचालित ध्रुव &मार्गः_item_str = मार्गः_item.string_मानम्();
		यदि (मार्गः_item_str.रिक्तानि())
			प्रत्यागम {};;
		परिणाम.प्रक्षिप_पृष्ठभागः(मार्गः_item_str);
	}
	प्रत्यागम परिणाम;
}

ReadWitness::ReadWitness(ध्रुव std::string &filename) :
	filename(filename)
{
	std::ifstream f(filename.c_str());
	यदि (f.fail() || Get_आकार(filename) == 0)
		log_error("Cannot open file `%s`\n", filename);
	std::stringstream buf;
	buf << f.rdbuf();
	std::string err;
	json11::Json json = json11::Json::parse(buf.str(), err);
	यदि (!err.रिक्तानि())
		log_error("Failed to parse `%s`: %s\n", filename, err);

	std::string format = json["format"].string_मानम्();

	यदि (format.रिक्तानि())
		log_error("Failed to parse `%s`: Unknown format\n", filename);
	यदि (format != "Yosys Witness Trace")
		log_error("Failed to parse `%s`: Unsupported format `%s`\n", filename, format);

	प्रति (स्वचालित &clock_json : json["clocks"].array_वस्तूनि()) {
		Clock clock;
		clock.मार्गः_2 = get_मार्गः(clock_json["path"]);
		यदि (clock.मार्गः_2.रिक्तानि())
			log_error("Failed to parse `%s`: Missing path for clock `%s`\n", filename, clock_json.dump());
		स्वचालित edge_str = clock_json["edge"];
		यदि (edge_str.string_मानम्() == "posedge")
			clock.is_posedge = सत्य;
		अन्यथा यदि (edge_str.string_मानम्() == "negedge")
			clock.is_negedge = सत्य;
		अन्यथा
			log_error("Failed to parse `%s`: Unknown edge type for clock `%s`\n", filename, clock_json.dump());
		यदि (!clock_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for clock `%s`\n", filename, clock_json.dump());
		clock.विस्थापनम् = clock_json["offset"].int_मानम्();
		यदि (clock.विस्थापनम् < 0)
			log_error("Failed to parse `%s`: Invalid offset for clock `%s`\n", filename, clock_json.dump());
		clocks.प्रक्षिप_पृष्ठभागः(clock);
	}

	पूर्णाङ्क द्वयङ्काः_विस्थापनम् = 0;
	प्रति (स्वचालित &signal_json : json["signals"].array_वस्तूनि()) {
		Signal सङ्केत;
		सङ्केत.द्वयङ्काः_विस्थापनम् = द्वयङ्काः_विस्थापनम्;
		सङ्केत.मार्गः_2 = get_मार्गः(signal_json["path"]);
		यदि (सङ्केत.मार्गः_2.रिक्तानि())
			log_error("Failed to parse `%s`: Missing path for signal `%s`\n", filename, signal_json.dump());
		यदि (!signal_json["width"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown width for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.विस्तारः = signal_json["width"].int_मानम्();
		यदि (सङ्केत.विस्तारः < 0)
			log_error("Failed to parse `%s`: Invalid width for signal `%s`\n", filename, signal_json.dump());
		द्वयङ्काः_विस्थापनम् += सङ्केत.विस्तारः;
		यदि (!signal_json["offset"].is_सङ्ख्या())
			log_error("Failed to parse `%s`: Unknown offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.विस्थापनम् = signal_json["offset"].int_मानम्();
		यदि (सङ्केत.विस्थापनम् < 0)
			log_error("Failed to parse `%s`: Invalid offset for signal `%s`\n", filename, signal_json.dump());
		सङ्केत.init_only = signal_json["init_only"].bool_मानम्();
		सङ्केताः.प्रक्षिप_पृष्ठभागः(सङ्केत);
	}

	प्रति (स्वचालित &चरणः_json : json["steps"].array_वस्तूनि()) {
		चरणः_2 चरणः;
		यदि (!चरणः_json["bits"].is_string())
			log_error("Failed to parse `%s`: Expected string as bits value for step %d\n", filename, Get_आकार(चरणाः));
		चरणः.द्वयङ्काः = चरणः_json["bits"].string_मानम्();
		प्रति (वर्ण c : चरणः.द्वयङ्काः) {
			यदि (c != '0' && c != '1' && c != 'x' && c != '?')
				log_error("Failed to parse `%s`: Invalid bit '%c' value for step %d\n", filename, c, Get_आकार(चरणाः));
		}
		चरणाः.प्रक्षिप_पृष्ठभागः(चरणः);
	}
}

RTLIL::Const ReadWitness::get_द्वयङ्काः(पूर्णाङ्क t, पूर्णाङ्क द्वयङ्काः_विस्थापनम्, पूर्णाङ्क विस्तारः) ध्रुव
{
	log_assert(t >= 0 && t < Get_आकार(चरणाः));

	ध्रुव std::string &द्वयङ्काः = चरणाः[t].द्वयङ्काः;

	RTLIL::Const परिणाम(State::Sa, विस्तारः);

	पूर्णाङ्क read_begin = Get_आकार(द्वयङ्काः) - 1 - द्वयङ्काः_विस्थापनम्;
	पूर्णाङ्क read_अन्तः = max(-1, read_begin - विस्तारः);

	प्रति (पूर्णाङ्क i = read_begin, j = 0; i > read_अन्तः; i--, j++) {
		RTLIL::State bit = State::Sa;
		विकल्प (द्वयङ्काः[i]) {
			पक्ष '0': bit = State::S0; भङ्ग;
			पक्ष '1': bit = State::S1; भङ्ग;
			पक्ष 'x': bit = State::Sx; भङ्ग;
			पक्ष '?': bit = State::Sa; भङ्ग;
			स्वभाव:
				log_abort();
		}
		परिणाम.समुच्चयः(j, bit);
	}

	प्रत्यागम परिणाम;
}

/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2025  Krystine Dawn <krystinedawn@yosyshq.com>
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

#अगर_अघोषित LOG_HELP_H
#घोषणा LOG_HELP_H

#समावेश "kernel/yosys_common.h"
#समावेश "kernel/json.h"
#समावेश <filesystem>
#समावेश <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (स्थिर अक्षर* source_file,
	स्थिर std::filesystem::path &source_मूल, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
खुला:
	string type;
	string body;
	स्थिर अक्षर* source_file;
	पूर्णांक source_पंक्ति;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		स्थिर अक्षर* source_file = "unknown", पूर्णांक source_पंक्ति = 0
	) : type(type), body(body), source_file(source_file), source_पंक्ति(source_पंक्ति) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.पंक्ति()) { }

	व्योम add_content(string type, string body, source_location location) {
		_content.धकेलो_back({type, body, location});
	}

	bool has_source() स्थिर { वापस म_माप(source_file) && म_भेद(source_file, "unknown") != 0; }
	bool has_content() स्थिर { वापस _content.आकार() != 0; }

	vector<ContentListing>::const_iterator begin() स्थिर { वापस _content.cbegin(); }
	vector<ContentListing>::const_iterator end() स्थिर { वापस _content.cend(); }

	ContentListing* back() {
		वापस &_content.back();
	}

	व्योम set_option(string कुंजी, string val = "") {
		options[कुंजी] = val;
	}

	व्योम usage(
		स्थिर string &text,
		स्थिर source_location location = source_location::current()
	);
	व्योम option(
		स्थिर string &text,
		स्थिर string &description = "",
		स्थिर source_location location = source_location::current()
	);
	व्योम codeblock(
		स्थिर string &code,
		स्थिर string &language = "none",
		स्थिर source_location location = source_location::current()
	);
	व्योम paragraph(
		स्थिर string &text,
		स्थिर source_location location = source_location::current()
	);

	ContentListing* open_usage(
		स्थिर string &text,
		स्थिर source_location location = source_location::current()
	);
	ContentListing* open_option(
		स्थिर string &text,
		स्थिर source_location location = source_location::current()
	);

	Json to_json(स्थिर std::filesystem::path &source_मूल) स्थिर;
};

class PrettyHelp
{
खुला:
	string group = "unknown";

निजी:
	PrettyHelp *_prior;
	ContentListing मूल_listing;

खुला:
	PrettyHelp();
	~PrettyHelp();

	अटल PrettyHelp *get_current();

	bool has_content() स्थिर { वापस मूल_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() स्थिर { वापस मूल_listing.begin(); }
	vector<ContentListing>::const_iterator end() स्थिर { वापस मूल_listing.end(); }

	ContentListing* get_मूल() {
		वापस &मूल_listing;
	}

	व्योम set_group(स्थिर string g) { group = g; }
	bool has_group() स्थिर { वापस group.compare("unknown") != 0; }

	व्योम log_help() स्थिर;
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर

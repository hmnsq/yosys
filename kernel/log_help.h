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

#ifndef LOG_HELP_H
#define LOG_HELP_H

#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/json.h"
#അന്തര്ഭാവ <filesystem>
#അന്തര്ഭാവ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (സ്ഥിരം അക്ഷരം* source_file,
	സ്ഥിരം std::filesystem::path &source_വേര്, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
പൊതു:
	string type;
	string body;
	സ്ഥിരം അക്ഷരം* source_file;
	പൂർണ്ണസംഖ്യ source_വരി;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		സ്ഥിരം അക്ഷരം* source_file = "unknown", പൂർണ്ണസംഖ്യ source_വരി = 0
	) : type(type), body(body), source_file(source_file), source_വരി(source_വരി) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.വരി()) { }

	ശൂന്യം add_content(string type, string body, source_location location) {
		_content.തള്ളുക_back({type, body, location});
	}

	bool has_source() സ്ഥിരം { തിരികെ strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() സ്ഥിരം { തിരികെ _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() സ്ഥിരം { തിരികെ _content.cbegin(); }
	vector<ContentListing>::const_iterator end() സ്ഥിരം { തിരികെ _content.cend(); }

	ContentListing* back() {
		തിരികെ &_content.back();
	}

	ശൂന്യം set_option(string താക്കോൽ, string val = "") {
		options[താക്കോൽ] = val;
	}

	ശൂന്യം usage(
		സ്ഥിരം string &text,
		സ്ഥിരം source_location location = source_location::current()
	);
	ശൂന്യം option(
		സ്ഥിരം string &text,
		സ്ഥിരം string &description = "",
		സ്ഥിരം source_location location = source_location::current()
	);
	ശൂന്യം codeblock(
		സ്ഥിരം string &code,
		സ്ഥിരം string &language = "none",
		സ്ഥിരം source_location location = source_location::current()
	);
	ശൂന്യം paragraph(
		സ്ഥിരം string &text,
		സ്ഥിരം source_location location = source_location::current()
	);

	ContentListing* open_usage(
		സ്ഥിരം string &text,
		സ്ഥിരം source_location location = source_location::current()
	);
	ContentListing* open_option(
		സ്ഥിരം string &text,
		സ്ഥിരം source_location location = source_location::current()
	);

	Json to_json(സ്ഥിരം std::filesystem::path &source_വേര്) സ്ഥിരം;
};

class PrettyHelp
{
പൊതു:
	string group = "unknown";

സ്വകാര്യ:
	PrettyHelp *_prior;
	ContentListing വേര്_listing;

പൊതു:
	PrettyHelp();
	~PrettyHelp();

	സ്ഥിതം PrettyHelp *get_current();

	bool has_content() സ്ഥിരം { തിരികെ വേര്_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() സ്ഥിരം { തിരികെ വേര്_listing.begin(); }
	vector<ContentListing>::const_iterator end() സ്ഥിരം { തിരികെ വേര്_listing.end(); }

	ContentListing* get_വേര്() {
		തിരികെ &വേര്_listing;
	}

	ശൂന്യം set_group(സ്ഥിരം string g) { group = g; }
	bool has_group() സ്ഥിരം { തിരികെ group.compare("unknown") != 0; }

	ശൂന്യം log_help() സ്ഥിരം;
};

YOSYS_NAMESPACE_END

#endif

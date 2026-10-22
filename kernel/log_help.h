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

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/json.h"
#ضمن <filesystem>
#ضمن <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (ثابت حرف* source_file,
	ثابت std::filesystem::path &source_سيو, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
عام:
	string type;
	string body;
	ثابت حرف* source_file;
	صحيح source_لي;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت حرف* source_file = "unknown", صحيح source_لي = 0
	) : type(type), body(body), source_file(source_file), source_لي(source_لي) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.لي()) { }

	فراغ add_content(string type, string body, source_location location) {
		_content.تور_back({type, body, location});
	}

	bool has_source() ثابت { أرجع strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() ثابت { أرجع _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() ثابت { أرجع _content.cbegin(); }
	vector<ContentListing>::const_iterator end() ثابت { أرجع _content.cend(); }

	ContentListing* back() {
		أرجع &_content.back();
	}

	فراغ set_option(string مبودي, string val = "") {
		options[مبودي] = val;
	}

	فراغ usage(
		ثابت string &text,
		ثابت source_location location = source_location::current()
	);
	فراغ option(
		ثابت string &text,
		ثابت string &description = "",
		ثابت source_location location = source_location::current()
	);
	فراغ codeblock(
		ثابت string &code,
		ثابت string &language = "none",
		ثابت source_location location = source_location::current()
	);
	فراغ paragraph(
		ثابت string &text,
		ثابت source_location location = source_location::current()
	);

	ContentListing* open_usage(
		ثابت string &text,
		ثابت source_location location = source_location::current()
	);
	ContentListing* open_option(
		ثابت string &text,
		ثابت source_location location = source_location::current()
	);

	Json to_json(ثابت std::filesystem::path &source_سيو) ثابت;
};

class PrettyHelp
{
عام:
	string group = "unknown";

خاص:
	PrettyHelp *_prior;
	ContentListing سيو_listing;

عام:
	PrettyHelp();
	~PrettyHelp();

	ساكن PrettyHelp *get_current();

	bool has_content() ثابت { أرجع سيو_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() ثابت { أرجع سيو_listing.begin(); }
	vector<ContentListing>::const_iterator end() ثابت { أرجع سيو_listing.end(); }

	ContentListing* get_سيو() {
		أرجع &سيو_listing;
	}

	فراغ set_group(ثابت string g) { group = g; }
	bool has_group() ثابت { أرجع group.compare("unknown") != 0; }

	فراغ log_help() ثابت;
};

YOSYS_NAMESPACE_END

#endif

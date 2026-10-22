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

#شامل "kernel/yosys_common.h"
#شامل "kernel/json.h"
#شامل <filesystem>
#شامل <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (ثابت نویسه* source_file,
	ثابت std::filesystem::path &source_ریشه, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
عمومی:
	string type;
	string body;
	ثابت نویسه* source_file;
	صحیح source_سطر;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت نویسه* source_file = "unknown", صحیح source_سطر = 0
	) : type(type), body(body), source_file(source_file), source_سطر(source_سطر) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.سطر()) { }

	تهی add_content(string type, string body, source_location location) {
		_content.افزودن_back({type, body, location});
	}

	bool has_source() ثابت { بازگشت strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() ثابت { بازگشت _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() ثابت { بازگشت _content.cbegin(); }
	vector<ContentListing>::const_iterator end() ثابت { بازگشت _content.cend(); }

	ContentListing* back() {
		بازگشت &_content.back();
	}

	تهی set_option(string کلید, string val = "") {
		options[کلید] = val;
	}

	تهی usage(
		ثابت string &text,
		ثابت source_location location = source_location::current()
	);
	تهی option(
		ثابت string &text,
		ثابت string &description = "",
		ثابت source_location location = source_location::current()
	);
	تهی codeblock(
		ثابت string &code,
		ثابت string &language = "none",
		ثابت source_location location = source_location::current()
	);
	تهی paragraph(
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

	Json to_json(ثابت std::filesystem::path &source_ریشه) ثابت;
};

class PrettyHelp
{
عمومی:
	string group = "unknown";

خصوصی:
	PrettyHelp *_prior;
	ContentListing ریشه_listing;

عمومی:
	PrettyHelp();
	~PrettyHelp();

	ایستا PrettyHelp *get_current();

	bool has_content() ثابت { بازگشت ریشه_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() ثابت { بازگشت ریشه_listing.begin(); }
	vector<ContentListing>::const_iterator end() ثابت { بازگشت ریشه_listing.end(); }

	ContentListing* get_ریشه() {
		بازگشت &ریشه_listing;
	}

	تهی set_group(ثابت string g) { group = g; }
	bool has_group() ثابت { بازگشت group.compare("unknown") != 0; }

	تهی log_help() ثابت;
};

YOSYS_NAMESPACE_END

#endif

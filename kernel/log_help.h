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

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/json.h"
#अन्तर्भाव <filesystem>
#अन्तर्भाव <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (ध्रुव वर्ण* source_file,
	ध्रुव std::filesystem::path &source_मूळ, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
सार्वजनिक:
	string type;
	string body;
	ध्रुव वर्ण* source_file;
	पूर्णाङ्क source_ओळ;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ध्रुव वर्ण* source_file = "unknown", पूर्णाङ्क source_ओळ = 0
	) : type(type), body(body), source_file(source_file), source_ओळ(source_ओळ) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.ओळ()) { }

	शून्य add_content(string type, string body, source_location location) {
		_content.ढकला_back({type, body, location});
	}

	bool has_source() ध्रुव { प्रत्यागम strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() ध्रुव { प्रत्यागम _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() ध्रुव { प्रत्यागम _content.cbegin(); }
	vector<ContentListing>::const_iterator end() ध्रुव { प्रत्यागम _content.cend(); }

	ContentListing* back() {
		प्रत्यागम &_content.back();
	}

	शून्य set_option(string किल्ली, string val = "") {
		options[किल्ली] = val;
	}

	शून्य usage(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);
	शून्य option(
		ध्रुव string &text,
		ध्रुव string &description = "",
		ध्रुव source_location location = source_location::current()
	);
	शून्य codeblock(
		ध्रुव string &code,
		ध्रुव string &language = "none",
		ध्रुव source_location location = source_location::current()
	);
	शून्य paragraph(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);

	ContentListing* open_usage(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);
	ContentListing* open_option(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);

	Json to_json(ध्रुव std::filesystem::path &source_मूळ) ध्रुव;
};

class PrettyHelp
{
सार्वजनिक:
	string group = "unknown";

गुप्त:
	PrettyHelp *_prior;
	ContentListing मूळ_listing;

सार्वजनिक:
	PrettyHelp();
	~PrettyHelp();

	स्थिर PrettyHelp *get_current();

	bool has_content() ध्रुव { प्रत्यागम मूळ_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() ध्रुव { प्रत्यागम मूळ_listing.begin(); }
	vector<ContentListing>::const_iterator end() ध्रुव { प्रत्यागम मूळ_listing.end(); }

	ContentListing* get_मूळ() {
		प्रत्यागम &मूळ_listing;
	}

	शून्य set_group(ध्रुव string g) { group = g; }
	bool has_group() ध्रुव { प्रत्यागम group.compare("unknown") != 0; }

	शून्य log_help() ध्रुव;
};

YOSYS_NAMESPACE_END

#endif

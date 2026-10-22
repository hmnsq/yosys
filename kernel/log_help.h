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

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/json.h"
#ಅನ್ತರ್ಭಾವ <filesystem>
#ಅನ್ತರ್ಭಾವ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (ಧ್ರುವ ವರ್ಣ* source_file,
	ಧ್ರುವ std::filesystem::path &source_ಬೇರು, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
ಸಾರ್ವಜನಿಕ:
	string type;
	string body;
	ಧ್ರುವ ವರ್ಣ* source_file;
	ಪೂರ್ಣಾಙ್ಕ source_ಸಾಲು;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ಧ್ರುವ ವರ್ಣ* source_file = "unknown", ಪೂರ್ಣಾಙ್ಕ source_ಸಾಲು = 0
	) : type(type), body(body), source_file(source_file), source_ಸಾಲು(source_ಸಾಲು) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.ಸಾಲು()) { }

	ಶೂನ್ಯ add_content(string type, string body, source_location location) {
		_content.ತಳ್ಳು_back({type, body, location});
	}

	bool has_source() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _content.cbegin(); }
	vector<ContentListing>::const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _content.cend(); }

	ContentListing* back() {
		ಪ್ರತ್ಯಾಗಮ &_content.back();
	}

	ಶೂನ್ಯ set_option(string ಕೀಲಿ, string val = "") {
		options[ಕೀಲಿ] = val;
	}

	ಶೂನ್ಯ usage(
		ಧ್ರುವ string &text,
		ಧ್ರುವ source_location location = source_location::current()
	);
	ಶೂನ್ಯ option(
		ಧ್ರುವ string &text,
		ಧ್ರುವ string &description = "",
		ಧ್ರುವ source_location location = source_location::current()
	);
	ಶೂನ್ಯ codeblock(
		ಧ್ರುವ string &code,
		ಧ್ರುವ string &language = "none",
		ಧ್ರುವ source_location location = source_location::current()
	);
	ಶೂನ್ಯ paragraph(
		ಧ್ರುವ string &text,
		ಧ್ರುವ source_location location = source_location::current()
	);

	ContentListing* open_usage(
		ಧ್ರುವ string &text,
		ಧ್ರುವ source_location location = source_location::current()
	);
	ContentListing* open_option(
		ಧ್ರುವ string &text,
		ಧ್ರುವ source_location location = source_location::current()
	);

	Json to_json(ಧ್ರುವ std::filesystem::path &source_ಬೇರು) ಧ್ರುವ;
};

class PrettyHelp
{
ಸಾರ್ವಜನಿಕ:
	string group = "unknown";

ಗುಪ್ತ:
	PrettyHelp *_prior;
	ContentListing ಬೇರು_listing;

ಸಾರ್ವಜನಿಕ:
	PrettyHelp();
	~PrettyHelp();

	ಸ್ಥಿರ PrettyHelp *get_current();

	bool has_content() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಬೇರು_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಬೇರು_listing.begin(); }
	vector<ContentListing>::const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಬೇರು_listing.end(); }

	ContentListing* get_ಬೇರು() {
		ಪ್ರತ್ಯಾಗಮ &ಬೇರು_listing;
	}

	ಶೂನ್ಯ set_group(ಧ್ರುವ string g) { group = g; }
	bool has_group() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ group.compare("unknown") != 0; }

	ಶೂನ್ಯ log_help() ಧ್ರುವ;
};

YOSYS_NAMESPACE_END

#endif

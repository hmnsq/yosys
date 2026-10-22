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

#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/json.h"
#અન્તર્ભાવ <filesystem>
#અન્તર્ભાવ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (અચલ અક્ષર* source_file,
	અચલ std::filesystem::path &source_મૂળ, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
જાહેર:
	string type;
	string body;
	અચલ અક્ષર* source_file;
	પૂર્ણાંક source_લીટી;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		અચલ અક્ષર* source_file = "unknown", પૂર્ણાંક source_લીટી = 0
	) : type(type), body(body), source_file(source_file), source_લીટી(source_લીટી) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.લીટી()) { }

	શૂન્ય add_content(string type, string body, source_location location) {
		_content.ધકેલો_back({type, body, location});
	}

	bool has_source() અચલ { પાછા strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() અચલ { પાછા _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() અચલ { પાછા _content.cbegin(); }
	vector<ContentListing>::const_iterator end() અચલ { પાછા _content.cend(); }

	ContentListing* back() {
		પાછા &_content.back();
	}

	શૂન્ય set_option(string ચાવી, string val = "") {
		options[ચાવી] = val;
	}

	શૂન્ય usage(
		અચલ string &text,
		અચલ source_location location = source_location::current()
	);
	શૂન્ય option(
		અચલ string &text,
		અચલ string &description = "",
		અચલ source_location location = source_location::current()
	);
	શૂન્ય codeblock(
		અચલ string &code,
		અચલ string &language = "none",
		અચલ source_location location = source_location::current()
	);
	શૂન્ય paragraph(
		અચલ string &text,
		અચલ source_location location = source_location::current()
	);

	ContentListing* open_usage(
		અચલ string &text,
		અચલ source_location location = source_location::current()
	);
	ContentListing* open_option(
		અચલ string &text,
		અચલ source_location location = source_location::current()
	);

	Json to_json(અચલ std::filesystem::path &source_મૂળ) અચલ;
};

class PrettyHelp
{
જાહેર:
	string group = "unknown";

ખાનગી:
	PrettyHelp *_prior;
	ContentListing મૂળ_listing;

જાહેર:
	PrettyHelp();
	~PrettyHelp();

	સ્થિર PrettyHelp *get_current();

	bool has_content() અચલ { પાછા મૂળ_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() અચલ { પાછા મૂળ_listing.begin(); }
	vector<ContentListing>::const_iterator end() અચલ { પાછા મૂળ_listing.end(); }

	ContentListing* get_મૂળ() {
		પાછા &મૂળ_listing;
	}

	શૂન્ય set_group(અચલ string g) { group = g; }
	bool has_group() અચલ { પાછા group.compare("unknown") != 0; }

	શૂન્ય log_help() અચલ;
};

YOSYS_NAMESPACE_END

#endif

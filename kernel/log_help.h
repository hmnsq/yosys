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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/json.h"
#ଅନ୍ତର୍ଭାଵ <filesystem>
#ଅନ୍ତର୍ଭାଵ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (ଅଚଳ ଅକ୍ଷର* source_file,
	ଅଚଳ std::filesystem::path &source_ମୂଳ, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
ସର୍ବସାଧାରଣ:
	string type;
	string body;
	ଅଚଳ ଅକ୍ଷର* source_file;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା source_ଧାଡ଼ି;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ଅଚଳ ଅକ୍ଷର* source_file = "unknown", ପୂର୍ଣ୍ଣସଂଖ୍ୟା source_ଧାଡ଼ି = 0
	) : type(type), body(body), source_file(source_file), source_ଧାଡ଼ି(source_ଧାଡ଼ି) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.ଧାଡ଼ି()) { }

	ଶୂନ୍ୟ add_content(string type, string body, source_location location) {
		_content.ଠେଲ_back({type, body, location});
	}

	bool has_source() ଅଚଳ { ଫେରନ୍ତୁ strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() ଅଚଳ { ଫେରନ୍ତୁ _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ _content.cbegin(); }
	vector<ContentListing>::const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ _content.cend(); }

	ContentListing* back() {
		ଫେରନ୍ତୁ &_content.back();
	}

	ଶୂନ୍ୟ set_option(string ଚାବି, string val = "") {
		options[ଚାବି] = val;
	}

	ଶୂନ୍ୟ usage(
		ଅଚଳ string &text,
		ଅଚଳ source_location location = source_location::current()
	);
	ଶୂନ୍ୟ option(
		ଅଚଳ string &text,
		ଅଚଳ string &description = "",
		ଅଚଳ source_location location = source_location::current()
	);
	ଶୂନ୍ୟ codeblock(
		ଅଚଳ string &code,
		ଅଚଳ string &language = "none",
		ଅଚଳ source_location location = source_location::current()
	);
	ଶୂନ୍ୟ paragraph(
		ଅଚଳ string &text,
		ଅଚଳ source_location location = source_location::current()
	);

	ContentListing* open_usage(
		ଅଚଳ string &text,
		ଅଚଳ source_location location = source_location::current()
	);
	ContentListing* open_option(
		ଅଚଳ string &text,
		ଅଚଳ source_location location = source_location::current()
	);

	Json to_json(ଅଚଳ std::filesystem::path &source_ମୂଳ) ଅଚଳ;
};

class PrettyHelp
{
ସର୍ବସାଧାରଣ:
	string group = "unknown";

ବ୍ୟକ୍ତିଗତ:
	PrettyHelp *_prior;
	ContentListing ମୂଳ_listing;

ସର୍ବସାଧାରଣ:
	PrettyHelp();
	~PrettyHelp();

	ସ୍ଥିର PrettyHelp *get_current();

	bool has_content() ଅଚଳ { ଫେରନ୍ତୁ ମୂଳ_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ ମୂଳ_listing.begin(); }
	vector<ContentListing>::const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ ମୂଳ_listing.end(); }

	ContentListing* get_ମୂଳ() {
		ଫେରନ୍ତୁ &ମୂଳ_listing;
	}

	ଶୂନ୍ୟ set_group(ଅଚଳ string g) { group = g; }
	bool has_group() ଅଚଳ { ଫେରନ୍ତୁ group.compare("unknown") != 0; }

	ଶୂନ୍ୟ log_help() ଅଚଳ;
};

YOSYS_NAMESPACE_END

#endif

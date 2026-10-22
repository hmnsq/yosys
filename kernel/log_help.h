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

#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/json.h"
#অন্তর্ভাব <filesystem>
#অন্তর্ভাব <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (ধ্রুব বর্ণ* source_file,
	ধ্রুব std::filesystem::path &source_মূল, bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
সার্বজনিক:
	string type;
	string body;
	ধ্রুব বর্ণ* source_file;
	পূর্ণাঙ্ক source_সারি;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ধ্রুব বর্ণ* source_file = "unknown", পূর্ণাঙ্ক source_সারি = 0
	) : type(type), body(body), source_file(source_file), source_সারি(source_সারি) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.সারি()) { }

	শূন্য add_content(string type, string body, source_location location) {
		_content.ঠেলো_back({type, body, location});
	}

	bool has_source() ধ্রুব { প্রত্যাগম strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() ধ্রুব { প্রত্যাগম _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() ধ্রুব { প্রত্যাগম _content.cbegin(); }
	vector<ContentListing>::const_iterator end() ধ্রুব { প্রত্যাগম _content.cend(); }

	ContentListing* back() {
		প্রত্যাগম &_content.back();
	}

	শূন্য set_option(string চাবি, string val = "") {
		options[চাবি] = val;
	}

	শূন্য usage(
		ধ্রুব string &text,
		ধ্রুব source_location location = source_location::current()
	);
	শূন্য option(
		ধ্রুব string &text,
		ধ্রুব string &description = "",
		ধ্রুব source_location location = source_location::current()
	);
	শূন্য codeblock(
		ধ্রুব string &code,
		ধ্রুব string &language = "none",
		ধ্রুব source_location location = source_location::current()
	);
	শূন্য paragraph(
		ধ্রুব string &text,
		ধ্রুব source_location location = source_location::current()
	);

	ContentListing* open_usage(
		ধ্রুব string &text,
		ধ্রুব source_location location = source_location::current()
	);
	ContentListing* open_option(
		ধ্রুব string &text,
		ধ্রুব source_location location = source_location::current()
	);

	Json to_json(ধ্রুব std::filesystem::path &source_মূল) ধ্রুব;
};

class PrettyHelp
{
সার্বজনিক:
	string group = "unknown";

গুপ্ত:
	PrettyHelp *_prior;
	ContentListing মূল_listing;

সার্বজনিক:
	PrettyHelp();
	~PrettyHelp();

	স্থির PrettyHelp *get_current();

	bool has_content() ধ্রুব { প্রত্যাগম মূল_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() ধ্রুব { প্রত্যাগম মূল_listing.begin(); }
	vector<ContentListing>::const_iterator end() ধ্রুব { প্রত্যাগম মূল_listing.end(); }

	ContentListing* get_মূল() {
		প্রত্যাগম &মূল_listing;
	}

	শূন্য set_group(ধ্রুব string g) { group = g; }
	bool has_group() ধ্রুব { প্রত্যাগম group.compare("unknown") != 0; }

	শূন্য log_help() ধ্রুব;
};

YOSYS_NAMESPACE_END

#endif

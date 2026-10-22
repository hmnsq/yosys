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

#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/json.h"
#அந்தர்பாவ <filesystem>
#அந்தர்பாவ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::path fixup_source_path (மாறிலி எழுத்து* source_file,
	மாறிலி std::filesystem::path &source_வேர், bool* skip_source_group = nullptr);

class ContentListing {
	vector<ContentListing> _content;
பொது:
	string type;
	string body;
	மாறிலி எழுத்து* source_file;
	முழு source_வரி;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		மாறிலி எழுத்து* source_file = "unknown", முழு source_வரி = 0
	) : type(type), body(body), source_file(source_file), source_வரி(source_வரி) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.வரி()) { }

	வெறுமை add_content(string type, string body, source_location location) {
		_content.தள்ளு_back({type, body, location});
	}

	bool has_source() மாறிலி { திரும்பு strlen(source_file) && strcmp(source_file, "unknown") != 0; }
	bool has_content() மாறிலி { திரும்பு _content.size() != 0; }

	vector<ContentListing>::const_iterator begin() மாறிலி { திரும்பு _content.cbegin(); }
	vector<ContentListing>::const_iterator end() மாறிலி { திரும்பு _content.cend(); }

	ContentListing* back() {
		திரும்பு &_content.back();
	}

	வெறுமை set_option(string சாவி, string val = "") {
		options[சாவி] = val;
	}

	வெறுமை usage(
		மாறிலி string &text,
		மாறிலி source_location location = source_location::current()
	);
	வெறுமை option(
		மாறிலி string &text,
		மாறிலி string &description = "",
		மாறிலி source_location location = source_location::current()
	);
	வெறுமை codeblock(
		மாறிலி string &code,
		மாறிலி string &language = "none",
		மாறிலி source_location location = source_location::current()
	);
	வெறுமை paragraph(
		மாறிலி string &text,
		மாறிலி source_location location = source_location::current()
	);

	ContentListing* open_usage(
		மாறிலி string &text,
		மாறிலி source_location location = source_location::current()
	);
	ContentListing* open_option(
		மாறிலி string &text,
		மாறிலி source_location location = source_location::current()
	);

	Json to_json(மாறிலி std::filesystem::path &source_வேர்) மாறிலி;
};

class PrettyHelp
{
பொது:
	string group = "unknown";

தனிப்பட்ட:
	PrettyHelp *_prior;
	ContentListing வேர்_listing;

பொது:
	PrettyHelp();
	~PrettyHelp();

	நிலையான PrettyHelp *get_current();

	bool has_content() மாறிலி { திரும்பு வேர்_listing.has_content(); }

	vector<ContentListing>::const_iterator begin() மாறிலி { திரும்பு வேர்_listing.begin(); }
	vector<ContentListing>::const_iterator end() மாறிலி { திரும்பு வேர்_listing.end(); }

	ContentListing* get_வேர்() {
		திரும்பு &வேர்_listing;
	}

	வெறுமை set_group(மாறிலி string g) { group = g; }
	bool has_group() மாறிலி { திரும்பு group.compare("unknown") != 0; }

	வெறுமை log_help() மாறிலி;
};

YOSYS_NAMESPACE_END

#endif

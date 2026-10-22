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

#که_نه_تعریف_شوی LOG_HELP_H
#تعریف_کړه LOG_HELP_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/json.h"
#ضمن <filesystem>
#ضمن <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::لار_2 fixup_سرچینه_لار (ثابت حرف* سرچینه_file,
	ثابت std::filesystem::لار_2 &سرچینه_ریښه, bool* skip_سرچینه_ډله = nullptr);

class ContentListing {
	ویکټور<ContentListing> _content;
عام:
	string type;
	string body;
	ثابت حرف* سرچینه_file;
	صحيح سرچینه_کرښه;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت حرف* سرچینه_file = "unknown", صحيح سرچینه_کرښه = 0
	) : type(type), body(body), سرچینه_file(سرچینه_file), سرچینه_کرښه(سرچینه_کرښه) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, سرچینه_location location) :
		ContentListing(type, body, location.file_name(), location.کرښه()) { }

	فراغ جمع_کړه_content(string type, string body, سرچینه_location location) {
		_content.ټېل_شا({type, body, location});
	}

	bool has_سرچینه() ثابت { أرجع مزي_اوږدوالی(سرچینه_file) && strcmp(سرچینه_file, "unknown") != 0; }
	bool has_content() ثابت { أرجع _content.اندازه() != 0; }

	ویکټور<ContentListing>::const_iterator begin() ثابت { أرجع _content.cbegin(); }
	ویکټور<ContentListing>::const_iterator پای_ټکی() ثابت { أرجع _content.cend(); }

	ContentListing* شا() {
		أرجع &_content.شا();
	}

	فراغ سېټ_option(string کلی, string val = "") {
		options[کلی] = val;
	}

	فراغ usage(
		ثابت string &متن,
		ثابت سرچینه_location location = سرچینه_location::current()
	);
	فراغ option(
		ثابت string &متن,
		ثابت string &description = "",
		ثابت سرچینه_location location = سرچینه_location::current()
	);
	فراغ codeblock(
		ثابت string &کوډ,
		ثابت string &language = "none",
		ثابت سرچینه_location location = سرچینه_location::current()
	);
	فراغ paragraph(
		ثابت string &متن,
		ثابت سرچینه_location location = سرچینه_location::current()
	);

	ContentListing* open_usage(
		ثابت string &متن,
		ثابت سرچینه_location location = سرچینه_location::current()
	);
	ContentListing* open_option(
		ثابت string &متن,
		ثابت سرچینه_location location = سرچینه_location::current()
	);

	Json منزل_json(ثابت std::filesystem::لار_2 &سرچینه_ریښه) ثابت;
};

class PrettyHelp
{
عام:
	string ډله = "unknown";

خاص:
	PrettyHelp *مخکینی;
	ContentListing ریښه_listing;

عام:
	PrettyHelp();
	~PrettyHelp();

	ساكن PrettyHelp *get_current();

	bool has_content() ثابت { أرجع ریښه_listing.has_content(); }

	ویکټور<ContentListing>::const_iterator begin() ثابت { أرجع ریښه_listing.begin(); }
	ویکټور<ContentListing>::const_iterator پای_ټکی() ثابت { أرجع ریښه_listing.پای_ټکی(); }

	ContentListing* get_ریښه() {
		أرجع &ریښه_listing;
	}

	فراغ سېټ_ډله(ثابت string g) { ډله = g; }
	bool has_ډله() ثابت { أرجع ډله.compare("unknown") != 0; }

	فراغ log_help() ثابت;
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای

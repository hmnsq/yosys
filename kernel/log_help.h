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

#اگر_تعریف_نشده LOG_HELP_H
#تعریف_کن LOG_HELP_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/json.h"
#ضمن <filesystem>
#ضمن <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::مسیر_2 fixup_منبع_مسیر (ثابت حرف* منبع_file,
	ثابت std::filesystem::مسیر_2 &منبع_ریشه, bool* skip_منبع_گروه = nullptr);

class ContentListing {
	وکتور<ContentListing> _content;
عام:
	string type;
	string body;
	ثابت حرف* منبع_file;
	صحيح منبع_سطر;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت حرف* منبع_file = "unknown", صحيح منبع_سطر = 0
	) : type(type), body(body), منبع_file(منبع_file), منبع_سطر(منبع_سطر) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, منبع_location location) :
		ContentListing(type, body, location.file_name(), location.سطر()) { }

	فراغ جمع_کن_content(string type, string body, منبع_location location) {
		_content.افزودن_پشت({type, body, location});
	}

	bool has_منبع() ثابت { أرجع طول_رشته(منبع_file) && strcmp(منبع_file, "unknown") != 0; }
	bool has_content() ثابت { أرجع _content.اندازه() != 0; }

	وکتور<ContentListing>::const_iterator begin() ثابت { أرجع _content.cbegin(); }
	وکتور<ContentListing>::const_iterator انتها() ثابت { أرجع _content.cend(); }

	ContentListing* پشت() {
		أرجع &_content.پشت();
	}

	فراغ مجموعه_option(string کلید, string val = "") {
		options[کلید] = val;
	}

	فراغ usage(
		ثابت string &متن,
		ثابت منبع_location location = منبع_location::current()
	);
	فراغ option(
		ثابت string &متن,
		ثابت string &description = "",
		ثابت منبع_location location = منبع_location::current()
	);
	فراغ codeblock(
		ثابت string &کد,
		ثابت string &language = "none",
		ثابت منبع_location location = منبع_location::current()
	);
	فراغ paragraph(
		ثابت string &متن,
		ثابت منبع_location location = منبع_location::current()
	);

	ContentListing* open_usage(
		ثابت string &متن,
		ثابت منبع_location location = منبع_location::current()
	);
	ContentListing* open_option(
		ثابت string &متن,
		ثابت منبع_location location = منبع_location::current()
	);

	Json مقصد_json(ثابت std::filesystem::مسیر_2 &منبع_ریشه) ثابت;
};

class PrettyHelp
{
عام:
	string گروه = "unknown";

خاص:
	PrettyHelp *پیشین;
	ContentListing ریشه_listing;

عام:
	PrettyHelp();
	~PrettyHelp();

	ساكن PrettyHelp *get_current();

	bool has_content() ثابت { أرجع ریشه_listing.has_content(); }

	وکتور<ContentListing>::const_iterator begin() ثابت { أرجع ریشه_listing.begin(); }
	وکتور<ContentListing>::const_iterator انتها() ثابت { أرجع ریشه_listing.انتها(); }

	ContentListing* get_ریشه() {
		أرجع &ریشه_listing;
	}

	فراغ مجموعه_گروه(ثابت string g) { گروه = g; }
	bool has_گروه() ثابت { أرجع گروه.compare("unknown") != 0; }

	فراغ log_help() ثابت;
};

YOSYS_NAMESPACE_انتها

#پایان_اگر

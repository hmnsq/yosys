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

#جے_بنا_تعریف LOG_HELP_H
#تعریف_کرو LOG_HELP_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/json.h"
#ضمن <filesystem>
#ضمن <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::رستہ_2 fixup_سروت_رستہ (ثابت حرف* سروت_file,
	ثابت std::filesystem::رستہ_2 &سروت_جڑ, bool* skip_سروت_گروہ = nullptr);

class ContentListing {
	سمتیہ<ContentListing> _content;
عام:
	string type;
	string body;
	ثابت حرف* سروت_file;
	صحيح سروت_سطر;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت حرف* سروت_file = "unknown", صحيح سروت_سطر = 0
	) : type(type), body(body), سروت_file(سروت_file), سروت_سطر(سروت_سطر) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, سروت_location location) :
		ContentListing(type, body, location.file_name(), location.سطر()) { }

	فراغ جوڑو_content(string type, string body, سروت_location location) {
		_content.ڈالو_پٹھ({type, body, location});
	}

	bool has_سروت() ثابت { أرجع لڑی_لمبائی(سروت_file) && strcmp(سروت_file, "unknown") != 0; }
	bool has_content() ثابت { أرجع _content.ناپ() != 0; }

	سمتیہ<ContentListing>::const_iterator begin() ثابت { أرجع _content.cbegin(); }
	سمتیہ<ContentListing>::const_iterator سرا() ثابت { أرجع _content.cend(); }

	ContentListing* پٹھ() {
		أرجع &_content.پٹھ();
	}

	فراغ سیٹ_option(string کلید, string val = "") {
		options[کلید] = val;
	}

	فراغ usage(
		ثابت string &متن,
		ثابت سروت_location location = سروت_location::current()
	);
	فراغ option(
		ثابت string &متن,
		ثابت string &description = "",
		ثابت سروت_location location = سروت_location::current()
	);
	فراغ codeblock(
		ثابت string &کوڈ,
		ثابت string &language = "none",
		ثابت سروت_location location = سروت_location::current()
	);
	فراغ paragraph(
		ثابت string &متن,
		ثابت سروت_location location = سروت_location::current()
	);

	ContentListing* open_usage(
		ثابت string &متن,
		ثابت سروت_location location = سروت_location::current()
	);
	ContentListing* open_option(
		ثابت string &متن,
		ثابت سروت_location location = سروت_location::current()
	);

	Json منزل_json(ثابت std::filesystem::رستہ_2 &سروت_جڑ) ثابت;
};

class PrettyHelp
{
عام:
	string گروہ = "unknown";

خاص:
	PrettyHelp *پہلوں;
	ContentListing جڑ_listing;

عام:
	PrettyHelp();
	~PrettyHelp();

	ساكن PrettyHelp *get_current();

	bool has_content() ثابت { أرجع جڑ_listing.has_content(); }

	سمتیہ<ContentListing>::const_iterator begin() ثابت { أرجع جڑ_listing.begin(); }
	سمتیہ<ContentListing>::const_iterator سرا() ثابت { أرجع جڑ_listing.سرا(); }

	ContentListing* get_جڑ() {
		أرجع &جڑ_listing;
	}

	فراغ سیٹ_گروہ(ثابت string g) { گروہ = g; }
	bool has_گروہ() ثابت { أرجع گروہ.compare("unknown") != 0; }

	فراغ log_help() ثابت;
};

YOSYS_NAMESPACE_سرا

#جے_انت

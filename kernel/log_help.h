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

#اگر_غیر_تعریف_شدہ LOG_HELP_H
#تعریف_کرو LOG_HELP_H

#شامل "kernel/yosys_common.h"
#شامل "kernel/json.h"
#شامل <filesystem>
#شامل <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::راستہ_2 fixup_ماخذ_راستہ (ثابت حرف* ماخذ_file,
	ثابت std::filesystem::راستہ_2 &ماخذ_جڑ, bool* skip_ماخذ_گروہ = nullptr);

class ContentListing {
	سمتیہ<ContentListing> _content;
عوامی:
	string type;
	string body;
	ثابت حرف* ماخذ_file;
	صحیح ماخذ_سطر;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت حرف* ماخذ_file = "unknown", صحیح ماخذ_سطر = 0
	) : type(type), body(body), ماخذ_file(ماخذ_file), ماخذ_سطر(ماخذ_سطر) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, ماخذ_location location) :
		ContentListing(type, body, location.file_name(), location.سطر()) { }

	خالی جوڑو_content(string type, string body, ماخذ_location location) {
		_content.ڈالو_عقب({type, body, location});
	}

	bool has_ماخذ() ثابت { واپس لڑی_لمبائی(ماخذ_file) && strcmp(ماخذ_file, "unknown") != 0; }
	bool has_content() ثابت { واپس _content.سائز() != 0; }

	سمتیہ<ContentListing>::const_iterator begin() ثابت { واپس _content.cbegin(); }
	سمتیہ<ContentListing>::const_iterator انتہا() ثابت { واپس _content.cend(); }

	ContentListing* عقب() {
		واپس &_content.عقب();
	}

	خالی سیٹ_option(string کلید, string val = "") {
		options[کلید] = val;
	}

	خالی usage(
		ثابت string &متن,
		ثابت ماخذ_location location = ماخذ_location::current()
	);
	خالی option(
		ثابت string &متن,
		ثابت string &description = "",
		ثابت ماخذ_location location = ماخذ_location::current()
	);
	خالی codeblock(
		ثابت string &رمز,
		ثابت string &language = "none",
		ثابت ماخذ_location location = ماخذ_location::current()
	);
	خالی paragraph(
		ثابت string &متن,
		ثابت ماخذ_location location = ماخذ_location::current()
	);

	ContentListing* open_usage(
		ثابت string &متن,
		ثابت ماخذ_location location = ماخذ_location::current()
	);
	ContentListing* open_option(
		ثابت string &متن,
		ثابت ماخذ_location location = ماخذ_location::current()
	);

	Json مقصد_json(ثابت std::filesystem::راستہ_2 &ماخذ_جڑ) ثابت;
};

class PrettyHelp
{
عوامی:
	string گروہ = "unknown";

نجی:
	PrettyHelp *پیشگی;
	ContentListing جڑ_listing;

عوامی:
	PrettyHelp();
	~PrettyHelp();

	ساکن PrettyHelp *get_current();

	bool has_content() ثابت { واپس جڑ_listing.has_content(); }

	سمتیہ<ContentListing>::const_iterator begin() ثابت { واپس جڑ_listing.begin(); }
	سمتیہ<ContentListing>::const_iterator انتہا() ثابت { واپس جڑ_listing.انتہا(); }

	ContentListing* get_جڑ() {
		واپس &جڑ_listing;
	}

	خالی سیٹ_گروہ(ثابت string g) { گروہ = g; }
	bool has_گروہ() ثابت { واپس گروہ.compare("unknown") != 0; }

	خالی log_help() ثابت;
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم

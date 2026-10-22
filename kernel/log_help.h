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

#جيڪڏهن_وصف_نه_ٿيل LOG_HELP_H
#وصف_ڪريو LOG_HELP_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/json.h"
#ضمن <filesystem>
#ضمن <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::رستو_2 fixup_ذريعو_رستو (ثابت حرف* ذريعو_file,
	ثابت std::filesystem::رستو_2 &ذريعو_پاڙ, bool* skip_ذريعو_گروهه = nullptr);

class ContentListing {
	ويڪٽر<ContentListing> _content;
عام:
	string type;
	string body;
	ثابت حرف* ذريعو_file;
	صحيح ذريعو_سٽ;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ثابت حرف* ذريعو_file = "unknown", صحيح ذريعو_سٽ = 0
	) : type(type), body(body), ذريعو_file(ذريعو_file), ذريعو_سٽ(ذريعو_سٽ) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, ذريعو_location location) :
		ContentListing(type, body, location.file_name(), location.سٽ()) { }

	فراغ جوڙيو_content(string type, string body, ذريعو_location location) {
		_content.ڌڪو_پٺ({type, body, location});
	}

	bool has_ذريعو() ثابت { أرجع لڙي_ڊيگهه(ذريعو_file) && strcmp(ذريعو_file, "unknown") != 0; }
	bool has_content() ثابت { أرجع _content.سائيز() != 0; }

	ويڪٽر<ContentListing>::const_iterator begin() ثابت { أرجع _content.cbegin(); }
	ويڪٽر<ContentListing>::const_iterator پڇاڙي() ثابت { أرجع _content.cend(); }

	ContentListing* پٺ() {
		أرجع &_content.پٺ();
	}

	فراغ سيٽ_option(string ڪنجي, string val = "") {
		options[ڪنجي] = val;
	}

	فراغ usage(
		ثابت string &متن,
		ثابت ذريعو_location location = ذريعو_location::current()
	);
	فراغ option(
		ثابت string &متن,
		ثابت string &description = "",
		ثابت ذريعو_location location = ذريعو_location::current()
	);
	فراغ codeblock(
		ثابت string &ڪوڊ,
		ثابت string &language = "none",
		ثابت ذريعو_location location = ذريعو_location::current()
	);
	فراغ paragraph(
		ثابت string &متن,
		ثابت ذريعو_location location = ذريعو_location::current()
	);

	ContentListing* open_usage(
		ثابت string &متن,
		ثابت ذريعو_location location = ذريعو_location::current()
	);
	ContentListing* open_option(
		ثابت string &متن,
		ثابت ذريعو_location location = ذريعو_location::current()
	);

	Json منزل_json(ثابت std::filesystem::رستو_2 &ذريعو_پاڙ) ثابت;
};

class PrettyHelp
{
عام:
	string گروهه = "unknown";

خاص:
	PrettyHelp *اڳواٽ;
	ContentListing پاڙ_listing;

عام:
	PrettyHelp();
	~PrettyHelp();

	ساكن PrettyHelp *get_current();

	bool has_content() ثابت { أرجع پاڙ_listing.has_content(); }

	ويڪٽر<ContentListing>::const_iterator begin() ثابت { أرجع پاڙ_listing.begin(); }
	ويڪٽر<ContentListing>::const_iterator پڇاڙي() ثابت { أرجع پاڙ_listing.پڇاڙي(); }

	ContentListing* get_پاڙ() {
		أرجع &پاڙ_listing;
	}

	فراغ سيٽ_گروهه(ثابت string g) { گروهه = g; }
	bool has_گروهه() ثابت { أرجع گروهه.compare("unknown") != 0; }

	فراغ log_help() ثابت;
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو

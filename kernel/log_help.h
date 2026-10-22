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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOG_HELP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_HELP_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/json.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <filesystem>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::ਰਸਤਾ_2 fixup_ਸਰੋਤ_ਰਸਤਾ (ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_file,
	ਅਚਲ std::filesystem::ਰਸਤਾ_2 &ਸਰੋਤ_ਜੜ੍ਹ, bool* skip_ਸਰੋਤ_ਸਮੂਹ = nullptr);

class ContentListing {
	ਸਦਿਸ਼<ContentListing> _content;
ਜਨਤਕ:
	string type;
	string body;
	ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_file;
	ਪੂਰਨ ਸਰੋਤ_ਸਤਰ;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_file = "unknown", ਪੂਰਨ ਸਰੋਤ_ਸਤਰ = 0
	) : type(type), body(body), ਸਰੋਤ_file(ਸਰੋਤ_file), ਸਰੋਤ_ਸਤਰ(ਸਰੋਤ_ਸਤਰ) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, ਸਰੋਤ_location location) :
		ContentListing(type, body, location.file_name(), location.ਸਤਰ()) { }

	ਖਾਲੀ ਜੋੜੋ_content(string type, string body, ਸਰੋਤ_location location) {
		_content.ਧੱਕੋ_ਪਿੱਠ({type, body, location});
	}

	bool has_ਸਰੋਤ() ਅਚਲ { ਵਾਪਸ ਲੜੀ_ਲੰਬਾਈ(ਸਰੋਤ_file) && strcmp(ਸਰੋਤ_file, "unknown") != 0; }
	bool has_content() ਅਚਲ { ਵਾਪਸ _content.ਮਾਪ() != 0; }

	ਸਦਿਸ਼<ContentListing>::const_iterator begin() ਅਚਲ { ਵਾਪਸ _content.cbegin(); }
	ਸਦਿਸ਼<ContentListing>::const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ _content.cend(); }

	ContentListing* ਪਿੱਠ() {
		ਵਾਪਸ &_content.ਪਿੱਠ();
	}

	ਖਾਲੀ ਸੈੱਟ_option(string ਕੁੰਜੀ, string val = "") {
		options[ਕੁੰਜੀ] = val;
	}

	ਖਾਲੀ usage(
		ਅਚਲ string &ਪਾਠ,
		ਅਚਲ ਸਰੋਤ_location location = ਸਰੋਤ_location::current()
	);
	ਖਾਲੀ option(
		ਅਚਲ string &ਪਾਠ,
		ਅਚਲ string &description = "",
		ਅਚਲ ਸਰੋਤ_location location = ਸਰੋਤ_location::current()
	);
	ਖਾਲੀ codeblock(
		ਅਚਲ string &ਕੋਡ,
		ਅਚਲ string &language = "none",
		ਅਚਲ ਸਰੋਤ_location location = ਸਰੋਤ_location::current()
	);
	ਖਾਲੀ paragraph(
		ਅਚਲ string &ਪਾਠ,
		ਅਚਲ ਸਰੋਤ_location location = ਸਰੋਤ_location::current()
	);

	ContentListing* open_usage(
		ਅਚਲ string &ਪਾਠ,
		ਅਚਲ ਸਰੋਤ_location location = ਸਰੋਤ_location::current()
	);
	ContentListing* open_option(
		ਅਚਲ string &ਪਾਠ,
		ਅਚਲ ਸਰੋਤ_location location = ਸਰੋਤ_location::current()
	);

	Json ਮੰਜ਼ਿਲ_json(ਅਚਲ std::filesystem::ਰਸਤਾ_2 &ਸਰੋਤ_ਜੜ੍ਹ) ਅਚਲ;
};

class PrettyHelp
{
ਜਨਤਕ:
	string ਸਮੂਹ = "unknown";

ਨਿਜੀ:
	PrettyHelp *ਪੂਰਵ;
	ContentListing ਜੜ੍ਹ_listing;

ਜਨਤਕ:
	PrettyHelp();
	~PrettyHelp();

	ਸਥਿਰ PrettyHelp *get_current();

	bool has_content() ਅਚਲ { ਵਾਪਸ ਜੜ੍ਹ_listing.has_content(); }

	ਸਦਿਸ਼<ContentListing>::const_iterator begin() ਅਚਲ { ਵਾਪਸ ਜੜ੍ਹ_listing.begin(); }
	ਸਦਿਸ਼<ContentListing>::const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ ਜੜ੍ਹ_listing.ਸਿਰਾ(); }

	ContentListing* get_ਜੜ੍ਹ() {
		ਵਾਪਸ &ਜੜ੍ਹ_listing;
	}

	ਖਾਲੀ ਸੈੱਟ_ਸਮੂਹ(ਅਚਲ string g) { ਸਮੂਹ = g; }
	bool has_ਸਮੂਹ() ਅਚਲ { ਵਾਪਸ ਸਮੂਹ.compare("unknown") != 0; }

	ਖਾਲੀ log_help() ਅਚਲ;
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

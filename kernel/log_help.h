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

#यदि_अपरिभाषितम् LOG_HELP_H
#परिभाषय LOG_HELP_H

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/json.h"
#अन्तर्भाव <filesystem>
#अन्तर्भाव <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::मार्गः_2 fixup_source_मार्गः (ध्रुव वर्ण* source_file,
	ध्रुव std::filesystem::मार्गः_2 &source_मूलम्, bool* skip_source_गणः = nullptr);

class ContentListing {
	सदिशम्<ContentListing> _content;
सार्वजनिक:
	string type;
	string body;
	ध्रुव वर्ण* source_file;
	पूर्णाङ्क source_पङ्क्तिः;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ध्रुव वर्ण* source_file = "unknown", पूर्णाङ्क source_पङ्क्तिः = 0
	) : type(type), body(body), source_file(source_file), source_पङ्क्तिः(source_पङ्क्तिः) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, source_location location) :
		ContentListing(type, body, location.file_name(), location.पङ्क्तिः()) { }

	शून्य सङ्कलय_content(string type, string body, source_location location) {
		_content.प्रक्षिप_पृष्ठभागः({type, body, location});
	}

	bool has_source() ध्रुव { प्रत्यागम वाक्यदैर्घ्यम्(source_file) && वाक्यं_तुलय(source_file, "unknown") != 0; }
	bool has_content() ध्रुव { प्रत्यागम _content.आकार() != 0; }

	सदिशम्<ContentListing>::const_iterator begin() ध्रुव { प्रत्यागम _content.cbegin(); }
	सदिशम्<ContentListing>::const_iterator अन्तः() ध्रुव { प्रत्यागम _content.cend(); }

	ContentListing* पृष्ठभागः() {
		प्रत्यागम &_content.पृष्ठभागः();
	}

	शून्य समुच्चयः_option(string कुञ्चिका, string val = "") {
		options[कुञ्चिका] = val;
	}

	शून्य usage(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);
	शून्य option(
		ध्रुव string &text,
		ध्रुव string &description = "",
		ध्रुव source_location location = source_location::current()
	);
	शून्य codeblock(
		ध्रुव string &संहिता,
		ध्रुव string &language = "none",
		ध्रुव source_location location = source_location::current()
	);
	शून्य paragraph(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);

	ContentListing* open_usage(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);
	ContentListing* open_option(
		ध्रुव string &text,
		ध्रुव source_location location = source_location::current()
	);

	Json गन्तव्यम्_json(ध्रुव std::filesystem::मार्गः_2 &source_मूलम्) ध्रुव;
};

class PrettyHelp
{
सार्वजनिक:
	string गणः = "unknown";

गुप्त:
	PrettyHelp *प्राक्;
	ContentListing मूलम्_listing;

सार्वजनिक:
	PrettyHelp();
	~PrettyHelp();

	स्थिर PrettyHelp *get_current();

	bool has_content() ध्रुव { प्रत्यागम मूलम्_listing.has_content(); }

	सदिशम्<ContentListing>::const_iterator begin() ध्रुव { प्रत्यागम मूलम्_listing.begin(); }
	सदिशम्<ContentListing>::const_iterator अन्तः() ध्रुव { प्रत्यागम मूलम्_listing.अन्तः(); }

	ContentListing* get_मूलम्() {
		प्रत्यागम &मूलम्_listing;
	}

	शून्य समुच्चयः_गणः(ध्रुव string g) { गणः = g; }
	bool has_गणः() ध्रुव { प्रत्यागम गणः.compare("unknown") != 0; }

	शून्य log_help() ध्रुव;
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः

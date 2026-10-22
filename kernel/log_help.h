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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOG_HELP_H
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_HELP_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/json.h"
#ಅನ್ತರ್ಭಾವ <filesystem>
#ಅನ್ತರ್ಭಾವ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::ಮಾರ್ಗ_2 fixup_ಮೂಲ_ಮಾರ್ಗ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_file,
	ಸ್ಥಿರಾಂಕ std::filesystem::ಮಾರ್ಗ_2 &ಮೂಲ_ಬೇರು, bool* skip_ಮೂಲ_ಗುಂಪು = nullptr);

class ContentListing {
	ಸದಿಶ<ContentListing> _content;
ಸಾರ್ವಜನಿಕ:
	string type;
	string body;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_file;
	ಪೂರ್ಣಾಂಕ ಮೂಲ_ಸಾಲು;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_file = "unknown", ಪೂರ್ಣಾಂಕ ಮೂಲ_ಸಾಲು = 0
	) : type(type), body(body), ಮೂಲ_file(ಮೂಲ_file), ಮೂಲ_ಸಾಲು(ಮೂಲ_ಸಾಲು) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, ಮೂಲ_location location) :
		ContentListing(type, body, location.file_name(), location.ಸಾಲು()) { }

	ಶೂನ್ಯ ಕೂಡಿಸು_content(string type, string body, ಮೂಲ_location location) {
		_content.ತಳ್ಳು_ಹಿಂಭಾಗ({type, body, location});
	}

	bool has_ಮೂಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೂಲ_file) && strcmp(ಮೂಲ_file, "unknown") != 0; }
	bool has_content() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _content.ಗಾತ್ರ_2() != 0; }

	ಸದಿಶ<ContentListing>::const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _content.cbegin(); }
	ಸದಿಶ<ContentListing>::const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _content.cend(); }

	ContentListing* ಹಿಂಭಾಗ() {
		ಹಿಂತಿರುಗಿ &_content.ಹಿಂಭಾಗ();
	}

	ಶೂನ್ಯ ಗಣ_option(string ಕೀಲಿ, string val = "") {
		options[ಕೀಲಿ] = val;
	}

	ಶೂನ್ಯ usage(
		ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
		ಸ್ಥಿರಾಂಕ ಮೂಲ_location location = ಮೂಲ_location::current()
	);
	ಶೂನ್ಯ option(
		ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
		ಸ್ಥಿರಾಂಕ string &description = "",
		ಸ್ಥಿರಾಂಕ ಮೂಲ_location location = ಮೂಲ_location::current()
	);
	ಶೂನ್ಯ codeblock(
		ಸ್ಥಿರಾಂಕ string &ಸಂಕೇತ_2,
		ಸ್ಥಿರಾಂಕ string &language = "none",
		ಸ್ಥಿರಾಂಕ ಮೂಲ_location location = ಮೂಲ_location::current()
	);
	ಶೂನ್ಯ paragraph(
		ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
		ಸ್ಥಿರಾಂಕ ಮೂಲ_location location = ಮೂಲ_location::current()
	);

	ContentListing* open_usage(
		ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
		ಸ್ಥಿರಾಂಕ ಮೂಲ_location location = ಮೂಲ_location::current()
	);
	ContentListing* open_option(
		ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
		ಸ್ಥಿರಾಂಕ ಮೂಲ_location location = ಮೂಲ_location::current()
	);

	Json ಗಮ್ಯ_json(ಸ್ಥಿರಾಂಕ std::filesystem::ಮಾರ್ಗ_2 &ಮೂಲ_ಬೇರು) ಸ್ಥಿರಾಂಕ;
};

class PrettyHelp
{
ಸಾರ್ವಜನಿಕ:
	string ಗುಂಪು = "unknown";

ಖಾಸಗಿ:
	PrettyHelp *ಪೂರ್ವ;
	ContentListing ಬೇರು_listing;

ಸಾರ್ವಜನಿಕ:
	PrettyHelp();
	~PrettyHelp();

	ಸ್ಥಿರ PrettyHelp *get_current();

	bool has_content() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬೇರು_listing.has_content(); }

	ಸದಿಶ<ContentListing>::const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬೇರು_listing.begin(); }
	ಸದಿಶ<ContentListing>::const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬೇರು_listing.ಕೊನೆ(); }

	ContentListing* get_ಬೇರು() {
		ಹಿಂತಿರುಗಿ &ಬೇರು_listing;
	}

	ಶೂನ್ಯ ಗಣ_ಗುಂಪು(ಸ್ಥಿರಾಂಕ string g) { ಗುಂಪು = g; }
	bool has_ಗುಂಪು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗುಂಪು.compare("unknown") != 0; }

	ಶೂನ್ಯ log_help() ಸ್ಥಿರಾಂಕ;
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

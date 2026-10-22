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

#ಅನ್ತರ್ಭಾವ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::ಮಾರ್ಗ_2 Yosys::fixup_ಮೂಲ_ಮಾರ್ಗ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_file,
	ಸ್ಥಿರಾಂಕ std::filesystem::ಮಾರ್ಗ_2 &ಮೂಲ_ಬೇರು, ತಾರ್ಕಿಕ* skip_ಮೂಲ_ಗುಂಪು)
{
	std::filesystem::ಮಾರ್ಗ_2 ಮೂಲ_ಮಾರ್ಗ(ಮೂಲ_file);
	ಆದರೆ (ಮೂಲ_ಮಾರ್ಗ.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		ಸ್ವಯಂಚಾಲಿತ proximate_ಮಾರ್ಗ = std::filesystem::proximate(ಮೂಲ_ಮಾರ್ಗ, ಮೂಲ_ಬೇರು);
		ಆದರೆ (proximate_ಮಾರ್ಗ == proximate_ಮಾರ್ಗ.lexically_ಸಾಮಾನ್ಯ()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ಹಿಂತಿರುಗಿ proximate_ಮಾರ್ಗ;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (skip_ಮೂಲ_ಗುಂಪು != ಶೂನ್ಯಸೂಚಕ) {
			// don't try to group external paths
			*skip_ಮೂಲ_ಗುಂಪು = ನಿಜ;
		}
	}
	ಹಿಂತಿರುಗಿ ಮೂಲ_ಮಾರ್ಗ;
}

Json ContentListing::ಗಮ್ಯ_json(ಸ್ಥಿರಾಂಕ std::filesystem::ಮಾರ್ಗ_2 &ಮೂಲ_ಬೇರು) ಸ್ಥಿರಾಂಕ {
	Json::object object;

	object["type"] = type;
	ಆದರೆ (body.ಉದ್ದಳತೆ())
		object["body"] = body;

	ಆದರೆ (has_ಮೂಲ())
		object["source_file"] = fixup_ಮೂಲ_ಮಾರ್ಗ(ಮೂಲ_file, ಮೂಲ_ಬೇರು).string();
	ಆದರೆ (ಮೂಲ_ಸಾಲು != 0)
		object["source_line"] = ಮೂಲ_ಸಾಲು;

	object["options"] = Json(options);

	Json::array content_array;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಮಗು : _content)
		content_array.ತಳ್ಳು_ಹಿಂಭಾಗ(ಮಗು.ಗಮ್ಯ_json(ಮೂಲ_ಬೇರು));
	object["content"] = content_array;

	ಹಿಂತಿರುಗಿ object;
}

ಶೂನ್ಯ ContentListing::usage(ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
	ಸ್ಥಿರಾಂಕ ಮೂಲ_location location)
{
	log_assert(type.compare("root") == 0);
	ಕೂಡಿಸು_content("usage", ಪಠ್ಯ, location);
}

ಶೂನ್ಯ ContentListing::option(ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ string &description,
	ಸ್ಥಿರಾಂಕ ಮೂಲ_location location)
{
	ಸ್ವಯಂಚಾಲಿತ option = open_option(ಪಠ್ಯ, ಮೂಲ_location());
	ಆದರೆ (description.ಉದ್ದಳತೆ())
		option->ಕೂಡಿಸು_content("text", description, location);
}

ಶೂನ್ಯ ContentListing::codeblock(ಸ್ಥಿರಾಂಕ string &ಸಂಕೇತ_2, ಸ್ಥಿರಾಂಕ string &language,
	ಸ್ಥಿರಾಂಕ ಮೂಲ_location location)
{
	ಕೂಡಿಸು_content("code", ಸಂಕೇತ_2, location);
	ಹಿಂಭಾಗ()->ಗಣ_option("language", language);
}

ಶೂನ್ಯ ContentListing::paragraph(ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
	ಸ್ಥಿರಾಂಕ ಮೂಲ_location location)
{
	ಕೂಡಿಸು_content("text", ಪಠ್ಯ, location);
}

ContentListing* ContentListing::open_usage(ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
	ಸ್ಥಿರಾಂಕ ಮೂಲ_location location)
{
	usage(ಪಠ್ಯ, location);
	ಹಿಂತಿರುಗಿ ಹಿಂಭಾಗ();
}

ContentListing* ContentListing::open_option(ಸ್ಥಿರಾಂಕ string &ಪಠ್ಯ,
	ಸ್ಥಿರಾಂಕ ಮೂಲ_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	ಕೂಡಿಸು_content("option", ಪಠ್ಯ, location);
	ಹಿಂತಿರುಗಿ ಹಿಂಭಾಗ();
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಸಾಲು_LEN 80
ಶೂನ್ಯ log_body_str(ಸ್ಥಿರಾಂಕ std::string &pass_str, std::string indent_str, ತಾರ್ಕಿಕ leading_newline=ಸುಳ್ಳು, ತಾರ್ಕಿಕ is_formatted=ಸುಳ್ಳು) {
	ಆದರೆ (pass_str.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ;
	std::istringstream iss(pass_str);
	ಆದರೆ (leading_newline)
		ಲಘುಗಣಕ("\n");
	ಗಾಗಿ (std::string ಸಾಲು; std::getline(iss, ಸಾಲು);) {
		std::string ಸಂದೇಶ_2 = indent_str;
		ಆದರೆ (is_formatted) {
			ಸಂದೇಶ_2 += ಸಾಲು;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸ್ವಯಂಚಾಲಿತ curr_len = indent_str.ಉದ್ದಳತೆ();
			std::istringstream lss(ಸಾಲು);
			ಗಾಗಿ (std::string ಶಬ್ದ; std::getline(lss, ಶಬ್ದ, ' ');) {
				ಆಗ (ಶಬ್ದ[0] == '`' && ಶಬ್ದ.ಹಿಂಭಾಗ() == '`')
					ಶಬ್ದ = ಶಬ್ದ.substr(1, ಶಬ್ದ.ಉದ್ದಳತೆ()-2);
				ಆದರೆ (curr_len + ಶಬ್ದ.ಉದ್ದಳತೆ() >= MAX_ಸಾಲು_LEN-1) {
					curr_len = 0;
					ಲಘುಗಣಕ("%s\n", ಸಂದೇಶ_2);
					ಸಂದೇಶ_2 = indent_str;
				}
				ಆದರೆ (ಶಬ್ದ.ಉದ್ದಳತೆ()) {
					ಸಂದೇಶ_2 += ಶಬ್ದ + " ";
					curr_len += ಶಬ್ದ.ಉದ್ದಳತೆ() + 1;
				}
			}
		}
		ಲಘುಗಣಕ("%s\n", ಸಂದೇಶ_2);
	}
}
ಶೂನ್ಯ log_body(ಸ್ಥಿರಾಂಕ ContentListing &content, ಪೂರ್ಣಾಂಕ indent=0, ತಾರ್ಕಿಕ leading_newline=ಸುಳ್ಳು) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = ಶೂನ್ಯಸೂಚಕ;

PrettyHelp::PrettyHelp()
{
	ಪೂರ್ವ = current_help;
	ಬೇರು_listing = ContentListing("root", "");

	current_help = ಇದು;
}

PrettyHelp::~PrettyHelp()
{
	current_help = ಪೂರ್ವ;
}

PrettyHelp *PrettyHelp::get_current()
{
	ಆದರೆ (current_help == ಶೂನ್ಯಸೂಚಕ)
		ಹೊಸ PrettyHelp();
	ಹಿಂತಿರುಗಿ current_help;
}

ಶೂನ್ಯ PrettyHelp::log_help() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &content : ಬೇರು_listing) {
		ಆದರೆ (content.type.compare("usage") == 0) {
			log_body(content, 1, ನಿಜ);
			ಲಘುಗಣಕ("\n");
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (content.type.compare("option") == 0) {
			log_body(content, 1);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಪಠ್ಯ : content) {
				log_body(ಪಠ್ಯ, 2);
				ಲಘುಗಣಕ("\n");
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			log_body(content, 0);
			ಲಘುಗಣಕ("\n");
		}
	}
}

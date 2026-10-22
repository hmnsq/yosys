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

std::filesystem::path Yosys::fixup_source_path (ಧ್ರುವ ವರ್ಣ* source_file,
	ಧ್ರುವ std::filesystem::path &source_ಬೇರು, ತಾರ್ಕಿಕ* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	ಯದಿ (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_ಬೇರು);
		ಯದಿ (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ಪ್ರತ್ಯಾಗಮ proximate_path;
		} ಅನ್ಯಥಾ ಯದಿ (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = ಸತ್ಯ;
		}
	}
	ಪ್ರತ್ಯಾಗಮ source_path;
}

Json ContentListing::to_json(ಧ್ರುವ std::filesystem::path &source_ಬೇರು) ಧ್ರುವ {
	Json::object object;

	object["type"] = type;
	ಯದಿ (body.length())
		object["body"] = body;

	ಯದಿ (has_source())
		object["source_file"] = fixup_source_path(source_file, source_ಬೇರು).string();
	ಯದಿ (source_ಸಾಲು != 0)
		object["source_line"] = source_ಸಾಲು;

	object["options"] = Json(options);

	Json::array content_array;
	ಪ್ರತಿ (auto child : _content)
		content_array.ತಳ್ಳು_back(child.to_json(source_ಬೇರು));
	object["content"] = content_array;

	ಪ್ರತ್ಯಾಗಮ object;
}

ಶೂನ್ಯ ContentListing::usage(ಧ್ರುವ string &text,
	ಧ್ರುವ source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

ಶೂನ್ಯ ContentListing::option(ಧ್ರುವ string &text, ಧ್ರುವ string &description,
	ಧ್ರುವ source_location location)
{
	auto option = open_option(text, source_location());
	ಯದಿ (description.length())
		option->add_content("text", description, location);
}

ಶೂನ್ಯ ContentListing::codeblock(ಧ್ರುವ string &code, ಧ್ರುವ string &language,
	ಧ್ರುವ source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

ಶೂನ್ಯ ContentListing::paragraph(ಧ್ರುವ string &text,
	ಧ್ರುವ source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ಧ್ರುವ string &text,
	ಧ್ರುವ source_location location)
{
	usage(text, location);
	ಪ್ರತ್ಯಾಗಮ back();
}

ContentListing* ContentListing::open_option(ಧ್ರುವ string &text,
	ಧ್ರುವ source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	ಪ್ರತ್ಯಾಗಮ back();
}

#define MAX_ಸಾಲು_LEN 80
ಶೂನ್ಯ log_body_str(ಧ್ರುವ std::string &pass_str, std::string indent_str, ತಾರ್ಕಿಕ leading_newline=ಅಸತ್ಯ, ತಾರ್ಕಿಕ is_formatted=ಅಸತ್ಯ) {
	ಯದಿ (pass_str.empty())
		ಪ್ರತ್ಯಾಗಮ;
	std::istringstream iss(pass_str);
	ಯದಿ (leading_newline)
		log("\n");
	ಪ್ರತಿ (std::string ಸಾಲು; std::getline(iss, ಸಾಲು);) {
		std::string message = indent_str;
		ಯದಿ (is_formatted) {
			message += ಸಾಲು;
		} ಅನ್ಯಥಾ {
			auto curr_len = indent_str.length();
			std::istringstream lss(ಸಾಲು);
			ಪ್ರತಿ (std::string word; std::getline(lss, word, ' ');) {
				ಯಾವತ್ (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				ಯದಿ (curr_len + word.length() >= MAX_ಸಾಲು_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				ಯದಿ (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
ಶೂನ್ಯ log_body(ಧ್ರುವ ContentListing &content, ಪೂರ್ಣಾಙ್ಕ indent=0, ತಾರ್ಕಿಕ leading_newline=ಅಸತ್ಯ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	ಬೇರು_listing = ContentListing("root", "");

	current_help = ಏತತ್;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	ಯದಿ (current_help == nullptr)
		ನೂತನ PrettyHelp();
	ಪ್ರತ್ಯಾಗಮ current_help;
}

ಶೂನ್ಯ PrettyHelp::log_help() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &content : ಬೇರು_listing) {
		ಯದಿ (content.type.compare("usage") == 0) {
			log_body(content, 1, ಸತ್ಯ);
			log("\n");
		} ಅನ್ಯಥಾ ಯದಿ (content.type.compare("option") == 0) {
			log_body(content, 1);
			ಪ್ರತಿ (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} ಅನ್ಯಥಾ {
			log_body(content, 0);
			log("\n");
		}
	}
}

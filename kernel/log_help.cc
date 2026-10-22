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

#अन्तर्भाव "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (ध्रुव वर्ण* source_file,
	ध्रुव std::filesystem::path &source_मूल, तार्किक* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	यदि (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		स्वतः proximate_path = std::filesystem::proximate(source_path, source_मूल);
		यदि (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			प्रत्यागम proximate_path;
		} अन्यथा यदि (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = सत्य;
		}
	}
	प्रत्यागम source_path;
}

Json ContentListing::to_json(ध्रुव std::filesystem::path &source_मूल) ध्रुव {
	Json::object object;

	object["type"] = type;
	यदि (body.length())
		object["body"] = body;

	यदि (has_source())
		object["source_file"] = fixup_source_path(source_file, source_मूल).string();
	यदि (source_पंक्ति != 0)
		object["source_line"] = source_पंक्ति;

	object["options"] = Json(options);

	Json::array content_array;
	प्रति (स्वतः child : _content)
		content_array.धकेलो_back(child.to_json(source_मूल));
	object["content"] = content_array;

	प्रत्यागम object;
}

शून्य ContentListing::usage(ध्रुव string &text,
	ध्रुव source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

शून्य ContentListing::option(ध्रुव string &text, ध्रुव string &description,
	ध्रुव source_location location)
{
	स्वतः option = open_option(text, source_location());
	यदि (description.length())
		option->add_content("text", description, location);
}

शून्य ContentListing::codeblock(ध्रुव string &code, ध्रुव string &language,
	ध्रुव source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

शून्य ContentListing::paragraph(ध्रुव string &text,
	ध्रुव source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ध्रुव string &text,
	ध्रुव source_location location)
{
	usage(text, location);
	प्रत्यागम back();
}

ContentListing* ContentListing::open_option(ध्रुव string &text,
	ध्रुव source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	प्रत्यागम back();
}

#define MAX_पंक्ति_LEN 80
शून्य log_body_str(ध्रुव std::string &pass_str, std::string indent_str, तार्किक leading_newline=असत्य, तार्किक is_formatted=असत्य) {
	यदि (pass_str.empty())
		प्रत्यागम;
	std::istringstream iss(pass_str);
	यदि (leading_newline)
		log("\n");
	प्रति (std::string पंक्ति; std::getline(iss, पंक्ति);) {
		std::string message = indent_str;
		यदि (is_formatted) {
			message += पंक्ति;
		} अन्यथा {
			स्वतः curr_len = indent_str.length();
			std::istringstream lss(पंक्ति);
			प्रति (std::string word; std::getline(lss, word, ' ');) {
				यावत् (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				यदि (curr_len + word.length() >= MAX_पंक्ति_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				यदि (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
शून्य log_body(ध्रुव ContentListing &content, पूर्णाङ्क indent=0, तार्किक leading_newline=असत्य) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	मूल_listing = ContentListing("root", "");

	current_help = एतत्;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	यदि (current_help == nullptr)
		नूतन PrettyHelp();
	प्रत्यागम current_help;
}

शून्य PrettyHelp::log_help() ध्रुव
{
	प्रति (स्वतः &content : मूल_listing) {
		यदि (content.type.compare("usage") == 0) {
			log_body(content, 1, सत्य);
			log("\n");
		} अन्यथा यदि (content.type.compare("option") == 0) {
			log_body(content, 1);
			प्रति (स्वतः text : content) {
				log_body(text, 2);
				log("\n");
			}
		} अन्यथा {
			log_body(content, 0);
			log("\n");
		}
	}
}

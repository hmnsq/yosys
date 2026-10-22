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

std::filesystem::मार्गः_2 Yosys::fixup_source_मार्गः (ध्रुव वर्ण* source_file,
	ध्रुव std::filesystem::मार्गः_2 &source_मूलम्, तार्किक* skip_source_गणः)
{
	std::filesystem::मार्गः_2 source_मार्गः(source_file);
	यदि (source_मार्गः.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		स्वचालित proximate_मार्गः = std::filesystem::proximate(source_मार्गः, source_मूलम्);
		यदि (proximate_मार्गः == proximate_मार्गः.lexically_प्रसामान्यम्()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			प्रत्यागम proximate_मार्गः;
		} अन्यथा यदि (skip_source_गणः != शून्यसूचकः) {
			// don't try to group external paths
			*skip_source_गणः = सत्य;
		}
	}
	प्रत्यागम source_मार्गः;
}

Json ContentListing::गन्तव्यम्_json(ध्रुव std::filesystem::मार्गः_2 &source_मूलम्) ध्रुव {
	Json::object object;

	object["type"] = type;
	यदि (body.दीर्घता_2())
		object["body"] = body;

	यदि (has_source())
		object["source_file"] = fixup_source_मार्गः(source_file, source_मूलम्).string();
	यदि (source_पङ्क्तिः != 0)
		object["source_line"] = source_पङ्क्तिः;

	object["options"] = Json(options);

	Json::array content_array;
	प्रति (स्वचालित child : _content)
		content_array.प्रक्षिप_पृष्ठभागः(child.गन्तव्यम्_json(source_मूलम्));
	object["content"] = content_array;

	प्रत्यागम object;
}

शून्य ContentListing::usage(ध्रुव string &text,
	ध्रुव source_location location)
{
	log_assert(type.compare("root") == 0);
	सङ्कलय_content("usage", text, location);
}

शून्य ContentListing::option(ध्रुव string &text, ध्रुव string &description,
	ध्रुव source_location location)
{
	स्वचालित option = open_option(text, source_location());
	यदि (description.दीर्घता_2())
		option->सङ्कलय_content("text", description, location);
}

शून्य ContentListing::codeblock(ध्रुव string &संहिता, ध्रुव string &language,
	ध्रुव source_location location)
{
	सङ्कलय_content("code", संहिता, location);
	पृष्ठभागः()->समुच्चयः_option("language", language);
}

शून्य ContentListing::paragraph(ध्रुव string &text,
	ध्रुव source_location location)
{
	सङ्कलय_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ध्रुव string &text,
	ध्रुव source_location location)
{
	usage(text, location);
	प्रत्यागम पृष्ठभागः();
}

ContentListing* ContentListing::open_option(ध्रुव string &text,
	ध्रुव source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	सङ्कलय_content("option", text, location);
	प्रत्यागम पृष्ठभागः();
}

#परिभाषय MAX_पङ्क्तिः_LEN 80
शून्य log_body_str(ध्रुव std::string &pass_str, std::string indent_str, तार्किक leading_newline=असत्य, तार्किक is_formatted=असत्य) {
	यदि (pass_str.रिक्तानि())
		प्रत्यागम;
	std::istringstream iss(pass_str);
	यदि (leading_newline)
		लघुगणकः("\n");
	प्रति (std::string पङ्क्तिः; std::getline(iss, पङ्क्तिः);) {
		std::string सन्देशः_2 = indent_str;
		यदि (is_formatted) {
			सन्देशः_2 += पङ्क्तिः;
		} अन्यथा {
			स्वचालित curr_len = indent_str.दीर्घता_2();
			std::istringstream lss(पङ्क्तिः);
			प्रति (std::string word; std::getline(lss, word, ' ');) {
				यावत् (word[0] == '`' && word.पृष्ठभागः() == '`')
					word = word.substr(1, word.दीर्घता_2()-2);
				यदि (curr_len + word.दीर्घता_2() >= MAX_पङ्क्तिः_LEN-1) {
					curr_len = 0;
					लघुगणकः("%s\n", सन्देशः_2);
					सन्देशः_2 = indent_str;
				}
				यदि (word.दीर्घता_2()) {
					सन्देशः_2 += word + " ";
					curr_len += word.दीर्घता_2() + 1;
				}
			}
		}
		लघुगणकः("%s\n", सन्देशः_2);
	}
}
शून्य log_body(ध्रुव ContentListing &content, पूर्णाङ्क indent=0, तार्किक leading_newline=असत्य) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = शून्यसूचकः;

PrettyHelp::PrettyHelp()
{
	प्राक् = current_help;
	मूलम्_listing = ContentListing("root", "");

	current_help = एतत्;
}

PrettyHelp::~PrettyHelp()
{
	current_help = प्राक्;
}

PrettyHelp *PrettyHelp::get_current()
{
	यदि (current_help == शून्यसूचकः)
		नूतन PrettyHelp();
	प्रत्यागम current_help;
}

शून्य PrettyHelp::log_help() ध्रुव
{
	प्रति (स्वचालित &content : मूलम्_listing) {
		यदि (content.type.compare("usage") == 0) {
			log_body(content, 1, सत्य);
			लघुगणकः("\n");
		} अन्यथा यदि (content.type.compare("option") == 0) {
			log_body(content, 1);
			प्रति (स्वचालित text : content) {
				log_body(text, 2);
				लघुगणकः("\n");
			}
		} अन्यथा {
			log_body(content, 0);
			लघुगणकः("\n");
		}
	}
}

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

std::filesystem::path Yosys::fixup_source_path (स्थिरांक अक्षर* source_file,
	स्थिरांक std::filesystem::path &source_मूळ, तार्किक* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	जर (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		स्वतः proximate_path = std::filesystem::proximate(source_path, source_मूळ);
		जर (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			परत proximate_path;
		} अन्यथा जर (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = सत्य;
		}
	}
	परत source_path;
}

Json ContentListing::to_json(स्थिरांक std::filesystem::path &source_मूळ) स्थिरांक {
	Json::object object;

	object["type"] = type;
	जर (body.length())
		object["body"] = body;

	जर (has_source())
		object["source_file"] = fixup_source_path(source_file, source_मूळ).string();
	जर (source_ओळ != 0)
		object["source_line"] = source_ओळ;

	object["options"] = Json(options);

	Json::array content_array;
	साठी (स्वतः child : _content)
		content_array.ढकला_back(child.to_json(source_मूळ));
	object["content"] = content_array;

	परत object;
}

शून्य ContentListing::usage(स्थिरांक string &text,
	स्थिरांक source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

शून्य ContentListing::option(स्थिरांक string &text, स्थिरांक string &description,
	स्थिरांक source_location location)
{
	स्वतः option = open_option(text, source_location());
	जर (description.length())
		option->add_content("text", description, location);
}

शून्य ContentListing::codeblock(स्थिरांक string &code, स्थिरांक string &language,
	स्थिरांक source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

शून्य ContentListing::paragraph(स्थिरांक string &text,
	स्थिरांक source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(स्थिरांक string &text,
	स्थिरांक source_location location)
{
	usage(text, location);
	परत back();
}

ContentListing* ContentListing::open_option(स्थिरांक string &text,
	स्थिरांक source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	परत back();
}

#define MAX_ओळ_LEN 80
शून्य log_body_str(स्थिरांक std::string &pass_str, std::string indent_str, तार्किक leading_newline=असत्य, तार्किक is_formatted=असत्य) {
	जर (pass_str.empty())
		परत;
	std::istringstream iss(pass_str);
	जर (leading_newline)
		log("\n");
	साठी (std::string ओळ; std::getline(iss, ओळ);) {
		std::string message = indent_str;
		जर (is_formatted) {
			message += ओळ;
		} अन्यथा {
			स्वतः curr_len = indent_str.length();
			std::istringstream lss(ओळ);
			साठी (std::string word; std::getline(lss, word, ' ');) {
				पर्यंत (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				जर (curr_len + word.length() >= MAX_ओळ_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				जर (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
शून्य log_body(स्थिरांक ContentListing &content, पूर्णांक indent=0, तार्किक leading_newline=असत्य) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	मूळ_listing = ContentListing("root", "");

	current_help = हे;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	जर (current_help == nullptr)
		नवीन PrettyHelp();
	परत current_help;
}

शून्य PrettyHelp::log_help() स्थिरांक
{
	साठी (स्वतः &content : मूळ_listing) {
		जर (content.type.compare("usage") == 0) {
			log_body(content, 1, सत्य);
			log("\n");
		} अन्यथा जर (content.type.compare("option") == 0) {
			log_body(content, 1);
			साठी (स्वतः text : content) {
				log_body(text, 2);
				log("\n");
			}
		} अन्यथा {
			log_body(content, 0);
			log("\n");
		}
	}
}

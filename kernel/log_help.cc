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

#समावेश "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (स्थिर अक्षर* source_file,
	स्थिर std::filesystem::path &source_मूल, bool* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	अगर (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		स्वतः proximate_path = std::filesystem::proximate(source_path, source_मूल);
		अगर (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			वापस proximate_path;
		} अन्यथा अगर (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = true;
		}
	}
	वापस source_path;
}

Json ContentListing::to_json(स्थिर std::filesystem::path &source_मूल) स्थिर {
	Json::object object;

	object["type"] = type;
	अगर (body.लंबाई())
		object["body"] = body;

	अगर (has_source())
		object["source_file"] = fixup_source_path(source_file, source_मूल).string();
	अगर (source_पंक्ति != 0)
		object["source_line"] = source_पंक्ति;

	object["options"] = Json(options);

	Json::array content_array;
	क्रम (स्वतः child : _content)
		content_array.धकेलो_back(child.to_json(source_मूल));
	object["content"] = content_array;

	वापस object;
}

व्योम ContentListing::usage(स्थिर string &text,
	स्थिर source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

व्योम ContentListing::option(स्थिर string &text, स्थिर string &description,
	स्थिर source_location location)
{
	स्वतः option = open_option(text, source_location());
	अगर (description.लंबाई())
		option->add_content("text", description, location);
}

व्योम ContentListing::codeblock(स्थिर string &code, स्थिर string &language,
	स्थिर source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

व्योम ContentListing::paragraph(स्थिर string &text,
	स्थिर source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(स्थिर string &text,
	स्थिर source_location location)
{
	usage(text, location);
	वापस back();
}

ContentListing* ContentListing::open_option(स्थिर string &text,
	स्थिर source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	वापस back();
}

#घोषणा MAX_पंक्ति_LEN 80
व्योम log_body_str(स्थिर std::string &pass_str, std::string indent_str, bool leading_newline=false, bool is_formatted=false) {
	अगर (pass_str.empty())
		वापस;
	std::istringstream iss(pass_str);
	अगर (leading_newline)
		log("\n");
	क्रम (std::string पंक्ति; std::लो_पंक्ति(iss, पंक्ति);) {
		std::string message = indent_str;
		अगर (is_formatted) {
			message += पंक्ति;
		} अन्यथा {
			स्वतः curr_len = indent_str.लंबाई();
			std::istringstream lss(पंक्ति);
			क्रम (std::string word; std::लो_पंक्ति(lss, word, ' ');) {
				जबतक (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.लंबाई()-2);
				अगर (curr_len + word.लंबाई() >= MAX_पंक्ति_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				अगर (word.लंबाई()) {
					message += word + " ";
					curr_len += word.लंबाई() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
व्योम log_body(स्थिर ContentListing &content, पूर्णांक indent=0, bool leading_newline=false) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	मूल_listing = ContentListing("root", "");

	current_help = यह;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	अगर (current_help == nullptr)
		नया PrettyHelp();
	वापस current_help;
}

व्योम PrettyHelp::log_help() स्थिर
{
	क्रम (स्वतः &content : मूल_listing) {
		अगर (content.type.compare("usage") == 0) {
			log_body(content, 1, true);
			log("\n");
		} अन्यथा अगर (content.type.compare("option") == 0) {
			log_body(content, 1);
			क्रम (स्वतः text : content) {
				log_body(text, 2);
				log("\n");
			}
		} अन्यथा {
			log_body(content, 0);
			log("\n");
		}
	}
}

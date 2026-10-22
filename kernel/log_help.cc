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

#അന്തര്ഭാവ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (സ്ഥിരം അക്ഷരം* source_file,
	സ്ഥിരം std::filesystem::path &source_വേര്, തർക്കം* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	എങ്കിൽ (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_വേര്);
		എങ്കിൽ (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			തിരികെ proximate_path;
		} അല്ലെങ്കിൽ എങ്കിൽ (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = സത്യം;
		}
	}
	തിരികെ source_path;
}

Json ContentListing::to_json(സ്ഥിരം std::filesystem::path &source_വേര്) സ്ഥിരം {
	Json::object object;

	object["type"] = type;
	എങ്കിൽ (body.length())
		object["body"] = body;

	എങ്കിൽ (has_source())
		object["source_file"] = fixup_source_path(source_file, source_വേര്).string();
	എങ്കിൽ (source_വരി != 0)
		object["source_line"] = source_വരി;

	object["options"] = Json(options);

	Json::array content_array;
	വേണ്ടി (auto child : _content)
		content_array.തള്ളുക_back(child.to_json(source_വേര്));
	object["content"] = content_array;

	തിരികെ object;
}

ശൂന്യം ContentListing::usage(സ്ഥിരം string &text,
	സ്ഥിരം source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

ശൂന്യം ContentListing::option(സ്ഥിരം string &text, സ്ഥിരം string &description,
	സ്ഥിരം source_location location)
{
	auto option = open_option(text, source_location());
	എങ്കിൽ (description.length())
		option->add_content("text", description, location);
}

ശൂന്യം ContentListing::codeblock(സ്ഥിരം string &code, സ്ഥിരം string &language,
	സ്ഥിരം source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

ശൂന്യം ContentListing::paragraph(സ്ഥിരം string &text,
	സ്ഥിരം source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(സ്ഥിരം string &text,
	സ്ഥിരം source_location location)
{
	usage(text, location);
	തിരികെ back();
}

ContentListing* ContentListing::open_option(സ്ഥിരം string &text,
	സ്ഥിരം source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	തിരികെ back();
}

#define MAX_വരി_LEN 80
ശൂന്യം log_body_str(സ്ഥിരം std::string &pass_str, std::string indent_str, തർക്കം leading_newline=അസത്യം, തർക്കം is_formatted=അസത്യം) {
	എങ്കിൽ (pass_str.empty())
		തിരികെ;
	std::istringstream iss(pass_str);
	എങ്കിൽ (leading_newline)
		log("\n");
	വേണ്ടി (std::string വരി; std::getline(iss, വരി);) {
		std::string message = indent_str;
		എങ്കിൽ (is_formatted) {
			message += വരി;
		} അല്ലെങ്കിൽ {
			auto curr_len = indent_str.length();
			std::istringstream lss(വരി);
			വേണ്ടി (std::string word; std::getline(lss, word, ' ');) {
				ആകുമ്പോൾ (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				എങ്കിൽ (curr_len + word.length() >= MAX_വരി_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				എങ്കിൽ (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
ശൂന്യം log_body(സ്ഥിരം ContentListing &content, പൂർണ്ണസംഖ്യ indent=0, തർക്കം leading_newline=അസത്യം) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	വേര്_listing = ContentListing("root", "");

	current_help = ഇത്;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	എങ്കിൽ (current_help == nullptr)
		പുതിയ PrettyHelp();
	തിരികെ current_help;
}

ശൂന്യം PrettyHelp::log_help() സ്ഥിരം
{
	വേണ്ടി (auto &content : വേര്_listing) {
		എങ്കിൽ (content.type.compare("usage") == 0) {
			log_body(content, 1, സത്യം);
			log("\n");
		} അല്ലെങ്കിൽ എങ്കിൽ (content.type.compare("option") == 0) {
			log_body(content, 1);
			വേണ്ടി (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} അല്ലെങ്കിൽ {
			log_body(content, 0);
			log("\n");
		}
	}
}

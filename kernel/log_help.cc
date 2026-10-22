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

#شامل "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (ثابت نویسه* source_file,
	ثابت std::filesystem::path &source_ریشه, منطقی* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	اگر (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_ریشه);
		اگر (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			بازگشت proximate_path;
		} وگرنه اگر (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = درست;
		}
	}
	بازگشت source_path;
}

Json ContentListing::to_json(ثابت std::filesystem::path &source_ریشه) ثابت {
	Json::object object;

	object["type"] = type;
	اگر (body.length())
		object["body"] = body;

	اگر (has_source())
		object["source_file"] = fixup_source_path(source_file, source_ریشه).string();
	اگر (source_سطر != 0)
		object["source_line"] = source_سطر;

	object["options"] = Json(options);

	Json::array content_array;
	برای (auto child : _content)
		content_array.افزودن_back(child.to_json(source_ریشه));
	object["content"] = content_array;

	بازگشت object;
}

تهی ContentListing::usage(ثابت string &text,
	ثابت source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

تهی ContentListing::option(ثابت string &text, ثابت string &description,
	ثابت source_location location)
{
	auto option = open_option(text, source_location());
	اگر (description.length())
		option->add_content("text", description, location);
}

تهی ContentListing::codeblock(ثابت string &code, ثابت string &language,
	ثابت source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

تهی ContentListing::paragraph(ثابت string &text,
	ثابت source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ثابت string &text,
	ثابت source_location location)
{
	usage(text, location);
	بازگشت back();
}

ContentListing* ContentListing::open_option(ثابت string &text,
	ثابت source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	بازگشت back();
}

#define MAX_سطر_LEN 80
تهی log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقی leading_newline=نادرست, منطقی is_formatted=نادرست) {
	اگر (pass_str.empty())
		بازگشت;
	std::istringstream iss(pass_str);
	اگر (leading_newline)
		log("\n");
	برای (std::string سطر; std::getline(iss, سطر);) {
		std::string message = indent_str;
		اگر (is_formatted) {
			message += سطر;
		} وگرنه {
			auto curr_len = indent_str.length();
			std::istringstream lss(سطر);
			برای (std::string word; std::getline(lss, word, ' ');) {
				وقتی (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				اگر (curr_len + word.length() >= MAX_سطر_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				اگر (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
تهی log_body(ثابت ContentListing &content, صحیح indent=0, منطقی leading_newline=نادرست) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	ریشه_listing = ContentListing("root", "");

	current_help = این;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	اگر (current_help == nullptr)
		جدید PrettyHelp();
	بازگشت current_help;
}

تهی PrettyHelp::log_help() ثابت
{
	برای (auto &content : ریشه_listing) {
		اگر (content.type.compare("usage") == 0) {
			log_body(content, 1, درست);
			log("\n");
		} وگرنه اگر (content.type.compare("option") == 0) {
			log_body(content, 1);
			برای (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} وگرنه {
			log_body(content, 0);
			log("\n");
		}
	}
}

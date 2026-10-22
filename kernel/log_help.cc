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

#অন্তর্ভাৱ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (ধ্রুৱ ৱর্ণ* source_file,
	ধ্রুৱ std::filesystem::path &source_মূল, তার্কিক* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	যদি (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_মূল);
		যদি (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ঘূৰাই proximate_path;
		} অন্যথা যদি (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = সঁচা;
		}
	}
	ঘূৰাই source_path;
}

Json ContentListing::to_json(ধ্রুৱ std::filesystem::path &source_মূল) ধ্রুৱ {
	Json::object object;

	object["type"] = type;
	যদি (body.length())
		object["body"] = body;

	যদি (has_source())
		object["source_file"] = fixup_source_path(source_file, source_মূল).string();
	যদি (source_সাৰি != 0)
		object["source_line"] = source_সাৰি;

	object["options"] = Json(options);

	Json::array content_array;
	বাবে (auto child : _content)
		content_array.ঠেলো_back(child.to_json(source_মূল));
	object["content"] = content_array;

	ঘূৰাই object;
}

শূন্য ContentListing::usage(ধ্রুৱ string &text,
	ধ্রুৱ source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

শূন্য ContentListing::option(ধ্রুৱ string &text, ধ্রুৱ string &description,
	ধ্রুৱ source_location location)
{
	auto option = open_option(text, source_location());
	যদি (description.length())
		option->add_content("text", description, location);
}

শূন্য ContentListing::codeblock(ধ্রুৱ string &code, ধ্রুৱ string &language,
	ধ্রুৱ source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

শূন্য ContentListing::paragraph(ধ্রুৱ string &text,
	ধ্রুৱ source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ধ্রুৱ string &text,
	ধ্রুৱ source_location location)
{
	usage(text, location);
	ঘূৰাই back();
}

ContentListing* ContentListing::open_option(ধ্রুৱ string &text,
	ধ্রুৱ source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	ঘূৰাই back();
}

#define MAX_সাৰি_LEN 80
শূন্য log_body_str(ধ্রুৱ std::string &pass_str, std::string indent_str, তার্কিক leading_newline=মিছা, তার্কিক is_formatted=মিছা) {
	যদি (pass_str.empty())
		ঘূৰাই;
	std::istringstream iss(pass_str);
	যদি (leading_newline)
		log("\n");
	বাবে (std::string সাৰি; std::getline(iss, সাৰি);) {
		std::string message = indent_str;
		যদি (is_formatted) {
			message += সাৰি;
		} অন্যথা {
			auto curr_len = indent_str.length();
			std::istringstream lss(সাৰি);
			বাবে (std::string word; std::getline(lss, word, ' ');) {
				যেতিয়ালৈ (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				যদি (curr_len + word.length() >= MAX_সাৰি_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				যদি (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
শূন্য log_body(ধ্রুৱ ContentListing &content, পূর্ণাঙ্ক indent=0, তার্কিক leading_newline=মিছা) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	মূল_listing = ContentListing("root", "");

	current_help = এতত্;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	যদি (current_help == nullptr)
		নূতন PrettyHelp();
	ঘূৰাই current_help;
}

শূন্য PrettyHelp::log_help() ধ্রুৱ
{
	বাবে (auto &content : মূল_listing) {
		যদি (content.type.compare("usage") == 0) {
			log_body(content, 1, সঁচা);
			log("\n");
		} অন্যথা যদি (content.type.compare("option") == 0) {
			log_body(content, 1);
			বাবে (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} অন্যথা {
			log_body(content, 0);
			log("\n");
		}
	}
}

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

#অন্তর্ভাব "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (ধ্রুবক অক্ষর* source_file,
	ধ্রুবক std::filesystem::path &source_মূল, যৌক্তিক* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	যদি (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_মূল);
		যদি (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ফেরত proximate_path;
		} অন্যথা যদি (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = সত্য;
		}
	}
	ফেরত source_path;
}

Json ContentListing::to_json(ধ্রুবক std::filesystem::path &source_মূল) ধ্রুবক {
	Json::object object;

	object["type"] = type;
	যদি (body.দৈর্ঘ্য())
		object["body"] = body;

	যদি (has_source())
		object["source_file"] = fixup_source_path(source_file, source_মূল).string();
	যদি (source_সারি != 0)
		object["source_line"] = source_সারি;

	object["options"] = Json(options);

	Json::array content_array;
	জন্য (auto child : _content)
		content_array.ঠেলো_back(child.to_json(source_মূল));
	object["content"] = content_array;

	ফেরত object;
}

শূন্য ContentListing::usage(ধ্রুবক string &text,
	ধ্রুবক source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

শূন্য ContentListing::option(ধ্রুবক string &text, ধ্রুবক string &description,
	ধ্রুবক source_location location)
{
	auto option = open_option(text, source_location());
	যদি (description.দৈর্ঘ্য())
		option->add_content("text", description, location);
}

শূন্য ContentListing::codeblock(ধ্রুবক string &code, ধ্রুবক string &language,
	ধ্রুবক source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

শূন্য ContentListing::paragraph(ধ্রুবক string &text,
	ধ্রুবক source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ধ্রুবক string &text,
	ধ্রুবক source_location location)
{
	usage(text, location);
	ফেরত back();
}

ContentListing* ContentListing::open_option(ধ্রুবক string &text,
	ধ্রুবক source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	ফেরত back();
}

#define MAX_সারি_LEN 80
শূন্য log_body_str(ধ্রুবক std::string &pass_str, std::string indent_str, যৌক্তিক leading_newline=মিথ্যা, যৌক্তিক is_formatted=মিথ্যা) {
	যদি (pass_str.empty())
		ফেরত;
	std::istringstream iss(pass_str);
	যদি (leading_newline)
		log("\n");
	জন্য (std::string সারি; std::getline(iss, সারি);) {
		std::string message = indent_str;
		যদি (is_formatted) {
			message += সারি;
		} অন্যথা {
			auto curr_len = indent_str.দৈর্ঘ্য();
			std::istringstream lss(সারি);
			জন্য (std::string word; std::getline(lss, word, ' ');) {
				যখন (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.দৈর্ঘ্য()-2);
				যদি (curr_len + word.দৈর্ঘ্য() >= MAX_সারি_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				যদি (word.দৈর্ঘ্য()) {
					message += word + " ";
					curr_len += word.দৈর্ঘ্য() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
শূন্য log_body(ধ্রুবক ContentListing &content, পূর্ণসংখ্যা indent=0, যৌক্তিক leading_newline=মিথ্যা) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	মূল_listing = ContentListing("root", "");

	current_help = এই;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	যদি (current_help == nullptr)
		নতুন PrettyHelp();
	ফেরত current_help;
}

শূন্য PrettyHelp::log_help() ধ্রুবক
{
	জন্য (auto &content : মূল_listing) {
		যদি (content.type.compare("usage") == 0) {
			log_body(content, 1, সত্য);
			log("\n");
		} অন্যথা যদি (content.type.compare("option") == 0) {
			log_body(content, 1);
			জন্য (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} অন্যথা {
			log_body(content, 0);
			log("\n");
		}
	}
}

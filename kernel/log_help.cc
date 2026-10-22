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

#include "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (const var_na* source_file,
	const std::filesystem::path &source_root, bool* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	အကယ်၍ (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_root);
		အကယ်၍ (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ပြန်ပေး proximate_path;
		} မဟုတ်ပါက အကယ်၍ (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = true;
		}
	}
	ပြန်ပေး source_path;
}

Json ContentListing::to_json(const std::filesystem::path &source_root) const {
	Json::object object;

	object["type"] = type;
	အကယ်၍ (body.length())
		object["body"] = body;

	အကယ်၍ (has_source())
		object["source_file"] = fixup_source_path(source_file, source_root).string();
	အကယ်၍ (source_line != 0)
		object["source_line"] = source_line;

	object["options"] = Json(options);

	Json::array content_array;
	အတွက် (auto child : _content)
		content_array.push_back(child.to_json(source_root));
	object["content"] = content_array;

	ပြန်ပေး object;
}

void ContentListing::usage(const string &text,
	const source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

void ContentListing::option(const string &text, const string &description,
	const source_location location)
{
	auto option = open_option(text, source_location());
	အကယ်၍ (description.length())
		option->add_content("text", description, location);
}

void ContentListing::codeblock(const string &code, const string &language,
	const source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

void ContentListing::paragraph(const string &text,
	const source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(const string &text,
	const source_location location)
{
	usage(text, location);
	ပြန်ပေး back();
}

ContentListing* ContentListing::open_option(const string &text,
	const source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	ပြန်ပေး back();
}

#define MAX_LINE_LEN 80
void log_body_str(const std::string &pass_str, std::string indent_str, bool leading_newline=false, bool is_formatted=false) {
	အကယ်၍ (pass_str.empty())
		ပြန်ပေး;
	std::istringstream iss(pass_str);
	အကယ်၍ (leading_newline)
		log("\n");
	အတွက် (std::string line; std::getline(iss, line);) {
		std::string message = indent_str;
		အကယ်၍ (is_formatted) {
			message += line;
		} မဟုတ်ပါက {
			auto curr_len = indent_str.length();
			std::istringstream lss(line);
			အတွက် (std::string word; std::getline(lss, word, ' ');) {
				နေစဉ် (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				အကယ်၍ (curr_len + word.length() >= MAX_LINE_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				အကယ်၍ (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
void log_body(const ContentListing &content, ကိန်းပြည့် indent=0, bool leading_newline=false) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	_root_listing = ContentListing("root", "");

	current_help = this;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	အကယ်၍ (current_help == nullptr)
		new PrettyHelp();
	ပြန်ပေး current_help;
}

void PrettyHelp::log_help() const
{
	အတွက် (auto &content : _root_listing) {
		အကယ်၍ (content.type.compare("usage") == 0) {
			log_body(content, 1, true);
			log("\n");
		} မဟုတ်ပါက အကယ်၍ (content.type.compare("option") == 0) {
			log_body(content, 1);
			အတွက် (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} မဟုတ်ပါက {
			log_body(content, 0);
			log("\n");
		}
	}
}

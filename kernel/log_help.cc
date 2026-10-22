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

std::filesystem::path Yosys::fixup_source_path (const char* source_file,
	const std::filesystem::path &source_root, bool* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	якщо (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_root);
		якщо (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			повернути proximate_path;
		} інакше якщо (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = true;
		}
	}
	повернути source_path;
}

Json ContentListing::to_json(const std::filesystem::path &source_root) const {
	Json::object object;

	object["type"] = type;
	якщо (body.length())
		object["body"] = body;

	якщо (has_source())
		object["source_file"] = fixup_source_path(source_file, source_root).string();
	якщо (source_line != 0)
		object["source_line"] = source_line;

	object["options"] = Json(options);

	Json::array content_array;
	для (auto child : _content)
		content_array.push_back(child.to_json(source_root));
	object["content"] = content_array;

	повернути object;
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
	якщо (description.length())
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
	повернути back();
}

ContentListing* ContentListing::open_option(const string &text,
	const source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	повернути back();
}

#define MAX_LINE_LEN 80
void log_body_str(const std::string &pass_str, std::string indent_str, bool leading_newline=false, bool is_formatted=false) {
	якщо (pass_str.empty())
		повернути;
	std::istringstream iss(pass_str);
	якщо (leading_newline)
		log("\n");
	для (std::string line; std::getline(iss, line);) {
		std::string message = indent_str;
		якщо (is_formatted) {
			message += line;
		} інакше {
			auto curr_len = indent_str.length();
			std::istringstream lss(line);
			для (std::string word; std::getline(lss, word, ' ');) {
				поки (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				якщо (curr_len + word.length() >= MAX_LINE_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				якщо (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
void log_body(const ContentListing &content, ціле indent=0, bool leading_newline=false) {
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
	якщо (current_help == nullptr)
		new PrettyHelp();
	повернути current_help;
}

void PrettyHelp::log_help() const
{
	для (auto &content : _root_listing) {
		якщо (content.type.compare("usage") == 0) {
			log_body(content, 1, true);
			log("\n");
		} інакше якщо (content.type.compare("option") == 0) {
			log_body(content, 1);
			для (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} інакше {
			log_body(content, 0);
			log("\n");
		}
	}
}

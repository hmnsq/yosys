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

#ضمن "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (ثابت حرف* source_file,
	ثابت std::filesystem::path &source_root, منطقي* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	𐤀𐤌 (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_root);
		𐤀𐤌 (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			أرجع proximate_path;
		} وإلا 𐤀𐤌 (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = true;
		}
	}
	أرجع source_path;
}

Json ContentListing::to_json(ثابت std::filesystem::path &source_root) ثابت {
	Json::object object;

	object["type"] = type;
	𐤀𐤌 (body.length())
		object["body"] = body;

	𐤀𐤌 (has_source())
		object["source_file"] = fixup_source_path(source_file, source_root).string();
	𐤀𐤌 (source_line != 0)
		object["source_line"] = source_line;

	object["options"] = Json(options);

	Json::array content_array;
	لكل (auto child : _content)
		content_array.push_back(child.to_json(source_root));
	object["content"] = content_array;

	أرجع object;
}

فراغ ContentListing::usage(ثابت string &text,
	ثابت source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

فراغ ContentListing::option(ثابت string &text, ثابت string &description,
	ثابت source_location location)
{
	auto option = open_option(text, source_location());
	𐤀𐤌 (description.length())
		option->add_content("text", description, location);
}

فراغ ContentListing::codeblock(ثابت string &code, ثابت string &language,
	ثابت source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

فراغ ContentListing::paragraph(ثابت string &text,
	ثابت source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ثابت string &text,
	ثابت source_location location)
{
	usage(text, location);
	أرجع back();
}

ContentListing* ContentListing::open_option(ثابت string &text,
	ثابت source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	أرجع back();
}

#define MAX_LINE_LEN 80
فراغ log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقي leading_newline=خطأ, منطقي is_formatted=خطأ) {
	𐤀𐤌 (pass_str.empty())
		أرجع;
	std::istringstream iss(pass_str);
	𐤀𐤌 (leading_newline)
		log("\n");
	لكل (std::string line; std::getline(iss, line);) {
		std::string message = indent_str;
		𐤀𐤌 (is_formatted) {
			message += line;
		} وإلا {
			auto curr_len = indent_str.length();
			std::istringstream lss(line);
			لكل (std::string word; std::getline(lss, word, ' ');) {
				طالما (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				𐤀𐤌 (curr_len + word.length() >= MAX_LINE_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				𐤀𐤌 (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
فراغ log_body(ثابت ContentListing &content, صحيح indent=0, منطقي leading_newline=خطأ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	_root_listing = ContentListing("root", "");

	current_help = هذا;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	𐤀𐤌 (current_help == nullptr)
		جديد PrettyHelp();
	أرجع current_help;
}

فراغ PrettyHelp::log_help() ثابت
{
	لكل (auto &content : _root_listing) {
		𐤀𐤌 (content.type.compare("usage") == 0) {
			log_body(content, 1, true);
			log("\n");
		} وإلا 𐤀𐤌 (content.type.compare("option") == 0) {
			log_body(content, 1);
			لكل (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} وإلا {
			log_body(content, 0);
			log("\n");
		}
	}
}

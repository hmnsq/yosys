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

#અન્તર્ભાવ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (અચલ અક્ષર* source_file,
	અચલ std::filesystem::path &source_મૂળ, તાર્કિક* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	જો (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_મૂળ);
		જો (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			પાછા proximate_path;
		} નહીંતર જો (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = સાચું;
		}
	}
	પાછા source_path;
}

Json ContentListing::to_json(અચલ std::filesystem::path &source_મૂળ) અચલ {
	Json::object object;

	object["type"] = type;
	જો (body.length())
		object["body"] = body;

	જો (has_source())
		object["source_file"] = fixup_source_path(source_file, source_મૂળ).string();
	જો (source_લીટી != 0)
		object["source_line"] = source_લીટી;

	object["options"] = Json(options);

	Json::array content_array;
	માટે (auto child : _content)
		content_array.ધકેલો_back(child.to_json(source_મૂળ));
	object["content"] = content_array;

	પાછા object;
}

શૂન્ય ContentListing::usage(અચલ string &text,
	અચલ source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

શૂન્ય ContentListing::option(અચલ string &text, અચલ string &description,
	અચલ source_location location)
{
	auto option = open_option(text, source_location());
	જો (description.length())
		option->add_content("text", description, location);
}

શૂન્ય ContentListing::codeblock(અચલ string &code, અચલ string &language,
	અચલ source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

શૂન્ય ContentListing::paragraph(અચલ string &text,
	અચલ source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(અચલ string &text,
	અચલ source_location location)
{
	usage(text, location);
	પાછા back();
}

ContentListing* ContentListing::open_option(અચલ string &text,
	અચલ source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	પાછા back();
}

#define MAX_લીટી_LEN 80
શૂન્ય log_body_str(અચલ std::string &pass_str, std::string indent_str, તાર્કિક leading_newline=ખોટું, તાર્કિક is_formatted=ખોટું) {
	જો (pass_str.empty())
		પાછા;
	std::istringstream iss(pass_str);
	જો (leading_newline)
		log("\n");
	માટે (std::string લીટી; std::getline(iss, લીટી);) {
		std::string message = indent_str;
		જો (is_formatted) {
			message += લીટી;
		} નહીંતર {
			auto curr_len = indent_str.length();
			std::istringstream lss(લીટી);
			માટે (std::string word; std::getline(lss, word, ' ');) {
				જ્યારે (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				જો (curr_len + word.length() >= MAX_લીટી_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				જો (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
શૂન્ય log_body(અચલ ContentListing &content, પૂર્ણાંક indent=0, તાર્કિક leading_newline=ખોટું) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	મૂળ_listing = ContentListing("root", "");

	current_help = આ;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	જો (current_help == nullptr)
		નવું PrettyHelp();
	પાછા current_help;
}

શૂન્ય PrettyHelp::log_help() અચલ
{
	માટે (auto &content : મૂળ_listing) {
		જો (content.type.compare("usage") == 0) {
			log_body(content, 1, સાચું);
			log("\n");
		} નહીંતર જો (content.type.compare("option") == 0) {
			log_body(content, 1);
			માટે (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} નહીંતર {
			log_body(content, 0);
			log("\n");
		}
	}
}

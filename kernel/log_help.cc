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

#ଅନ୍ତର୍ଭାଵ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (ଅଚଳ ଅକ୍ଷର* source_file,
	ଅଚଳ std::filesystem::path &source_ମୂଳ, ତାର୍କିକ* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	ଯଦି (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_ମୂଳ);
		ଯଦି (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ଫେରନ୍ତୁ proximate_path;
		} ଅନ୍ୟଥା ଯଦି (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = ସତ୍ୟ;
		}
	}
	ଫେରନ୍ତୁ source_path;
}

Json ContentListing::to_json(ଅଚଳ std::filesystem::path &source_ମୂଳ) ଅଚଳ {
	Json::object object;

	object["type"] = type;
	ଯଦି (body.length())
		object["body"] = body;

	ଯଦି (has_source())
		object["source_file"] = fixup_source_path(source_file, source_ମୂଳ).string();
	ଯଦି (source_ଧାଡ଼ି != 0)
		object["source_line"] = source_ଧାଡ଼ି;

	object["options"] = Json(options);

	Json::array content_array;
	ପାଇଁ (auto child : _content)
		content_array.ଠେଲ_back(child.to_json(source_ମୂଳ));
	object["content"] = content_array;

	ଫେରନ୍ତୁ object;
}

ଶୂନ୍ୟ ContentListing::usage(ଅଚଳ string &text,
	ଅଚଳ source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

ଶୂନ୍ୟ ContentListing::option(ଅଚଳ string &text, ଅଚଳ string &description,
	ଅଚଳ source_location location)
{
	auto option = open_option(text, source_location());
	ଯଦି (description.length())
		option->add_content("text", description, location);
}

ଶୂନ୍ୟ ContentListing::codeblock(ଅଚଳ string &code, ଅଚଳ string &language,
	ଅଚଳ source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

ଶୂନ୍ୟ ContentListing::paragraph(ଅଚଳ string &text,
	ଅଚଳ source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(ଅଚଳ string &text,
	ଅଚଳ source_location location)
{
	usage(text, location);
	ଫେରନ୍ତୁ back();
}

ContentListing* ContentListing::open_option(ଅଚଳ string &text,
	ଅଚଳ source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	ଫେରନ୍ତୁ back();
}

#define MAX_ଧାଡ଼ି_LEN 80
ଶୂନ୍ୟ log_body_str(ଅଚଳ std::string &pass_str, std::string indent_str, ତାର୍କିକ leading_newline=ମିଥ୍ୟା, ତାର୍କିକ is_formatted=ମିଥ୍ୟା) {
	ଯଦି (pass_str.empty())
		ଫେରନ୍ତୁ;
	std::istringstream iss(pass_str);
	ଯଦି (leading_newline)
		log("\n");
	ପାଇଁ (std::string ଧାଡ଼ି; std::getline(iss, ଧାଡ଼ି);) {
		std::string message = indent_str;
		ଯଦି (is_formatted) {
			message += ଧାଡ଼ି;
		} ଅନ୍ୟଥା {
			auto curr_len = indent_str.length();
			std::istringstream lss(ଧାଡ଼ି);
			ପାଇଁ (std::string word; std::getline(lss, word, ' ');) {
				ଯେତେବେଳେ (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				ଯଦି (curr_len + word.length() >= MAX_ଧାଡ଼ି_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				ଯଦି (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
ଶୂନ୍ୟ log_body(ଅଚଳ ContentListing &content, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent=0, ତାର୍କିକ leading_newline=ମିଥ୍ୟା) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	ମୂଳ_listing = ContentListing("root", "");

	current_help = ଏହି;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	ଯଦି (current_help == nullptr)
		ନୂତନ PrettyHelp();
	ଫେରନ୍ତୁ current_help;
}

ଶୂନ୍ୟ PrettyHelp::log_help() ଅଚଳ
{
	ପାଇଁ (auto &content : ମୂଳ_listing) {
		ଯଦି (content.type.compare("usage") == 0) {
			log_body(content, 1, ସତ୍ୟ);
			log("\n");
		} ଅନ୍ୟଥା ଯଦି (content.type.compare("option") == 0) {
			log_body(content, 1);
			ପାଇଁ (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} ଅନ୍ୟଥା {
			log_body(content, 0);
			log("\n");
		}
	}
}

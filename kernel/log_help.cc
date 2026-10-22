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

#அந்தர்பாவ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::path Yosys::fixup_source_path (மாறிலி எழுத்து* source_file,
	மாறிலி std::filesystem::path &source_வேர், தருக்க* skip_source_group)
{
	std::filesystem::path source_path(source_file);
	எனில் (source_path.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		auto proximate_path = std::filesystem::proximate(source_path, source_வேர்);
		எனில் (proximate_path == proximate_path.lexically_normal()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			திரும்பு proximate_path;
		} இல்லையெனில் எனில் (skip_source_group != nullptr) {
			// don't try to group external paths
			*skip_source_group = உண்மை;
		}
	}
	திரும்பு source_path;
}

Json ContentListing::to_json(மாறிலி std::filesystem::path &source_வேர்) மாறிலி {
	Json::object object;

	object["type"] = type;
	எனில் (body.length())
		object["body"] = body;

	எனில் (has_source())
		object["source_file"] = fixup_source_path(source_file, source_வேர்).string();
	எனில் (source_வரி != 0)
		object["source_line"] = source_வரி;

	object["options"] = Json(options);

	Json::array content_array;
	க்கு (auto child : _content)
		content_array.தள்ளு_back(child.to_json(source_வேர்));
	object["content"] = content_array;

	திரும்பு object;
}

வெறுமை ContentListing::usage(மாறிலி string &text,
	மாறிலி source_location location)
{
	log_assert(type.compare("root") == 0);
	add_content("usage", text, location);
}

வெறுமை ContentListing::option(மாறிலி string &text, மாறிலி string &description,
	மாறிலி source_location location)
{
	auto option = open_option(text, source_location());
	எனில் (description.length())
		option->add_content("text", description, location);
}

வெறுமை ContentListing::codeblock(மாறிலி string &code, மாறிலி string &language,
	மாறிலி source_location location)
{
	add_content("code", code, location);
	back()->set_option("language", language);
}

வெறுமை ContentListing::paragraph(மாறிலி string &text,
	மாறிலி source_location location)
{
	add_content("text", text, location);
}

ContentListing* ContentListing::open_usage(மாறிலி string &text,
	மாறிலி source_location location)
{
	usage(text, location);
	திரும்பு back();
}

ContentListing* ContentListing::open_option(மாறிலி string &text,
	மாறிலி source_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	add_content("option", text, location);
	திரும்பு back();
}

#define MAX_வரி_LEN 80
வெறுமை log_body_str(மாறிலி std::string &pass_str, std::string indent_str, தருக்க leading_newline=பொய், தருக்க is_formatted=பொய்) {
	எனில் (pass_str.empty())
		திரும்பு;
	std::istringstream iss(pass_str);
	எனில் (leading_newline)
		log("\n");
	க்கு (std::string வரி; std::getline(iss, வரி);) {
		std::string message = indent_str;
		எனில் (is_formatted) {
			message += வரி;
		} இல்லையெனில் {
			auto curr_len = indent_str.length();
			std::istringstream lss(வரி);
			க்கு (std::string word; std::getline(lss, word, ' ');) {
				போது (word[0] == '`' && word.back() == '`')
					word = word.substr(1, word.length()-2);
				எனில் (curr_len + word.length() >= MAX_வரி_LEN-1) {
					curr_len = 0;
					log("%s\n", message);
					message = indent_str;
				}
				எனில் (word.length()) {
					message += word + " ";
					curr_len += word.length() + 1;
				}
			}
		}
		log("%s\n", message);
	}
}
வெறுமை log_body(மாறிலி ContentListing &content, முழு indent=0, தருக்க leading_newline=பொய்) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = nullptr;

PrettyHelp::PrettyHelp()
{
	_prior = current_help;
	வேர்_listing = ContentListing("root", "");

	current_help = இது;
}

PrettyHelp::~PrettyHelp()
{
	current_help = _prior;
}

PrettyHelp *PrettyHelp::get_current()
{
	எனில் (current_help == nullptr)
		புதிய PrettyHelp();
	திரும்பு current_help;
}

வெறுமை PrettyHelp::log_help() மாறிலி
{
	க்கு (auto &content : வேர்_listing) {
		எனில் (content.type.compare("usage") == 0) {
			log_body(content, 1, உண்மை);
			log("\n");
		} இல்லையெனில் எனில் (content.type.compare("option") == 0) {
			log_body(content, 1);
			க்கு (auto text : content) {
				log_body(text, 2);
				log("\n");
			}
		} இல்லையெனில் {
			log_body(content, 0);
			log("\n");
		}
	}
}

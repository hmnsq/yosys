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

std::filesystem::لار_2 Yosys::fixup_سرچینه_لار (ثابت حرف* سرچینه_file,
	ثابت std::filesystem::لار_2 &سرچینه_ریښه, منطقي* skip_سرچینه_ډله)
{
	std::filesystem::لار_2 سرچینه_لار(سرچینه_file);
	إذا (سرچینه_لار.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		خپلکاری proximate_لار = std::filesystem::proximate(سرچینه_لار, سرچینه_ریښه);
		إذا (proximate_لار == proximate_لار.lexically_نورمال()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			أرجع proximate_لار;
		} وإلا إذا (skip_سرچینه_ډله != تش_اشاره) {
			// don't try to group external paths
			*skip_سرچینه_ډله = ریښتیا;
		}
	}
	أرجع سرچینه_لار;
}

Json ContentListing::منزل_json(ثابت std::filesystem::لار_2 &سرچینه_ریښه) ثابت {
	Json::object object;

	object["type"] = type;
	إذا (body.اوږدتیا())
		object["body"] = body;

	إذا (has_سرچینه())
		object["source_file"] = fixup_سرچینه_لار(سرچینه_file, سرچینه_ریښه).string();
	إذا (سرچینه_کرښه != 0)
		object["source_line"] = سرچینه_کرښه;

	object["options"] = Json(options);

	Json::array content_array;
	لكل (خپلکاری ماشوم : _content)
		content_array.ټېل_شا(ماشوم.منزل_json(سرچینه_ریښه));
	object["content"] = content_array;

	أرجع object;
}

فراغ ContentListing::usage(ثابت string &متن,
	ثابت سرچینه_location location)
{
	log_assert(type.compare("root") == 0);
	جمع_کړه_content("usage", متن, location);
}

فراغ ContentListing::option(ثابت string &متن, ثابت string &description,
	ثابت سرچینه_location location)
{
	خپلکاری option = open_option(متن, سرچینه_location());
	إذا (description.اوږدتیا())
		option->جمع_کړه_content("text", description, location);
}

فراغ ContentListing::codeblock(ثابت string &کوډ, ثابت string &language,
	ثابت سرچینه_location location)
{
	جمع_کړه_content("code", کوډ, location);
	شا()->سېټ_option("language", language);
}

فراغ ContentListing::paragraph(ثابت string &متن,
	ثابت سرچینه_location location)
{
	جمع_کړه_content("text", متن, location);
}

ContentListing* ContentListing::open_usage(ثابت string &متن,
	ثابت سرچینه_location location)
{
	usage(متن, location);
	أرجع شا();
}

ContentListing* ContentListing::open_option(ثابت string &متن,
	ثابت سرچینه_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	جمع_کړه_content("option", متن, location);
	أرجع شا();
}

#تعریف_کړه MAX_کرښه_LEN 80
فراغ log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقي leading_newline=خطأ, منطقي is_formatted=خطأ) {
	إذا (pass_str.تشه())
		أرجع;
	std::istringstream iss(pass_str);
	إذا (leading_newline)
		لوګاریتم("\n");
	لكل (std::string کرښه; std::getline(iss, کرښه);) {
		std::string پیغام_2 = indent_str;
		إذا (is_formatted) {
			پیغام_2 += کرښه;
		} وإلا {
			خپلکاری curr_len = indent_str.اوږدتیا();
			std::istringstream lss(کرښه);
			لكل (std::string کلمه; std::getline(lss, کلمه, ' ');) {
				طالما (کلمه[0] == '`' && کلمه.شا() == '`')
					کلمه = کلمه.substr(1, کلمه.اوږدتیا()-2);
				إذا (curr_len + کلمه.اوږدتیا() >= MAX_کرښه_LEN-1) {
					curr_len = 0;
					لوګاریتم("%s\n", پیغام_2);
					پیغام_2 = indent_str;
				}
				إذا (کلمه.اوږدتیا()) {
					پیغام_2 += کلمه + " ";
					curr_len += کلمه.اوږدتیا() + 1;
				}
			}
		}
		لوګاریتم("%s\n", پیغام_2);
	}
}
فراغ log_body(ثابت ContentListing &content, صحيح indent=0, منطقي leading_newline=خطأ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = تش_اشاره;

PrettyHelp::PrettyHelp()
{
	مخکینی = current_help;
	ریښه_listing = ContentListing("root", "");

	current_help = هذا;
}

PrettyHelp::~PrettyHelp()
{
	current_help = مخکینی;
}

PrettyHelp *PrettyHelp::get_current()
{
	إذا (current_help == تش_اشاره)
		جديد PrettyHelp();
	أرجع current_help;
}

فراغ PrettyHelp::log_help() ثابت
{
	لكل (خپلکاری &content : ریښه_listing) {
		إذا (content.type.compare("usage") == 0) {
			log_body(content, 1, ریښتیا);
			لوګاریتم("\n");
		} وإلا إذا (content.type.compare("option") == 0) {
			log_body(content, 1);
			لكل (خپلکاری متن : content) {
				log_body(متن, 2);
				لوګاریتم("\n");
			}
		} وإلا {
			log_body(content, 0);
			لوګاریتم("\n");
		}
	}
}

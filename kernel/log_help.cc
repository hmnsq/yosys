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

std::filesystem::مسیر_2 Yosys::fixup_منبع_مسیر (ثابت حرف* منبع_file,
	ثابت std::filesystem::مسیر_2 &منبع_ریشه, منطقي* skip_منبع_گروه)
{
	std::filesystem::مسیر_2 منبع_مسیر(منبع_file);
	إذا (منبع_مسیر.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		خودکار proximate_مسیر = std::filesystem::proximate(منبع_مسیر, منبع_ریشه);
		إذا (proximate_مسیر == proximate_مسیر.lexically_نرمال()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			أرجع proximate_مسیر;
		} وإلا إذا (skip_منبع_گروه != اشاره_تهی) {
			// don't try to group external paths
			*skip_منبع_گروه = درست;
		}
	}
	أرجع منبع_مسیر;
}

Json ContentListing::مقصد_json(ثابت std::filesystem::مسیر_2 &منبع_ریشه) ثابت {
	Json::object object;

	object["type"] = type;
	إذا (body.درازی())
		object["body"] = body;

	إذا (has_منبع())
		object["source_file"] = fixup_منبع_مسیر(منبع_file, منبع_ریشه).string();
	إذا (منبع_سطر != 0)
		object["source_line"] = منبع_سطر;

	object["options"] = Json(options);

	Json::array content_array;
	لكل (خودکار فرزند : _content)
		content_array.افزودن_پشت(فرزند.مقصد_json(منبع_ریشه));
	object["content"] = content_array;

	أرجع object;
}

فراغ ContentListing::usage(ثابت string &متن,
	ثابت منبع_location location)
{
	log_assert(type.compare("root") == 0);
	جمع_کن_content("usage", متن, location);
}

فراغ ContentListing::option(ثابت string &متن, ثابت string &description,
	ثابت منبع_location location)
{
	خودکار option = open_option(متن, منبع_location());
	إذا (description.درازی())
		option->جمع_کن_content("text", description, location);
}

فراغ ContentListing::codeblock(ثابت string &کد, ثابت string &language,
	ثابت منبع_location location)
{
	جمع_کن_content("code", کد, location);
	پشت()->مجموعه_option("language", language);
}

فراغ ContentListing::paragraph(ثابت string &متن,
	ثابت منبع_location location)
{
	جمع_کن_content("text", متن, location);
}

ContentListing* ContentListing::open_usage(ثابت string &متن,
	ثابت منبع_location location)
{
	usage(متن, location);
	أرجع پشت();
}

ContentListing* ContentListing::open_option(ثابت string &متن,
	ثابت منبع_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	جمع_کن_content("option", متن, location);
	أرجع پشت();
}

#تعریف_کن MAX_سطر_LEN 80
فراغ log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقي leading_newline=خطأ, منطقي is_formatted=خطأ) {
	إذا (pass_str.خالی())
		أرجع;
	std::istringstream iss(pass_str);
	إذا (leading_newline)
		لگاریتم("\n");
	لكل (std::string سطر; std::getline(iss, سطر);) {
		std::string پیام_2 = indent_str;
		إذا (is_formatted) {
			پیام_2 += سطر;
		} وإلا {
			خودکار curr_len = indent_str.درازی();
			std::istringstream lss(سطر);
			لكل (std::string کلمه; std::getline(lss, کلمه, ' ');) {
				طالما (کلمه[0] == '`' && کلمه.پشت() == '`')
					کلمه = کلمه.substr(1, کلمه.درازی()-2);
				إذا (curr_len + کلمه.درازی() >= MAX_سطر_LEN-1) {
					curr_len = 0;
					لگاریتم("%s\n", پیام_2);
					پیام_2 = indent_str;
				}
				إذا (کلمه.درازی()) {
					پیام_2 += کلمه + " ";
					curr_len += کلمه.درازی() + 1;
				}
			}
		}
		لگاریتم("%s\n", پیام_2);
	}
}
فراغ log_body(ثابت ContentListing &content, صحيح indent=0, منطقي leading_newline=خطأ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = اشاره_تهی;

PrettyHelp::PrettyHelp()
{
	پیشین = current_help;
	ریشه_listing = ContentListing("root", "");

	current_help = هذا;
}

PrettyHelp::~PrettyHelp()
{
	current_help = پیشین;
}

PrettyHelp *PrettyHelp::get_current()
{
	إذا (current_help == اشاره_تهی)
		جديد PrettyHelp();
	أرجع current_help;
}

فراغ PrettyHelp::log_help() ثابت
{
	لكل (خودکار &content : ریشه_listing) {
		إذا (content.type.compare("usage") == 0) {
			log_body(content, 1, درست);
			لگاریتم("\n");
		} وإلا إذا (content.type.compare("option") == 0) {
			log_body(content, 1);
			لكل (خودکار متن : content) {
				log_body(متن, 2);
				لگاریتم("\n");
			}
		} وإلا {
			log_body(content, 0);
			لگاریتم("\n");
		}
	}
}

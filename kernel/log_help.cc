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

#شامل "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::راستہ_2 Yosys::fixup_ماخذ_راستہ (ثابت حرف* ماخذ_file,
	ثابت std::filesystem::راستہ_2 &ماخذ_جڑ, منطقی* skip_ماخذ_گروہ)
{
	std::filesystem::راستہ_2 ماخذ_راستہ(ماخذ_file);
	اگر (ماخذ_راستہ.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		خودکار proximate_راستہ = std::filesystem::proximate(ماخذ_راستہ, ماخذ_جڑ);
		اگر (proximate_راستہ == proximate_راستہ.lexically_معمول()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			واپس proximate_راستہ;
		} ورنہ اگر (skip_ماخذ_گروہ != خالی_اشاریہ) {
			// don't try to group external paths
			*skip_ماخذ_گروہ = سچ;
		}
	}
	واپس ماخذ_راستہ;
}

Json ContentListing::مقصد_json(ثابت std::filesystem::راستہ_2 &ماخذ_جڑ) ثابت {
	Json::object object;

	object["type"] = type;
	اگر (body.لمبائی_2())
		object["body"] = body;

	اگر (has_ماخذ())
		object["source_file"] = fixup_ماخذ_راستہ(ماخذ_file, ماخذ_جڑ).string();
	اگر (ماخذ_سطر != 0)
		object["source_line"] = ماخذ_سطر;

	object["options"] = Json(options);

	Json::array content_array;
	برائے (خودکار بچہ : _content)
		content_array.ڈالو_عقب(بچہ.مقصد_json(ماخذ_جڑ));
	object["content"] = content_array;

	واپس object;
}

خالی ContentListing::usage(ثابت string &متن,
	ثابت ماخذ_location location)
{
	log_assert(type.compare("root") == 0);
	جوڑو_content("usage", متن, location);
}

خالی ContentListing::option(ثابت string &متن, ثابت string &description,
	ثابت ماخذ_location location)
{
	خودکار option = open_option(متن, ماخذ_location());
	اگر (description.لمبائی_2())
		option->جوڑو_content("text", description, location);
}

خالی ContentListing::codeblock(ثابت string &رمز, ثابت string &language,
	ثابت ماخذ_location location)
{
	جوڑو_content("code", رمز, location);
	عقب()->سیٹ_option("language", language);
}

خالی ContentListing::paragraph(ثابت string &متن,
	ثابت ماخذ_location location)
{
	جوڑو_content("text", متن, location);
}

ContentListing* ContentListing::open_usage(ثابت string &متن,
	ثابت ماخذ_location location)
{
	usage(متن, location);
	واپس عقب();
}

ContentListing* ContentListing::open_option(ثابت string &متن,
	ثابت ماخذ_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	جوڑو_content("option", متن, location);
	واپس عقب();
}

#تعریف_کرو MAX_سطر_LEN 80
خالی log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقی leading_newline=جھوٹ, منطقی is_formatted=جھوٹ) {
	اگر (pass_str.خالی_خانے())
		واپس;
	std::istringstream iss(pass_str);
	اگر (leading_newline)
		لوگارتھم("\n");
	برائے (std::string سطر; std::getline(iss, سطر);) {
		std::string پیغام_2 = indent_str;
		اگر (is_formatted) {
			پیغام_2 += سطر;
		} ورنہ {
			خودکار curr_len = indent_str.لمبائی_2();
			std::istringstream lss(سطر);
			برائے (std::string لفظ; std::getline(lss, لفظ, ' ');) {
				جب (لفظ[0] == '`' && لفظ.عقب() == '`')
					لفظ = لفظ.substr(1, لفظ.لمبائی_2()-2);
				اگر (curr_len + لفظ.لمبائی_2() >= MAX_سطر_LEN-1) {
					curr_len = 0;
					لوگارتھم("%s\n", پیغام_2);
					پیغام_2 = indent_str;
				}
				اگر (لفظ.لمبائی_2()) {
					پیغام_2 += لفظ + " ";
					curr_len += لفظ.لمبائی_2() + 1;
				}
			}
		}
		لوگارتھم("%s\n", پیغام_2);
	}
}
خالی log_body(ثابت ContentListing &content, صحیح indent=0, منطقی leading_newline=جھوٹ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = خالی_اشاریہ;

PrettyHelp::PrettyHelp()
{
	پیشگی = current_help;
	جڑ_listing = ContentListing("root", "");

	current_help = یہ;
}

PrettyHelp::~PrettyHelp()
{
	current_help = پیشگی;
}

PrettyHelp *PrettyHelp::get_current()
{
	اگر (current_help == خالی_اشاریہ)
		نیا PrettyHelp();
	واپس current_help;
}

خالی PrettyHelp::log_help() ثابت
{
	برائے (خودکار &content : جڑ_listing) {
		اگر (content.type.compare("usage") == 0) {
			log_body(content, 1, سچ);
			لوگارتھم("\n");
		} ورنہ اگر (content.type.compare("option") == 0) {
			log_body(content, 1);
			برائے (خودکار متن : content) {
				log_body(متن, 2);
				لوگارتھم("\n");
			}
		} ورنہ {
			log_body(content, 0);
			لوگارتھم("\n");
		}
	}
}

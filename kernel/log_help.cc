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

std::filesystem::رستہ_2 Yosys::fixup_سروت_رستہ (ثابت حرف* سروت_file,
	ثابت std::filesystem::رستہ_2 &سروت_جڑ, منطقي* skip_سروت_گروہ)
{
	std::filesystem::رستہ_2 سروت_رستہ(سروت_file);
	إذا (سروت_رستہ.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		خودکار proximate_رستہ = std::filesystem::proximate(سروت_رستہ, سروت_جڑ);
		إذا (proximate_رستہ == proximate_رستہ.lexically_عام()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			أرجع proximate_رستہ;
		} وإلا إذا (skip_سروت_گروہ != خالی_سوچک) {
			// don't try to group external paths
			*skip_سروت_گروہ = سچ;
		}
	}
	أرجع سروت_رستہ;
}

Json ContentListing::منزل_json(ثابت std::filesystem::رستہ_2 &سروت_جڑ) ثابت {
	Json::object object;

	object["type"] = type;
	إذا (body.لمائی())
		object["body"] = body;

	إذا (has_سروت())
		object["source_file"] = fixup_سروت_رستہ(سروت_file, سروت_جڑ).string();
	إذا (سروت_سطر != 0)
		object["source_line"] = سروت_سطر;

	object["options"] = Json(options);

	Json::array content_array;
	لكل (خودکار بچہ : _content)
		content_array.ڈالو_پٹھ(بچہ.منزل_json(سروت_جڑ));
	object["content"] = content_array;

	أرجع object;
}

فراغ ContentListing::usage(ثابت string &متن,
	ثابت سروت_location location)
{
	log_assert(type.compare("root") == 0);
	جوڑو_content("usage", متن, location);
}

فراغ ContentListing::option(ثابت string &متن, ثابت string &description,
	ثابت سروت_location location)
{
	خودکار option = open_option(متن, سروت_location());
	إذا (description.لمائی())
		option->جوڑو_content("text", description, location);
}

فراغ ContentListing::codeblock(ثابت string &کوڈ, ثابت string &language,
	ثابت سروت_location location)
{
	جوڑو_content("code", کوڈ, location);
	پٹھ()->سیٹ_option("language", language);
}

فراغ ContentListing::paragraph(ثابت string &متن,
	ثابت سروت_location location)
{
	جوڑو_content("text", متن, location);
}

ContentListing* ContentListing::open_usage(ثابت string &متن,
	ثابت سروت_location location)
{
	usage(متن, location);
	أرجع پٹھ();
}

ContentListing* ContentListing::open_option(ثابت string &متن,
	ثابت سروت_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	جوڑو_content("option", متن, location);
	أرجع پٹھ();
}

#تعریف_کرو MAX_سطر_LEN 80
فراغ log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقي leading_newline=خطأ, منطقي is_formatted=خطأ) {
	إذا (pass_str.خالی_تھاواں())
		أرجع;
	std::istringstream iss(pass_str);
	إذا (leading_newline)
		لوگارتھم("\n");
	لكل (std::string سطر; std::getline(iss, سطر);) {
		std::string سنیہا = indent_str;
		إذا (is_formatted) {
			سنیہا += سطر;
		} وإلا {
			خودکار curr_len = indent_str.لمائی();
			std::istringstream lss(سطر);
			لكل (std::string لفظ; std::getline(lss, لفظ, ' ');) {
				طالما (لفظ[0] == '`' && لفظ.پٹھ() == '`')
					لفظ = لفظ.substr(1, لفظ.لمائی()-2);
				إذا (curr_len + لفظ.لمائی() >= MAX_سطر_LEN-1) {
					curr_len = 0;
					لوگارتھم("%s\n", سنیہا);
					سنیہا = indent_str;
				}
				إذا (لفظ.لمائی()) {
					سنیہا += لفظ + " ";
					curr_len += لفظ.لمائی() + 1;
				}
			}
		}
		لوگارتھم("%s\n", سنیہا);
	}
}
فراغ log_body(ثابت ContentListing &content, صحيح indent=0, منطقي leading_newline=خطأ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = خالی_سوچک;

PrettyHelp::PrettyHelp()
{
	پہلوں = current_help;
	جڑ_listing = ContentListing("root", "");

	current_help = هذا;
}

PrettyHelp::~PrettyHelp()
{
	current_help = پہلوں;
}

PrettyHelp *PrettyHelp::get_current()
{
	إذا (current_help == خالی_سوچک)
		جديد PrettyHelp();
	أرجع current_help;
}

فراغ PrettyHelp::log_help() ثابت
{
	لكل (خودکار &content : جڑ_listing) {
		إذا (content.type.compare("usage") == 0) {
			log_body(content, 1, سچ);
			لوگارتھم("\n");
		} وإلا إذا (content.type.compare("option") == 0) {
			log_body(content, 1);
			لكل (خودکار متن : content) {
				log_body(متن, 2);
				لوگارتھم("\n");
			}
		} وإلا {
			log_body(content, 0);
			لوگارتھم("\n");
		}
	}
}

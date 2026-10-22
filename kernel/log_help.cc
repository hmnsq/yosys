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

std::filesystem::رستو_2 Yosys::fixup_ذريعو_رستو (ثابت حرف* ذريعو_file,
	ثابت std::filesystem::رستو_2 &ذريعو_پاڙ, منطقي* skip_ذريعو_گروهه)
{
	std::filesystem::رستو_2 ذريعو_رستو(ذريعو_file);
	إذا (ذريعو_رستو.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		خودڪار proximate_رستو = std::filesystem::proximate(ذريعو_رستو, ذريعو_پاڙ);
		إذا (proximate_رستو == proximate_رستو.lexically_عام()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			أرجع proximate_رستو;
		} وإلا إذا (skip_ذريعو_گروهه != خالي_اشارو) {
			// don't try to group external paths
			*skip_ذريعو_گروهه = سچ;
		}
	}
	أرجع ذريعو_رستو;
}

Json ContentListing::منزل_json(ثابت std::filesystem::رستو_2 &ذريعو_پاڙ) ثابت {
	Json::object object;

	object["type"] = type;
	إذا (body.ڊيگهائي())
		object["body"] = body;

	إذا (has_ذريعو())
		object["source_file"] = fixup_ذريعو_رستو(ذريعو_file, ذريعو_پاڙ).string();
	إذا (ذريعو_سٽ != 0)
		object["source_line"] = ذريعو_سٽ;

	object["options"] = Json(options);

	Json::array content_array;
	لكل (خودڪار ٻار : _content)
		content_array.ڌڪو_پٺ(ٻار.منزل_json(ذريعو_پاڙ));
	object["content"] = content_array;

	أرجع object;
}

فراغ ContentListing::usage(ثابت string &متن,
	ثابت ذريعو_location location)
{
	log_assert(type.compare("root") == 0);
	جوڙيو_content("usage", متن, location);
}

فراغ ContentListing::option(ثابت string &متن, ثابت string &description,
	ثابت ذريعو_location location)
{
	خودڪار option = open_option(متن, ذريعو_location());
	إذا (description.ڊيگهائي())
		option->جوڙيو_content("text", description, location);
}

فراغ ContentListing::codeblock(ثابت string &ڪوڊ, ثابت string &language,
	ثابت ذريعو_location location)
{
	جوڙيو_content("code", ڪوڊ, location);
	پٺ()->سيٽ_option("language", language);
}

فراغ ContentListing::paragraph(ثابت string &متن,
	ثابت ذريعو_location location)
{
	جوڙيو_content("text", متن, location);
}

ContentListing* ContentListing::open_usage(ثابت string &متن,
	ثابت ذريعو_location location)
{
	usage(متن, location);
	أرجع پٺ();
}

ContentListing* ContentListing::open_option(ثابت string &متن,
	ثابت ذريعو_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	جوڙيو_content("option", متن, location);
	أرجع پٺ();
}

#وصف_ڪريو MAX_سٽ_LEN 80
فراغ log_body_str(ثابت std::string &pass_str, std::string indent_str, منطقي leading_newline=خطأ, منطقي is_formatted=خطأ) {
	إذا (pass_str.خالي_جايون())
		أرجع;
	std::istringstream iss(pass_str);
	إذا (leading_newline)
		لاگرٿم("\n");
	لكل (std::string سٽ; std::getline(iss, سٽ);) {
		std::string پيغام = indent_str;
		إذا (is_formatted) {
			پيغام += سٽ;
		} وإلا {
			خودڪار curr_len = indent_str.ڊيگهائي();
			std::istringstream lss(سٽ);
			لكل (std::string لفظ; std::getline(lss, لفظ, ' ');) {
				طالما (لفظ[0] == '`' && لفظ.پٺ() == '`')
					لفظ = لفظ.substr(1, لفظ.ڊيگهائي()-2);
				إذا (curr_len + لفظ.ڊيگهائي() >= MAX_سٽ_LEN-1) {
					curr_len = 0;
					لاگرٿم("%s\n", پيغام);
					پيغام = indent_str;
				}
				إذا (لفظ.ڊيگهائي()) {
					پيغام += لفظ + " ";
					curr_len += لفظ.ڊيگهائي() + 1;
				}
			}
		}
		لاگرٿم("%s\n", پيغام);
	}
}
فراغ log_body(ثابت ContentListing &content, صحيح indent=0, منطقي leading_newline=خطأ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = خالي_اشارو;

PrettyHelp::PrettyHelp()
{
	اڳواٽ = current_help;
	پاڙ_listing = ContentListing("root", "");

	current_help = هذا;
}

PrettyHelp::~PrettyHelp()
{
	current_help = اڳواٽ;
}

PrettyHelp *PrettyHelp::get_current()
{
	إذا (current_help == خالي_اشارو)
		جديد PrettyHelp();
	أرجع current_help;
}

فراغ PrettyHelp::log_help() ثابت
{
	لكل (خودڪار &content : پاڙ_listing) {
		إذا (content.type.compare("usage") == 0) {
			log_body(content, 1, سچ);
			لاگرٿم("\n");
		} وإلا إذا (content.type.compare("option") == 0) {
			log_body(content, 1);
			لكل (خودڪار متن : content) {
				log_body(متن, 2);
				لاگرٿم("\n");
			}
		} وإلا {
			log_body(content, 0);
			لاگرٿم("\n");
		}
	}
}

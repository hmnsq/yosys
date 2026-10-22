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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::ਰਸਤਾ_2 Yosys::fixup_ਸਰੋਤ_ਰਸਤਾ (ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_file,
	ਅਚਲ std::filesystem::ਰਸਤਾ_2 &ਸਰੋਤ_ਜੜ੍ਹ, ਤਾਰਕਿਕ* skip_ਸਰੋਤ_ਸਮੂਹ)
{
	std::filesystem::ਰਸਤਾ_2 ਸਰੋਤ_ਰਸਤਾ(ਸਰੋਤ_file);
	ਜੇ (ਸਰੋਤ_ਰਸਤਾ.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		ਸਵੈਚਾਲਿਤ proximate_ਰਸਤਾ = std::filesystem::proximate(ਸਰੋਤ_ਰਸਤਾ, ਸਰੋਤ_ਜੜ੍ਹ);
		ਜੇ (proximate_ਰਸਤਾ == proximate_ਰਸਤਾ.lexically_ਸਾਧਾਰਨ()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			ਵਾਪਸ proximate_ਰਸਤਾ;
		} ਨਹੀਂਤਾਂ ਜੇ (skip_ਸਰੋਤ_ਸਮੂਹ != ਖਾਲੀ_ਸੂਚਕ) {
			// don't try to group external paths
			*skip_ਸਰੋਤ_ਸਮੂਹ = ਸੱਚ;
		}
	}
	ਵਾਪਸ ਸਰੋਤ_ਰਸਤਾ;
}

Json ContentListing::ਮੰਜ਼ਿਲ_json(ਅਚਲ std::filesystem::ਰਸਤਾ_2 &ਸਰੋਤ_ਜੜ੍ਹ) ਅਚਲ {
	Json::object object;

	object["type"] = type;
	ਜੇ (body.ਲੰਮਾਈ())
		object["body"] = body;

	ਜੇ (has_ਸਰੋਤ())
		object["source_file"] = fixup_ਸਰੋਤ_ਰਸਤਾ(ਸਰੋਤ_file, ਸਰੋਤ_ਜੜ੍ਹ).string();
	ਜੇ (ਸਰੋਤ_ਸਤਰ != 0)
		object["source_line"] = ਸਰੋਤ_ਸਤਰ;

	object["options"] = Json(options);

	Json::array content_array;
	ਲਈ (ਸਵੈਚਾਲਿਤ ਬੱਚਾ : _content)
		content_array.ਧੱਕੋ_ਪਿੱਠ(ਬੱਚਾ.ਮੰਜ਼ਿਲ_json(ਸਰੋਤ_ਜੜ੍ਹ));
	object["content"] = content_array;

	ਵਾਪਸ object;
}

ਖਾਲੀ ContentListing::usage(ਅਚਲ string &ਪਾਠ,
	ਅਚਲ ਸਰੋਤ_location location)
{
	log_assert(type.compare("root") == 0);
	ਜੋੜੋ_content("usage", ਪਾਠ, location);
}

ਖਾਲੀ ContentListing::option(ਅਚਲ string &ਪਾਠ, ਅਚਲ string &description,
	ਅਚਲ ਸਰੋਤ_location location)
{
	ਸਵੈਚਾਲਿਤ option = open_option(ਪਾਠ, ਸਰੋਤ_location());
	ਜੇ (description.ਲੰਮਾਈ())
		option->ਜੋੜੋ_content("text", description, location);
}

ਖਾਲੀ ContentListing::codeblock(ਅਚਲ string &ਕੋਡ, ਅਚਲ string &language,
	ਅਚਲ ਸਰੋਤ_location location)
{
	ਜੋੜੋ_content("code", ਕੋਡ, location);
	ਪਿੱਠ()->ਸੈੱਟ_option("language", language);
}

ਖਾਲੀ ContentListing::paragraph(ਅਚਲ string &ਪਾਠ,
	ਅਚਲ ਸਰੋਤ_location location)
{
	ਜੋੜੋ_content("text", ਪਾਠ, location);
}

ContentListing* ContentListing::open_usage(ਅਚਲ string &ਪਾਠ,
	ਅਚਲ ਸਰੋਤ_location location)
{
	usage(ਪਾਠ, location);
	ਵਾਪਸ ਪਿੱਠ();
}

ContentListing* ContentListing::open_option(ਅਚਲ string &ਪਾਠ,
	ਅਚਲ ਸਰੋਤ_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	ਜੋੜੋ_content("option", ਪਾਠ, location);
	ਵਾਪਸ ਪਿੱਠ();
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਸਤਰ_LEN 80
ਖਾਲੀ log_body_str(ਅਚਲ std::string &pass_str, std::string indent_str, ਤਾਰਕਿਕ leading_newline=ਝੂਠ, ਤਾਰਕਿਕ is_formatted=ਝੂਠ) {
	ਜੇ (pass_str.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ;
	std::istringstream iss(pass_str);
	ਜੇ (leading_newline)
		ਲਘੂਗਣਕ("\n");
	ਲਈ (std::string ਸਤਰ; std::getline(iss, ਸਤਰ);) {
		std::string ਸੁਨੇਹਾ_2 = indent_str;
		ਜੇ (is_formatted) {
			ਸੁਨੇਹਾ_2 += ਸਤਰ;
		} ਨਹੀਂਤਾਂ {
			ਸਵੈਚਾਲਿਤ curr_len = indent_str.ਲੰਮਾਈ();
			std::istringstream lss(ਸਤਰ);
			ਲਈ (std::string ਸ਼ਬਦ; std::getline(lss, ਸ਼ਬਦ, ' ');) {
				ਜਦੋਂ (ਸ਼ਬਦ[0] == '`' && ਸ਼ਬਦ.ਪਿੱਠ() == '`')
					ਸ਼ਬਦ = ਸ਼ਬਦ.substr(1, ਸ਼ਬਦ.ਲੰਮਾਈ()-2);
				ਜੇ (curr_len + ਸ਼ਬਦ.ਲੰਮਾਈ() >= MAX_ਸਤਰ_LEN-1) {
					curr_len = 0;
					ਲਘੂਗਣਕ("%s\n", ਸੁਨੇਹਾ_2);
					ਸੁਨੇਹਾ_2 = indent_str;
				}
				ਜੇ (ਸ਼ਬਦ.ਲੰਮਾਈ()) {
					ਸੁਨੇਹਾ_2 += ਸ਼ਬਦ + " ";
					curr_len += ਸ਼ਬਦ.ਲੰਮਾਈ() + 1;
				}
			}
		}
		ਲਘੂਗਣਕ("%s\n", ਸੁਨੇਹਾ_2);
	}
}
ਖਾਲੀ log_body(ਅਚਲ ContentListing &content, ਪੂਰਨ indent=0, ਤਾਰਕਿਕ leading_newline=ਝੂਠ) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = ਖਾਲੀ_ਸੂਚਕ;

PrettyHelp::PrettyHelp()
{
	ਪੂਰਵ = current_help;
	ਜੜ੍ਹ_listing = ContentListing("root", "");

	current_help = ਇਹ;
}

PrettyHelp::~PrettyHelp()
{
	current_help = ਪੂਰਵ;
}

PrettyHelp *PrettyHelp::get_current()
{
	ਜੇ (current_help == ਖਾਲੀ_ਸੂਚਕ)
		ਨਵਾਂ PrettyHelp();
	ਵਾਪਸ current_help;
}

ਖਾਲੀ PrettyHelp::log_help() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &content : ਜੜ੍ਹ_listing) {
		ਜੇ (content.type.compare("usage") == 0) {
			log_body(content, 1, ਸੱਚ);
			ਲਘੂਗਣਕ("\n");
		} ਨਹੀਂਤਾਂ ਜੇ (content.type.compare("option") == 0) {
			log_body(content, 1);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਪਾਠ : content) {
				log_body(ਪਾਠ, 2);
				ਲਘੂਗਣਕ("\n");
			}
		} ਨਹੀਂਤਾਂ {
			log_body(content, 0);
			ਲਘੂਗਣਕ("\n");
		}
	}
}

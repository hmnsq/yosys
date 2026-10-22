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

#అన్తర్భావ "kernel/log_help.h"

USING_YOSYS_NAMESPACE

std::filesystem::మార్గం_2 Yosys::fixup_ఆధారం_మార్గం (స్థిరరాశి అక్షరం* ఆధారం_file,
	స్థిరరాశి std::filesystem::మార్గం_2 &ఆధారం_మూలం, తార్కిక* skip_ఆధారం_సమూహం)
{
	std::filesystem::మార్గం_2 ఆధారం_మార్గం(ఆధారం_file);
	ఒకవేళ (ఆధారం_మార్గం.is_absolute()) {
		// using proximate instead of relative means that we
		// still get the source path if they aren't relative
		స్వయంచాలక proximate_మార్గం = std::filesystem::proximate(ఆధారం_మార్గం, ఆధారం_మూలం);
		ఒకవేళ (proximate_మార్గం == proximate_మార్గం.lexically_సాధారణ()) {
			// we're only interested if it's a subpath of our root dir
			// if the normal form differs, then the proximate path includes ".."
			తిరిగి proximate_మార్గం;
		} లేకపోతే ఒకవేళ (skip_ఆధారం_సమూహం != శూన్యసూచిక) {
			// don't try to group external paths
			*skip_ఆధారం_సమూహం = నిజం;
		}
	}
	తిరిగి ఆధారం_మార్గం;
}

Json ContentListing::గమ్యం_json(స్థిరరాశి std::filesystem::మార్గం_2 &ఆధారం_మూలం) స్థిరరాశి {
	Json::object object;

	object["type"] = type;
	ఒకవేళ (body.నిడివి())
		object["body"] = body;

	ఒకవేళ (has_ఆధారం())
		object["source_file"] = fixup_ఆధారం_మార్గం(ఆధారం_file, ఆధారం_మూలం).string();
	ఒకవేళ (ఆధారం_పంక్తి != 0)
		object["source_line"] = ఆధారం_పంక్తి;

	object["options"] = Json(options);

	Json::array content_array;
	కొరకు (స్వయంచాలక బిడ్డ : _content)
		content_array.నెట్టు_వెనుక(బిడ్డ.గమ్యం_json(ఆధారం_మూలం));
	object["content"] = content_array;

	తిరిగి object;
}

శూన్య ContentListing::usage(స్థిరరాశి string &పాఠ్యం,
	స్థిరరాశి ఆధారం_location location)
{
	log_assert(type.compare("root") == 0);
	కూడు_content("usage", పాఠ్యం, location);
}

శూన్య ContentListing::option(స్థిరరాశి string &పాఠ్యం, స్థిరరాశి string &description,
	స్థిరరాశి ఆధారం_location location)
{
	స్వయంచాలక option = open_option(పాఠ్యం, ఆధారం_location());
	ఒకవేళ (description.నిడివి())
		option->కూడు_content("text", description, location);
}

శూన్య ContentListing::codeblock(స్థిరరాశి string &సంకేతం_2, స్థిరరాశి string &language,
	స్థిరరాశి ఆధారం_location location)
{
	కూడు_content("code", సంకేతం_2, location);
	వెనుక()->సమితి_option("language", language);
}

శూన్య ContentListing::paragraph(స్థిరరాశి string &పాఠ్యం,
	స్థిరరాశి ఆధారం_location location)
{
	కూడు_content("text", పాఠ్యం, location);
}

ContentListing* ContentListing::open_usage(స్థిరరాశి string &పాఠ్యం,
	స్థిరరాశి ఆధారం_location location)
{
	usage(పాఠ్యం, location);
	తిరిగి వెనుక();
}

ContentListing* ContentListing::open_option(స్థిరరాశి string &పాఠ్యం,
	స్థిరరాశి ఆధారం_location location)
{
	log_assert(type.compare("root") == 0 || type.compare("usage") == 0);
	కూడు_content("option", పాఠ్యం, location);
	తిరిగి వెనుక();
}

#నిర్వచించు MAX_పంక్తి_LEN 80
శూన్య log_body_str(స్థిరరాశి std::string &pass_str, std::string indent_str, తార్కిక leading_newline=అబద్ధం, తార్కిక is_formatted=అబద్ధం) {
	ఒకవేళ (pass_str.ఖాళీలు())
		తిరిగి;
	std::istringstream iss(pass_str);
	ఒకవేళ (leading_newline)
		సంవర్గమానం("\n");
	కొరకు (std::string పంక్తి; std::getline(iss, పంక్తి);) {
		std::string సందేశం_2 = indent_str;
		ఒకవేళ (is_formatted) {
			సందేశం_2 += పంక్తి;
		} లేకపోతే {
			స్వయంచాలక curr_len = indent_str.నిడివి();
			std::istringstream lss(పంక్తి);
			కొరకు (std::string మాట; std::getline(lss, మాట, ' ');) {
				అయితే (మాట[0] == '`' && మాట.వెనుక() == '`')
					మాట = మాట.substr(1, మాట.నిడివి()-2);
				ఒకవేళ (curr_len + మాట.నిడివి() >= MAX_పంక్తి_LEN-1) {
					curr_len = 0;
					సంవర్గమానం("%s\n", సందేశం_2);
					సందేశం_2 = indent_str;
				}
				ఒకవేళ (మాట.నిడివి()) {
					సందేశం_2 += మాట + " ";
					curr_len += మాట.నిడివి() + 1;
				}
			}
		}
		సంవర్గమానం("%s\n", సందేశం_2);
	}
}
శూన్య log_body(స్థిరరాశి ContentListing &content, పూర్ణాంకం indent=0, తార్కిక leading_newline=అబద్ధం) {
	std::string indent_str(indent*4, ' ');
	log_body_str(content.body, indent_str, leading_newline, content.type.compare("code") == 0);
}

PrettyHelp *current_help = శూన్యసూచిక;

PrettyHelp::PrettyHelp()
{
	పూర్వ = current_help;
	మూలం_listing = ContentListing("root", "");

	current_help = ఇది;
}

PrettyHelp::~PrettyHelp()
{
	current_help = పూర్వ;
}

PrettyHelp *PrettyHelp::get_current()
{
	ఒకవేళ (current_help == శూన్యసూచిక)
		కొత్త PrettyHelp();
	తిరిగి current_help;
}

శూన్య PrettyHelp::log_help() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &content : మూలం_listing) {
		ఒకవేళ (content.type.compare("usage") == 0) {
			log_body(content, 1, నిజం);
			సంవర్గమానం("\n");
		} లేకపోతే ఒకవేళ (content.type.compare("option") == 0) {
			log_body(content, 1);
			కొరకు (స్వయంచాలక పాఠ్యం : content) {
				log_body(పాఠ్యం, 2);
				సంవర్గమానం("\n");
			}
		} లేకపోతే {
			log_body(content, 0);
			సంవర్గమానం("\n");
		}
	}
}

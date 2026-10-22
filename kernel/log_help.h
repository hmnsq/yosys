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

#నిర్వచితం_కాకపోతే LOG_HELP_H
#నిర్వచించు LOG_HELP_H

#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/json.h"
#అన్తర్భావ <filesystem>
#అన్తర్భావ <string_view>

YOSYS_NAMESPACE_BEGIN

std::filesystem::మార్గం_2 fixup_ఆధారం_మార్గం (స్థిరరాశి అక్షరం* ఆధారం_file,
	స్థిరరాశి std::filesystem::మార్గం_2 &ఆధారం_మూలం, bool* skip_ఆధారం_సమూహం = nullptr);

class ContentListing {
	సదిశ<ContentListing> _content;
బహిరంగ:
	string type;
	string body;
	స్థిరరాశి అక్షరం* ఆధారం_file;
	పూర్ణాంకం ఆధారం_పంక్తి;
	std::map<string, string> options;

	ContentListing(
		string type = "root", string body = "",
		స్థిరరాశి అక్షరం* ఆధారం_file = "unknown", పూర్ణాంకం ఆధారం_పంక్తి = 0
	) : type(type), body(body), ఆధారం_file(ఆధారం_file), ఆధారం_పంక్తి(ఆధారం_పంక్తి) {
		_content = {};
		options = {};
	}

	ContentListing(string type, string body, ఆధారం_location location) :
		ContentListing(type, body, location.file_name(), location.పంక్తి()) { }

	శూన్య కూడు_content(string type, string body, ఆధారం_location location) {
		_content.నెట్టు_వెనుక({type, body, location});
	}

	bool has_ఆధారం() స్థిరరాశి { తిరిగి అక్షరశ్రేణి_పొడవు(ఆధారం_file) && strcmp(ఆధారం_file, "unknown") != 0; }
	bool has_content() స్థిరరాశి { తిరిగి _content.కొలత() != 0; }

	సదిశ<ContentListing>::const_iterator begin() స్థిరరాశి { తిరిగి _content.cbegin(); }
	సదిశ<ContentListing>::const_iterator చివర() స్థిరరాశి { తిరిగి _content.cend(); }

	ContentListing* వెనుక() {
		తిరిగి &_content.వెనుక();
	}

	శూన్య సమితి_option(string కీలకం, string val = "") {
		options[కీలకం] = val;
	}

	శూన్య usage(
		స్థిరరాశి string &పాఠ్యం,
		స్థిరరాశి ఆధారం_location location = ఆధారం_location::current()
	);
	శూన్య option(
		స్థిరరాశి string &పాఠ్యం,
		స్థిరరాశి string &description = "",
		స్థిరరాశి ఆధారం_location location = ఆధారం_location::current()
	);
	శూన్య codeblock(
		స్థిరరాశి string &సంకేతం_2,
		స్థిరరాశి string &language = "none",
		స్థిరరాశి ఆధారం_location location = ఆధారం_location::current()
	);
	శూన్య paragraph(
		స్థిరరాశి string &పాఠ్యం,
		స్థిరరాశి ఆధారం_location location = ఆధారం_location::current()
	);

	ContentListing* open_usage(
		స్థిరరాశి string &పాఠ్యం,
		స్థిరరాశి ఆధారం_location location = ఆధారం_location::current()
	);
	ContentListing* open_option(
		స్థిరరాశి string &పాఠ్యం,
		స్థిరరాశి ఆధారం_location location = ఆధారం_location::current()
	);

	Json గమ్యం_json(స్థిరరాశి std::filesystem::మార్గం_2 &ఆధారం_మూలం) స్థిరరాశి;
};

class PrettyHelp
{
బహిరంగ:
	string సమూహం = "unknown";

అంతరంగ:
	PrettyHelp *పూర్వ;
	ContentListing మూలం_listing;

బహిరంగ:
	PrettyHelp();
	~PrettyHelp();

	స్థిర PrettyHelp *get_current();

	bool has_content() స్థిరరాశి { తిరిగి మూలం_listing.has_content(); }

	సదిశ<ContentListing>::const_iterator begin() స్థిరరాశి { తిరిగి మూలం_listing.begin(); }
	సదిశ<ContentListing>::const_iterator చివర() స్థిరరాశి { తిరిగి మూలం_listing.చివర(); }

	ContentListing* get_మూలం() {
		తిరిగి &మూలం_listing;
	}

	శూన్య సమితి_సమూహం(స్థిరరాశి string g) { సమూహం = g; }
	bool has_సమూహం() స్థిరరాశి { తిరిగి సమూహం.compare("unknown") != 0; }

	శూన్య log_help() స్థిరరాశి;
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు

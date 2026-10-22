/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2022  Jannis Harder <jix@yosyshq.com> <me@jix.one>
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

#నిర్వచితం_కాకపోతే JSON_H
#నిర్వచించు JSON_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "libs/json11/json11.hpp"
#అన్తర్భావ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    గణన Scope {
        విలువ_2,
        OBJECT_మొదటి,
        OBJECT,
        ARRAY_మొదటి,
        ARRAY,
    };

    నిర్మాణం లక్ష్యం {
        మాయా శూన్య emit(స్థిరరాశి అక్షరం *దత్తాంశం) = 0;
        మాయా శూన్య flush() {};
        మాయా ~లక్ష్యం() {};
    };

    std::string newline_indent = "\n";
    std::సదిశ<std::unique_ptr<లక్ష్యం>> targets;
    std::సదిశ<Scope> state = {విలువ_2};
    పూర్ణాంకం compact_లోతు = గరిష్ఠ_పూర్ణాంకం;
బహిరంగ:

    శూన్య emit_గమ్యం_log();
    శూన్య append_గమ్యం_string(std::string &లక్ష్యం_2);
    bool write_గమ్యం_file(స్థిరరాశి std::string &మార్గం_2);

    bool చురుకైన() { తిరిగి !targets.ఖాళీలు(); }

    శూన్య compact() { compact_లోతు = Get_కొలత(state); }

    శూన్య పంక్తి(bool space_if_inline = true);
    శూన్య raw(స్థిరరాశి అక్షరం *raw_json);
    శూన్య flush();
    శూన్య begin_object();
    శూన్య begin_array();
    శూన్య చివర_object();
    శూన్య చివర_array();
    శూన్య name(స్థిరరాశి అక్షరం *name);
    శూన్య begin_విలువ();
    శూన్య చివర_విలువ();
    శూన్య విలువ_json(స్థిరరాశి Json &విలువ);
    శూన్య విలువ(గుర్తులేని పూర్ణాంకం విలువ) { విలువ_json(Json((పూర్ణాంకం)విలువ)); }
    మూస<typename T>
    శూన్య విలువ(T &&విలువ) { విలువ_json(Json(std::forward<T>(విలువ))); };

    శూన్య నమోదు_json(స్థిరరాశి అక్షరం *name, స్థిరరాశి Json &విలువ);
    శూన్య నమోదు_2(స్థిరరాశి అక్షరం *name, గుర్తులేని పూర్ణాంకం విలువ) { నమోదు_json(name, Json((పూర్ణాంకం)విలువ)); }
    మూస<typename T>
    శూన్య నమోదు_2(స్థిరరాశి అక్షరం *name, T &&విలువ) { నమోదు_json(name, Json(std::forward<T>(విలువ))); };

    మూస<typename T>
    శూన్య object(స్థిరరాశి T &&విలువలు)
    {
        begin_object();
        కొరకు (స్వయంచాలక &అంశం : విలువలు)
            నమోదు_2(అంశం.మొదటి, అంశం.రెండవ);
        చివర_object();
    }

    మూస<typename T>
    శూన్య array(స్థిరరాశి T &&విలువలు)
    {
        begin_array();
        కొరకు (స్వయంచాలక &అంశం : విలువలు)
            విలువ(అంశం);
        చివర_array();
    }
};



YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు

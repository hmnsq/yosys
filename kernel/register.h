/* -*- c++ -*-
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#నిర్వచితం_కాకపోతే రిజిస్టరు_H
#నిర్వచించు రిజిస్టరు_H

#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	స్థిర bool is_enabled_;
బహిరంగ:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	స్థిర bool is_enabled() { తిరిగి is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
శూన్య request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
శూన్య try_collect_garbage();

నిర్మాణం Pass
{
	std::string pass_name, short_help;
	ఆధారం_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		ఆధారం_location location = ఆధారం_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	మాయా ~Pass();

	// Makes calls to log() to generate help message
	మాయా శూన్య help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	మాయా bool formatted_help();
	మాయా శూన్య clear_flags();
	మాయా శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) = 0;

	పూర్ణాంకం call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool అంతర్గత_flag = false;

	స్థిర శూన్య వ్యవకలనం_from_current_runtime_ns(int64_t సమయం_ns);

	శూన్య experimental() {
		experimental_flag = true;
	}

	శూన్య అంతర్గత() {
		అంతర్గత_flag = true;
	}

	నిర్మాణం pre_post_exec_state_t {
		Pass *మాతృ_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	శూన్య post_execute(pre_post_exec_state_t state);

	శూన్య cmd_log_వాదనలు(స్థిరరాశి std::సదిశ<std::string> &వాదనలు_2);
	శూన్య cmd_error(స్థిరరాశి std::సదిశ<std::string> &వాదనలు_2, కొలత_t argidx, std::string సందేశం);
	శూన్య extra_వాదనలు(std::సదిశ<std::string> వాదనలు_2, కొలత_t argidx, RTLIL::Design *design, bool select = true);

	స్థిర శూన్య call(RTLIL::Design *design, std::string command);
	స్థిర శూన్య call(RTLIL::Design *design, std::సదిశ<std::string> వాదనలు_2);

	స్థిర శూన్య call_on_selection(RTLIL::Design *design, స్థిరరాశి RTLIL::Selection &selection, std::string command);
	స్థిర శూన్య call_on_selection(RTLIL::Design *design, స్థిరరాశి RTLIL::Selection &selection, std::సదిశ<std::string> వాదనలు_2);

	స్థిర శూన్య call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	స్థిర శూన్య call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::సదిశ<std::string> వాదనలు_2);

	Pass *తదుపరి_queued_pass;
	మాయా శూన్య నిరంతరం_రిజిస్టరు();
	స్థిర శూన్య init_రిజిస్టరు();
	స్థిర శూన్య పూర్తి_రిజిస్టరు();

	మాయా శూన్య on_రిజిస్టరు();
	మాయా శూన్య on_shutdown();
	మాయా bool replace_existing_pass() స్థిరరాశి { తిరిగి false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	మాయా bool allow_garbage_collection_during_pass() స్థిరరాశి { తిరిగి false; }
};

నిర్మాణం ScriptPass : Pass
{
	bool ఖండం_చురుకైన, help_బహుళకం;
	RTLIL::Design *చురుకైన_design;
	std::string చురుకైన_నిరంతరం_from, చురుకైన_నిరంతరం_గమ్యం;

	ScriptPass(std::string name, std::string short_help = "** document me **", ఆధారం_location location = ఆధారం_location::current()) :
		Pass(name, short_help, location) { }

	మాయా శూన్య script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	శూన్య నిరంతరం(std::string command, std::string info = std::string());
	శూన్య నిరంతరం_nocheck(std::string command, std::string info = std::string());
	శూన్య నిరంతరం_script(RTLIL::Design *design, std::string నిరంతరం_from = std::string(), std::string నిరంతరం_గమ్యం = std::string());
	శూన్య help_script();

	bool allow_garbage_collection_during_pass() స్థిరరాశి override { తిరిగి true; }
};

నిర్మాణం Frontend : Pass
{
	// for reading of here documents
	స్థిర FILE *current_script_file;
	స్థిర std::string చివరి_ఇక్కడ_document;

	// script file and last line read from it
	స్థిర std::string current_script_filename;
	స్థిర పూర్ణాంకం current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		ఆధారం_location location = ఆధారం_location::current());
	శూన్య నిరంతరం_రిజిస్టరు() override;
	~Frontend() override;
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) override final;
	మాయా శూన్య execute(std::istream *&f, std::string filename, std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) = 0;

	స్థిర std::సదిశ<std::string> తదుపరి_వాదనలు;
	శూన్య extra_వాదనలు(std::istream *&f, std::string &filename, std::సదిశ<std::string> వాదనలు_2, కొలత_t argidx, bool bin_ఆదానం = false);

	స్థిర శూన్య frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	స్థిర శూన్య frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::సదిశ<std::string> వాదనలు_2);
};

నిర్మాణం Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		ఆధారం_location location = ఆధారం_location::current());
	శూన్య నిరంతరం_రిజిస్టరు() override;
	~Backend() override;
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) override final;
	మాయా శూన్య execute(std::ostream *&f, std::string filename,  std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design) = 0;

	శూన్య extra_వాదనలు(std::ostream *&f, std::string &filename, std::సదిశ<std::string> వాదనలు_2, కొలత_t argidx, bool bin_output = false);

	స్థిర శూన్య backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	స్థిర శూన్య backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::సదిశ<std::string> వాదనలు_2);
};

// implemented in passes/cmds/select.cc
బాహ్య శూన్య handle_extra_select_వాదనలు(Pass *pass, స్థిరరాశి std::సదిశ<std::string> &వాదనలు_2, కొలత_t argidx, కొలత_t వాదనలు_కొలత, RTLIL::Design *design);
బాహ్య RTLIL::Selection eval_select_వాదనలు(స్థిరరాశి సదిశ<string> &వాదనలు_2, RTLIL::Design *design);
బాహ్య శూన్య eval_select_op(సదిశ<RTLIL::Selection> &work, స్థిరరాశి string &op, RTLIL::Design *design);

బాహ్య std::map<std::string, Pass*> pass_రిజిస్టరు;
బాహ్య std::map<std::string, Frontend*> frontend_రిజిస్టరు;
బాహ్య std::map<std::string, Backend*> backend_రిజిస్టరు;

నిర్మాణం లక్ష్యం_Family {
	std::string description;
	std::సదిశ<std::string> files;
};

నిర్మాణం లక్ష్యం {
	std::string description;
	std::map<std::string, లక్ష్యం_Family> families;
};

బాహ్య std::map<std::string, లక్ష్యం> లక్ష్యం_రిజిస్టరు;

#నిర్వచించు YOSYS_HAS_లక్ష్యం_రిజిస్టరు 1

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు

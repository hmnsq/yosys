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

#ifndef REGISTER_H
#define REGISTER_H

#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	સ્થિર bool is_enabled_;
જાહેર:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	સ્થિર bool is_enabled() { પાછા is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
શૂન્ય request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
શૂન્ય try_collect_garbage();

સંરચના Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	આભાસી ~Pass();

	// Makes calls to log() to generate help message
	આભાસી શૂન્ય help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	આભાસી bool formatted_help();
	આભાસી શૂન્ય clear_flags();
	આભાસી શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	પૂર્ણાંક call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	સ્થિર શૂન્ય subtract_from_current_runtime_ns(int64_t time_ns);

	શૂન્ય experimental() {
		experimental_flag = true;
	}

	શૂન્ય internal() {
		internal_flag = true;
	}

	સંરચના pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	શૂન્ય post_execute(pre_post_exec_state_t state);

	શૂન્ય cmd_log_args(અચલ std::vector<std::string> &args);
	શૂન્ય cmd_error(અચલ std::vector<std::string> &args, size_t argidx, std::string સંદેશ);
	શૂન્ય extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	સ્થિર શૂન્ય call(RTLIL::Design *design, std::string command);
	સ્થિર શૂન્ય call(RTLIL::Design *design, std::vector<std::string> args);

	સ્થિર શૂન્ય call_on_selection(RTLIL::Design *design, અચલ RTLIL::Selection &selection, std::string command);
	સ્થિર શૂન્ય call_on_selection(RTLIL::Design *design, અચલ RTLIL::Selection &selection, std::vector<std::string> args);

	સ્થિર શૂન્ય call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	સ્થિર શૂન્ય call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	આભાસી શૂન્ય run_register();
	સ્થિર શૂન્ય init_register();
	સ્થિર શૂન્ય done_register();

	આભાસી શૂન્ય on_register();
	આભાસી શૂન્ય on_shutdown();
	આભાસી bool replace_existing_pass() અચલ { પાછા false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	આભાસી bool allow_garbage_collection_during_pass() અચલ { પાછા false; }
};

સંરચના ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	આભાસી શૂન્ય script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	શૂન્ય run(std::string command, std::string info = std::string());
	શૂન્ય run_nocheck(std::string command, std::string info = std::string());
	શૂન્ય run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	શૂન્ય help_script();

	bool allow_garbage_collection_during_pass() અચલ override { પાછા true; }
};

સંરચના Frontend : Pass
{
	// for reading of here documents
	સ્થિર FILE *current_script_file;
	સ્થિર std::string last_here_document;

	// script file and last line read from it
	સ્થિર std::string current_script_filename;
	સ્થિર પૂર્ણાંક current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	શૂન્ય run_register() override;
	~Frontend() override;
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	આભાસી શૂન્ય execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	સ્થિર std::vector<std::string> next_args;
	શૂન્ય extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_આગત = false);

	સ્થિર શૂન્ય frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	સ્થિર શૂન્ય frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

સંરચના Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	શૂન્ય run_register() override;
	~Backend() override;
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	આભાસી શૂન્ય execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	શૂન્ય extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	સ્થિર શૂન્ય backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	સ્થિર શૂન્ય backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
બાહ્ય શૂન્ય handle_extra_select_args(Pass *pass, અચલ std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
બાહ્ય RTLIL::Selection eval_select_args(અચલ vector<string> &args, RTLIL::Design *design);
બાહ્ય શૂન્ય eval_select_op(vector<RTLIL::Selection> &work, અચલ string &op, RTLIL::Design *design);

બાહ્ય std::map<std::string, Pass*> pass_register;
બાહ્ય std::map<std::string, Frontend*> frontend_register;
બાહ્ય std::map<std::string, Backend*> backend_register;

સંરચના TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

સંરચના Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

બાહ્ય std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

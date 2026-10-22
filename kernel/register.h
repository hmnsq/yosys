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

#അന്തര്ഭാവ "kernel/yosys_common.h"
#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	സ്ഥിതം bool is_enabled_;
പൊതു:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	സ്ഥിതം bool is_enabled() { തിരികെ is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
ശൂന്യം request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
ശൂന്യം try_collect_garbage();

ഘടന Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	മായാ ~Pass();

	// Makes calls to log() to generate help message
	മായാ ശൂന്യം help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	മായാ bool formatted_help();
	മായാ ശൂന്യം clear_flags();
	മായാ ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	പൂർണ്ണസംഖ്യ call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	സ്ഥിതം ശൂന്യം subtract_from_current_runtime_ns(int64_t time_ns);

	ശൂന്യം experimental() {
		experimental_flag = true;
	}

	ശൂന്യം internal() {
		internal_flag = true;
	}

	ഘടന pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	ശൂന്യം post_execute(pre_post_exec_state_t state);

	ശൂന്യം cmd_log_args(സ്ഥിരം std::vector<std::string> &args);
	ശൂന്യം cmd_error(സ്ഥിരം std::vector<std::string> &args, size_t argidx, std::string സന്ദേശം);
	ശൂന്യം extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	സ്ഥിതം ശൂന്യം call(RTLIL::Design *design, std::string command);
	സ്ഥിതം ശൂന്യം call(RTLIL::Design *design, std::vector<std::string> args);

	സ്ഥിതം ശൂന്യം call_on_selection(RTLIL::Design *design, സ്ഥിരം RTLIL::Selection &selection, std::string command);
	സ്ഥിതം ശൂന്യം call_on_selection(RTLIL::Design *design, സ്ഥിരം RTLIL::Selection &selection, std::vector<std::string> args);

	സ്ഥിതം ശൂന്യം call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	സ്ഥിതം ശൂന്യം call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	മായാ ശൂന്യം run_register();
	സ്ഥിതം ശൂന്യം init_register();
	സ്ഥിതം ശൂന്യം done_register();

	മായാ ശൂന്യം on_register();
	മായാ ശൂന്യം on_shutdown();
	മായാ bool replace_existing_pass() സ്ഥിരം { തിരികെ false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	മായാ bool allow_garbage_collection_during_pass() സ്ഥിരം { തിരികെ false; }
};

ഘടന ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	മായാ ശൂന്യം script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	ശൂന്യം run(std::string command, std::string info = std::string());
	ശൂന്യം run_nocheck(std::string command, std::string info = std::string());
	ശൂന്യം run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	ശൂന്യം help_script();

	bool allow_garbage_collection_during_pass() സ്ഥിരം override { തിരികെ true; }
};

ഘടന Frontend : Pass
{
	// for reading of here documents
	സ്ഥിതം FILE *current_script_file;
	സ്ഥിതം std::string last_here_document;

	// script file and last line read from it
	സ്ഥിതം std::string current_script_filename;
	സ്ഥിതം പൂർണ്ണസംഖ്യ current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	ശൂന്യം run_register() override;
	~Frontend() override;
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	മായാ ശൂന്യം execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	സ്ഥിതം std::vector<std::string> next_args;
	ശൂന്യം extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_നിവേശം = false);

	സ്ഥിതം ശൂന്യം frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	സ്ഥിതം ശൂന്യം frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

ഘടന Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	ശൂന്യം run_register() override;
	~Backend() override;
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	മായാ ശൂന്യം execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	ശൂന്യം extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	സ്ഥിതം ശൂന്യം backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	സ്ഥിതം ശൂന്യം backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
ബാഹ്യ ശൂന്യം handle_extra_select_args(Pass *pass, സ്ഥിരം std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
ബാഹ്യ RTLIL::Selection eval_select_args(സ്ഥിരം vector<string> &args, RTLIL::Design *design);
ബാഹ്യ ശൂന്യം eval_select_op(vector<RTLIL::Selection> &work, സ്ഥിരം string &op, RTLIL::Design *design);

ബാഹ്യ std::map<std::string, Pass*> pass_register;
ബാഹ്യ std::map<std::string, Frontend*> frontend_register;
ബാഹ്യ std::map<std::string, Backend*> backend_register;

ഘടന TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

ഘടന Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

ബാഹ്യ std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

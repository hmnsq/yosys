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

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	ಸ್ಥಿರ bool is_enabled_;
ಸಾರ್ವಜನಿಕ:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ಸ್ಥಿರ bool is_enabled() { ಪ್ರತ್ಯಾಗಮ is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
ಶೂನ್ಯ request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
ಶೂನ್ಯ try_collect_garbage();

ಸಂರಚನಾ Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	ಆಭಾಸ ~Pass();

	// Makes calls to log() to generate help message
	ಆಭಾಸ ಶೂನ್ಯ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	ಆಭಾಸ bool formatted_help();
	ಆಭಾಸ ಶೂನ್ಯ clear_flags();
	ಆಭಾಸ ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	ಪೂರ್ಣಾಙ್ಕ call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	ಸ್ಥಿರ ಶೂನ್ಯ subtract_from_current_runtime_ns(int64_t time_ns);

	ಶೂನ್ಯ experimental() {
		experimental_flag = true;
	}

	ಶೂನ್ಯ internal() {
		internal_flag = true;
	}

	ಸಂರಚನಾ pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	ಶೂನ್ಯ post_execute(pre_post_exec_state_t state);

	ಶೂನ್ಯ cmd_log_args(ಧ್ರುವ std::vector<std::string> &args);
	ಶೂನ್ಯ cmd_error(ಧ್ರುವ std::vector<std::string> &args, size_t argidx, std::string ಸಂದೇಶ);
	ಶೂನ್ಯ extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	ಸ್ಥಿರ ಶೂನ್ಯ call(RTLIL::Design *design, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ call(RTLIL::Design *design, std::vector<std::string> args);

	ಸ್ಥಿರ ಶೂನ್ಯ call_on_selection(RTLIL::Design *design, ಧ್ರುವ RTLIL::Selection &selection, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ call_on_selection(RTLIL::Design *design, ಧ್ರುವ RTLIL::Selection &selection, std::vector<std::string> args);

	ಸ್ಥಿರ ಶೂನ್ಯ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	ಆಭಾಸ ಶೂನ್ಯ run_register();
	ಸ್ಥಿರ ಶೂನ್ಯ init_register();
	ಸ್ಥಿರ ಶೂನ್ಯ done_register();

	ಆಭಾಸ ಶೂನ್ಯ on_register();
	ಆಭಾಸ ಶೂನ್ಯ on_shutdown();
	ಆಭಾಸ bool replace_existing_pass() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	ಆಭಾಸ bool allow_garbage_collection_during_pass() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ false; }
};

ಸಂರಚನಾ ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	ಆಭಾಸ ಶೂನ್ಯ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	ಶೂನ್ಯ run(std::string command, std::string info = std::string());
	ಶೂನ್ಯ run_nocheck(std::string command, std::string info = std::string());
	ಶೂನ್ಯ run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	ಶೂನ್ಯ help_script();

	bool allow_garbage_collection_during_pass() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ true; }
};

ಸಂರಚನಾ Frontend : Pass
{
	// for reading of here documents
	ಸ್ಥಿರ FILE *current_script_file;
	ಸ್ಥಿರ std::string last_here_document;

	// script file and last line read from it
	ಸ್ಥಿರ std::string current_script_filename;
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	ಶೂನ್ಯ run_register() override;
	~Frontend() override;
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	ಆಭಾಸ ಶೂನ್ಯ execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	ಸ್ಥಿರ std::vector<std::string> next_args;
	ಶೂನ್ಯ extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_ಒಳಹರಿವು = false);

	ಸ್ಥಿರ ಶೂನ್ಯ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

ಸಂರಚನಾ Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	ಶೂನ್ಯ run_register() override;
	~Backend() override;
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	ಆಭಾಸ ಶೂನ್ಯ execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	ಶೂನ್ಯ extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	ಸ್ಥಿರ ಶೂನ್ಯ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
ಬಾಹ್ಯ ಶೂನ್ಯ handle_extra_select_args(Pass *pass, ಧ್ರುವ std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
ಬಾಹ್ಯ RTLIL::Selection eval_select_args(ಧ್ರುವ vector<string> &args, RTLIL::Design *design);
ಬಾಹ್ಯ ಶೂನ್ಯ eval_select_op(vector<RTLIL::Selection> &work, ಧ್ರುವ string &op, RTLIL::Design *design);

ಬಾಹ್ಯ std::map<std::string, Pass*> pass_register;
ಬಾಹ್ಯ std::map<std::string, Frontend*> frontend_register;
ಬಾಹ್ಯ std::map<std::string, Backend*> backend_register;

ಸಂರಚನಾ TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

ಸಂರಚನಾ Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

ಬಾಹ್ಯ std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

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

#شامل "kernel/yosys_common.h"
#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	ایستا bool is_enabled_;
عمومی:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ایستا bool is_enabled() { بازگشت is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
تهی request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
تهی try_collect_garbage();

ساختار Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	مجازی ~Pass();

	// Makes calls to log() to generate help message
	مجازی تهی help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	مجازی bool formatted_help();
	مجازی تهی clear_flags();
	مجازی تهی execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	صحیح call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	ایستا تهی subtract_from_current_runtime_ns(int64_t time_ns);

	تهی experimental() {
		experimental_flag = true;
	}

	تهی internal() {
		internal_flag = true;
	}

	ساختار pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	تهی post_execute(pre_post_exec_state_t state);

	تهی cmd_log_args(ثابت std::vector<std::string> &args);
	تهی cmd_error(ثابت std::vector<std::string> &args, size_t argidx, std::string پیام);
	تهی extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	ایستا تهی call(RTLIL::Design *design, std::string command);
	ایستا تهی call(RTLIL::Design *design, std::vector<std::string> args);

	ایستا تهی call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ایستا تهی call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::vector<std::string> args);

	ایستا تهی call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ایستا تهی call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	مجازی تهی run_register();
	ایستا تهی init_register();
	ایستا تهی done_register();

	مجازی تهی on_register();
	مجازی تهی on_shutdown();
	مجازی bool replace_existing_pass() ثابت { بازگشت false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	مجازی bool allow_garbage_collection_during_pass() ثابت { بازگشت false; }
};

ساختار ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	مجازی تهی script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	تهی run(std::string command, std::string info = std::string());
	تهی run_nocheck(std::string command, std::string info = std::string());
	تهی run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	تهی help_script();

	bool allow_garbage_collection_during_pass() ثابت override { بازگشت true; }
};

ساختار Frontend : Pass
{
	// for reading of here documents
	ایستا FILE *current_script_file;
	ایستا std::string last_here_document;

	// script file and last line read from it
	ایستا std::string current_script_filename;
	ایستا صحیح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	تهی run_register() override;
	~Frontend() override;
	تهی execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	مجازی تهی execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	ایستا std::vector<std::string> next_args;
	تهی extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_درون_داد = false);

	ایستا تهی frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ایستا تهی frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

ساختار Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	تهی run_register() override;
	~Backend() override;
	تهی execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	مجازی تهی execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	تهی extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	ایستا تهی backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ایستا تهی backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
بیرونی تهی handle_extra_select_args(Pass *pass, ثابت std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
بیرونی RTLIL::Selection eval_select_args(ثابت vector<string> &args, RTLIL::Design *design);
بیرونی تهی eval_select_op(vector<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

بیرونی std::map<std::string, Pass*> pass_register;
بیرونی std::map<std::string, Frontend*> frontend_register;
بیرونی std::map<std::string, Backend*> backend_register;

ساختار TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

ساختار Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

بیرونی std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

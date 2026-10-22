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

#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	স্থির bool is_enabled_;
সর্বজনীন:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	স্থির bool is_enabled() { ফেরত is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
শূন্য request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
শূন্য try_collect_garbage();

কাঠামো Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	আভাসী ~Pass();

	// Makes calls to log() to generate help message
	আভাসী শূন্য help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	আভাসী bool formatted_help();
	আভাসী শূন্য clear_flags();
	আভাসী শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	পূর্ণসংখ্যা call_গণক;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	স্থির শূন্য subtract_from_current_runtime_ns(int64_t time_ns);

	শূন্য experimental() {
		experimental_flag = true;
	}

	শূন্য internal() {
		internal_flag = true;
	}

	কাঠামো pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	শূন্য post_execute(pre_post_exec_state_t state);

	শূন্য cmd_log_args(ধ্রুবক std::vector<std::string> &args);
	শূন্য cmd_error(ধ্রুবক std::vector<std::string> &args, আকার_t argidx, std::string বার্তা);
	শূন্য extra_args(std::vector<std::string> args, আকার_t argidx, RTLIL::Design *design, bool select = true);

	স্থির শূন্য call(RTLIL::Design *design, std::string command);
	স্থির শূন্য call(RTLIL::Design *design, std::vector<std::string> args);

	স্থির শূন্য call_on_selection(RTLIL::Design *design, ধ্রুবক RTLIL::Selection &selection, std::string command);
	স্থির শূন্য call_on_selection(RTLIL::Design *design, ধ্রুবক RTLIL::Selection &selection, std::vector<std::string> args);

	স্থির শূন্য call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	স্থির শূন্য call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *পরবর্তী_queued_pass;
	আভাসী শূন্য run_register();
	স্থির শূন্য init_register();
	স্থির শূন্য done_register();

	আভাসী শূন্য on_register();
	আভাসী শূন্য on_shutdown();
	আভাসী bool replace_existing_pass() ধ্রুবক { ফেরত false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	আভাসী bool allow_garbage_collection_during_pass() ধ্রুবক { ফেরত false; }
};

কাঠামো ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	আভাসী শূন্য script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	শূন্য run(std::string command, std::string info = std::string());
	শূন্য run_nocheck(std::string command, std::string info = std::string());
	শূন্য run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	শূন্য help_script();

	bool allow_garbage_collection_during_pass() ধ্রুবক override { ফেরত true; }
};

কাঠামো Frontend : Pass
{
	// for reading of here documents
	স্থির FILE *current_script_file;
	স্থির std::string last_here_document;

	// script file and last line read from it
	স্থির std::string current_script_filename;
	স্থির পূর্ণসংখ্যা current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	শূন্য run_register() override;
	~Frontend() override;
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	আভাসী শূন্য execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	স্থির std::vector<std::string> পরবর্তী_args;
	শূন্য extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, আকার_t argidx, bool bin_প্রবেশ = false);

	স্থির শূন্য frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	স্থির শূন্য frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

কাঠামো Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	শূন্য run_register() override;
	~Backend() override;
	শূন্য execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	আভাসী শূন্য execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	শূন্য extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, আকার_t argidx, bool bin_output = false);

	স্থির শূন্য backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	স্থির শূন্য backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
বাহ্যিক শূন্য handle_extra_select_args(Pass *pass, ধ্রুবক std::vector<std::string> &args, আকার_t argidx, আকার_t args_আকার, RTLIL::Design *design);
বাহ্যিক RTLIL::Selection eval_select_args(ধ্রুবক vector<string> &args, RTLIL::Design *design);
বাহ্যিক শূন্য eval_select_op(vector<RTLIL::Selection> &work, ধ্রুবক string &op, RTLIL::Design *design);

বাহ্যিক std::map<std::string, Pass*> pass_register;
বাহ্যিক std::map<std::string, Frontend*> frontend_register;
বাহ্যিক std::map<std::string, Backend*> backend_register;

কাঠামো TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

কাঠামো Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

বাহ্যিক std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

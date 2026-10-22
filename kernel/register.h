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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	ସ୍ଥିର bool is_enabled_;
ସର୍ବସାଧାରଣ:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ସ୍ଥିର bool is_enabled() { ଫେରନ୍ତୁ is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
ଶୂନ୍ୟ request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
ଶୂନ୍ୟ try_collect_garbage();

ସଂରଚନା Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	ଆଭାସୀ ~Pass();

	// Makes calls to log() to generate help message
	ଆଭାସୀ ଶୂନ୍ୟ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	ଆଭାସୀ bool formatted_help();
	ଆଭାସୀ ଶୂନ୍ୟ clear_flags();
	ଆଭାସୀ ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	ସ୍ଥିର ଶୂନ୍ୟ subtract_from_current_runtime_ns(int64_t time_ns);

	ଶୂନ୍ୟ experimental() {
		experimental_flag = true;
	}

	ଶୂନ୍ୟ internal() {
		internal_flag = true;
	}

	ସଂରଚନା pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	ଶୂନ୍ୟ post_execute(pre_post_exec_state_t state);

	ଶୂନ୍ୟ cmd_log_args(ଅଚଳ std::vector<std::string> &args);
	ଶୂନ୍ୟ cmd_error(ଅଚଳ std::vector<std::string> &args, size_t argidx, std::string ସନ୍ଦେଶ);
	ଶୂନ୍ୟ extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	ସ୍ଥିର ଶୂନ୍ୟ call(RTLIL::Design *design, std::string command);
	ସ୍ଥିର ଶୂନ୍ୟ call(RTLIL::Design *design, std::vector<std::string> args);

	ସ୍ଥିର ଶୂନ୍ୟ call_on_selection(RTLIL::Design *design, ଅଚଳ RTLIL::Selection &selection, std::string command);
	ସ୍ଥିର ଶୂନ୍ୟ call_on_selection(RTLIL::Design *design, ଅଚଳ RTLIL::Selection &selection, std::vector<std::string> args);

	ସ୍ଥିର ଶୂନ୍ୟ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ସ୍ଥିର ଶୂନ୍ୟ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	ଆଭାସୀ ଶୂନ୍ୟ run_register();
	ସ୍ଥିର ଶୂନ୍ୟ init_register();
	ସ୍ଥିର ଶୂନ୍ୟ done_register();

	ଆଭାସୀ ଶୂନ୍ୟ on_register();
	ଆଭାସୀ ଶୂନ୍ୟ on_shutdown();
	ଆଭାସୀ bool replace_existing_pass() ଅଚଳ { ଫେରନ୍ତୁ false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	ଆଭାସୀ bool allow_garbage_collection_during_pass() ଅଚଳ { ଫେରନ୍ତୁ false; }
};

ସଂରଚନା ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	ଆଭାସୀ ଶୂନ୍ୟ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	ଶୂନ୍ୟ run(std::string command, std::string info = std::string());
	ଶୂନ୍ୟ run_nocheck(std::string command, std::string info = std::string());
	ଶୂନ୍ୟ run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	ଶୂନ୍ୟ help_script();

	bool allow_garbage_collection_during_pass() ଅଚଳ override { ଫେରନ୍ତୁ true; }
};

ସଂରଚନା Frontend : Pass
{
	// for reading of here documents
	ସ୍ଥିର FILE *current_script_file;
	ସ୍ଥିର std::string last_here_document;

	// script file and last line read from it
	ସ୍ଥିର std::string current_script_filename;
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	ଶୂନ୍ୟ run_register() override;
	~Frontend() override;
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	ଆଭାସୀ ଶୂନ୍ୟ execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	ସ୍ଥିର std::vector<std::string> next_args;
	ଶୂନ୍ୟ extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_ନିବେଶ = false);

	ସ୍ଥିର ଶୂନ୍ୟ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ସ୍ଥିର ଶୂନ୍ୟ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

ସଂରଚନା Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	ଶୂନ୍ୟ run_register() override;
	~Backend() override;
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	ଆଭାସୀ ଶୂନ୍ୟ execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	ଶୂନ୍ୟ extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	ସ୍ଥିର ଶୂନ୍ୟ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ସ୍ଥିର ଶୂନ୍ୟ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
ବାହ୍ୟ ଶୂନ୍ୟ handle_extra_select_args(Pass *pass, ଅଚଳ std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
ବାହ୍ୟ RTLIL::Selection eval_select_args(ଅଚଳ vector<string> &args, RTLIL::Design *design);
ବାହ୍ୟ ଶୂନ୍ୟ eval_select_op(vector<RTLIL::Selection> &work, ଅଚଳ string &op, RTLIL::Design *design);

ବାହ୍ୟ std::map<std::string, Pass*> pass_register;
ବାହ୍ୟ std::map<std::string, Frontend*> frontend_register;
ବାହ୍ୟ std::map<std::string, Backend*> backend_register;

ସଂରଚନା TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

ସଂରଚନା Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

ବାହ୍ୟ std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

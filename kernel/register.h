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

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	स्थिर bool is_enabled_;
सार्वजनिक:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	स्थिर bool is_enabled() { प्रत्यागम is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
शून्य request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
शून्य try_collect_garbage();

संरचना Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	आभास ~Pass();

	// Makes calls to log() to generate help message
	आभास शून्य help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	आभास bool formatted_help();
	आभास शून्य clear_flags();
	आभास शून्य execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	पूर्णाङ्क call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	स्थिर शून्य subtract_from_current_runtime_ns(int64_t time_ns);

	शून्य experimental() {
		experimental_flag = true;
	}

	शून्य internal() {
		internal_flag = true;
	}

	संरचना pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	शून्य post_execute(pre_post_exec_state_t state);

	शून्य cmd_log_args(ध्रुव std::vector<std::string> &args);
	शून्य cmd_error(ध्रुव std::vector<std::string> &args, size_t argidx, std::string संदेश);
	शून्य extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	स्थिर शून्य call(RTLIL::Design *design, std::string command);
	स्थिर शून्य call(RTLIL::Design *design, std::vector<std::string> args);

	स्थिर शून्य call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::string command);
	स्थिर शून्य call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::vector<std::string> args);

	स्थिर शून्य call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	स्थिर शून्य call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	आभास शून्य run_register();
	स्थिर शून्य init_register();
	स्थिर शून्य done_register();

	आभास शून्य on_register();
	आभास शून्य on_shutdown();
	आभास bool replace_existing_pass() ध्रुव { प्रत्यागम false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	आभास bool allow_garbage_collection_during_pass() ध्रुव { प्रत्यागम false; }
};

संरचना ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	आभास शून्य script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	शून्य run(std::string command, std::string info = std::string());
	शून्य run_nocheck(std::string command, std::string info = std::string());
	शून्य run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	शून्य help_script();

	bool allow_garbage_collection_during_pass() ध्रुव override { प्रत्यागम true; }
};

संरचना Frontend : Pass
{
	// for reading of here documents
	स्थिर FILE *current_script_file;
	स्थिर std::string last_here_document;

	// script file and last line read from it
	स्थिर std::string current_script_filename;
	स्थिर पूर्णाङ्क current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	शून्य run_register() override;
	~Frontend() override;
	शून्य execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	आभास शून्य execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	स्थिर std::vector<std::string> next_args;
	शून्य extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_निवेश = false);

	स्थिर शून्य frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	स्थिर शून्य frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

संरचना Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	शून्य run_register() override;
	~Backend() override;
	शून्य execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	आभास शून्य execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	शून्य extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	स्थिर शून्य backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	स्थिर शून्य backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
बाह्य शून्य handle_extra_select_args(Pass *pass, ध्रुव std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
बाह्य RTLIL::Selection eval_select_args(ध्रुव vector<string> &args, RTLIL::Design *design);
बाह्य शून्य eval_select_op(vector<RTLIL::Selection> &work, ध्रुव string &op, RTLIL::Design *design);

बाह्य std::map<std::string, Pass*> pass_register;
बाह्य std::map<std::string, Frontend*> frontend_register;
बाह्य std::map<std::string, Backend*> backend_register;

संरचना TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

संरचना Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

बाह्य std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

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

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	ساكن bool is_enabled_;
عام:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ساكن bool is_enabled() { أرجع is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
فراغ request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
فراغ try_collect_garbage();

بنية Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	virtual ~Pass();

	// Makes calls to log() to generate help message
	virtual فراغ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	virtual bool formatted_help();
	virtual فراغ clear_flags();
	virtual فراغ execute(std::vector<std::string> وسائط_2, RTLIL::Design *design) = 0;

	صحيح call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	ساكن فراغ subtract_from_current_runtime_ns(int64_t time_ns);

	فراغ experimental() {
		experimental_flag = true;
	}

	فراغ internal() {
		internal_flag = true;
	}

	بنية pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	فراغ post_execute(pre_post_exec_state_t state);

	فراغ cmd_log_وسائط(ثابت std::vector<std::string> &وسائط_2);
	فراغ cmd_error(ثابت std::vector<std::string> &وسائط_2, size_t argidx, std::string رسالة);
	فراغ extra_وسائط(std::vector<std::string> وسائط_2, size_t argidx, RTLIL::Design *design, bool select = true);

	ساكن فراغ call(RTLIL::Design *design, std::string command);
	ساكن فراغ call(RTLIL::Design *design, std::vector<std::string> وسائط_2);

	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::vector<std::string> وسائط_2);

	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> وسائط_2);

	Pass *next_queued_pass;
	virtual فراغ run_register();
	ساكن فراغ init_register();
	ساكن فراغ done_register();

	virtual فراغ on_register();
	virtual فراغ on_shutdown();
	virtual bool replace_existing_pass() ثابت { أرجع false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	virtual bool allow_garbage_collection_during_pass() ثابت { أرجع false; }
};

بنية ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	virtual فراغ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	فراغ run(std::string command, std::string info = std::string());
	فراغ run_nocheck(std::string command, std::string info = std::string());
	فراغ run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	فراغ help_script();

	bool allow_garbage_collection_during_pass() ثابت override { أرجع true; }
};

بنية Frontend : Pass
{
	// for reading of here documents
	ساكن FILE *current_script_file;
	ساكن std::string last_here_document;

	// script file and last line read from it
	ساكن std::string current_script_filename;
	ساكن صحيح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	فراغ run_register() override;
	~Frontend() override;
	فراغ execute(std::vector<std::string> وسائط_2, RTLIL::Design *design) override final;
	virtual فراغ execute(std::istream *&f, std::string filename, std::vector<std::string> وسائط_2, RTLIL::Design *design) = 0;

	ساكن std::vector<std::string> next_وسائط;
	فراغ extra_وسائط(std::istream *&f, std::string &filename, std::vector<std::string> وسائط_2, size_t argidx, bool bin_مدخل = false);

	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> وسائط_2);
};

بنية Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	فراغ run_register() override;
	~Backend() override;
	فراغ execute(std::vector<std::string> وسائط_2, RTLIL::Design *design) override final;
	virtual فراغ execute(std::ostream *&f, std::string filename,  std::vector<std::string> وسائط_2, RTLIL::Design *design) = 0;

	فراغ extra_وسائط(std::ostream *&f, std::string &filename, std::vector<std::string> وسائط_2, size_t argidx, bool bin_output = false);

	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> وسائط_2);
};

// implemented in passes/cmds/select.cc
خارجي فراغ handle_extra_select_وسائط(Pass *pass, ثابت std::vector<std::string> &وسائط_2, size_t argidx, size_t وسائط_size, RTLIL::Design *design);
خارجي RTLIL::Selection eval_select_وسائط(ثابت vector<string> &وسائط_2, RTLIL::Design *design);
خارجي فراغ eval_select_op(vector<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

خارجي std::map<std::string, Pass*> pass_register;
خارجي std::map<std::string, Frontend*> frontend_register;
خارجي std::map<std::string, Backend*> backend_register;

بنية TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

بنية Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

خارجي std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

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

#அந்தர்பாவ "kernel/yosys_common.h"
#அந்தர்பாவ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	நிலையான bool is_enabled_;
பொது:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	நிலையான bool is_enabled() { திரும்பு is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
வெறுமை request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
வெறுமை try_collect_garbage();

கட்டமைப்பு Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	மெய்நிகர் ~Pass();

	// Makes calls to log() to generate help message
	மெய்நிகர் வெறுமை help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	மெய்நிகர் bool formatted_help();
	மெய்நிகர் வெறுமை clear_flags();
	மெய்நிகர் வெறுமை execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	முழு call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	நிலையான வெறுமை subtract_from_current_runtime_ns(int64_t time_ns);

	வெறுமை experimental() {
		experimental_flag = true;
	}

	வெறுமை internal() {
		internal_flag = true;
	}

	கட்டமைப்பு pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	வெறுமை post_execute(pre_post_exec_state_t state);

	வெறுமை cmd_log_args(மாறிலி std::vector<std::string> &args);
	வெறுமை cmd_error(மாறிலி std::vector<std::string> &args, size_t argidx, std::string செய்தி);
	வெறுமை extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, bool select = true);

	நிலையான வெறுமை call(RTLIL::Design *design, std::string command);
	நிலையான வெறுமை call(RTLIL::Design *design, std::vector<std::string> args);

	நிலையான வெறுமை call_on_selection(RTLIL::Design *design, மாறிலி RTLIL::Selection &selection, std::string command);
	நிலையான வெறுமை call_on_selection(RTLIL::Design *design, மாறிலி RTLIL::Selection &selection, std::vector<std::string> args);

	நிலையான வெறுமை call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	நிலையான வெறுமை call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *next_queued_pass;
	மெய்நிகர் வெறுமை run_register();
	நிலையான வெறுமை init_register();
	நிலையான வெறுமை done_register();

	மெய்நிகர் வெறுமை on_register();
	மெய்நிகர் வெறுமை on_shutdown();
	மெய்நிகர் bool replace_existing_pass() மாறிலி { திரும்பு false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	மெய்நிகர் bool allow_garbage_collection_during_pass() மாறிலி { திரும்பு false; }
};

கட்டமைப்பு ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	மெய்நிகர் வெறுமை script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	வெறுமை run(std::string command, std::string info = std::string());
	வெறுமை run_nocheck(std::string command, std::string info = std::string());
	வெறுமை run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	வெறுமை help_script();

	bool allow_garbage_collection_during_pass() மாறிலி override { திரும்பு true; }
};

கட்டமைப்பு Frontend : Pass
{
	// for reading of here documents
	நிலையான FILE *current_script_file;
	நிலையான std::string last_here_document;

	// script file and last line read from it
	நிலையான std::string current_script_filename;
	நிலையான முழு current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	வெறுமை run_register() override;
	~Frontend() override;
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	மெய்நிகர் வெறுமை execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	நிலையான std::vector<std::string> next_args;
	வெறுமை extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_உள்ளீடு = false);

	நிலையான வெறுமை frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	நிலையான வெறுமை frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

கட்டமைப்பு Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	வெறுமை run_register() override;
	~Backend() override;
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	மெய்நிகர் வெறுமை execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	வெறுமை extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, bool bin_output = false);

	நிலையான வெறுமை backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	நிலையான வெறுமை backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
வெளி வெறுமை handle_extra_select_args(Pass *pass, மாறிலி std::vector<std::string> &args, size_t argidx, size_t args_size, RTLIL::Design *design);
வெளி RTLIL::Selection eval_select_args(மாறிலி vector<string> &args, RTLIL::Design *design);
வெளி வெறுமை eval_select_op(vector<RTLIL::Selection> &work, மாறிலி string &op, RTLIL::Design *design);

வெளி std::map<std::string, Pass*> pass_register;
வெளி std::map<std::string, Frontend*> frontend_register;
வெளி std::map<std::string, Backend*> backend_register;

கட்டமைப்பு TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

கட்டமைப்பு Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

வெளி std::map<std::string, Target> target_register;

#define YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#endif

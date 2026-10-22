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

#अगर_अघोषित REGISTER_H
#घोषणा REGISTER_H

#समावेश "kernel/yosys_common.h"
#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	अटल bool is_enabled_;
खुला:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	अटल bool is_enabled() { वापस is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
व्योम request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
व्योम try_collect_garbage();

काष्ठा Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	भव ~Pass();

	// Makes calls to log() to generate help message
	भव व्योम help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	भव bool formatted_help();
	भव व्योम clear_flags();
	भव व्योम execute(std::vector<std::string> args, RTLIL::Design *design) = 0;

	पूर्णांक call_गणक;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool internal_flag = false;

	अटल व्योम subtract_from_current_runtime_ns(int64_t time_ns);

	व्योम experimental() {
		experimental_flag = true;
	}

	व्योम internal() {
		internal_flag = true;
	}

	काष्ठा pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	व्योम post_execute(pre_post_exec_state_t state);

	व्योम cmd_log_args(स्थिर std::vector<std::string> &args);
	व्योम cmd_error(स्थिर std::vector<std::string> &args, माप_प्रकार argidx, std::string संदेश);
	व्योम extra_args(std::vector<std::string> args, माप_प्रकार argidx, RTLIL::Design *design, bool select = true);

	अटल व्योम call(RTLIL::Design *design, std::string command);
	अटल व्योम call(RTLIL::Design *design, std::vector<std::string> args);

	अटल व्योम call_on_selection(RTLIL::Design *design, स्थिर RTLIL::Selection &selection, std::string command);
	अटल व्योम call_on_selection(RTLIL::Design *design, स्थिर RTLIL::Selection &selection, std::vector<std::string> args);

	अटल व्योम call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	अटल व्योम call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args);

	Pass *अग्र_queued_pass;
	भव व्योम run_register();
	अटल व्योम init_register();
	अटल व्योम done_register();

	भव व्योम on_register();
	भव व्योम on_shutdown();
	भव bool replace_existing_pass() स्थिर { वापस false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	भव bool allow_garbage_collection_during_pass() स्थिर { वापस false; }
};

काष्ठा ScriptPass : Pass
{
	bool block_active, help_mode;
	RTLIL::Design *active_design;
	std::string active_run_from, active_run_to;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	भव व्योम script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	व्योम run(std::string command, std::string info = std::string());
	व्योम run_nocheck(std::string command, std::string info = std::string());
	व्योम run_script(RTLIL::Design *design, std::string run_from = std::string(), std::string run_to = std::string());
	व्योम help_script();

	bool allow_garbage_collection_during_pass() स्थिर override { वापस true; }
};

काष्ठा Frontend : Pass
{
	// for reading of here documents
	अटल खाता *current_script_file;
	अटल std::string last_here_document;

	// script file and last line read from it
	अटल std::string current_script_filename;
	अटल पूर्णांक current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	व्योम run_register() override;
	~Frontend() override;
	व्योम execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	भव व्योम execute(std::istream *&f, std::string filename, std::vector<std::string> args, RTLIL::Design *design) = 0;

	अटल std::vector<std::string> अग्र_args;
	व्योम extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, माप_प्रकार argidx, bool bin_निवेश = false);

	अटल व्योम frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	अटल व्योम frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args);
};

काष्ठा Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	व्योम run_register() override;
	~Backend() override;
	व्योम execute(std::vector<std::string> args, RTLIL::Design *design) override final;
	भव व्योम execute(std::ostream *&f, std::string filename,  std::vector<std::string> args, RTLIL::Design *design) = 0;

	व्योम extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, माप_प्रकार argidx, bool bin_output = false);

	अटल व्योम backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	अटल व्योम backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args);
};

// implemented in passes/cmds/select.cc
बाह्य व्योम handle_extra_select_args(Pass *pass, स्थिर std::vector<std::string> &args, माप_प्रकार argidx, माप_प्रकार args_आकार, RTLIL::Design *design);
बाह्य RTLIL::Selection eval_select_args(स्थिर vector<string> &args, RTLIL::Design *design);
बाह्य व्योम eval_select_op(vector<RTLIL::Selection> &work, स्थिर string &op, RTLIL::Design *design);

बाह्य std::map<std::string, Pass*> pass_register;
बाह्य std::map<std::string, Frontend*> frontend_register;
बाह्य std::map<std::string, Backend*> backend_register;

काष्ठा TargetFamily {
	std::string description;
	std::vector<std::string> files;
};

काष्ठा Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

बाह्य std::map<std::string, Target> target_register;

#घोषणा YOSYS_HAS_TARGET_REGISTER 1

YOSYS_NAMESPACE_END

#पूर्ण_अगर

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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਰਜਿਸਟਰ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਜਿਸਟਰ_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	ਸਥਿਰ bool is_enabled_;
ਜਨਤਕ:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ਸਥਿਰ bool is_enabled() { ਵਾਪਸ is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
ਖਾਲੀ request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
ਖਾਲੀ try_collect_garbage();

ਬਣਤਰ Pass
{
	std::string pass_name, short_help;
	ਸਰੋਤ_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		ਸਰੋਤ_location location = ਸਰੋਤ_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	ਆਭਾਸੀ ~Pass();

	// Makes calls to log() to generate help message
	ਆਭਾਸੀ ਖਾਲੀ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	ਆਭਾਸੀ bool formatted_help();
	ਆਭਾਸੀ ਖਾਲੀ clear_flags();
	ਆਭਾਸੀ ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) = 0;

	ਪੂਰਨ call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool ਅੰਦਰੂਨੀ_flag = false;

	ਸਥਿਰ ਖਾਲੀ ਘਟਾਓ_from_current_runtime_ns(int64_t ਸਮਾਂ_ns);

	ਖਾਲੀ experimental() {
		experimental_flag = true;
	}

	ਖਾਲੀ ਅੰਦਰੂਨੀ() {
		ਅੰਦਰੂਨੀ_flag = true;
	}

	ਬਣਤਰ pre_post_exec_state_t {
		Pass *ਮਾਪਾ_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	ਖਾਲੀ post_execute(pre_post_exec_state_t state);

	ਖਾਲੀ cmd_log_ਦਲੀਲਾਂ(ਅਚਲ std::ਸਦਿਸ਼<std::string> &ਦਲੀਲਾਂ_2);
	ਖਾਲੀ cmd_error(ਅਚਲ std::ਸਦਿਸ਼<std::string> &ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, std::string ਸੁਨੇਹਾ);
	ਖਾਲੀ extra_ਦਲੀਲਾਂ(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, RTLIL::Design *design, bool select = true);

	ਸਥਿਰ ਖਾਲੀ call(RTLIL::Design *design, std::string command);
	ਸਥਿਰ ਖਾਲੀ call(RTLIL::Design *design, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2);

	ਸਥਿਰ ਖਾਲੀ call_on_selection(RTLIL::Design *design, ਅਚਲ RTLIL::Selection &selection, std::string command);
	ਸਥਿਰ ਖਾਲੀ call_on_selection(RTLIL::Design *design, ਅਚਲ RTLIL::Selection &selection, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2);

	ਸਥਿਰ ਖਾਲੀ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ਸਥਿਰ ਖਾਲੀ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2);

	Pass *ਅਗਲਾ_queued_pass;
	ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਰਜਿਸਟਰ();
	ਸਥਿਰ ਖਾਲੀ init_ਰਜਿਸਟਰ();
	ਸਥਿਰ ਖਾਲੀ ਪੂਰਾ_ਰਜਿਸਟਰ();

	ਆਭਾਸੀ ਖਾਲੀ on_ਰਜਿਸਟਰ();
	ਆਭਾਸੀ ਖਾਲੀ on_shutdown();
	ਆਭਾਸੀ bool replace_existing_pass() ਅਚਲ { ਵਾਪਸ false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	ਆਭਾਸੀ bool allow_garbage_collection_during_pass() ਅਚਲ { ਵਾਪਸ false; }
};

ਬਣਤਰ ScriptPass : Pass
{
	bool ਖੰਡ_ਸਰਗਰਮ, help_ਬਹੁਲਕ;
	RTLIL::Design *ਸਰਗਰਮ_design;
	std::string ਸਰਗਰਮ_ਲੜੀਵਾਰ_from, ਸਰਗਰਮ_ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ;

	ScriptPass(std::string name, std::string short_help = "** document me **", ਸਰੋਤ_location location = ਸਰੋਤ_location::current()) :
		Pass(name, short_help, location) { }

	ਆਭਾਸੀ ਖਾਲੀ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	ਖਾਲੀ ਲੜੀਵਾਰ(std::string command, std::string info = std::string());
	ਖਾਲੀ ਲੜੀਵਾਰ_nocheck(std::string command, std::string info = std::string());
	ਖਾਲੀ ਲੜੀਵਾਰ_script(RTLIL::Design *design, std::string ਲੜੀਵਾਰ_from = std::string(), std::string ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ = std::string());
	ਖਾਲੀ help_script();

	bool allow_garbage_collection_during_pass() ਅਚਲ override { ਵਾਪਸ true; }
};

ਬਣਤਰ Frontend : Pass
{
	// for reading of here documents
	ਸਥਿਰ FILE *current_script_file;
	ਸਥਿਰ std::string ਆਖਰੀ_ਇੱਥੇ_document;

	// script file and last line read from it
	ਸਥਿਰ std::string current_script_filename;
	ਸਥਿਰ ਪੂਰਨ current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		ਸਰੋਤ_location location = ਸਰੋਤ_location::current());
	ਖਾਲੀ ਲੜੀਵਾਰ_ਰਜਿਸਟਰ() override;
	~Frontend() override;
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) override final;
	ਆਭਾਸੀ ਖਾਲੀ execute(std::istream *&f, std::string filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) = 0;

	ਸਥਿਰ std::ਸਦਿਸ਼<std::string> ਅਗਲਾ_ਦਲੀਲਾਂ;
	ਖਾਲੀ extra_ਦਲੀਲਾਂ(std::istream *&f, std::string &filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, bool bin_ਨਿਵੇਸ਼ = false);

	ਸਥਿਰ ਖਾਲੀ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ਸਥਿਰ ਖਾਲੀ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2);
};

ਬਣਤਰ Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		ਸਰੋਤ_location location = ਸਰੋਤ_location::current());
	ਖਾਲੀ ਲੜੀਵਾਰ_ਰਜਿਸਟਰ() override;
	~Backend() override;
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) override final;
	ਆਭਾਸੀ ਖਾਲੀ execute(std::ostream *&f, std::string filename,  std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design) = 0;

	ਖਾਲੀ extra_ਦਲੀਲਾਂ(std::ostream *&f, std::string &filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, bool bin_output = false);

	ਸਥਿਰ ਖਾਲੀ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ਸਥਿਰ ਖਾਲੀ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2);
};

// implemented in passes/cmds/select.cc
ਬਾਹਰੀ ਖਾਲੀ handle_extra_select_ਦਲੀਲਾਂ(Pass *pass, ਅਚਲ std::ਸਦਿਸ਼<std::string> &ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, ਮਾਪ_t ਦਲੀਲਾਂ_ਮਾਪ, RTLIL::Design *design);
ਬਾਹਰੀ RTLIL::Selection eval_select_ਦਲੀਲਾਂ(ਅਚਲ ਸਦਿਸ਼<string> &ਦਲੀਲਾਂ_2, RTLIL::Design *design);
ਬਾਹਰੀ ਖਾਲੀ eval_select_op(ਸਦਿਸ਼<RTLIL::Selection> &work, ਅਚਲ string &op, RTLIL::Design *design);

ਬਾਹਰੀ std::map<std::string, Pass*> pass_ਰਜਿਸਟਰ;
ਬਾਹਰੀ std::map<std::string, Frontend*> frontend_ਰਜਿਸਟਰ;
ਬਾਹਰੀ std::map<std::string, Backend*> backend_ਰਜਿਸਟਰ;

ਬਣਤਰ ਨਿਸ਼ਾਨਾ_Family {
	std::string description;
	std::ਸਦਿਸ਼<std::string> files;
};

ਬਣਤਰ ਨਿਸ਼ਾਨਾ {
	std::string description;
	std::map<std::string, ਨਿਸ਼ਾਨਾ_Family> families;
};

ਬਾਹਰੀ std::map<std::string, ਨਿਸ਼ਾਨਾ> ਨਿਸ਼ਾਨਾ_ਰਜਿਸਟਰ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_HAS_ਨਿਸ਼ਾਨਾ_ਰਜਿਸਟਰ 1

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

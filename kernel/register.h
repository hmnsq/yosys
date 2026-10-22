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

#اگر_تعریف_نشده ثبات_H
#تعریف_کن ثبات_H

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
	منبع_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		منبع_location location = منبع_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	مجازی ~Pass();

	// Makes calls to log() to generate help message
	مجازی فراغ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	مجازی bool formatted_help();
	مجازی فراغ clear_flags();
	مجازی فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) = 0;

	صحيح call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool داخلی_flag = false;

	ساكن فراغ تفریق_from_current_runtime_ns(int64_t زمان_ns);

	فراغ experimental() {
		experimental_flag = true;
	}

	فراغ داخلی() {
		داخلی_flag = true;
	}

	بنية pre_post_exec_state_t {
		Pass *والد_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	فراغ post_execute(pre_post_exec_state_t state);

	فراغ cmd_log_آرگومانها(ثابت std::وکتور<std::string> &آرگومانها_2);
	فراغ cmd_error(ثابت std::وکتور<std::string> &آرگومانها_2, اندازه_t argidx, std::string پیام);
	فراغ extra_آرگومانها(std::وکتور<std::string> آرگومانها_2, اندازه_t argidx, RTLIL::Design *design, bool select = true);

	ساكن فراغ call(RTLIL::Design *design, std::string command);
	ساكن فراغ call(RTLIL::Design *design, std::وکتور<std::string> آرگومانها_2);

	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::وکتور<std::string> آرگومانها_2);

	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::وکتور<std::string> آرگومانها_2);

	Pass *بعدی_queued_pass;
	مجازی فراغ پیاپی_ثبات();
	ساكن فراغ init_ثبات();
	ساكن فراغ انجامشده_ثبات();

	مجازی فراغ on_ثبات();
	مجازی فراغ on_shutdown();
	مجازی bool replace_existing_pass() ثابت { أرجع false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	مجازی bool allow_garbage_collection_during_pass() ثابت { أرجع false; }
};

بنية ScriptPass : Pass
{
	bool بلوک_فعال, help_نما;
	RTLIL::Design *فعال_design;
	std::string فعال_پیاپی_from, فعال_پیاپی_مقصد;

	ScriptPass(std::string name, std::string short_help = "** document me **", منبع_location location = منبع_location::current()) :
		Pass(name, short_help, location) { }

	مجازی فراغ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	فراغ پیاپی(std::string command, std::string info = std::string());
	فراغ پیاپی_nocheck(std::string command, std::string info = std::string());
	فراغ پیاپی_script(RTLIL::Design *design, std::string پیاپی_from = std::string(), std::string پیاپی_مقصد = std::string());
	فراغ help_script();

	bool allow_garbage_collection_during_pass() ثابت override { أرجع true; }
};

بنية Frontend : Pass
{
	// for reading of here documents
	ساكن FILE *current_script_file;
	ساكن std::string آخرین_اینجا_document;

	// script file and last line read from it
	ساكن std::string current_script_filename;
	ساكن صحيح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		منبع_location location = منبع_location::current());
	فراغ پیاپی_ثبات() override;
	~Frontend() override;
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) override final;
	مجازی فراغ execute(std::istream *&f, std::string filename, std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) = 0;

	ساكن std::وکتور<std::string> بعدی_آرگومانها;
	فراغ extra_آرگومانها(std::istream *&f, std::string &filename, std::وکتور<std::string> آرگومانها_2, اندازه_t argidx, bool bin_درون_داد = false);

	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::وکتور<std::string> آرگومانها_2);
};

بنية Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		منبع_location location = منبع_location::current());
	فراغ پیاپی_ثبات() override;
	~Backend() override;
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) override final;
	مجازی فراغ execute(std::ostream *&f, std::string filename,  std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design) = 0;

	فراغ extra_آرگومانها(std::ostream *&f, std::string &filename, std::وکتور<std::string> آرگومانها_2, اندازه_t argidx, bool bin_output = false);

	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::وکتور<std::string> آرگومانها_2);
};

// implemented in passes/cmds/select.cc
خارجي فراغ handle_extra_select_آرگومانها(Pass *pass, ثابت std::وکتور<std::string> &آرگومانها_2, اندازه_t argidx, اندازه_t آرگومانها_اندازه, RTLIL::Design *design);
خارجي RTLIL::Selection eval_select_آرگومانها(ثابت وکتور<string> &آرگومانها_2, RTLIL::Design *design);
خارجي فراغ eval_select_op(وکتور<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

خارجي std::map<std::string, Pass*> pass_ثبات;
خارجي std::map<std::string, Frontend*> frontend_ثبات;
خارجي std::map<std::string, Backend*> backend_ثبات;

بنية هدف_Family {
	std::string description;
	std::وکتور<std::string> files;
};

بنية هدف {
	std::string description;
	std::map<std::string, هدف_Family> families;
};

خارجي std::map<std::string, هدف> هدف_ثبات;

#تعریف_کن YOSYS_HAS_هدف_ثبات 1

YOSYS_NAMESPACE_انتها

#پایان_اگر

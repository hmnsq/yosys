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

#اگر_غیر_تعریف_شدہ رجسٹر_H
#تعریف_کرو رجسٹر_H

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
	ساکن bool is_enabled_;
عوامی:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ساکن bool is_enabled() { واپس is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
خالی request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
خالی try_collect_garbage();

ساخت Pass
{
	std::string pass_name, short_help;
	ماخذ_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		ماخذ_location location = ماخذ_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	مجازی ~Pass();

	// Makes calls to log() to generate help message
	مجازی خالی help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	مجازی bool formatted_help();
	مجازی خالی clear_flags();
	مجازی خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) = 0;

	صحیح call_شمار;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool اندرونی_flag = false;

	ساکن خالی تفریق_from_current_runtime_ns(int64_t وقت_ns);

	خالی experimental() {
		experimental_flag = true;
	}

	خالی اندرونی_2() {
		اندرونی_flag = true;
	}

	ساخت pre_post_exec_state_t {
		Pass *والد_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	خالی post_execute(pre_post_exec_state_t state);

	خالی cmd_log_دلائل(ثابت std::سمتیہ<std::string> &دلائل_2);
	خالی cmd_error(ثابت std::سمتیہ<std::string> &دلائل_2, سائز_t argidx, std::string پیغام);
	خالی extra_دلائل(std::سمتیہ<std::string> دلائل_2, سائز_t argidx, RTLIL::Design *design, bool select = true);

	ساکن خالی call(RTLIL::Design *design, std::string command);
	ساکن خالی call(RTLIL::Design *design, std::سمتیہ<std::string> دلائل_2);

	ساکن خالی call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ساکن خالی call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::سمتیہ<std::string> دلائل_2);

	ساکن خالی call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ساکن خالی call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::سمتیہ<std::string> دلائل_2);

	Pass *اگلا_queued_pass;
	مجازی خالی تسلسل_رجسٹر();
	ساکن خالی init_رجسٹر();
	ساکن خالی مکمل_رجسٹر();

	مجازی خالی on_رجسٹر();
	مجازی خالی on_shutdown();
	مجازی bool replace_existing_pass() ثابت { واپس false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	مجازی bool allow_garbage_collection_during_pass() ثابت { واپس false; }
};

ساخت ScriptPass : Pass
{
	bool بلاک_فعال, help_منوال;
	RTLIL::Design *فعال_design;
	std::string فعال_تسلسل_from, فعال_تسلسل_مقصد;

	ScriptPass(std::string name, std::string short_help = "** document me **", ماخذ_location location = ماخذ_location::current()) :
		Pass(name, short_help, location) { }

	مجازی خالی script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	خالی تسلسل(std::string command, std::string info = std::string());
	خالی تسلسل_nocheck(std::string command, std::string info = std::string());
	خالی تسلسل_script(RTLIL::Design *design, std::string تسلسل_from = std::string(), std::string تسلسل_مقصد = std::string());
	خالی help_script();

	bool allow_garbage_collection_during_pass() ثابت override { واپس true; }
};

ساخت Frontend : Pass
{
	// for reading of here documents
	ساکن FILE *current_script_file;
	ساکن std::string آخری_یہاں_document;

	// script file and last line read from it
	ساکن std::string current_script_filename;
	ساکن صحیح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		ماخذ_location location = ماخذ_location::current());
	خالی تسلسل_رجسٹر() override;
	~Frontend() override;
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) override final;
	مجازی خالی execute(std::istream *&f, std::string filename, std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) = 0;

	ساکن std::سمتیہ<std::string> اگلا_دلائل;
	خالی extra_دلائل(std::istream *&f, std::string &filename, std::سمتیہ<std::string> دلائل_2, سائز_t argidx, bool bin_داخلہ = false);

	ساکن خالی frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ساکن خالی frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::سمتیہ<std::string> دلائل_2);
};

ساخت Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		ماخذ_location location = ماخذ_location::current());
	خالی تسلسل_رجسٹر() override;
	~Backend() override;
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) override final;
	مجازی خالی execute(std::ostream *&f, std::string filename,  std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design) = 0;

	خالی extra_دلائل(std::ostream *&f, std::string &filename, std::سمتیہ<std::string> دلائل_2, سائز_t argidx, bool bin_output = false);

	ساکن خالی backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ساکن خالی backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::سمتیہ<std::string> دلائل_2);
};

// implemented in passes/cmds/select.cc
خارجی خالی handle_extra_select_دلائل(Pass *pass, ثابت std::سمتیہ<std::string> &دلائل_2, سائز_t argidx, سائز_t دلائل_سائز, RTLIL::Design *design);
خارجی RTLIL::Selection eval_select_دلائل(ثابت سمتیہ<string> &دلائل_2, RTLIL::Design *design);
خارجی خالی eval_select_op(سمتیہ<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

خارجی std::map<std::string, Pass*> pass_رجسٹر;
خارجی std::map<std::string, Frontend*> frontend_رجسٹر;
خارجی std::map<std::string, Backend*> backend_رجسٹر;

ساخت ہدف_Family {
	std::string description;
	std::سمتیہ<std::string> files;
};

ساخت ہدف {
	std::string description;
	std::map<std::string, ہدف_Family> families;
};

خارجی std::map<std::string, ہدف> ہدف_رجسٹر;

#تعریف_کرو YOSYS_HAS_ہدف_رجسٹر 1

YOSYS_NAMESPACE_انتہا

#اگر_ختم

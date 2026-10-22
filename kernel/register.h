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

#جے_بنا_تعریف رجسٹر_H
#تعریف_کرو رجسٹر_H

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
	سروت_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		سروت_location location = سروت_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	مجازی ~Pass();

	// Makes calls to log() to generate help message
	مجازی فراغ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	مجازی bool formatted_help();
	مجازی فراغ clear_flags();
	مجازی فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) = 0;

	صحيح call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool اندرونی_flag = false;

	ساكن فراغ گھٹاؤ_from_current_runtime_ns(int64_t ویلا_ns);

	فراغ experimental() {
		experimental_flag = true;
	}

	فراغ اندرونی() {
		اندرونی_flag = true;
	}

	بنية pre_post_exec_state_t {
		Pass *ماپا_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	فراغ post_execute(pre_post_exec_state_t state);

	فراغ cmd_log_دلیلاں(ثابت std::سمتیہ<std::string> &دلیلاں_2);
	فراغ cmd_error(ثابت std::سمتیہ<std::string> &دلیلاں_2, ناپ_t argidx, std::string پیغام);
	فراغ extra_دلیلاں(std::سمتیہ<std::string> دلیلاں_2, ناپ_t argidx, RTLIL::Design *design, bool select = true);

	ساكن فراغ call(RTLIL::Design *design, std::string command);
	ساكن فراغ call(RTLIL::Design *design, std::سمتیہ<std::string> دلیلاں_2);

	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::سمتیہ<std::string> دلیلاں_2);

	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::سمتیہ<std::string> دلیلاں_2);

	Pass *اگلا_queued_pass;
	مجازی فراغ لگاتار_رجسٹر();
	ساكن فراغ init_رجسٹر();
	ساكن فراغ پورا_رجسٹر();

	مجازی فراغ on_رجسٹر();
	مجازی فراغ on_shutdown();
	مجازی bool replace_existing_pass() ثابت { أرجع false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	مجازی bool allow_garbage_collection_during_pass() ثابت { أرجع false; }
};

بنية ScriptPass : Pass
{
	bool بلاک_سرگرم, help_منوال;
	RTLIL::Design *سرگرم_design;
	std::string سرگرم_لگاتار_from, سرگرم_لگاتار_منزل;

	ScriptPass(std::string name, std::string short_help = "** document me **", سروت_location location = سروت_location::current()) :
		Pass(name, short_help, location) { }

	مجازی فراغ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	فراغ لگاتار(std::string command, std::string info = std::string());
	فراغ لگاتار_nocheck(std::string command, std::string info = std::string());
	فراغ لگاتار_script(RTLIL::Design *design, std::string لگاتار_from = std::string(), std::string لگاتار_منزل = std::string());
	فراغ help_script();

	bool allow_garbage_collection_during_pass() ثابت override { أرجع true; }
};

بنية Frontend : Pass
{
	// for reading of here documents
	ساكن FILE *current_script_file;
	ساكن std::string آخری_ایتھے_document;

	// script file and last line read from it
	ساكن std::string current_script_filename;
	ساكن صحيح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		سروت_location location = سروت_location::current());
	فراغ لگاتار_رجسٹر() override;
	~Frontend() override;
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) override final;
	مجازی فراغ execute(std::istream *&f, std::string filename, std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) = 0;

	ساكن std::سمتیہ<std::string> اگلا_دلیلاں;
	فراغ extra_دلیلاں(std::istream *&f, std::string &filename, std::سمتیہ<std::string> دلیلاں_2, ناپ_t argidx, bool bin_داخلہ = false);

	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::سمتیہ<std::string> دلیلاں_2);
};

بنية Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		سروت_location location = سروت_location::current());
	فراغ لگاتار_رجسٹر() override;
	~Backend() override;
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) override final;
	مجازی فراغ execute(std::ostream *&f, std::string filename,  std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design) = 0;

	فراغ extra_دلیلاں(std::ostream *&f, std::string &filename, std::سمتیہ<std::string> دلیلاں_2, ناپ_t argidx, bool bin_output = false);

	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::سمتیہ<std::string> دلیلاں_2);
};

// implemented in passes/cmds/select.cc
خارجي فراغ handle_extra_select_دلیلاں(Pass *pass, ثابت std::سمتیہ<std::string> &دلیلاں_2, ناپ_t argidx, ناپ_t دلیلاں_ناپ, RTLIL::Design *design);
خارجي RTLIL::Selection eval_select_دلیلاں(ثابت سمتیہ<string> &دلیلاں_2, RTLIL::Design *design);
خارجي فراغ eval_select_op(سمتیہ<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

خارجي std::map<std::string, Pass*> pass_رجسٹر;
خارجي std::map<std::string, Frontend*> frontend_رجسٹر;
خارجي std::map<std::string, Backend*> backend_رجسٹر;

بنية نشانہ_Family {
	std::string description;
	std::سمتیہ<std::string> files;
};

بنية نشانہ {
	std::string description;
	std::map<std::string, نشانہ_Family> families;
};

خارجي std::map<std::string, نشانہ> نشانہ_رجسٹر;

#تعریف_کرو YOSYS_HAS_نشانہ_رجسٹر 1

YOSYS_NAMESPACE_سرا

#جے_انت

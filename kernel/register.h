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

#جيڪڏهن_وصف_نه_ٿيل رجسٽر_H
#وصف_ڪريو رجسٽر_H

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
	ذريعو_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		ذريعو_location location = ذريعو_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	مجازي ~Pass();

	// Makes calls to log() to generate help message
	مجازي فراغ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	مجازي bool formatted_help();
	مجازي فراغ clear_flags();
	مجازي فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) = 0;

	صحيح call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool اندروني_flag = false;

	ساكن فراغ ڪٽيو_from_current_runtime_ns(int64_t وقت_ns);

	فراغ experimental() {
		experimental_flag = true;
	}

	فراغ اندروني() {
		اندروني_flag = true;
	}

	بنية pre_post_exec_state_t {
		Pass *والد_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	فراغ post_execute(pre_post_exec_state_t state);

	فراغ cmd_log_دليلون(ثابت std::ويڪٽر<std::string> &دليلون_2);
	فراغ cmd_error(ثابت std::ويڪٽر<std::string> &دليلون_2, سائيز_t argidx, std::string نياپو);
	فراغ extra_دليلون(std::ويڪٽر<std::string> دليلون_2, سائيز_t argidx, RTLIL::Design *design, bool select = true);

	ساكن فراغ call(RTLIL::Design *design, std::string command);
	ساكن فراغ call(RTLIL::Design *design, std::ويڪٽر<std::string> دليلون_2);

	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::ويڪٽر<std::string> دليلون_2);

	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ويڪٽر<std::string> دليلون_2);

	Pass *ايندڙ_queued_pass;
	مجازي فراغ لاڳيتو_رجسٽر();
	ساكن فراغ init_رجسٽر();
	ساكن فراغ پورو_رجسٽر();

	مجازي فراغ on_رجسٽر();
	مجازي فراغ on_shutdown();
	مجازي bool replace_existing_pass() ثابت { أرجع false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	مجازي bool allow_garbage_collection_during_pass() ثابت { أرجع false; }
};

بنية ScriptPass : Pass
{
	bool بلاڪ_سرگرم, help_گهڻائي;
	RTLIL::Design *سرگرم_design;
	std::string سرگرم_لاڳيتو_from, سرگرم_لاڳيتو_منزل;

	ScriptPass(std::string name, std::string short_help = "** document me **", ذريعو_location location = ذريعو_location::current()) :
		Pass(name, short_help, location) { }

	مجازي فراغ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	فراغ لاڳيتو(std::string command, std::string info = std::string());
	فراغ لاڳيتو_nocheck(std::string command, std::string info = std::string());
	فراغ لاڳيتو_script(RTLIL::Design *design, std::string لاڳيتو_from = std::string(), std::string لاڳيتو_منزل = std::string());
	فراغ help_script();

	bool allow_garbage_collection_during_pass() ثابت override { أرجع true; }
};

بنية Frontend : Pass
{
	// for reading of here documents
	ساكن FILE *current_script_file;
	ساكن std::string آخري_هتي_document;

	// script file and last line read from it
	ساكن std::string current_script_filename;
	ساكن صحيح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		ذريعو_location location = ذريعو_location::current());
	فراغ لاڳيتو_رجسٽر() override;
	~Frontend() override;
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) override final;
	مجازي فراغ execute(std::istream *&f, std::string filename, std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) = 0;

	ساكن std::ويڪٽر<std::string> ايندڙ_دليلون;
	فراغ extra_دليلون(std::istream *&f, std::string &filename, std::ويڪٽر<std::string> دليلون_2, سائيز_t argidx, bool bin_داخلا = false);

	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ويڪٽر<std::string> دليلون_2);
};

بنية Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		ذريعو_location location = ذريعو_location::current());
	فراغ لاڳيتو_رجسٽر() override;
	~Backend() override;
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) override final;
	مجازي فراغ execute(std::ostream *&f, std::string filename,  std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design) = 0;

	فراغ extra_دليلون(std::ostream *&f, std::string &filename, std::ويڪٽر<std::string> دليلون_2, سائيز_t argidx, bool bin_output = false);

	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ويڪٽر<std::string> دليلون_2);
};

// implemented in passes/cmds/select.cc
خارجي فراغ handle_extra_select_دليلون(Pass *pass, ثابت std::ويڪٽر<std::string> &دليلون_2, سائيز_t argidx, سائيز_t دليلون_سائيز, RTLIL::Design *design);
خارجي RTLIL::Selection eval_select_دليلون(ثابت ويڪٽر<string> &دليلون_2, RTLIL::Design *design);
خارجي فراغ eval_select_op(ويڪٽر<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

خارجي std::map<std::string, Pass*> pass_رجسٽر;
خارجي std::map<std::string, Frontend*> frontend_رجسٽر;
خارجي std::map<std::string, Backend*> backend_رجسٽر;

بنية هدف_Family {
	std::string description;
	std::ويڪٽر<std::string> files;
};

بنية هدف {
	std::string description;
	std::map<std::string, هدف_Family> families;
};

خارجي std::map<std::string, هدف> هدف_رجسٽر;

#وصف_ڪريو YOSYS_HAS_هدف_رجسٽر 1

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو

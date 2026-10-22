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

#که_نه_تعریف_شوی ثبتوونکی_H
#تعریف_کړه ثبتوونکی_H

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
	سرچینه_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		سرچینه_location location = سرچینه_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	مجازي ~Pass();

	// Makes calls to log() to generate help message
	مجازي فراغ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	مجازي bool formatted_help();
	مجازي فراغ clear_flags();
	مجازي فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) = 0;

	صحيح call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool دننی_flag = false;

	ساكن فراغ کمول_from_current_runtime_ns(int64_t وخت_ns);

	فراغ experimental() {
		experimental_flag = true;
	}

	فراغ دننی() {
		دننی_flag = true;
	}

	بنية pre_post_exec_state_t {
		Pass *پلار_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	فراغ post_execute(pre_post_exec_state_t state);

	فراغ cmd_log_دلیلونه(ثابت std::ویکټور<std::string> &دلیلونه_2);
	فراغ cmd_error(ثابت std::ویکټور<std::string> &دلیلونه_2, اندازه_t argidx, std::string پیغام);
	فراغ extra_دلیلونه(std::ویکټور<std::string> دلیلونه_2, اندازه_t argidx, RTLIL::Design *design, bool select = true);

	ساكن فراغ call(RTLIL::Design *design, std::string command);
	ساكن فراغ call(RTLIL::Design *design, std::ویکټور<std::string> دلیلونه_2);

	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command);
	ساكن فراغ call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::ویکټور<std::string> دلیلونه_2);

	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ساكن فراغ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ویکټور<std::string> دلیلونه_2);

	Pass *بل_queued_pass;
	مجازي فراغ پرلپسې_ثبتوونکی();
	ساكن فراغ init_ثبتوونکی();
	ساكن فراغ بشپړ_ثبتوونکی();

	مجازي فراغ on_ثبتوونکی();
	مجازي فراغ on_shutdown();
	مجازي bool replace_existing_pass() ثابت { أرجع false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	مجازي bool allow_garbage_collection_during_pass() ثابت { أرجع false; }
};

بنية ScriptPass : Pass
{
	bool بلاک_فعال, help_ډېرپېښ;
	RTLIL::Design *فعال_design;
	std::string فعال_پرلپسې_from, فعال_پرلپسې_منزل;

	ScriptPass(std::string name, std::string short_help = "** document me **", سرچینه_location location = سرچینه_location::current()) :
		Pass(name, short_help, location) { }

	مجازي فراغ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	فراغ پرلپسې(std::string command, std::string info = std::string());
	فراغ پرلپسې_nocheck(std::string command, std::string info = std::string());
	فراغ پرلپسې_script(RTLIL::Design *design, std::string پرلپسې_from = std::string(), std::string پرلپسې_منزل = std::string());
	فراغ help_script();

	bool allow_garbage_collection_during_pass() ثابت override { أرجع true; }
};

بنية Frontend : Pass
{
	// for reading of here documents
	ساكن FILE *current_script_file;
	ساكن std::string وروستی_دلته_document;

	// script file and last line read from it
	ساكن std::string current_script_filename;
	ساكن صحيح current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		سرچینه_location location = سرچینه_location::current());
	فراغ پرلپسې_ثبتوونکی() override;
	~Frontend() override;
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) override final;
	مجازي فراغ execute(std::istream *&f, std::string filename, std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) = 0;

	ساكن std::ویکټور<std::string> بل_دلیلونه;
	فراغ extra_دلیلونه(std::istream *&f, std::string &filename, std::ویکټور<std::string> دلیلونه_2, اندازه_t argidx, bool bin_ننوت = false);

	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ساكن فراغ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ویکټور<std::string> دلیلونه_2);
};

بنية Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		سرچینه_location location = سرچینه_location::current());
	فراغ پرلپسې_ثبتوونکی() override;
	~Backend() override;
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) override final;
	مجازي فراغ execute(std::ostream *&f, std::string filename,  std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design) = 0;

	فراغ extra_دلیلونه(std::ostream *&f, std::string &filename, std::ویکټور<std::string> دلیلونه_2, اندازه_t argidx, bool bin_output = false);

	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ساكن فراغ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ویکټور<std::string> دلیلونه_2);
};

// implemented in passes/cmds/select.cc
خارجي فراغ handle_extra_select_دلیلونه(Pass *pass, ثابت std::ویکټور<std::string> &دلیلونه_2, اندازه_t argidx, اندازه_t دلیلونه_اندازه, RTLIL::Design *design);
خارجي RTLIL::Selection eval_select_دلیلونه(ثابت ویکټور<string> &دلیلونه_2, RTLIL::Design *design);
خارجي فراغ eval_select_op(ویکټور<RTLIL::Selection> &work, ثابت string &op, RTLIL::Design *design);

خارجي std::map<std::string, Pass*> pass_ثبتوونکی;
خارجي std::map<std::string, Frontend*> frontend_ثبتوونکی;
خارجي std::map<std::string, Backend*> backend_ثبتوونکی;

بنية موخه_Family {
	std::string description;
	std::ویکټور<std::string> files;
};

بنية موخه {
	std::string description;
	std::map<std::string, موخه_Family> families;
};

خارجي std::map<std::string, موخه> موخه_ثبتوونکی;

#تعریف_کړه YOSYS_HAS_موخه_ثبتوونکی 1

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای

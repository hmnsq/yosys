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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ರಿಜಿಸ್ಟರ್_H
#ವ್ಯಾಖ್ಯಾನಿಸು ರಿಜಿಸ್ಟರ್_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	ಸ್ಥಿರ bool is_enabled_;
ಸಾರ್ವಜನಿಕ:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	ಸ್ಥಿರ bool is_enabled() { ಹಿಂತಿರುಗಿ is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
ಶೂನ್ಯ request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
ಶೂನ್ಯ try_collect_garbage();

ರಚನೆ Pass
{
	std::string pass_name, short_help;
	ಮೂಲ_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		ಮೂಲ_location location = ಮೂಲ_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	ವಾಸ್ತವಿಕ ~Pass();

	// Makes calls to log() to generate help message
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	ವಾಸ್ತವಿಕ bool formatted_help();
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ clear_flags();
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) = 0;

	ಪೂರ್ಣಾಂಕ call_counter;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool ಆಂತರಿಕ_flag = false;

	ಸ್ಥಿರ ಶೂನ್ಯ ಕಳೆ_from_current_runtime_ns(int64_t ಸಮಯ_ns);

	ಶೂನ್ಯ experimental() {
		experimental_flag = true;
	}

	ಶೂನ್ಯ ಆಂತರಿಕ() {
		ಆಂತರಿಕ_flag = true;
	}

	ರಚನೆ pre_post_exec_state_t {
		Pass *ಪೋಷಕ_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	ಶೂನ್ಯ post_execute(pre_post_exec_state_t state);

	ಶೂನ್ಯ cmd_log_ವಾದಗಳು(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string> &ವಾದಗಳು_2);
	ಶೂನ್ಯ cmd_error(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string> &ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, std::string ಸಂದೇಶ);
	ಶೂನ್ಯ extra_ವಾದಗಳು(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, RTLIL::Design *design, bool select = true);

	ಸ್ಥಿರ ಶೂನ್ಯ call(RTLIL::Design *design, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ call(RTLIL::Design *design, std::ಸದಿಶ<std::string> ವಾದಗಳು_2);

	ಸ್ಥಿರ ಶೂನ್ಯ call_on_selection(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ RTLIL::Selection &selection, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ call_on_selection(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ RTLIL::Selection &selection, std::ಸದಿಶ<std::string> ವಾದಗಳು_2);

	ಸ್ಥಿರ ಶೂನ್ಯ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ಸದಿಶ<std::string> ವಾದಗಳು_2);

	Pass *ಮುಂದಿನ_queued_pass;
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ರಿಜಿಸ್ಟರ್();
	ಸ್ಥಿರ ಶೂನ್ಯ init_ರಿಜಿಸ್ಟರ್();
	ಸ್ಥಿರ ಶೂನ್ಯ ಮುಗಿದ_ರಿಜಿಸ್ಟರ್();

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ on_ರಿಜಿಸ್ಟರ್();
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ on_shutdown();
	ವಾಸ್ತವಿಕ bool replace_existing_pass() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	ವಾಸ್ತವಿಕ bool allow_garbage_collection_during_pass() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ false; }
};

ರಚನೆ ScriptPass : Pass
{
	bool ಖಂಡ_ಸಕ್ರಿಯ, help_ಬಹುಲಕ;
	RTLIL::Design *ಸಕ್ರಿಯ_design;
	std::string ಸಕ್ರಿಯ_ಸತತ_from, ಸಕ್ರಿಯ_ಸತತ_ಗಮ್ಯ;

	ScriptPass(std::string name, std::string short_help = "** document me **", ಮೂಲ_location location = ಮೂಲ_location::current()) :
		Pass(name, short_help, location) { }

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	ಶೂನ್ಯ ಸತತ(std::string command, std::string info = std::string());
	ಶೂನ್ಯ ಸತತ_nocheck(std::string command, std::string info = std::string());
	ಶೂನ್ಯ ಸತತ_script(RTLIL::Design *design, std::string ಸತತ_from = std::string(), std::string ಸತತ_ಗಮ್ಯ = std::string());
	ಶೂನ್ಯ help_script();

	bool allow_garbage_collection_during_pass() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ true; }
};

ರಚನೆ Frontend : Pass
{
	// for reading of here documents
	ಸ್ಥಿರ FILE *current_script_file;
	ಸ್ಥಿರ std::string ಕೊನೆಯ_ಇಲ್ಲಿ_document;

	// script file and last line read from it
	ಸ್ಥಿರ std::string current_script_filename;
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		ಮೂಲ_location location = ಮೂಲ_location::current());
	ಶೂನ್ಯ ಸತತ_ರಿಜಿಸ್ಟರ್() override;
	~Frontend() override;
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) override final;
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ execute(std::istream *&f, std::string filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) = 0;

	ಸ್ಥಿರ std::ಸದಿಶ<std::string> ಮುಂದಿನ_ವಾದಗಳು;
	ಶೂನ್ಯ extra_ವಾದಗಳು(std::istream *&f, std::string &filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, bool bin_ಒಳಹರಿವು = false);

	ಸ್ಥಿರ ಶೂನ್ಯ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2);
};

ರಚನೆ Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		ಮೂಲ_location location = ಮೂಲ_location::current());
	ಶೂನ್ಯ ಸತತ_ರಿಜಿಸ್ಟರ್() override;
	~Backend() override;
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) override final;
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ execute(std::ostream *&f, std::string filename,  std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design) = 0;

	ಶೂನ್ಯ extra_ವಾದಗಳು(std::ostream *&f, std::string &filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, bool bin_output = false);

	ಸ್ಥಿರ ಶೂನ್ಯ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	ಸ್ಥಿರ ಶೂನ್ಯ backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2);
};

// implemented in passes/cmds/select.cc
ಬಾಹ್ಯ ಶೂನ್ಯ handle_extra_select_ವಾದಗಳು(Pass *pass, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string> &ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, ಗಾತ್ರ_t ವಾದಗಳು_ಗಾತ್ರ, RTLIL::Design *design);
ಬಾಹ್ಯ RTLIL::Selection eval_select_ವಾದಗಳು(ಸ್ಥಿರಾಂಕ ಸದಿಶ<string> &ವಾದಗಳು_2, RTLIL::Design *design);
ಬಾಹ್ಯ ಶೂನ್ಯ eval_select_op(ಸದಿಶ<RTLIL::Selection> &work, ಸ್ಥಿರಾಂಕ string &op, RTLIL::Design *design);

ಬಾಹ್ಯ std::map<std::string, Pass*> pass_ರಿಜಿಸ್ಟರ್;
ಬಾಹ್ಯ std::map<std::string, Frontend*> frontend_ರಿಜಿಸ್ಟರ್;
ಬಾಹ್ಯ std::map<std::string, Backend*> backend_ರಿಜಿಸ್ಟರ್;

ರಚನೆ ಗುರಿ_Family {
	std::string description;
	std::ಸದಿಶ<std::string> files;
};

ರಚನೆ ಗುರಿ {
	std::string description;
	std::map<std::string, ಗುರಿ_Family> families;
};

ಬಾಹ್ಯ std::map<std::string, ಗುರಿ> ಗುರಿ_ರಿಜಿಸ್ಟರ್;

#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_HAS_ಗುರಿ_ರಿಜಿಸ್ಟರ್ 1

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

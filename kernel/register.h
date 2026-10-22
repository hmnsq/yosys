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

#यदि_अपरिभाषितम् पञ्जिकाकोशः_H
#परिभाषय पञ्जिकाकोशः_H

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Track whether garbage collection is enabled. Garbage collection must be disabled
// while any RTLIL objects (e.g. non-owning non-immortal IdStrings) exist outside Designs.
// Garbage collection is disabled whenever any GarbageCollectionGuard(false) is on the
// stack. These objects must be stack-allocated on the main thread.
class GarbageCollectionGuard
{
	bool was_enabled;
	स्थिर bool is_enabled_;
सार्वजनिक:
	GarbageCollectionGuard(bool allow) : was_enabled(is_enabled_) {
		is_enabled_ &= allow;
	}
	~GarbageCollectionGuard() {
		is_enabled_ = was_enabled;
	}
	स्थिर bool is_enabled() { प्रत्यागम is_enabled_; }
};

// Call from anywhere to request GC at the next safe point.
शून्य request_garbage_collection();

// GC the active design if GarbageCollectionGuard::is_enabled() and GC was requested.
शून्य try_collect_garbage();

संरचना Pass
{
	std::string pass_name, short_help;
	source_location location;
	Pass(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	// Prefer overriding 'Pass::on_shutdown()' if possible
	आभास ~Pass();

	// Makes calls to log() to generate help message
	आभास शून्य help();
	// Uses PrettyHelp::get_current() to produce a more portable formatted help message
	आभास bool formatted_help();
	आभास शून्य clear_flags();
	आभास शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) = 0;

	पूर्णाङ्क call_गणक;
	int64_t runtime_ns;
	bool experimental_flag = false;
	bool आन्तरिकम्_flag = false;

	स्थिर शून्य व्यवकलय_from_current_runtime_ns(int64_t time_ns);

	शून्य experimental() {
		experimental_flag = true;
	}

	शून्य आन्तरिकम्() {
		आन्तरिकम्_flag = true;
	}

	संरचना pre_post_exec_state_t {
		Pass *parent_pass;
		int64_t begin_ns;
	};

	pre_post_exec_state_t pre_execute();
	शून्य post_execute(pre_post_exec_state_t state);

	शून्य cmd_log_तर्काः(ध्रुव std::सदिशम्<std::string> &तर्काः_2);
	शून्य cmd_error(ध्रुव std::सदिशम्<std::string> &तर्काः_2, आकार_t argidx, std::string सन्देशः);
	शून्य extra_तर्काः(std::सदिशम्<std::string> तर्काः_2, आकार_t argidx, RTLIL::Design *design, bool select = true);

	स्थिर शून्य call(RTLIL::Design *design, std::string command);
	स्थिर शून्य call(RTLIL::Design *design, std::सदिशम्<std::string> तर्काः_2);

	स्थिर शून्य call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::string command);
	स्थिर शून्य call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::सदिशम्<std::string> तर्काः_2);

	स्थिर शून्य call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command);
	स्थिर शून्य call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::सदिशम्<std::string> तर्काः_2);

	Pass *अग्र_queued_pass;
	आभास शून्य सततम्_पञ्जिकाकोशः();
	स्थिर शून्य init_पञ्जिकाकोशः();
	स्थिर शून्य समाप्तम्_पञ्जिकाकोशः();

	आभास शून्य on_पञ्जिकाकोशः();
	आभास शून्य on_shutdown();
	आभास bool replace_existing_pass() ध्रुव { प्रत्यागम false; }

	// This should return false if the pass holds onto RTLIL objects outside a Design while it
	// calls nested passes. For safety, we default to assuming the worst.
	आभास bool allow_garbage_collection_during_pass() ध्रुव { प्रत्यागम false; }
};

संरचना ScriptPass : Pass
{
	bool खण्डम्_सक्रिय, help_बहुलकम्;
	RTLIL::Design *सक्रिय_design;
	std::string सक्रिय_सततम्_from, सक्रिय_सततम्_गन्तव्यम्;

	ScriptPass(std::string name, std::string short_help = "** document me **", source_location location = source_location::current()) :
		Pass(name, short_help, location) { }

	आभास शून्य script() = 0;

	bool check_label(std::string label, std::string info = std::string());
	शून्य सततम्(std::string command, std::string info = std::string());
	शून्य सततम्_nocheck(std::string command, std::string info = std::string());
	शून्य सततम्_script(RTLIL::Design *design, std::string सततम्_from = std::string(), std::string सततम्_गन्तव्यम् = std::string());
	शून्य help_script();

	bool allow_garbage_collection_during_pass() ध्रुव override { प्रत्यागम true; }
};

संरचना Frontend : Pass
{
	// for reading of here documents
	स्थिर FILE *current_script_file;
	स्थिर std::string चरमः_अत्र_document;

	// script file and last line read from it
	स्थिर std::string current_script_filename;
	स्थिर पूर्णाङ्क current_script_lineno;

	std::string frontend_name;
	Frontend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	शून्य सततम्_पञ्जिकाकोशः() override;
	~Frontend() override;
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) override final;
	आभास शून्य execute(std::istream *&f, std::string filename, std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) = 0;

	स्थिर std::सदिशम्<std::string> अग्र_तर्काः;
	शून्य extra_तर्काः(std::istream *&f, std::string &filename, std::सदिशम्<std::string> तर्काः_2, आकार_t argidx, bool bin_निवेशः = false);

	स्थिर शून्य frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command);
	स्थिर शून्य frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::सदिशम्<std::string> तर्काः_2);
};

संरचना Backend : Pass
{
	std::string backend_name;
	Backend(std::string name, std::string short_help = "** document me **",
		source_location location = source_location::current());
	शून्य सततम्_पञ्जिकाकोशः() override;
	~Backend() override;
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) override final;
	आभास शून्य execute(std::ostream *&f, std::string filename,  std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design) = 0;

	शून्य extra_तर्काः(std::ostream *&f, std::string &filename, std::सदिशम्<std::string> तर्काः_2, आकार_t argidx, bool bin_output = false);

	स्थिर शून्य backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command);
	स्थिर शून्य backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::सदिशम्<std::string> तर्काः_2);
};

// implemented in passes/cmds/select.cc
बाह्य शून्य handle_extra_select_तर्काः(Pass *pass, ध्रुव std::सदिशम्<std::string> &तर्काः_2, आकार_t argidx, आकार_t तर्काः_आकार, RTLIL::Design *design);
बाह्य RTLIL::Selection eval_select_तर्काः(ध्रुव सदिशम्<string> &तर्काः_2, RTLIL::Design *design);
बाह्य शून्य eval_select_op(सदिशम्<RTLIL::Selection> &work, ध्रुव string &op, RTLIL::Design *design);

बाह्य std::map<std::string, Pass*> pass_पञ्जिकाकोशः;
बाह्य std::map<std::string, Frontend*> frontend_पञ्जिकाकोशः;
बाह्य std::map<std::string, Backend*> backend_पञ्जिकाकोशः;

संरचना TargetFamily {
	std::string description;
	std::सदिशम्<std::string> files;
};

संरचना Target {
	std::string description;
	std::map<std::string, TargetFamily> families;
};

बाह्य std::map<std::string, Target> target_पञ्जिकाकोशः;

#परिभाषय YOSYS_HAS_TARGET_पञ्जिकाकोशः 1

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः

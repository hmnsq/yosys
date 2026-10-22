/*
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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/satgen.h"
#ಅನ್ತರ್ಭಾವ "kernel/json.h"
#ಅನ್ತರ್ಭಾವ "kernel/gzip.h"
#ಅನ್ತರ್ಭಾವ "kernel/log_help.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <filesystem>

YOSYS_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_REG_ಎಣಿಕೆ 1000

ತಾರ್ಕಿಕ echo_ಬಹುಲಕ = ಸುಳ್ಳು;
Pass *ಮೊದಲ_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_ರಿಜಿಸ್ಟರ್;
std::map<std::string, Pass*> pass_ರಿಜಿಸ್ಟರ್;
std::map<std::string, Backend*> backend_ರಿಜಿಸ್ಟರ್;
std::map<std::string, ಗುರಿ> ಗುರಿ_ರಿಜಿಸ್ಟರ್;

std::ಸದಿಶ<std::string> Frontend::ಮುಂದಿನ_ವಾದಗಳು;

ತಾರ್ಕಿಕ GarbageCollectionGuard::is_enabled_ = ನಿಜ;

ಸ್ಥಿರ ತಾರ್ಕಿಕ garbage_collection_requested = ಸುಳ್ಳು;

ಶೂನ್ಯ request_garbage_collection()
{
	garbage_collection_requested = ನಿಜ;
}

ಶೂನ್ಯ try_collect_garbage()
{
	ಆದರೆ (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		ಹಿಂತಿರುಗಿ;
	garbage_collection_requested = ಸುಳ್ಳು;
	ಆದರೆ (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, ಮೂಲ_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	ಮುಂದಿನ_queued_pass = ಮೊದಲ_queued_pass;
	ಮೊದಲ_queued_pass = ಇದು;
	call_counter = 0;
	runtime_ns = 0;
}

ಶೂನ್ಯ Pass::ಸತತ_ರಿಜಿಸ್ಟರ್()
{
	ಆದರೆ (pass_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ರಿಜಿಸ್ಟರ್[pass_name] = ಇದು;
}

ಶೂನ್ಯ Pass::init_ರಿಜಿಸ್ಟರ್()
{
	ಸದಿಶ<Pass*> added_passes;
	ಆಗ (ಮೊದಲ_queued_pass) {
		added_passes.ತಳ್ಳು_ಹಿಂಭಾಗ(ಮೊದಲ_queued_pass);
		ಮೊದಲ_queued_pass->ಸತತ_ರಿಜಿಸ್ಟರ್();
		ಮೊದಲ_queued_pass = ಮೊದಲ_queued_pass->ಮುಂದಿನ_queued_pass;
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ added_pass : added_passes)
		added_pass->on_ರಿಜಿಸ್ಟರ್();
}

ಶೂನ್ಯ Pass::ಮುಗಿದ_ರಿಜಿಸ್ಟರ್()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : pass_ರಿಜಿಸ್ಟರ್)
		it.ಎರಡನೇ->on_shutdown();

	frontend_ರಿಜಿಸ್ಟರ್.clear();
	pass_ರಿಜಿಸ್ಟರ್.clear();
	backend_ರಿಜಿಸ್ಟರ್.clear();
	log_assert(ಮೊದಲ_queued_pass == ಖಾಲಿ);
}

ಶೂನ್ಯ Pass::on_ರಿಜಿಸ್ಟರ್()
{
}

ಶೂನ್ಯ Pass::on_shutdown()
{
}

Pass::~Pass()
{
}

Pass::pre_post_exec_state_t Pass::pre_execute()
{
	pre_post_exec_state_t state;
	call_counter++;
	state.begin_ns = PerformanceTimer::query();
	state.ಪೋಷಕ_pass = current_pass;
	current_pass = ಇದು;
	clear_flags();
	ಹಿಂತಿರುಗಿ state;
}

ಶೂನ್ಯ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t ಸಮಯ_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += ಸಮಯ_ns;
	current_pass = state.ಪೋಷಕ_pass;
	ಕಳೆ_from_current_runtime_ns(ಸಮಯ_ns);
}

ಶೂನ್ಯ Pass::ಕಳೆ_from_current_runtime_ns(int64_t ಸಮಯ_ns)
{
	ಆದರೆ (current_pass)
		current_pass->runtime_ns -= ಸಮಯ_ns;
}

ಶೂನ್ಯ Pass::help()
{
	ಸ್ವಯಂಚಾಲಿತ prettyHelp = PrettyHelp();
	ಆದರೆ (formatted_help()) {
		prettyHelp.log_help();
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("No help message for command `%s'.\n", pass_name);
		ಲಘುಗಣಕ("\n");
	}
}

ತಾರ್ಕಿಕ Pass::formatted_help()
{
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ Pass::clear_flags()
{
}

ಶೂನ್ಯ Pass::cmd_log_ವಾದಗಳು(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string> &ವಾದಗಳು_2)
{
	ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() <= 1)
		ಹಿಂತಿರುಗಿ;
	std::string ಸಂದೇಶ_2 = "Full command line:";
	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < ವಾದಗಳು_2.ಗಾತ್ರ_2(); i++)
		ಸಂದೇಶ_2 += stringf(" %s", ವಾದಗಳು_2[i]);
	ಲಘುಗಣಕ("%s\n", ಸಂದೇಶ_2);
}

ಶೂನ್ಯ Pass::cmd_error(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string> &ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, std::string ಸಂದೇಶ)
{
	std::string command_ಪಠ್ಯ;
	ಪೂರ್ಣಾಂಕ error_pos = 0;

	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < ವಾದಗಳು_2.ಗಾತ್ರ_2(); i++) {
		ಆದರೆ (i < argidx)
			error_pos += ವಾದಗಳು_2[i].ಗಾತ್ರ_2() + 1;
		command_ಪಠ್ಯ = command_ಪಠ್ಯ + (command_ಪಠ್ಯ.ಖಾಲಿಗಳು() ? "" : " ") + ವಾದಗಳು_2[i];
	}

	ಲಘುಗಣಕ("\n");
	ಲಘುಗಣಕ("Syntax error in command `%s':\n", command_ಪಠ್ಯ);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			ಸಂದೇಶ.c_str(), command_ಪಠ್ಯ.c_str(), error_pos, "");
}

ಶೂನ್ಯ Pass::extra_ವಾದಗಳು(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, RTLIL::Design *design, ತಾರ್ಕಿಕ select)
{
	ಗಾಗಿ (; argidx < ವಾದಗಳು_2.ಗಾತ್ರ_2(); argidx++)
	{
		std::string arg = ವಾದಗಳು_2[argidx];

		ಆದರೆ (arg.compare(0, 1, "-") == 0)
			cmd_error(ವಾದಗಳು_2, argidx, "Unknown option or option in arguments.");

		ಆದರೆ (!select)
			cmd_error(ವಾದಗಳು_2, argidx, "Extra argument.");

		handle_extra_select_ವಾದಗಳು(ಇದು, ವಾದಗಳು_2, argidx, ವಾದಗಳು_2.ಗಾತ್ರ_2(), design);
		ನಿಲ್ಲಿಸು;
	}
	// cmd_log_args(args);
}

ಶೂನ್ಯ Pass::call(RTLIL::Design *design, std::string command)
{
	std::ಸದಿಶ<std::string> ವಾದಗಳು_2;

	std::string cmd_buf = command;
	std::string tok = ಮುಂದಿನ_token(cmd_buf, " \t\r\n", ನಿಜ);

	ಆದರೆ (tok.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (tok[0] == '!') {
#ಆದರೆ defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.ಕಂಡುಹಿಡಿ('!') + 1);
		ಆಗ (!cmd_buf.ಖಾಲಿಗಳು() && (cmd_buf.ಹಿಂಭಾಗ() == ' ' || cmd_buf.ಹಿಂಭಾಗ() == '\t' ||
				cmd_buf.ಹಿಂಭಾಗ() == '\r' || cmd_buf.ಹಿಂಭಾಗ() == '\n'))
			cmd_buf.resize(cmd_buf.ಗಾತ್ರ_2()-1);
		log_ಶೀರ್ಷಿಕೆ(design, "Shell command: %s\n", cmd_buf);
		ಪೂರ್ಣಾಂಕ ret_ಸಂಕೇತ = ಸತತ_command(cmd_buf);
		ಆದರೆ (ret_ಸಂಕೇತ != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_ಸಂಕೇತ);
		ಹಿಂತಿರುಗಿ;
#ಇಲ್ಲದಿದ್ದರೆ
		log_cmd_error("Shell is not available.\n");
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆಗ (!tok.ಖಾಲಿಗಳು()) {
		ಆದರೆ (tok[0] == '#') {
			ಪೂರ್ಣಾಂಕ stop;
			ಗಾಗಿ (stop = 0; stop < Get_ಗಾತ್ರ(cmd_buf); stop++)
				ಆದರೆ (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					ನಿಲ್ಲಿಸು;
			cmd_buf = cmd_buf.substr(stop);
		} ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (tok.ಹಿಂಭಾಗ() == ';') {
			ಪೂರ್ಣಾಂಕ ಅಂಕೆ_semikolon = 0;
			ಆಗ (!tok.ಖಾಲಿಗಳು() && tok.ಹಿಂಭಾಗ() == ';')
				tok.resize(tok.ಗಾತ್ರ_2()-1), ಅಂಕೆ_semikolon++;
			ಆದರೆ (!tok.ಖಾಲಿಗಳು())
				ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(tok);
			call(design, ವಾದಗಳು_2);
			ವಾದಗಳು_2.clear();
			ಆದರೆ (ಅಂಕೆ_semikolon == 2)
				call(design, "clean");
			ಆದರೆ (ಅಂಕೆ_semikolon == 3)
				call(design, "clean -purge");
		} ಇಲ್ಲದಿದ್ದರೆ
			ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(tok);
		ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು_nl = ಸುಳ್ಳು;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c : cmd_buf) {
			ಆದರೆ (c == ' ' || c == '\t')
				ಮುಂದುವರಿ;
			ಆದರೆ (c == '\r' || c == '\n')
				ಸಿಕ್ಕಿತು_nl = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (ಸಿಕ್ಕಿತು_nl) {
			call(design, ವಾದಗಳು_2);
			ವಾದಗಳು_2.clear();
		}
		tok = ಮುಂದಿನ_token(cmd_buf, " \t\r\n", ನಿಜ);
	}

	call(design, ವಾದಗಳು_2);
}

ಶೂನ್ಯ Pass::call(RTLIL::Design *design, std::ಸದಿಶ<std::string> ವಾದಗಳು_2)
{
	ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 0 || ವಾದಗಳು_2[0][0] == '#' || ವಾದಗಳು_2[0][0] == ':')
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (echo_ಬಹುಲಕ) {
		std::string ಸಂದೇಶ_2 = stringf("%s", create_prompt(design, 0, ಸುಳ್ಳು));
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < ವಾದಗಳು_2.ಗಾತ್ರ_2(); i++)
			ಸಂದೇಶ_2 += stringf("%s%s", i ? " " : "", ವಾದಗಳು_2[i]);
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("%s\n", ಸಂದೇಶ_2);
	}

	ಆದರೆ (pass_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(ವಾದಗಳು_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", ವಾದಗಳು_2[0]);
	Pass *pass = pass_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	ಆದರೆ (pass->experimental_flag)
		log_experimental(ವಾದಗಳು_2[0]);

	ಗಾತ್ರ_t orig_sel_ರಾಶಿ_pos = design->selection_ರಾಶಿ.ಗಾತ್ರ_2();
	ಸ್ವಯಂಚಾಲಿತ state = pass->pre_execute();
	pass->execute(ವಾದಗಳು_2, design);
	pass->post_execute(state);
	ಆಗ (design->selection_ರಾಶಿ.ಗಾತ್ರ_2() > orig_sel_ರಾಶಿ_pos)
		design->ತೆಗೆ_selection();
}

ಶೂನ್ಯ Pass::call_on_selection(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_ಸಕ್ರಿಯ_module = design->selected_ಸಕ್ರಿಯ_module;
	design->selected_ಸಕ್ರಿಯ_module.clear();
	design->ತಳ್ಳು_selection(selection);

	Pass::call(design, command);

	design->ತೆಗೆ_selection();
	design->selected_ಸಕ್ರಿಯ_module = backup_selected_ಸಕ್ರಿಯ_module;
}

ಶೂನ್ಯ Pass::call_on_selection(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ RTLIL::Selection &selection, std::ಸದಿಶ<std::string> ವಾದಗಳು_2)
{
	IdString backup_selected_ಸಕ್ರಿಯ_module = design->selected_ಸಕ್ರಿಯ_module;
	design->selected_ಸಕ್ರಿಯ_module.clear();
	design->ತಳ್ಳು_selection(selection);

	Pass::call(design, ವಾದಗಳು_2);

	design->ತೆಗೆ_selection();
	design->selected_ಸಕ್ರಿಯ_module = backup_selected_ಸಕ್ರಿಯ_module;
}

ಶೂನ್ಯ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_ಸಕ್ರಿಯ_module = design->selected_ಸಕ್ರಿಯ_module;
	design->selected_ಸಕ್ರಿಯ_module = module->name;
	design->ತಳ್ಳು_ಖಾಲಿಗಳು_selection();
	design->select(module);

	Pass::call(design, command);

	design->ತೆಗೆ_selection();
	design->selected_ಸಕ್ರಿಯ_module = backup_selected_ಸಕ್ರಿಯ_module;
}

ಶೂನ್ಯ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ಸದಿಶ<std::string> ವಾದಗಳು_2)
{
	IdString backup_selected_ಸಕ್ರಿಯ_module = design->selected_ಸಕ್ರಿಯ_module;
	design->selected_ಸಕ್ರಿಯ_module = module->name;
	design->ತಳ್ಳು_ಖಾಲಿಗಳು_selection();
	design->select(module);

	Pass::call(design, ವಾದಗಳು_2);

	design->ತೆಗೆ_selection();
	design->selected_ಸಕ್ರಿಯ_module = backup_selected_ಸಕ್ರಿಯ_module;
}

ತಾರ್ಕಿಕ ScriptPass::check_label(std::string label, std::string info)
{
	ಆದರೆ (ಸಕ್ರಿಯ_design == ಶೂನ್ಯಸೂಚಕ) {
		ಲಘುಗಣಕ("\n");
		ಆದರೆ (info.ಖಾಲಿಗಳು())
			ಲಘುಗಣಕ("    %s:\n", label);
		ಇಲ್ಲದಿದ್ದರೆ
			ಲಘುಗಣಕ("    %s:    %s\n", label, info);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!ಸಕ್ರಿಯ_ಸತತ_from.ಖಾಲಿಗಳು() && ಸಕ್ರಿಯ_ಸತತ_from == ಸಕ್ರಿಯ_ಸತತ_ಗಮ್ಯ) {
			ಖಂಡ_ಸಕ್ರಿಯ = (label == ಸಕ್ರಿಯ_ಸತತ_from);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (label == ಸಕ್ರಿಯ_ಸತತ_from)
				ಖಂಡ_ಸಕ್ರಿಯ = ನಿಜ;
			ಆದರೆ (label == ಸಕ್ರಿಯ_ಸತತ_ಗಮ್ಯ)
				ಖಂಡ_ಸಕ್ರಿಯ = ಸುಳ್ಳು;
		}
		ಹಿಂತಿರುಗಿ ಖಂಡ_ಸಕ್ರಿಯ;
	}
}

ಶೂನ್ಯ ScriptPass::ಸತತ(std::string command, std::string info)
{
	ಆದರೆ (ಸಕ್ರಿಯ_design == ಶೂನ್ಯಸೂಚಕ) {
		ಆದರೆ (info.ಖಾಲಿಗಳು())
			ಲಘುಗಣಕ("        %s\n", command);
		ಇಲ್ಲದಿದ್ದರೆ
			ಲಘುಗಣಕ("        %s    %s\n", command, info);
	} ಇಲ್ಲದಿದ್ದರೆ {
		Pass::call(ಸಕ್ರಿಯ_design, command);
		ಸಕ್ರಿಯ_design->check();
	}
}

ಶೂನ್ಯ ScriptPass::ಸತತ_nocheck(std::string command, std::string info)
{
	ಆದರೆ (ಸಕ್ರಿಯ_design == ಶೂನ್ಯಸೂಚಕ) {
		ಆದರೆ (info.ಖಾಲಿಗಳು())
			ಲಘುಗಣಕ("        %s\n", command);
		ಇಲ್ಲದಿದ್ದರೆ
			ಲಘುಗಣಕ("        %s    %s\n", command, info);
	} ಇಲ್ಲದಿದ್ದರೆ {
		Pass::call(ಸಕ್ರಿಯ_design, command);
	}
}

ಶೂನ್ಯ ScriptPass::ಸತತ_script(RTLIL::Design *design, std::string ಸತತ_from, std::string ಸತತ_ಗಮ್ಯ)
{
	help_ಬಹುಲಕ = ಸುಳ್ಳು;
	ಸಕ್ರಿಯ_design = design;
	ಖಂಡ_ಸಕ್ರಿಯ = ಸತತ_from.ಖಾಲಿಗಳು();
	ಸಕ್ರಿಯ_ಸತತ_from = ಸತತ_from;
	ಸಕ್ರಿಯ_ಸತತ_ಗಮ್ಯ = ಸತತ_ಗಮ್ಯ;
	script();
}

ಶೂನ್ಯ ScriptPass::help_script()
{
	clear_flags();
	help_ಬಹುಲಕ = ನಿಜ;
	ಸಕ್ರಿಯ_design = ಶೂನ್ಯಸೂಚಕ;
	ಖಂಡ_ಸಕ್ರಿಯ = ನಿಜ;
	ಸಕ್ರಿಯ_ಸತತ_from.clear();
	ಸಕ್ರಿಯ_ಸತತ_ಗಮ್ಯ.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, ಮೂಲ_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ಶೂನ್ಯ Frontend::ಸತತ_ರಿಜಿಸ್ಟರ್()
{
	ಆದರೆ (pass_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ರಿಜಿಸ್ಟರ್[pass_name] = ಇದು;

	ಆದರೆ (frontend_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_ರಿಜಿಸ್ಟರ್[frontend_name] = ಇದು;
}

Frontend::~Frontend()
{
}

ಶೂನ್ಯ Frontend::execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design)
{
	log_assert(ಮುಂದಿನ_ವಾದಗಳು.ಖಾಲಿಗಳು());
	ಮಾಡು {
		std::istream *f = ಖಾಲಿ;
		ಮುಂದಿನ_ವಾದಗಳು.clear();
		ಸ್ವಯಂಚಾಲಿತ state = pre_execute();
		execute(f, std::string(), ವಾದಗಳು_2, design);
		post_execute(state);
		ವಾದಗಳು_2 = ಮುಂದಿನ_ವಾದಗಳು;
		ಅಳಿಸು f;
	} ಆಗ (!ವಾದಗಳು_2.ಖಾಲಿಗಳು());
}

FILE *Frontend::current_script_file = ಖಾಲಿ;
std::string Frontend::ಕೊನೆಯ_ಇಲ್ಲಿ_document;
std::string Frontend::current_script_filename;
ಪೂರ್ಣಾಂಕ Frontend::current_script_lineno = 0;

ಶೂನ್ಯ Frontend::extra_ವಾದಗಳು(std::istream *&f, std::string &filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, ತಾರ್ಕಿಕ bin_ಒಳಹರಿವು)
{
	ತಾರ್ಕಿಕ called_with_fp = f != ಖಾಲಿ;

	ಮುಂದಿನ_ವಾದಗಳು.clear();

	ಆದರೆ (argidx < ವಾದಗಳು_2.ಗಾತ್ರ_2())
	{
		std::string arg = ವಾದಗಳು_2[argidx];

		ಆದರೆ (arg.compare(0, 1, "-") == 0)
			cmd_error(ವಾದಗಳು_2, argidx, "Unknown option or option in arguments.");
		ಆದರೆ (f != ಖಾಲಿ)
			cmd_error(ವಾದಗಳು_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		ಆದರೆ (filename == "<<" && argidx+1 < ವಾದಗಳು_2.ಗಾತ್ರ_2())
			filename += ವಾದಗಳು_2[++argidx];
		ಆದರೆ (filename.compare(0, 2, "<<") == 0) {
			ಆದರೆ (filename.ಗಾತ್ರ_2() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == ಶೂನ್ಯಸೂಚಕ) ?  "<stdin>" : "<inlined>";
			ಕೊನೆಯ_ಇಲ್ಲಿ_document.clear();
			ಆಗ (1) {
				std::string ಬಫರ್;
				ಅಕ್ಷರ ಖಂಡ[4096];
				ಆಗ (1) {
					ಆದರೆ (fgets(ಖಂಡ, 4096, Frontend::current_script_file == ಶೂನ್ಯಸೂಚಕ? stdin : Frontend::current_script_file) == ಶೂನ್ಯಸೂಚಕ)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					ಬಫರ್ += ಖಂಡ;
					ಆದರೆ (ಬಫರ್.ಗಾತ್ರ_2() > 0 && (ಬಫರ್[ಬಫರ್.ಗಾತ್ರ_2() - 1] == '\n' || ಬಫರ್[ಬಫರ್.ಗಾತ್ರ_2() - 1] == '\r'))
						ನಿಲ್ಲಿಸು;
				}
				Frontend::current_script_lineno++;
				ಗಾತ್ರ_t indent = ಬಫರ್.ಕಂಡುಹಿಡಿ_ಮೊದಲ_not_ದ(" \t\r\n");
				ಆದರೆ (indent != std::string::npos && ಬಫರ್.compare(indent, eot_marker.ಗಾತ್ರ_2(), eot_marker) == 0)
					ನಿಲ್ಲಿಸು;
				ಕೊನೆಯ_ಇಲ್ಲಿ_document += ಬಫರ್;
			}
			f = ಹೊಸ std::istringstream(ಕೊನೆಯ_ಇಲ್ಲಿ_document);
		} ಇಲ್ಲದಿದ್ದರೆ {
			rewrite_filename(filename);
			ಸದಿಶ<string> filenames = glob_filename(filename);
			filename = filenames.front();
			ಆದರೆ (Get_ಗಾತ್ರ(filenames) > 1) {
				ಮುಂದಿನ_ವಾದಗಳು.ಸೇರಿಸು(ಮುಂದಿನ_ವಾದಗಳು.ಕೊನೆ(), ವಾದಗಳು_2.begin(), ವಾದಗಳು_2.begin()+argidx);
				ಮುಂದಿನ_ವಾದಗಳು.ಸೇರಿಸು(ಮುಂದಿನ_ವಾದಗಳು.ಕೊನೆ(), filenames.begin()+1, filenames.ಕೊನೆ());
			}
			yosys_ಒಳಹರಿವು_files.ಸೇರಿಸು(filename);
			f = uncompressed(filename, bin_ಒಳಹರಿವು ? std::ifstream::ದ್ವಿಮಾನ : std::ifstream::ಒಳ);
		}

		ಗಾಗಿ (ಗಾತ್ರ_t i = argidx+1; i < ವಾದಗಳು_2.ಗಾತ್ರ_2(); i++)
			ಆದರೆ (ವಾದಗಳು_2[i].compare(0, 1, "-") == 0)
				cmd_error(ವಾದಗಳು_2, i, "Found option, expected arguments.");

		ಆದರೆ (argidx+1 < ವಾದಗಳು_2.ಗಾತ್ರ_2()) {
			ಆದರೆ (ಮುಂದಿನ_ವಾದಗಳು.ಖಾಲಿಗಳು())
				ಮುಂದಿನ_ವಾದಗಳು.ಸೇರಿಸು(ಮುಂದಿನ_ವಾದಗಳು.ಕೊನೆ(), ವಾದಗಳು_2.begin(), ವಾದಗಳು_2.begin()+argidx);
			ಮುಂದಿನ_ವಾದಗಳು.ಸೇರಿಸು(ಮುಂದಿನ_ವಾದಗಳು.ಕೊನೆ(), ವಾದಗಳು_2.begin()+argidx+1, ವಾದಗಳು_2.ಕೊನೆ());
			ವಾದಗಳು_2.erase(ವಾದಗಳು_2.begin()+argidx+1, ವಾದಗಳು_2.ಕೊನೆ());
		}
	}

	ಆದರೆ (f == ಖಾಲಿ)
		cmd_error(ವಾದಗಳು_2, argidx, "No filename given.");

	ಆದರೆ (called_with_fp)
		ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(filename);
	ವಾದಗಳು_2[0] = pass_name;
	// cmd_log_args(args);
}

ಶೂನ್ಯ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::ಸದಿಶ<std::string> ವಾದಗಳು_2;
	ಅಕ್ಷರ *s = strdup(command.c_str());
	ಗಾಗಿ (ಅಕ್ಷರ *p = strtok(s, " \t\r\n"); p; p = strtok(ಖಾಲಿ, " \t\r\n"))
		ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(p);
	ಬಿಡುಗಡೆ(s);
	frontend_call(design, f, filename, ವಾದಗಳು_2);
}

ಶೂನ್ಯ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2)
{
	ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 0)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (frontend_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(ವಾದಗಳು_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", ವಾದಗಳು_2[0]);

	ಆದರೆ (f != ಖಾಲಿ) {
		ಸ್ವಯಂಚಾಲಿತ state = frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->pre_execute();
		frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->execute(f, filename, ವಾದಗಳು_2, design);
		frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->post_execute(state);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename == "-") {
		std::istream *f_cin = &std::ಒಳಗೆ;
		ಸ್ವಯಂಚಾಲಿತ state = frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->pre_execute();
		frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->execute(f_cin, "<stdin>", ವಾದಗಳು_2, design);
		frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->post_execute(state);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!filename.ಖಾಲಿಗಳು())
			ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(filename);
		frontend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->execute(ವಾದಗಳು_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, ಮೂಲ_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ಶೂನ್ಯ Backend::ಸತತ_ರಿಜಿಸ್ಟರ್()
{
	ಆದರೆ (pass_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ರಿಜಿಸ್ಟರ್[pass_name] = ಇದು;

	ಆದರೆ (backend_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_ರಿಜಿಸ್ಟರ್[backend_name] = ಇದು;
}

Backend::~Backend()
{
}

ಶೂನ್ಯ Backend::execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design *design)
{
	std::ostream *f = ಖಾಲಿ;
	ಸ್ವಯಂಚಾಲಿತ state = pre_execute();
	execute(f, std::string(), ವಾದಗಳು_2, design);
	post_execute(state);
	ಆದರೆ (f != &std::ಹೊರಗೆ)
		ಅಳಿಸು f;
}

ಶೂನ್ಯ Backend::extra_ವಾದಗಳು(std::ostream *&f, std::string &filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2, ಗಾತ್ರ_t argidx, ತಾರ್ಕಿಕ bin_output)
{
	ತಾರ್ಕಿಕ called_with_fp = f != ಖಾಲಿ;

	ಗಾಗಿ (; argidx < ವಾದಗಳು_2.ಗಾತ್ರ_2(); argidx++)
	{
		std::string arg = ವಾದಗಳು_2[argidx];

		ಆದರೆ (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(ವಾದಗಳು_2, argidx, "Unknown option or option in arguments.");
		ಆದರೆ (f != ಖಾಲಿ)
			cmd_error(ವಾದಗಳು_2, argidx, "Extra filename argument in direct file mode.");

		ಆದರೆ (arg == "-") {
			filename = "<stdout>";
			f = &std::ಹೊರಗೆ;
			ಮುಂದುವರಿ;
		}

		filename = arg;
		rewrite_filename(filename);
		ಆದರೆ (filename.ಗಾತ್ರ_2() > 3 && filename.compare(filename.ಗಾತ್ರ_2()-3, std::string::npos, ".gz") == 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = ಹೊಸ gzip_ostream;
			ಆದರೆ (!gf->open(filename)) {
				ಅಳಿಸು gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ಸೇರಿಸು(filename);
			f = gf;
#ಇಲ್ಲದಿದ್ದರೆ
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#ಆದರೆ_ಅಂತ್ಯ
		} ಇಲ್ಲದಿದ್ದರೆ {
			std::ofstream *ff = ಹೊಸ std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ದ್ವಿಮಾನ) : std::ofstream::trunc);
			yosys_output_files.ಸೇರಿಸು(filename);
			ಆದರೆ (ff->fail()) {
				ಅಳಿಸು ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	ಆದರೆ (called_with_fp)
		ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(filename);
	ವಾದಗಳು_2[0] = pass_name;
	// cmd_log_args(args);

	ಆದರೆ (f == ಖಾಲಿ) {
		filename = "<stdout>";
		f = &std::ಹೊರಗೆ;
	}
}

ಶೂನ್ಯ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::ಸದಿಶ<std::string> ವಾದಗಳು_2;
	ಅಕ್ಷರ *s = strdup(command.c_str());
	ಗಾಗಿ (ಅಕ್ಷರ *p = strtok(s, " \t\r\n"); p; p = strtok(ಖಾಲಿ, " \t\r\n"))
		ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(p);
	ಬಿಡುಗಡೆ(s);
	backend_call(design, f, filename, ವಾದಗಳು_2);
}

ಶೂನ್ಯ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ಸದಿಶ<std::string> ವಾದಗಳು_2)
{
	ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 0)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (backend_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(ವಾದಗಳು_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", ವಾದಗಳು_2[0]);

	ಗಾತ್ರ_t orig_sel_ರಾಶಿ_pos = design->selection_ರಾಶಿ.ಗಾತ್ರ_2();

	ಆದರೆ (f != ಖಾಲಿ) {
		ಸ್ವಯಂಚಾಲಿತ state = backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->pre_execute();
		backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->execute(f, filename, ವಾದಗಳು_2, design);
		backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->post_execute(state);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filename == "-") {
		std::ostream *f_cout = &std::ಹೊರಗೆ;
		ಸ್ವಯಂಚಾಲಿತ state = backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->pre_execute();
		backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->execute(f_cout, "<stdout>", ವಾದಗಳು_2, design);
		backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->post_execute(state);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (!filename.ಖಾಲಿಗಳು())
			ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(filename);
		backend_ರಿಜಿಸ್ಟರ್[ವಾದಗಳು_2[0]]->execute(ವಾದಗಳು_2, design);
	}

	ಆಗ (design->selection_ರಾಶಿ.ಗಾತ್ರ_2() > orig_sel_ರಾಶಿ_pos)
		design->ತೆಗೆ_selection();
}

ರಚನೆ SimHelper {
	string name;
	ಸಾಲೊಳಗೆ string filesafe_name() {
		ಆದರೆ (name.at(0) == '$')
			ಆದರೆ (name.at(1) == '_')
				ಹಿಂತಿರುಗಿ "gate" + name.substr(1);
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ "word_" + name.substr(1);
		ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ name;
	}
	string title;
	string ports;
	string ಮೂಲ;
	string desc;
	string ಸಂಕೇತ_2;
	string ಗುಂಪು;
	string ver;
	string ಟ್ಯಾಗುಗಳು;
};

ಸ್ಥಿರ ತಾರ್ಕಿಕ is_ಸಂಕೇತ_getter(string name) {
	ಹಿಂತಿರುಗಿ *(--(name.ಕೊನೆ())) == '+';
}

ಸ್ಥಿರ string get_cell_name(string name) {
	ಹಿಂತಿರುಗಿ is_ಸಂಕೇತ_getter(name) ? name.substr(0, name.ಉದ್ದಳತೆ()-1) : name;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_warning_flags(Pass *pass) {
	ತಾರ್ಕಿಕ has_warnings = ಸುಳ್ಳು;
	ಸ್ಥಿರಾಂಕ string name = pass->pass_name;
	ಆದರೆ (pass->experimental_flag) {
		ಆದರೆ (!has_warnings) ಲಘುಗಣಕ("\n");
		has_warnings = ನಿಜ;
		ಲಘುಗಣಕ("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	ಆದರೆ (pass->ಆಂತರಿಕ_flag) {
		ಆದರೆ (!has_warnings) ಲಘುಗಣಕ("\n");
		has_warnings = ನಿಜ;
		ಲಘುಗಣಕ("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	ಆದರೆ (has_warnings)
		ಲಘುಗಣಕ("\n");
}

ಸ್ಥಿರ ರಚನೆ CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ಅನ್ತರ್ಭಾವ "kernel/simlib_help.inc"
#ಅನ್ತರ್ಭಾವ "kernel/simcells_help.inc"
		cell_help.ವಿಂಗಡಿಸು();
	}
	ತಾರ್ಕಿಕ ಒಳಗೊಂಡಿದೆ(string name) { ಹಿಂತಿರುಗಿ cell_help.ಎಣಿಕೆ_2(get_cell_name(name)) > 0; }
	SimHelper get(string name) { ಹಿಂತಿರುಗಿ cell_help[get_cell_name(name)]; }
} cell_help_messages;

ರಚನೆ HelpPass : ಸಾರ್ವಜನಿಕ Pass {
	HelpPass() : Pass("help", "display help messages") { }
	ಶೂನ್ಯ help() override
	{
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    help  ................  list all commands\n");
		ಲಘುಗಣಕ("    help <command>  ......  print help message for given command\n");
		ಲಘುಗಣಕ("    help -all  ...........  print complete command reference\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    help -cells ..........  list all cell types\n");
		ಲಘುಗಣಕ("    help <celltype>  .....  print help message for given cell type\n");
		ಲಘುಗಣಕ("    help <celltype>+  ....  print verilog code for given cell type\n");
		ಲಘುಗಣಕ("\n");
	}
	ತಾರ್ಕಿಕ dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.ನಮೂದು("version", "Yosys command reference");
		json.ನಮೂದು("generator", yosys_ಆವೃತ್ತಿ_str);

		ತಾರ್ಕಿಕ raise_error = ಸುಳ್ಳು;
		std::map<string, ಸದಿಶ<string>> groups;

		// get root path
		ಸ್ವಯಂಚಾಲಿತ this_ಮಾರ್ಗ = std::filesystem::ಮಾರ್ಗ_2(ಮೂಲ_location::current().file_name());
		ಸ್ವಯಂಚಾಲಿತ ಮೂಲ_ಬೇರು = this_ಮಾರ್ಗ.ಪೋಷಕ_ಮಾರ್ಗ().ಪೋಷಕ_ಮಾರ್ಗ();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : pass_ರಿಜಿಸ್ಟರ್) {
			ಸ್ವಯಂಚಾಲಿತ name = it.ಮೊದಲ;
			ಸ್ವಯಂಚಾಲಿತ pass = it.ಎರಡನೇ;
			ಸ್ವಯಂಚಾಲಿತ title = pass->short_help;

			ಸ್ವಯಂಚಾಲಿತ cmd_help = PrettyHelp();
			ಸ್ವಯಂಚಾಲಿತ has_pretty_help = pass->formatted_help();

			ಆದರೆ (!has_pretty_help) {
				ಎಣಿಕೆ PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				ಮೂಲ_location null_ಮೂಲ;
				string current_ಬಫರ್ = "";
				ಸ್ವಯಂಚಾಲಿತ ಬೇರು_listing_2 = cmd_help.get_ಬೇರು();
				ಸ್ವಯಂಚಾಲಿತ current_listing = ಬೇರು_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					ಸ್ವಯಂಚಾಲಿತ log_scope = logger().sink_scope();
					logger().ಕೂಡಿಸು_sink<ಹರಿವು_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				ಗಾತ್ರ_t def_strip_ಎಣಿಕೆ = 0;
				ಸ್ವಯಂಚಾಲಿತ current_state = PUState_none;
				ಸ್ವಯಂಚಾಲಿತ catch_verific = ಸುಳ್ಳು;
				ಸ್ವಯಂಚಾಲಿತ blank_ಸಾಲುಗಳು = 2;
				ಗಾಗಿ (string ಸಾಲು; std::getline(ss, ಸಾಲು, '\n');) {
					// find position of first non space character
					std::ಗಾತ್ರ_t ಮೊದಲ_pos = ಸಾಲು.ಕಂಡುಹಿಡಿ_ಮೊದಲ_not_ದ(" \t");
					std::ಗಾತ್ರ_t ಕೊನೆಯ_pos = ಸಾಲು.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_not_ದ(" \t");
					ಆದರೆ (ಮೊದಲ_pos == std::string::npos) {
						ಆಯ್ಕೆ (current_state)
						{
						ಸ್ಥಿತಿ PUState_signature:
							ಬೇರು_listing_2->usage(current_ಬಫರ್, null_ಮೂಲ);
							current_listing = ಬೇರು_listing_2;
							current_state = PUState_none;
							current_ಬಫರ್ = "";
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ PUState_none:
						ಸ್ಥಿತಿ PUState_optionbody:
							blank_ಸಾಲುಗಳು += 1;
							ನಿಲ್ಲಿಸು;
						ಪೂರ್ವನಿಯೋಜಿತ:
							ನಿಲ್ಲಿಸು;
						}
						// skip empty lines
						ಮುಂದುವರಿ;
					}

					// strip leading and trailing whitespace
					std::string stripped_ಸಾಲು = ಸಾಲು.substr(ಮೊದಲ_pos, ಕೊನೆಯ_pos - ಮೊದಲ_pos +1);
					ತಾರ್ಕಿಕ IsDefinition = stripped_ಸಾಲು[0] == '-';
					IsDefinition &= stripped_ಸಾಲು[1] != ' ' && stripped_ಸಾಲು[1] != '>';
					ತಾರ್ಕಿಕ IsDedent = def_strip_ಎಣಿಕೆ && ಮೊದಲ_pos < def_strip_ಎಣಿಕೆ;
					ತಾರ್ಕಿಕ IsIndent = def_strip_ಎಣಿಕೆ < ಮೊದಲ_pos;

					// line looks like a signature
					ತಾರ್ಕಿಕ IsSignature = stripped_ಸಾಲು.ಕಂಡುಹಿಡಿ(name) == 0 && (stripped_ಸಾಲು.ಉದ್ದಳತೆ() == name.ಉದ್ದಳತೆ() || stripped_ಸಾಲು.at(name.ಗಾತ್ರ_2()) == ' ');

					ಆದರೆ (IsSignature && ಮೊದಲ_pos <= 4 && (blank_ಸಾಲುಗಳು >= 2 || current_state == PUState_signature)) {
						ಆದರೆ (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_ಬಫರ್, "none", null_ಮೂಲ);
							current_ಬಫರ್ = "";
						} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_state == PUState_signature) {
							ಬೇರು_listing_2->usage(current_ಬಫರ್, null_ಮೂಲ);
							current_ಬಫರ್ = "";
						} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_state == PUState_none && !current_ಬಫರ್.ಖಾಲಿಗಳು()) {
							current_listing->codeblock(current_ಬಫರ್, "none", null_ಮೂಲ);
							current_ಬಫರ್ = "";
						}
						current_listing = ಬೇರು_listing_2;
						current_state = PUState_signature;
						def_strip_ಎಣಿಕೆ = ಮೊದಲ_pos;
						catch_verific = ಸುಳ್ಳು;
					} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IsDedent) {
						def_strip_ಎಣಿಕೆ = ಮೊದಲ_pos;
						ಆದರೆ (current_state == PUState_optionbody) {
							ಆದರೆ (!current_ಬಫರ್.ಖಾಲಿಗಳು()) {
								current_listing->codeblock(current_ಬಫರ್, "none", null_ಮೂಲ);
								current_ಬಫರ್ = "";
							}
							ಆದರೆ (IsIndent) {
								current_state = PUState_options;
								current_listing = ಬೇರು_listing_2->ಹಿಂಭಾಗ();
							} ಇಲ್ಲದಿದ್ದರೆ {
								current_state = PUState_none;
								current_listing = ಬೇರು_listing_2;
							}
						} ಇಲ್ಲದಿದ್ದರೆ {
							current_state = PUState_none;
						}
					}

					ಆದರೆ (IsDefinition && !catch_verific && current_state != PUState_signature) {
						ಆದರೆ (!current_ಬಫರ್.ಖಾಲಿಗಳು()) {
							current_listing->codeblock(current_ಬಫರ್, "none", null_ಮೂಲ);
							current_ಬಫರ್ = "";
						}
						current_state = PUState_options;
						current_listing = ಬೇರು_listing_2->open_option(stripped_ಸಾಲು, null_ಮೂಲ);
						def_strip_ಎಣಿಕೆ = ಮೊದಲ_pos;
					} ಇಲ್ಲದಿದ್ದರೆ {
						ಆದರೆ (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						ಆದರೆ (current_ಬಫರ್.ಖಾಲಿಗಳು())
							current_ಬಫರ್ = stripped_ಸಾಲು;
						ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_state == PUState_signature && IsIndent)
							current_ಬಫರ್ += stripped_ಸಾಲು;
						ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_state == PUState_none) {
							current_ಬಫರ್ += (blank_ಸಾಲುಗಳು > 0 ? "\n\n" : "\n") + ಸಾಲು;
						} ಇಲ್ಲದಿದ್ದರೆ
							current_ಬಫರ್ += (blank_ಸಾಲುಗಳು > 0 ? "\n\n" : "\n") + stripped_ಸಾಲು;
						ಆದರೆ (stripped_ಸಾಲು.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = ನಿಜ;
					}
					blank_ಸಾಲುಗಳು = 0;
				}

				ಆದರೆ (!current_ಬಫರ್.ಖಾಲಿಗಳು()) {
					ಆದರೆ (current_ಬಫರ್.ಗಾತ್ರ_2() > 64 && current_ಬಫರ್.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_ಬಫರ್.substr(0, 62), null_ಮೂಲ);
						current_listing->codeblock(current_ಬಫರ್.substr(64), "yoscrypt", null_ಮೂಲ);
					} ಇಲ್ಲದಿದ್ದರೆ
						current_listing->codeblock(current_ಬಫರ್, "none", null_ಮೂಲ);
					current_ಬಫರ್ = "";
				}
			}

			// fix path
			string ಮೂಲ_file = pass->location.file_name();
			ತಾರ್ಕಿಕ has_ಮೂಲ = ಮೂಲ_file.compare("unknown") != 0;
			std::filesystem::ಮಾರ್ಗ_2 ಮೂಲ_ಮಾರ್ಗ;
			ಸ್ವಯಂಚಾಲಿತ skip_ಮೂಲ_ಗುಂಪು = ಸುಳ್ಳು;

			ಆದರೆ (has_ಮೂಲ) {
				ಮೂಲ_ಮಾರ್ಗ = fixup_ಮೂಲ_ಮಾರ್ಗ(pass->location.file_name(), ಮೂಲ_ಬೇರು, &skip_ಮೂಲ_ಗುಂಪು);
				ಮೂಲ_file = ಮೂಲ_ಮಾರ್ಗ.string();
			}

			// attempt auto group
			ಆದರೆ (!cmd_help.has_ಗುಂಪು()) {
				ಆದರೆ (pass->ಆಂತರಿಕ_flag)
					cmd_help.ಗುಂಪು = "internal";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೂಲ_file.ಕಂಡುಹಿಡಿ("backends/") == 0 || (!has_ಮೂಲ && name.ಕಂಡುಹಿಡಿ("read_") == 0))
					cmd_help.ಗುಂಪು = "backends";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೂಲ_file.ಕಂಡುಹಿಡಿ("frontends/") == 0 || (!has_ಮೂಲ && name.ಕಂಡುಹಿಡಿ("write_") == 0))
					cmd_help.ಗುಂಪು = "frontends";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_ಮೂಲ) {
					ಆದರೆ (ಮೂಲ_ಮಾರ್ಗ.has_ಪೋಷಕ_ಮಾರ್ಗ() && !skip_ಮೂಲ_ಗುಂಪು)
						cmd_help.ಗುಂಪು = ಮೂಲ_ಮಾರ್ಗ.ಪೋಷಕ_ಮಾರ್ಗ().string();
				}
				// implicit !has_source
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name.ಕಂಡುಹಿಡಿ("equiv") == 0)
					cmd_help.ಗುಂಪು = "passes/equiv";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name.ಕಂಡುಹಿಡಿ("fsm") == 0)
					cmd_help.ಗುಂಪು = "passes/fsm";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name.ಕಂಡುಹಿಡಿ("memory") == 0)
					cmd_help.ಗುಂಪು = "passes/memory";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name.ಕಂಡುಹಿಡಿ("opt") == 0)
					cmd_help.ಗುಂಪು = "passes/opt";
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name.ಕಂಡುಹಿಡಿ("proc") == 0)
					cmd_help.ಗುಂಪು = "passes/proc";
			}

			ಆದರೆ (groups.ಎಣಿಕೆ_2(cmd_help.ಗುಂಪು) == 0) {
				groups[cmd_help.ಗುಂಪು] = ಸದಿಶ<string>();
			}
			groups[cmd_help.ಗುಂಪು].ತಳ್ಳು_ಹಿಂಭಾಗ(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.ನಮೂದು("title", title);
			json.name("content"); json.begin_array();
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &content : cmd_help)
				json.ಮೌಲ್ಯ(content.ಗಮ್ಯ_json(ಮೂಲ_ಬೇರು));
			json.ಕೊನೆ_array();
			json.ನಮೂದು("group", cmd_help.ಗುಂಪು);
			json.ನಮೂದು("source_file", ಮೂಲ_file);
			json.ನಮೂದು("source_line", pass->location.ಸಾಲು());
			json.ನಮೂದು("source_func", pass->location.function_name());
			json.ನಮೂದು("experimental_flag", pass->experimental_flag);
			json.ನಮೂದು("internal_flag", pass->ಆಂತರಿಕ_flag);
			json.ಕೊನೆ_object();
		}
		json.ಕೊನೆ_object();

		json.ನಮೂದು("groups", groups);

		json.ಕೊನೆ_object();
		ಹಿಂತಿರುಗಿ raise_error;
	}
	ತಾರ್ಕಿಕ dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.ನಮೂದು("version", "Yosys internal cells");
		json.ನಮೂದು("generator", yosys_maybe_ಆವೃತ್ತಿ());

		dict<string, ಸದಿಶ<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_ಕೋಷ್ಟಕ_Builder::CellInfo>> cells;

		// iterate over cells
		ತಾರ್ಕಿಕ raise_error = ಸುಳ್ಳು;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : StaticCellTypes::builder.cells) {
			ಆದರೆ (!StaticCellTypes::categories.is_ತಿಳಿದ(it.type))
				ಮುಂದುವರಿ;
			ಸ್ವಯಂಚಾಲಿತ name = ID::str(it.type);
			ಆದರೆ (cell_help_messages.ಒಳಗೊಂಡಿದೆ(name)) {
				ಸ್ವಯಂಚಾಲಿತ cell_help = cell_help_messages.get(name);
				groups[cell_help.ಗುಂಪು].emplace_ಹಿಂಭಾಗ(name);
				ಸ್ವಯಂಚಾಲಿತ cell_pair = pair<SimHelper, StaticCellTypes::Cell_ಕೋಷ್ಟಕ_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಲಘುಗಣಕ("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= ನಿಜ;
			}
		}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cell_help_messages.cell_help) {
			ಆದರೆ (cells.ಎಣಿಕೆ_2(it.ಮೊದಲ) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.ಮೊದಲ);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.ವಿಂಗಡಿಸು();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : groups) {
			json.name(it.ಮೊದಲ.c_str()); json.ಮೌಲ್ಯ(it.ಎರಡನೇ);
		}
		json.ಕೊನೆ_object();

		json.name("cells"); json.begin_object();
		cells.ವಿಂಗಡಿಸು();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cells) {
			ಸ್ವಯಂಚಾಲಿತ ch = it.ಎರಡನೇ.ಮೊದಲ;
			ಸ್ವಯಂಚಾಲಿತ ct = it.ಎರಡನೇ.ಎರಡನೇ;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.ಮೌಲ್ಯ(ch.title);
			json.name("ports"); json.ಮೌಲ್ಯ(ch.ports);
			json.name("source"); json.ಮೌಲ್ಯ(ch.ಮೂಲ);
			json.name("desc"); json.ಮೌಲ್ಯ(ch.desc);
			json.name("code"); json.ಮೌಲ್ಯ(ch.ಸಂಕೇತ_2);
			ಸದಿಶ<string> inputs, outputs;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಒಳಹರಿವು : ct.inputs)
				inputs.ತಳ್ಳು_ಹಿಂಭಾಗ(ID::str(ಒಳಹರಿವು));
			json.name("inputs"); json.ಮೌಲ್ಯ(inputs);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &output : ct.outputs)
				outputs.ತಳ್ಳು_ಹಿಂಭಾಗ(ID::str(output));
			json.name("outputs"); json.ಮೌಲ್ಯ(outputs);
			ಸದಿಶ<string> properties;
			// CellType properties
			ಆದರೆ (ct.features.is_evaluable) properties.ತಳ್ಳು_ಹಿಂಭಾಗ("is_evaluable");
			ಆದರೆ (ct.features.is_combinatorial) properties.ತಳ್ಳು_ಹಿಂಭಾಗ("is_combinatorial");
			ಆದರೆ (ct.features.is_synthesizable) properties.ತಳ್ಳು_ಹಿಂಭಾಗ("is_synthesizable");
			// SimHelper properties
			ಗಾತ್ರ_t ಕೊನೆಯ = 0; ಗಾತ್ರ_t ಮುಂದಿನ = 0;
			ಆಗ ((ಮುಂದಿನ = ch.ಟ್ಯಾಗುಗಳು.ಕಂಡುಹಿಡಿ(", ", ಕೊನೆಯ)) != string::npos) {
				properties.ತಳ್ಳು_ಹಿಂಭಾಗ(ch.ಟ್ಯಾಗುಗಳು.substr(ಕೊನೆಯ, ಮುಂದಿನ-ಕೊನೆಯ));
				ಕೊನೆಯ = ಮುಂದಿನ + 2;
			}
			ಸ್ವಯಂಚಾಲಿತ final_ಟ್ಯಾಗ್ = ch.ಟ್ಯಾಗುಗಳು.substr(ಕೊನೆಯ);
			ಆದರೆ (final_ಟ್ಯಾಗ್.ಗಾತ್ರ_2()) properties.ತಳ್ಳು_ಹಿಂಭಾಗ(final_ಟ್ಯಾಗ್);
			json.name("properties"); json.ಮೌಲ್ಯ(properties);
			json.ಕೊನೆ_object();
		}
		json.ಕೊನೆ_object();

		json.ಕೊನೆ_object();
		ಹಿಂತಿರುಗಿ raise_error;
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design*) override
	{
		ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 1) {
			ಲಘುಗಣಕ("\n");
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : pass_ರಿಜಿಸ್ಟರ್)
				ಲಘುಗಣಕ("    %-20s %s\n", it.ಮೊದಲ, it.ಎರಡನೇ->short_help);
			ಲಘುಗಣಕ("\n");
			ಲಘುಗಣಕ("Type 'help <command>' for more information on a command.\n");
			ಲಘುಗಣಕ("Type 'help -cells' for a list of all cell types.\n");
			ಲಘುಗಣಕ("\n");
			ಹಿಂತಿರುಗಿ;
		}

		ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 2) {
			ಆದರೆ (ವಾದಗಳು_2[1] == "-all") {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : pass_ರಿಜಿಸ್ಟರ್) {
					ಲಘುಗಣಕ("\n");
					ಲಘುಗಣಕ("\n");
					ಲಘುಗಣಕ("%s  --  %s\n", it.ಮೊದಲ, it.ಎರಡನೇ->short_help);
					std::string ಸಂದೇಶ;
					ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < it.ಮೊದಲ.ಗಾತ್ರ_2() + it.ಎರಡನೇ->short_help.ಗಾತ್ರ_2() + 6; i++)
						ಸಂದೇಶ += '=';
					ಲಘುಗಣಕ("%s\n", ಸಂದೇಶ);
					it.ಎರಡನೇ->help();
					log_warning_flags(it.ಎರಡನೇ);
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2[1] == "-cells") {
				ಲಘುಗಣಕ("\n");
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.ಎರಡನೇ;
					ಲಘುಗಣಕ("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				ಲಘುಗಣಕ("\n");
				ಲಘುಗಣಕ("Type 'help <cell_type>' for more information on a cell type.\n");
				ಲಘುಗಣಕ("\n");
				ಹಿಂತಿರುಗಿ;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pass_ರಿಜಿಸ್ಟರ್.ಎಣಿಕೆ_2(ವಾದಗಳು_2[1])) {
				pass_ರಿಜಿಸ್ಟರ್.at(ವಾದಗಳು_2[1])->help();
				log_warning_flags(pass_ರಿಜಿಸ್ಟರ್.at(ವಾದಗಳು_2[1]));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell_help_messages.ಒಳಗೊಂಡಿದೆ(ವಾದಗಳು_2[1])) {
				ಸ್ವಯಂಚಾಲಿತ help_cell = cell_help_messages.get(ವಾದಗಳು_2[1]);
				ಆದರೆ (is_ಸಂಕೇತ_getter(ವಾದಗಳು_2[1])) {
						ಲಘುಗಣಕ("\n");
						ಲಘುಗಣಕ("%s\n", help_cell.ಸಂಕೇತ_2);
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಲಘುಗಣಕ("\n");
					ಲಘುಗಣಕ("    %s %s\n", help_cell.name, help_cell.ports);
					ಲಘುಗಣಕ("\n");
					ಆದರೆ (help_cell.ver == "2" || help_cell.ver == "2a") {
						ಆದರೆ (help_cell.title != "") ಲಘುಗಣಕ("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						ಗಾಗಿ (std::string ಸಾಲು; std::getline(ss, ಸಾಲು, '\n');) {
							ಆದರೆ (ಸಾಲು != "::") ಲಘುಗಣಕ("%s\n", ಸಾಲು);
						}
					} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (help_cell.desc.ಉದ್ದಳತೆ()) {
						ಲಘುಗಣಕ("%s\n", help_cell.desc);
					} ಇಲ್ಲದಿದ್ದರೆ {
						ಲಘುಗಣಕ("No help message for this cell type found.\n");
					}
					ಲಘುಗಣಕ("\n");
					ಲಘುಗಣಕ("Run 'help %s+' to display the Verilog model for this cell type.\n", ವಾದಗಳು_2[1]);
					ಲಘುಗಣಕ("\n");
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ
				ಲಘುಗಣಕ("No such command or cell type: %s\n", ವಾದಗಳು_2[1]);
			ಹಿಂತಿರುಗಿ;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 3) {
			// this option is undocumented as it is for internal use only
			ಆದರೆ (ವಾದಗಳು_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				ಆದರೆ (!json.write_ಗಮ್ಯ_file(ವಾದಗಳು_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", ವಾದಗಳು_2[2], strerror(errno));
				ಆದರೆ (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2[1] == "-dump-cells-json") {
				PrettyJson json;
				ಆದರೆ (!json.write_ಗಮ್ಯ_file(ವಾದಗಳು_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", ವಾದಗಳು_2[2], strerror(errno));
				ಆದರೆ (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ
				ಲಘುಗಣಕ("Unknown help command: `%s %s'\n", ವಾದಗಳು_2[1], ವಾದಗಳು_2[2]);
			ಹಿಂತಿರುಗಿ;
		}

		help();
	}
} HelpPass;

ರಚನೆ EchoPass : ಸಾರ್ವಜನಿಕ Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	ಶೂನ್ಯ help() override
	{
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    echo on\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("Print all commands to log before executing them.\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    echo off\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("Do not print all commands to log before executing them. (default)\n");
		ಲಘುಗಣಕ("\n");
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string> ವಾದಗಳು_2, RTLIL::Design*) override
	{
		ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() > 2)
			cmd_error(ವಾದಗಳು_2, 2, "Unexpected argument.");

		ಆದರೆ (ವಾದಗಳು_2.ಗಾತ್ರ_2() == 2) {
			ಆದರೆ (ವಾದಗಳು_2[1] == "on")
				echo_ಬಹುಲಕ = ನಿಜ;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ವಾದಗಳು_2[1] == "off")
				echo_ಬಹುಲಕ = ಸುಳ್ಳು;
			ಇಲ್ಲದಿದ್ದರೆ
				cmd_error(ವಾದಗಳು_2, 1, "Unexpected argument.");
		}

		ಲಘುಗಣಕ("echo %s\n", echo_ಬಹುಲಕ ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ರಚನೆ MinisatSatSolver : ಸಾರ್ವಜನಿಕ SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = ಇದು;
	}
	ezSAT *create() override {
		ಹಿಂತಿರುಗಿ ಹೊಸ ezMiniSAT();
	}
} MinisatSatSolver;

ರಚನೆ LicensePass : ಸಾರ್ವಜನಿಕ Pass {
	LicensePass() : Pass("license", "print license terms") { }
	ಶೂನ್ಯ help() override
	{
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("    license\n");
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ("This command produces the following notice.\n");
		notice();
	}
	ಶೂನ್ಯ execute(std::ಸದಿಶ<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	ಶೂನ್ಯ notice()
	{
		ಲಘುಗಣಕ("\n");
		ಲಘುಗಣಕ(" /----------------------------------------------------------------------------\\\n");
		ಲಘುಗಣಕ(" |                                                                            |\n");
		ಲಘುಗಣಕ(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		ಲಘುಗಣಕ(" |                                                                            |\n");
		ಲಘುಗಣಕ(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		ಲಘುಗಣಕ(" |                                                                            |\n");
		ಲಘುಗಣಕ(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		ಲಘುಗಣಕ(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		ಲಘುಗಣಕ(" |  copyright notice and this permission notice appear in all copies.         |\n");
		ಲಘುಗಣಕ(" |                                                                            |\n");
		ಲಘುಗಣಕ(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		ಲಘುಗಣಕ(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		ಲಘುಗಣಕ(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		ಲಘುಗಣಕ(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		ಲಘುಗಣಕ(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		ಲಘುಗಣಕ(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		ಲಘುಗಣಕ(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		ಲಘುಗಣಕ(" |                                                                            |\n");
		ಲಘುಗಣಕ(" \\----------------------------------------------------------------------------/\n");
		ಲಘುಗಣಕ("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_ಕೊನೆ

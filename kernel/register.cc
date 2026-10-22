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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/satgen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/json.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/gzip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log_help.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <filesystem>

YOSYS_NAMESPACE_BEGIN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_REG_ਗਿਣਤੀ 1000

ਤਾਰਕਿਕ echo_ਬਹੁਲਕ = ਝੂਠ;
Pass *ਪਹਿਲਾ_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_ਰਜਿਸਟਰ;
std::map<std::string, Pass*> pass_ਰਜਿਸਟਰ;
std::map<std::string, Backend*> backend_ਰਜਿਸਟਰ;
std::map<std::string, ਨਿਸ਼ਾਨਾ> ਨਿਸ਼ਾਨਾ_ਰਜਿਸਟਰ;

std::ਸਦਿਸ਼<std::string> Frontend::ਅਗਲਾ_ਦਲੀਲਾਂ;

ਤਾਰਕਿਕ GarbageCollectionGuard::is_enabled_ = ਸੱਚ;

ਸਥਿਰ ਤਾਰਕਿਕ garbage_collection_requested = ਝੂਠ;

ਖਾਲੀ request_garbage_collection()
{
	garbage_collection_requested = ਸੱਚ;
}

ਖਾਲੀ try_collect_garbage()
{
	ਜੇ (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		ਵਾਪਸ;
	garbage_collection_requested = ਝੂਠ;
	ਜੇ (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, ਸਰੋਤ_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	ਅਗਲਾ_queued_pass = ਪਹਿਲਾ_queued_pass;
	ਪਹਿਲਾ_queued_pass = ਇਹ;
	call_counter = 0;
	runtime_ns = 0;
}

ਖਾਲੀ Pass::ਲੜੀਵਾਰ_ਰਜਿਸਟਰ()
{
	ਜੇ (pass_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ਰਜਿਸਟਰ[pass_name] = ਇਹ;
}

ਖਾਲੀ Pass::init_ਰਜਿਸਟਰ()
{
	ਸਦਿਸ਼<Pass*> added_passes;
	ਜਦੋਂ (ਪਹਿਲਾ_queued_pass) {
		added_passes.ਧੱਕੋ_ਪਿੱਠ(ਪਹਿਲਾ_queued_pass);
		ਪਹਿਲਾ_queued_pass->ਲੜੀਵਾਰ_ਰਜਿਸਟਰ();
		ਪਹਿਲਾ_queued_pass = ਪਹਿਲਾ_queued_pass->ਅਗਲਾ_queued_pass;
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ added_pass : added_passes)
		added_pass->on_ਰਜਿਸਟਰ();
}

ਖਾਲੀ Pass::ਪੂਰਾ_ਰਜਿਸਟਰ()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : pass_ਰਜਿਸਟਰ)
		it.ਦੂਜਾ->on_shutdown();

	frontend_ਰਜਿਸਟਰ.clear();
	pass_ਰਜਿਸਟਰ.clear();
	backend_ਰਜਿਸਟਰ.clear();
	log_assert(ਪਹਿਲਾ_queued_pass == ਨਿਰਾ);
}

ਖਾਲੀ Pass::on_ਰਜਿਸਟਰ()
{
}

ਖਾਲੀ Pass::on_shutdown()
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
	state.ਮਾਪਾ_pass = current_pass;
	current_pass = ਇਹ;
	clear_flags();
	ਵਾਪਸ state;
}

ਖਾਲੀ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t ਸਮਾਂ_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += ਸਮਾਂ_ns;
	current_pass = state.ਮਾਪਾ_pass;
	ਘਟਾਓ_from_current_runtime_ns(ਸਮਾਂ_ns);
}

ਖਾਲੀ Pass::ਘਟਾਓ_from_current_runtime_ns(int64_t ਸਮਾਂ_ns)
{
	ਜੇ (current_pass)
		current_pass->runtime_ns -= ਸਮਾਂ_ns;
}

ਖਾਲੀ Pass::help()
{
	ਸਵੈਚਾਲਿਤ prettyHelp = PrettyHelp();
	ਜੇ (formatted_help()) {
		prettyHelp.log_help();
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("No help message for command `%s'.\n", pass_name);
		ਲਘੂਗਣਕ("\n");
	}
}

ਤਾਰਕਿਕ Pass::formatted_help()
{
	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ Pass::clear_flags()
{
}

ਖਾਲੀ Pass::cmd_log_ਦਲੀਲਾਂ(ਅਚਲ std::ਸਦਿਸ਼<std::string> &ਦਲੀਲਾਂ_2)
{
	ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() <= 1)
		ਵਾਪਸ;
	std::string ਸੁਨੇਹਾ_2 = "Full command line:";
	ਲਈ (ਮਾਪ_t i = 0; i < ਦਲੀਲਾਂ_2.ਮਾਪ(); i++)
		ਸੁਨੇਹਾ_2 += stringf(" %s", ਦਲੀਲਾਂ_2[i]);
	ਲਘੂਗਣਕ("%s\n", ਸੁਨੇਹਾ_2);
}

ਖਾਲੀ Pass::cmd_error(ਅਚਲ std::ਸਦਿਸ਼<std::string> &ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, std::string ਸੁਨੇਹਾ)
{
	std::string command_ਪਾਠ;
	ਪੂਰਨ error_pos = 0;

	ਲਈ (ਮਾਪ_t i = 0; i < ਦਲੀਲਾਂ_2.ਮਾਪ(); i++) {
		ਜੇ (i < argidx)
			error_pos += ਦਲੀਲਾਂ_2[i].ਮਾਪ() + 1;
		command_ਪਾਠ = command_ਪਾਠ + (command_ਪਾਠ.ਖਾਲੀ_ਥਾਵਾਂ() ? "" : " ") + ਦਲੀਲਾਂ_2[i];
	}

	ਲਘੂਗਣਕ("\n");
	ਲਘੂਗਣਕ("Syntax error in command `%s':\n", command_ਪਾਠ);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			ਸੁਨੇਹਾ.c_str(), command_ਪਾਠ.c_str(), error_pos, "");
}

ਖਾਲੀ Pass::extra_ਦਲੀਲਾਂ(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, RTLIL::Design *design, ਤਾਰਕਿਕ select)
{
	ਲਈ (; argidx < ਦਲੀਲਾਂ_2.ਮਾਪ(); argidx++)
	{
		std::string arg = ਦਲੀਲਾਂ_2[argidx];

		ਜੇ (arg.compare(0, 1, "-") == 0)
			cmd_error(ਦਲੀਲਾਂ_2, argidx, "Unknown option or option in arguments.");

		ਜੇ (!select)
			cmd_error(ਦਲੀਲਾਂ_2, argidx, "Extra argument.");

		handle_extra_select_ਦਲੀਲਾਂ(ਇਹ, ਦਲੀਲਾਂ_2, argidx, ਦਲੀਲਾਂ_2.ਮਾਪ(), design);
		ਰੋਕੋ;
	}
	// cmd_log_args(args);
}

ਖਾਲੀ Pass::call(RTLIL::Design *design, std::string command)
{
	std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2;

	std::string cmd_buf = command;
	std::string tok = ਅਗਲਾ_token(cmd_buf, " \t\r\n", ਸੱਚ);

	ਜੇ (tok.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ;

	ਜੇ (tok[0] == '!') {
#ਜੇ defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.ਲੱਭੋ('!') + 1);
		ਜਦੋਂ (!cmd_buf.ਖਾਲੀ_ਥਾਵਾਂ() && (cmd_buf.ਪਿੱਠ() == ' ' || cmd_buf.ਪਿੱਠ() == '\t' ||
				cmd_buf.ਪਿੱਠ() == '\r' || cmd_buf.ਪਿੱਠ() == '\n'))
			cmd_buf.resize(cmd_buf.ਮਾਪ()-1);
		log_ਸਿਰਲੇਖ(design, "Shell command: %s\n", cmd_buf);
		ਪੂਰਨ ret_ਕੋਡ = ਲੜੀਵਾਰ_command(cmd_buf);
		ਜੇ (ret_ਕੋਡ != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_ਕੋਡ);
		ਵਾਪਸ;
#ਨਹੀਂਤਾਂ
		log_cmd_error("Shell is not available.\n");
#ਜੇ_ਅੰਤ
	}

	ਜਦੋਂ (!tok.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਜੇ (tok[0] == '#') {
			ਪੂਰਨ stop;
			ਲਈ (stop = 0; stop < Get_ਮਾਪ(cmd_buf); stop++)
				ਜੇ (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					ਰੋਕੋ;
			cmd_buf = cmd_buf.substr(stop);
		} ਨਹੀਂਤਾਂ
		ਜੇ (tok.ਪਿੱਠ() == ';') {
			ਪੂਰਨ ਅੰਕ_semikolon = 0;
			ਜਦੋਂ (!tok.ਖਾਲੀ_ਥਾਵਾਂ() && tok.ਪਿੱਠ() == ';')
				tok.resize(tok.ਮਾਪ()-1), ਅੰਕ_semikolon++;
			ਜੇ (!tok.ਖਾਲੀ_ਥਾਵਾਂ())
				ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(tok);
			call(design, ਦਲੀਲਾਂ_2);
			ਦਲੀਲਾਂ_2.clear();
			ਜੇ (ਅੰਕ_semikolon == 2)
				call(design, "clean");
			ਜੇ (ਅੰਕ_semikolon == 3)
				call(design, "clean -purge");
		} ਨਹੀਂਤਾਂ
			ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(tok);
		ਤਾਰਕਿਕ ਮਿਲਿਆ_nl = ਝੂਠ;
		ਲਈ (ਸਵੈਚਾਲਿਤ c : cmd_buf) {
			ਜੇ (c == ' ' || c == '\t')
				ਜਾਰੀ;
			ਜੇ (c == '\r' || c == '\n')
				ਮਿਲਿਆ_nl = ਸੱਚ;
			ਰੋਕੋ;
		}
		ਜੇ (ਮਿਲਿਆ_nl) {
			call(design, ਦਲੀਲਾਂ_2);
			ਦਲੀਲਾਂ_2.clear();
		}
		tok = ਅਗਲਾ_token(cmd_buf, " \t\r\n", ਸੱਚ);
	}

	call(design, ਦਲੀਲਾਂ_2);
}

ਖਾਲੀ Pass::call(RTLIL::Design *design, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2)
{
	ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 0 || ਦਲੀਲਾਂ_2[0][0] == '#' || ਦਲੀਲਾਂ_2[0][0] == ':')
		ਵਾਪਸ;

	ਜੇ (echo_ਬਹੁਲਕ) {
		std::string ਸੁਨੇਹਾ_2 = stringf("%s", create_prompt(design, 0, ਝੂਠ));
		ਲਈ (ਮਾਪ_t i = 0; i < ਦਲੀਲਾਂ_2.ਮਾਪ(); i++)
			ਸੁਨੇਹਾ_2 += stringf("%s%s", i ? " " : "", ਦਲੀਲਾਂ_2[i]);
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("%s\n", ਸੁਨੇਹਾ_2);
	}

	ਜੇ (pass_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(ਦਲੀਲਾਂ_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", ਦਲੀਲਾਂ_2[0]);
	Pass *pass = pass_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	ਜੇ (pass->experimental_flag)
		log_experimental(ਦਲੀਲਾਂ_2[0]);

	ਮਾਪ_t orig_sel_ਢੇਰ_pos = design->selection_ਢੇਰ.ਮਾਪ();
	ਸਵੈਚਾਲਿਤ state = pass->pre_execute();
	pass->execute(ਦਲੀਲਾਂ_2, design);
	pass->post_execute(state);
	ਜਦੋਂ (design->selection_ਢੇਰ.ਮਾਪ() > orig_sel_ਢੇਰ_pos)
		design->ਕੱਢੋ_selection();
}

ਖਾਲੀ Pass::call_on_selection(RTLIL::Design *design, ਅਚਲ RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_ਸਰਗਰਮ_module = design->selected_ਸਰਗਰਮ_module;
	design->selected_ਸਰਗਰਮ_module.clear();
	design->ਧੱਕੋ_selection(selection);

	Pass::call(design, command);

	design->ਕੱਢੋ_selection();
	design->selected_ਸਰਗਰਮ_module = backup_selected_ਸਰਗਰਮ_module;
}

ਖਾਲੀ Pass::call_on_selection(RTLIL::Design *design, ਅਚਲ RTLIL::Selection &selection, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2)
{
	IdString backup_selected_ਸਰਗਰਮ_module = design->selected_ਸਰਗਰਮ_module;
	design->selected_ਸਰਗਰਮ_module.clear();
	design->ਧੱਕੋ_selection(selection);

	Pass::call(design, ਦਲੀਲਾਂ_2);

	design->ਕੱਢੋ_selection();
	design->selected_ਸਰਗਰਮ_module = backup_selected_ਸਰਗਰਮ_module;
}

ਖਾਲੀ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_ਸਰਗਰਮ_module = design->selected_ਸਰਗਰਮ_module;
	design->selected_ਸਰਗਰਮ_module = module->name;
	design->ਧੱਕੋ_ਖਾਲੀ_ਥਾਵਾਂ_selection();
	design->select(module);

	Pass::call(design, command);

	design->ਕੱਢੋ_selection();
	design->selected_ਸਰਗਰਮ_module = backup_selected_ਸਰਗਰਮ_module;
}

ਖਾਲੀ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2)
{
	IdString backup_selected_ਸਰਗਰਮ_module = design->selected_ਸਰਗਰਮ_module;
	design->selected_ਸਰਗਰਮ_module = module->name;
	design->ਧੱਕੋ_ਖਾਲੀ_ਥਾਵਾਂ_selection();
	design->select(module);

	Pass::call(design, ਦਲੀਲਾਂ_2);

	design->ਕੱਢੋ_selection();
	design->selected_ਸਰਗਰਮ_module = backup_selected_ਸਰਗਰਮ_module;
}

ਤਾਰਕਿਕ ScriptPass::check_label(std::string label, std::string info)
{
	ਜੇ (ਸਰਗਰਮ_design == ਖਾਲੀ_ਸੂਚਕ) {
		ਲਘੂਗਣਕ("\n");
		ਜੇ (info.ਖਾਲੀ_ਥਾਵਾਂ())
			ਲਘੂਗਣਕ("    %s:\n", label);
		ਨਹੀਂਤਾਂ
			ਲਘੂਗਣਕ("    %s:    %s\n", label, info);
		ਵਾਪਸ ਸੱਚ;
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!ਸਰਗਰਮ_ਲੜੀਵਾਰ_from.ਖਾਲੀ_ਥਾਵਾਂ() && ਸਰਗਰਮ_ਲੜੀਵਾਰ_from == ਸਰਗਰਮ_ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ) {
			ਖੰਡ_ਸਰਗਰਮ = (label == ਸਰਗਰਮ_ਲੜੀਵਾਰ_from);
		} ਨਹੀਂਤਾਂ {
			ਜੇ (label == ਸਰਗਰਮ_ਲੜੀਵਾਰ_from)
				ਖੰਡ_ਸਰਗਰਮ = ਸੱਚ;
			ਜੇ (label == ਸਰਗਰਮ_ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ)
				ਖੰਡ_ਸਰਗਰਮ = ਝੂਠ;
		}
		ਵਾਪਸ ਖੰਡ_ਸਰਗਰਮ;
	}
}

ਖਾਲੀ ScriptPass::ਲੜੀਵਾਰ(std::string command, std::string info)
{
	ਜੇ (ਸਰਗਰਮ_design == ਖਾਲੀ_ਸੂਚਕ) {
		ਜੇ (info.ਖਾਲੀ_ਥਾਵਾਂ())
			ਲਘੂਗਣਕ("        %s\n", command);
		ਨਹੀਂਤਾਂ
			ਲਘੂਗਣਕ("        %s    %s\n", command, info);
	} ਨਹੀਂਤਾਂ {
		Pass::call(ਸਰਗਰਮ_design, command);
		ਸਰਗਰਮ_design->check();
	}
}

ਖਾਲੀ ScriptPass::ਲੜੀਵਾਰ_nocheck(std::string command, std::string info)
{
	ਜੇ (ਸਰਗਰਮ_design == ਖਾਲੀ_ਸੂਚਕ) {
		ਜੇ (info.ਖਾਲੀ_ਥਾਵਾਂ())
			ਲਘੂਗਣਕ("        %s\n", command);
		ਨਹੀਂਤਾਂ
			ਲਘੂਗਣਕ("        %s    %s\n", command, info);
	} ਨਹੀਂਤਾਂ {
		Pass::call(ਸਰਗਰਮ_design, command);
	}
}

ਖਾਲੀ ScriptPass::ਲੜੀਵਾਰ_script(RTLIL::Design *design, std::string ਲੜੀਵਾਰ_from, std::string ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ)
{
	help_ਬਹੁਲਕ = ਝੂਠ;
	ਸਰਗਰਮ_design = design;
	ਖੰਡ_ਸਰਗਰਮ = ਲੜੀਵਾਰ_from.ਖਾਲੀ_ਥਾਵਾਂ();
	ਸਰਗਰਮ_ਲੜੀਵਾਰ_from = ਲੜੀਵਾਰ_from;
	ਸਰਗਰਮ_ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ = ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ;
	script();
}

ਖਾਲੀ ScriptPass::help_script()
{
	clear_flags();
	help_ਬਹੁਲਕ = ਸੱਚ;
	ਸਰਗਰਮ_design = ਖਾਲੀ_ਸੂਚਕ;
	ਖੰਡ_ਸਰਗਰਮ = ਸੱਚ;
	ਸਰਗਰਮ_ਲੜੀਵਾਰ_from.clear();
	ਸਰਗਰਮ_ਲੜੀਵਾਰ_ਮੰਜ਼ਿਲ.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, ਸਰੋਤ_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ਖਾਲੀ Frontend::ਲੜੀਵਾਰ_ਰਜਿਸਟਰ()
{
	ਜੇ (pass_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ਰਜਿਸਟਰ[pass_name] = ਇਹ;

	ਜੇ (frontend_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_ਰਜਿਸਟਰ[frontend_name] = ਇਹ;
}

Frontend::~Frontend()
{
}

ਖਾਲੀ Frontend::execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design)
{
	log_assert(ਅਗਲਾ_ਦਲੀਲਾਂ.ਖਾਲੀ_ਥਾਵਾਂ());
	ਕਰੋ {
		std::istream *f = ਨਿਰਾ;
		ਅਗਲਾ_ਦਲੀਲਾਂ.clear();
		ਸਵੈਚਾਲਿਤ state = pre_execute();
		execute(f, std::string(), ਦਲੀਲਾਂ_2, design);
		post_execute(state);
		ਦਲੀਲਾਂ_2 = ਅਗਲਾ_ਦਲੀਲਾਂ;
		ਮਿਟਾਓ f;
	} ਜਦੋਂ (!ਦਲੀਲਾਂ_2.ਖਾਲੀ_ਥਾਵਾਂ());
}

FILE *Frontend::current_script_file = ਨਿਰਾ;
std::string Frontend::ਆਖਰੀ_ਇੱਥੇ_document;
std::string Frontend::current_script_filename;
ਪੂਰਨ Frontend::current_script_lineno = 0;

ਖਾਲੀ Frontend::extra_ਦਲੀਲਾਂ(std::istream *&f, std::string &filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, ਤਾਰਕਿਕ bin_ਨਿਵੇਸ਼)
{
	ਤਾਰਕਿਕ called_with_fp = f != ਨਿਰਾ;

	ਅਗਲਾ_ਦਲੀਲਾਂ.clear();

	ਜੇ (argidx < ਦਲੀਲਾਂ_2.ਮਾਪ())
	{
		std::string arg = ਦਲੀਲਾਂ_2[argidx];

		ਜੇ (arg.compare(0, 1, "-") == 0)
			cmd_error(ਦਲੀਲਾਂ_2, argidx, "Unknown option or option in arguments.");
		ਜੇ (f != ਨਿਰਾ)
			cmd_error(ਦਲੀਲਾਂ_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		ਜੇ (filename == "<<" && argidx+1 < ਦਲੀਲਾਂ_2.ਮਾਪ())
			filename += ਦਲੀਲਾਂ_2[++argidx];
		ਜੇ (filename.compare(0, 2, "<<") == 0) {
			ਜੇ (filename.ਮਾਪ() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == ਖਾਲੀ_ਸੂਚਕ) ?  "<stdin>" : "<inlined>";
			ਆਖਰੀ_ਇੱਥੇ_document.clear();
			ਜਦੋਂ (1) {
				std::string ਬਫਰ;
				ਅੱਖਰ ਖੰਡ[4096];
				ਜਦੋਂ (1) {
					ਜੇ (fgets(ਖੰਡ, 4096, Frontend::current_script_file == ਖਾਲੀ_ਸੂਚਕ? stdin : Frontend::current_script_file) == ਖਾਲੀ_ਸੂਚਕ)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					ਬਫਰ += ਖੰਡ;
					ਜੇ (ਬਫਰ.ਮਾਪ() > 0 && (ਬਫਰ[ਬਫਰ.ਮਾਪ() - 1] == '\n' || ਬਫਰ[ਬਫਰ.ਮਾਪ() - 1] == '\r'))
						ਰੋਕੋ;
				}
				Frontend::current_script_lineno++;
				ਮਾਪ_t indent = ਬਫਰ.ਲੱਭੋ_ਪਹਿਲਾ_not_ਦਾ(" \t\r\n");
				ਜੇ (indent != std::string::npos && ਬਫਰ.compare(indent, eot_marker.ਮਾਪ(), eot_marker) == 0)
					ਰੋਕੋ;
				ਆਖਰੀ_ਇੱਥੇ_document += ਬਫਰ;
			}
			f = ਨਵਾਂ std::istringstream(ਆਖਰੀ_ਇੱਥੇ_document);
		} ਨਹੀਂਤਾਂ {
			rewrite_filename(filename);
			ਸਦਿਸ਼<string> filenames = glob_filename(filename);
			filename = filenames.front();
			ਜੇ (Get_ਮਾਪ(filenames) > 1) {
				ਅਗਲਾ_ਦਲੀਲਾਂ.ਪਾਓ(ਅਗਲਾ_ਦਲੀਲਾਂ.ਸਿਰਾ(), ਦਲੀਲਾਂ_2.begin(), ਦਲੀਲਾਂ_2.begin()+argidx);
				ਅਗਲਾ_ਦਲੀਲਾਂ.ਪਾਓ(ਅਗਲਾ_ਦਲੀਲਾਂ.ਸਿਰਾ(), filenames.begin()+1, filenames.ਸਿਰਾ());
			}
			yosys_ਨਿਵੇਸ਼_files.ਪਾਓ(filename);
			f = uncompressed(filename, bin_ਨਿਵੇਸ਼ ? std::ifstream::ਦੁਆਧਾਰੀ : std::ifstream::ਵਿੱਚ);
		}

		ਲਈ (ਮਾਪ_t i = argidx+1; i < ਦਲੀਲਾਂ_2.ਮਾਪ(); i++)
			ਜੇ (ਦਲੀਲਾਂ_2[i].compare(0, 1, "-") == 0)
				cmd_error(ਦਲੀਲਾਂ_2, i, "Found option, expected arguments.");

		ਜੇ (argidx+1 < ਦਲੀਲਾਂ_2.ਮਾਪ()) {
			ਜੇ (ਅਗਲਾ_ਦਲੀਲਾਂ.ਖਾਲੀ_ਥਾਵਾਂ())
				ਅਗਲਾ_ਦਲੀਲਾਂ.ਪਾਓ(ਅਗਲਾ_ਦਲੀਲਾਂ.ਸਿਰਾ(), ਦਲੀਲਾਂ_2.begin(), ਦਲੀਲਾਂ_2.begin()+argidx);
			ਅਗਲਾ_ਦਲੀਲਾਂ.ਪਾਓ(ਅਗਲਾ_ਦਲੀਲਾਂ.ਸਿਰਾ(), ਦਲੀਲਾਂ_2.begin()+argidx+1, ਦਲੀਲਾਂ_2.ਸਿਰਾ());
			ਦਲੀਲਾਂ_2.erase(ਦਲੀਲਾਂ_2.begin()+argidx+1, ਦਲੀਲਾਂ_2.ਸਿਰਾ());
		}
	}

	ਜੇ (f == ਨਿਰਾ)
		cmd_error(ਦਲੀਲਾਂ_2, argidx, "No filename given.");

	ਜੇ (called_with_fp)
		ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(filename);
	ਦਲੀਲਾਂ_2[0] = pass_name;
	// cmd_log_args(args);
}

ਖਾਲੀ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2;
	ਅੱਖਰ *s = strdup(command.c_str());
	ਲਈ (ਅੱਖਰ *p = strtok(s, " \t\r\n"); p; p = strtok(ਨਿਰਾ, " \t\r\n"))
		ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(p);
	ਮੁਕਤ(s);
	frontend_call(design, f, filename, ਦਲੀਲਾਂ_2);
}

ਖਾਲੀ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2)
{
	ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 0)
		ਵਾਪਸ;
	ਜੇ (frontend_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(ਦਲੀਲਾਂ_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", ਦਲੀਲਾਂ_2[0]);

	ਜੇ (f != ਨਿਰਾ) {
		ਸਵੈਚਾਲਿਤ state = frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->pre_execute();
		frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->execute(f, filename, ਦਲੀਲਾਂ_2, design);
		frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->post_execute(state);
	} ਨਹੀਂਤਾਂ ਜੇ (filename == "-") {
		std::istream *f_cin = &std::ਅੰਦਰ;
		ਸਵੈਚਾਲਿਤ state = frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->pre_execute();
		frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->execute(f_cin, "<stdin>", ਦਲੀਲਾਂ_2, design);
		frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->post_execute(state);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!filename.ਖਾਲੀ_ਥਾਵਾਂ())
			ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(filename);
		frontend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->execute(ਦਲੀਲਾਂ_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, ਸਰੋਤ_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ਖਾਲੀ Backend::ਲੜੀਵਾਰ_ਰਜਿਸਟਰ()
{
	ਜੇ (pass_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ਰਜਿਸਟਰ[pass_name] = ਇਹ;

	ਜੇ (backend_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_ਰਜਿਸਟਰ[backend_name] = ਇਹ;
}

Backend::~Backend()
{
}

ਖਾਲੀ Backend::execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design *design)
{
	std::ostream *f = ਨਿਰਾ;
	ਸਵੈਚਾਲਿਤ state = pre_execute();
	execute(f, std::string(), ਦਲੀਲਾਂ_2, design);
	post_execute(state);
	ਜੇ (f != &std::ਬਾਹਰ)
		ਮਿਟਾਓ f;
}

ਖਾਲੀ Backend::extra_ਦਲੀਲਾਂ(std::ostream *&f, std::string &filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, ਮਾਪ_t argidx, ਤਾਰਕਿਕ bin_output)
{
	ਤਾਰਕਿਕ called_with_fp = f != ਨਿਰਾ;

	ਲਈ (; argidx < ਦਲੀਲਾਂ_2.ਮਾਪ(); argidx++)
	{
		std::string arg = ਦਲੀਲਾਂ_2[argidx];

		ਜੇ (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(ਦਲੀਲਾਂ_2, argidx, "Unknown option or option in arguments.");
		ਜੇ (f != ਨਿਰਾ)
			cmd_error(ਦਲੀਲਾਂ_2, argidx, "Extra filename argument in direct file mode.");

		ਜੇ (arg == "-") {
			filename = "<stdout>";
			f = &std::ਬਾਹਰ;
			ਜਾਰੀ;
		}

		filename = arg;
		rewrite_filename(filename);
		ਜੇ (filename.ਮਾਪ() > 3 && filename.compare(filename.ਮਾਪ()-3, std::string::npos, ".gz") == 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = ਨਵਾਂ gzip_ostream;
			ਜੇ (!gf->open(filename)) {
				ਮਿਟਾਓ gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ਪਾਓ(filename);
			f = gf;
#ਨਹੀਂਤਾਂ
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#ਜੇ_ਅੰਤ
		} ਨਹੀਂਤਾਂ {
			std::ofstream *ff = ਨਵਾਂ std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ਦੁਆਧਾਰੀ) : std::ofstream::trunc);
			yosys_output_files.ਪਾਓ(filename);
			ਜੇ (ff->fail()) {
				ਮਿਟਾਓ ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	ਜੇ (called_with_fp)
		ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(filename);
	ਦਲੀਲਾਂ_2[0] = pass_name;
	// cmd_log_args(args);

	ਜੇ (f == ਨਿਰਾ) {
		filename = "<stdout>";
		f = &std::ਬਾਹਰ;
	}
}

ਖਾਲੀ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2;
	ਅੱਖਰ *s = strdup(command.c_str());
	ਲਈ (ਅੱਖਰ *p = strtok(s, " \t\r\n"); p; p = strtok(ਨਿਰਾ, " \t\r\n"))
		ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(p);
	ਮੁਕਤ(s);
	backend_call(design, f, filename, ਦਲੀਲਾਂ_2);
}

ਖਾਲੀ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2)
{
	ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 0)
		ਵਾਪਸ;
	ਜੇ (backend_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(ਦਲੀਲਾਂ_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", ਦਲੀਲਾਂ_2[0]);

	ਮਾਪ_t orig_sel_ਢੇਰ_pos = design->selection_ਢੇਰ.ਮਾਪ();

	ਜੇ (f != ਨਿਰਾ) {
		ਸਵੈਚਾਲਿਤ state = backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->pre_execute();
		backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->execute(f, filename, ਦਲੀਲਾਂ_2, design);
		backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->post_execute(state);
	} ਨਹੀਂਤਾਂ ਜੇ (filename == "-") {
		std::ostream *f_cout = &std::ਬਾਹਰ;
		ਸਵੈਚਾਲਿਤ state = backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->pre_execute();
		backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->execute(f_cout, "<stdout>", ਦਲੀਲਾਂ_2, design);
		backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->post_execute(state);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (!filename.ਖਾਲੀ_ਥਾਵਾਂ())
			ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(filename);
		backend_ਰਜਿਸਟਰ[ਦਲੀਲਾਂ_2[0]]->execute(ਦਲੀਲਾਂ_2, design);
	}

	ਜਦੋਂ (design->selection_ਢੇਰ.ਮਾਪ() > orig_sel_ਢੇਰ_pos)
		design->ਕੱਢੋ_selection();
}

ਬਣਤਰ SimHelper {
	string name;
	ਸਤਰਅੰਦਰ string filesafe_name() {
		ਜੇ (name.at(0) == '$')
			ਜੇ (name.at(1) == '_')
				ਵਾਪਸ "gate" + name.substr(1);
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ "word_" + name.substr(1);
		ਨਹੀਂਤਾਂ
			ਵਾਪਸ name;
	}
	string title;
	string ports;
	string ਸਰੋਤ;
	string desc;
	string ਕੋਡ;
	string ਸਮੂਹ;
	string ver;
	string ਟੈਗ_ਸੂਚੀ;
};

ਸਥਿਰ ਤਾਰਕਿਕ is_ਕੋਡ_getter(string name) {
	ਵਾਪਸ *(--(name.ਸਿਰਾ())) == '+';
}

ਸਥਿਰ string get_cell_name(string name) {
	ਵਾਪਸ is_ਕੋਡ_getter(name) ? name.substr(0, name.ਲੰਮਾਈ()-1) : name;
}

ਸਥਿਰ ਖਾਲੀ log_warning_flags(Pass *pass) {
	ਤਾਰਕਿਕ has_warnings = ਝੂਠ;
	ਅਚਲ string name = pass->pass_name;
	ਜੇ (pass->experimental_flag) {
		ਜੇ (!has_warnings) ਲਘੂਗਣਕ("\n");
		has_warnings = ਸੱਚ;
		ਲਘੂਗਣਕ("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	ਜੇ (pass->ਅੰਦਰੂਨੀ_flag) {
		ਜੇ (!has_warnings) ਲਘੂਗਣਕ("\n");
		has_warnings = ਸੱਚ;
		ਲਘੂਗਣਕ("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	ਜੇ (has_warnings)
		ਲਘੂਗਣਕ("\n");
}

ਸਥਿਰ ਬਣਤਰ CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/simlib_help.inc"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/simcells_help.inc"
		cell_help.ਛਾਂਟੋ();
	}
	ਤਾਰਕਿਕ ਸ਼ਾਮਲ_ਹੈ(string name) { ਵਾਪਸ cell_help.ਗਿਣਤੀ_2(get_cell_name(name)) > 0; }
	SimHelper get(string name) { ਵਾਪਸ cell_help[get_cell_name(name)]; }
} cell_help_messages;

ਬਣਤਰ HelpPass : ਜਨਤਕ Pass {
	HelpPass() : Pass("help", "display help messages") { }
	ਖਾਲੀ help() override
	{
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    help  ................  list all commands\n");
		ਲਘੂਗਣਕ("    help <command>  ......  print help message for given command\n");
		ਲਘੂਗਣਕ("    help -all  ...........  print complete command reference\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    help -cells ..........  list all cell types\n");
		ਲਘੂਗਣਕ("    help <celltype>  .....  print help message for given cell type\n");
		ਲਘੂਗਣਕ("    help <celltype>+  ....  print verilog code for given cell type\n");
		ਲਘੂਗਣਕ("\n");
	}
	ਤਾਰਕਿਕ dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.ਇੰਦਰਾਜ("version", "Yosys command reference");
		json.ਇੰਦਰਾਜ("generator", yosys_ਸੰਸਕਰਣ_str);

		ਤਾਰਕਿਕ raise_error = ਝੂਠ;
		std::map<string, ਸਦਿਸ਼<string>> groups;

		// get root path
		ਸਵੈਚਾਲਿਤ this_ਰਸਤਾ = std::filesystem::ਰਸਤਾ_2(ਸਰੋਤ_location::current().file_name());
		ਸਵੈਚਾਲਿਤ ਸਰੋਤ_ਜੜ੍ਹ = this_ਰਸਤਾ.ਮਾਪਾ_ਰਸਤਾ().ਮਾਪਾ_ਰਸਤਾ();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : pass_ਰਜਿਸਟਰ) {
			ਸਵੈਚਾਲਿਤ name = it.ਪਹਿਲਾ;
			ਸਵੈਚਾਲਿਤ pass = it.ਦੂਜਾ;
			ਸਵੈਚਾਲਿਤ title = pass->short_help;

			ਸਵੈਚਾਲਿਤ cmd_help = PrettyHelp();
			ਸਵੈਚਾਲਿਤ has_pretty_help = pass->formatted_help();

			ਜੇ (!has_pretty_help) {
				ਗਿਣਤੀ PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				ਸਰੋਤ_location null_ਸਰੋਤ;
				string current_ਬਫਰ = "";
				ਸਵੈਚਾਲਿਤ ਜੜ੍ਹ_listing_2 = cmd_help.get_ਜੜ੍ਹ();
				ਸਵੈਚਾਲਿਤ current_listing = ਜੜ੍ਹ_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					ਸਵੈਚਾਲਿਤ log_scope = logger().sink_scope();
					logger().ਜੋੜੋ_sink<ਧਾਰਾ_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				ਮਾਪ_t def_strip_ਗਿਣਤੀ = 0;
				ਸਵੈਚਾਲਿਤ current_state = PUState_none;
				ਸਵੈਚਾਲਿਤ catch_verific = ਝੂਠ;
				ਸਵੈਚਾਲਿਤ blank_ਸਤਰਾਂ = 2;
				ਲਈ (string ਸਤਰ; std::getline(ss, ਸਤਰ, '\n');) {
					// find position of first non space character
					std::ਮਾਪ_t ਪਹਿਲਾ_pos = ਸਤਰ.ਲੱਭੋ_ਪਹਿਲਾ_not_ਦਾ(" \t");
					std::ਮਾਪ_t ਆਖਰੀ_pos = ਸਤਰ.ਲੱਭੋ_ਆਖਰੀ_not_ਦਾ(" \t");
					ਜੇ (ਪਹਿਲਾ_pos == std::string::npos) {
						ਚੋਣ (current_state)
						{
						ਹਾਲਤ PUState_signature:
							ਜੜ੍ਹ_listing_2->usage(current_ਬਫਰ, null_ਸਰੋਤ);
							current_listing = ਜੜ੍ਹ_listing_2;
							current_state = PUState_none;
							current_ਬਫਰ = "";
							ਰੋਕੋ;
						ਹਾਲਤ PUState_none:
						ਹਾਲਤ PUState_optionbody:
							blank_ਸਤਰਾਂ += 1;
							ਰੋਕੋ;
						ਮੂਲ:
							ਰੋਕੋ;
						}
						// skip empty lines
						ਜਾਰੀ;
					}

					// strip leading and trailing whitespace
					std::string stripped_ਸਤਰ = ਸਤਰ.substr(ਪਹਿਲਾ_pos, ਆਖਰੀ_pos - ਪਹਿਲਾ_pos +1);
					ਤਾਰਕਿਕ IsDefinition = stripped_ਸਤਰ[0] == '-';
					IsDefinition &= stripped_ਸਤਰ[1] != ' ' && stripped_ਸਤਰ[1] != '>';
					ਤਾਰਕਿਕ IsDedent = def_strip_ਗਿਣਤੀ && ਪਹਿਲਾ_pos < def_strip_ਗਿਣਤੀ;
					ਤਾਰਕਿਕ IsIndent = def_strip_ਗਿਣਤੀ < ਪਹਿਲਾ_pos;

					// line looks like a signature
					ਤਾਰਕਿਕ IsSignature = stripped_ਸਤਰ.ਲੱਭੋ(name) == 0 && (stripped_ਸਤਰ.ਲੰਮਾਈ() == name.ਲੰਮਾਈ() || stripped_ਸਤਰ.at(name.ਮਾਪ()) == ' ');

					ਜੇ (IsSignature && ਪਹਿਲਾ_pos <= 4 && (blank_ਸਤਰਾਂ >= 2 || current_state == PUState_signature)) {
						ਜੇ (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_ਬਫਰ, "none", null_ਸਰੋਤ);
							current_ਬਫਰ = "";
						} ਨਹੀਂਤਾਂ ਜੇ (current_state == PUState_signature) {
							ਜੜ੍ਹ_listing_2->usage(current_ਬਫਰ, null_ਸਰੋਤ);
							current_ਬਫਰ = "";
						} ਨਹੀਂਤਾਂ ਜੇ (current_state == PUState_none && !current_ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
							current_listing->codeblock(current_ਬਫਰ, "none", null_ਸਰੋਤ);
							current_ਬਫਰ = "";
						}
						current_listing = ਜੜ੍ਹ_listing_2;
						current_state = PUState_signature;
						def_strip_ਗਿਣਤੀ = ਪਹਿਲਾ_pos;
						catch_verific = ਝੂਠ;
					} ਨਹੀਂਤਾਂ ਜੇ (IsDedent) {
						def_strip_ਗਿਣਤੀ = ਪਹਿਲਾ_pos;
						ਜੇ (current_state == PUState_optionbody) {
							ਜੇ (!current_ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
								current_listing->codeblock(current_ਬਫਰ, "none", null_ਸਰੋਤ);
								current_ਬਫਰ = "";
							}
							ਜੇ (IsIndent) {
								current_state = PUState_options;
								current_listing = ਜੜ੍ਹ_listing_2->ਪਿੱਠ();
							} ਨਹੀਂਤਾਂ {
								current_state = PUState_none;
								current_listing = ਜੜ੍ਹ_listing_2;
							}
						} ਨਹੀਂਤਾਂ {
							current_state = PUState_none;
						}
					}

					ਜੇ (IsDefinition && !catch_verific && current_state != PUState_signature) {
						ਜੇ (!current_ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
							current_listing->codeblock(current_ਬਫਰ, "none", null_ਸਰੋਤ);
							current_ਬਫਰ = "";
						}
						current_state = PUState_options;
						current_listing = ਜੜ੍ਹ_listing_2->open_option(stripped_ਸਤਰ, null_ਸਰੋਤ);
						def_strip_ਗਿਣਤੀ = ਪਹਿਲਾ_pos;
					} ਨਹੀਂਤਾਂ {
						ਜੇ (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						ਜੇ (current_ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ())
							current_ਬਫਰ = stripped_ਸਤਰ;
						ਨਹੀਂਤਾਂ ਜੇ (current_state == PUState_signature && IsIndent)
							current_ਬਫਰ += stripped_ਸਤਰ;
						ਨਹੀਂਤਾਂ ਜੇ (current_state == PUState_none) {
							current_ਬਫਰ += (blank_ਸਤਰਾਂ > 0 ? "\n\n" : "\n") + ਸਤਰ;
						} ਨਹੀਂਤਾਂ
							current_ਬਫਰ += (blank_ਸਤਰਾਂ > 0 ? "\n\n" : "\n") + stripped_ਸਤਰ;
						ਜੇ (stripped_ਸਤਰ.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = ਸੱਚ;
					}
					blank_ਸਤਰਾਂ = 0;
				}

				ਜੇ (!current_ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
					ਜੇ (current_ਬਫਰ.ਮਾਪ() > 64 && current_ਬਫਰ.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_ਬਫਰ.substr(0, 62), null_ਸਰੋਤ);
						current_listing->codeblock(current_ਬਫਰ.substr(64), "yoscrypt", null_ਸਰੋਤ);
					} ਨਹੀਂਤਾਂ
						current_listing->codeblock(current_ਬਫਰ, "none", null_ਸਰੋਤ);
					current_ਬਫਰ = "";
				}
			}

			// fix path
			string ਸਰੋਤ_file = pass->location.file_name();
			ਤਾਰਕਿਕ has_ਸਰੋਤ = ਸਰੋਤ_file.compare("unknown") != 0;
			std::filesystem::ਰਸਤਾ_2 ਸਰੋਤ_ਰਸਤਾ;
			ਸਵੈਚਾਲਿਤ skip_ਸਰੋਤ_ਸਮੂਹ = ਝੂਠ;

			ਜੇ (has_ਸਰੋਤ) {
				ਸਰੋਤ_ਰਸਤਾ = fixup_ਸਰੋਤ_ਰਸਤਾ(pass->location.file_name(), ਸਰੋਤ_ਜੜ੍ਹ, &skip_ਸਰੋਤ_ਸਮੂਹ);
				ਸਰੋਤ_file = ਸਰੋਤ_ਰਸਤਾ.string();
			}

			// attempt auto group
			ਜੇ (!cmd_help.has_ਸਮੂਹ()) {
				ਜੇ (pass->ਅੰਦਰੂਨੀ_flag)
					cmd_help.ਸਮੂਹ = "internal";
				ਨਹੀਂਤਾਂ ਜੇ (ਸਰੋਤ_file.ਲੱਭੋ("backends/") == 0 || (!has_ਸਰੋਤ && name.ਲੱਭੋ("read_") == 0))
					cmd_help.ਸਮੂਹ = "backends";
				ਨਹੀਂਤਾਂ ਜੇ (ਸਰੋਤ_file.ਲੱਭੋ("frontends/") == 0 || (!has_ਸਰੋਤ && name.ਲੱਭੋ("write_") == 0))
					cmd_help.ਸਮੂਹ = "frontends";
				ਨਹੀਂਤਾਂ ਜੇ (has_ਸਰੋਤ) {
					ਜੇ (ਸਰੋਤ_ਰਸਤਾ.has_ਮਾਪਾ_ਰਸਤਾ() && !skip_ਸਰੋਤ_ਸਮੂਹ)
						cmd_help.ਸਮੂਹ = ਸਰੋਤ_ਰਸਤਾ.ਮਾਪਾ_ਰਸਤਾ().string();
				}
				// implicit !has_source
				ਨਹੀਂਤਾਂ ਜੇ (name.ਲੱਭੋ("equiv") == 0)
					cmd_help.ਸਮੂਹ = "passes/equiv";
				ਨਹੀਂਤਾਂ ਜੇ (name.ਲੱਭੋ("fsm") == 0)
					cmd_help.ਸਮੂਹ = "passes/fsm";
				ਨਹੀਂਤਾਂ ਜੇ (name.ਲੱਭੋ("memory") == 0)
					cmd_help.ਸਮੂਹ = "passes/memory";
				ਨਹੀਂਤਾਂ ਜੇ (name.ਲੱਭੋ("opt") == 0)
					cmd_help.ਸਮੂਹ = "passes/opt";
				ਨਹੀਂਤਾਂ ਜੇ (name.ਲੱਭੋ("proc") == 0)
					cmd_help.ਸਮੂਹ = "passes/proc";
			}

			ਜੇ (groups.ਗਿਣਤੀ_2(cmd_help.ਸਮੂਹ) == 0) {
				groups[cmd_help.ਸਮੂਹ] = ਸਦਿਸ਼<string>();
			}
			groups[cmd_help.ਸਮੂਹ].ਧੱਕੋ_ਪਿੱਠ(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.ਇੰਦਰਾਜ("title", title);
			json.name("content"); json.begin_array();
			ਲਈ (ਸਵੈਚਾਲਿਤ &content : cmd_help)
				json.ਮੁੱਲ(content.ਮੰਜ਼ਿਲ_json(ਸਰੋਤ_ਜੜ੍ਹ));
			json.ਸਿਰਾ_array();
			json.ਇੰਦਰਾਜ("group", cmd_help.ਸਮੂਹ);
			json.ਇੰਦਰਾਜ("source_file", ਸਰੋਤ_file);
			json.ਇੰਦਰਾਜ("source_line", pass->location.ਸਤਰ());
			json.ਇੰਦਰਾਜ("source_func", pass->location.function_name());
			json.ਇੰਦਰਾਜ("experimental_flag", pass->experimental_flag);
			json.ਇੰਦਰਾਜ("internal_flag", pass->ਅੰਦਰੂਨੀ_flag);
			json.ਸਿਰਾ_object();
		}
		json.ਸਿਰਾ_object();

		json.ਇੰਦਰਾਜ("groups", groups);

		json.ਸਿਰਾ_object();
		ਵਾਪਸ raise_error;
	}
	ਤਾਰਕਿਕ dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.ਇੰਦਰਾਜ("version", "Yosys internal cells");
		json.ਇੰਦਰਾਜ("generator", yosys_maybe_ਸੰਸਕਰਣ());

		dict<string, ਸਦਿਸ਼<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_ਸਾਰਣੀ_Builder::CellInfo>> cells;

		// iterate over cells
		ਤਾਰਕਿਕ raise_error = ਝੂਠ;
		ਲਈ (ਸਵੈਚਾਲਿਤ it : StaticCellTypes::builder.cells) {
			ਜੇ (!StaticCellTypes::categories.is_ਗਿਆਤ(it.type))
				ਜਾਰੀ;
			ਸਵੈਚਾਲਿਤ name = ID::str(it.type);
			ਜੇ (cell_help_messages.ਸ਼ਾਮਲ_ਹੈ(name)) {
				ਸਵੈਚਾਲਿਤ cell_help = cell_help_messages.get(name);
				groups[cell_help.ਸਮੂਹ].emplace_ਪਿੱਠ(name);
				ਸਵੈਚਾਲਿਤ cell_pair = pair<SimHelper, StaticCellTypes::Cell_ਸਾਰਣੀ_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} ਨਹੀਂਤਾਂ {
				ਲਘੂਗਣਕ("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= ਸੱਚ;
			}
		}
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : cell_help_messages.cell_help) {
			ਜੇ (cells.ਗਿਣਤੀ_2(it.ਪਹਿਲਾ) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.ਪਹਿਲਾ);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.ਛਾਂਟੋ();
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : groups) {
			json.name(it.ਪਹਿਲਾ.c_str()); json.ਮੁੱਲ(it.ਦੂਜਾ);
		}
		json.ਸਿਰਾ_object();

		json.name("cells"); json.begin_object();
		cells.ਛਾਂਟੋ();
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : cells) {
			ਸਵੈਚਾਲਿਤ ch = it.ਦੂਜਾ.ਪਹਿਲਾ;
			ਸਵੈਚਾਲਿਤ ct = it.ਦੂਜਾ.ਦੂਜਾ;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.ਮੁੱਲ(ch.title);
			json.name("ports"); json.ਮੁੱਲ(ch.ports);
			json.name("source"); json.ਮੁੱਲ(ch.ਸਰੋਤ);
			json.name("desc"); json.ਮੁੱਲ(ch.desc);
			json.name("code"); json.ਮੁੱਲ(ch.ਕੋਡ);
			ਸਦਿਸ਼<string> inputs, outputs;
			ਲਈ (ਸਵੈਚਾਲਿਤ &ਨਿਵੇਸ਼ : ct.inputs)
				inputs.ਧੱਕੋ_ਪਿੱਠ(ID::str(ਨਿਵੇਸ਼));
			json.name("inputs"); json.ਮੁੱਲ(inputs);
			ਲਈ (ਸਵੈਚਾਲਿਤ &output : ct.outputs)
				outputs.ਧੱਕੋ_ਪਿੱਠ(ID::str(output));
			json.name("outputs"); json.ਮੁੱਲ(outputs);
			ਸਦਿਸ਼<string> properties;
			// CellType properties
			ਜੇ (ct.features.is_evaluable) properties.ਧੱਕੋ_ਪਿੱਠ("is_evaluable");
			ਜੇ (ct.features.is_combinatorial) properties.ਧੱਕੋ_ਪਿੱਠ("is_combinatorial");
			ਜੇ (ct.features.is_synthesizable) properties.ਧੱਕੋ_ਪਿੱਠ("is_synthesizable");
			// SimHelper properties
			ਮਾਪ_t ਆਖਰੀ = 0; ਮਾਪ_t ਅਗਲਾ = 0;
			ਜਦੋਂ ((ਅਗਲਾ = ch.ਟੈਗ_ਸੂਚੀ.ਲੱਭੋ(", ", ਆਖਰੀ)) != string::npos) {
				properties.ਧੱਕੋ_ਪਿੱਠ(ch.ਟੈਗ_ਸੂਚੀ.substr(ਆਖਰੀ, ਅਗਲਾ-ਆਖਰੀ));
				ਆਖਰੀ = ਅਗਲਾ + 2;
			}
			ਸਵੈਚਾਲਿਤ final_ਟੈਗ = ch.ਟੈਗ_ਸੂਚੀ.substr(ਆਖਰੀ);
			ਜੇ (final_ਟੈਗ.ਮਾਪ()) properties.ਧੱਕੋ_ਪਿੱਠ(final_ਟੈਗ);
			json.name("properties"); json.ਮੁੱਲ(properties);
			json.ਸਿਰਾ_object();
		}
		json.ਸਿਰਾ_object();

		json.ਸਿਰਾ_object();
		ਵਾਪਸ raise_error;
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design*) override
	{
		ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 1) {
			ਲਘੂਗਣਕ("\n");
			ਲਈ (ਸਵੈਚਾਲਿਤ &it : pass_ਰਜਿਸਟਰ)
				ਲਘੂਗਣਕ("    %-20s %s\n", it.ਪਹਿਲਾ, it.ਦੂਜਾ->short_help);
			ਲਘੂਗਣਕ("\n");
			ਲਘੂਗਣਕ("Type 'help <command>' for more information on a command.\n");
			ਲਘੂਗਣਕ("Type 'help -cells' for a list of all cell types.\n");
			ਲਘੂਗਣਕ("\n");
			ਵਾਪਸ;
		}

		ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 2) {
			ਜੇ (ਦਲੀਲਾਂ_2[1] == "-all") {
				ਲਈ (ਸਵੈਚਾਲਿਤ &it : pass_ਰਜਿਸਟਰ) {
					ਲਘੂਗਣਕ("\n");
					ਲਘੂਗਣਕ("\n");
					ਲਘੂਗਣਕ("%s  --  %s\n", it.ਪਹਿਲਾ, it.ਦੂਜਾ->short_help);
					std::string ਸੁਨੇਹਾ;
					ਲਈ (ਮਾਪ_t i = 0; i < it.ਪਹਿਲਾ.ਮਾਪ() + it.ਦੂਜਾ->short_help.ਮਾਪ() + 6; i++)
						ਸੁਨੇਹਾ += '=';
					ਲਘੂਗਣਕ("%s\n", ਸੁਨੇਹਾ);
					it.ਦੂਜਾ->help();
					log_warning_flags(it.ਦੂਜਾ);
				}
			}
			ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2[1] == "-cells") {
				ਲਘੂਗਣਕ("\n");
				ਲਈ (ਸਵੈਚਾਲਿਤ &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.ਦੂਜਾ;
					ਲਘੂਗਣਕ("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				ਲਘੂਗਣਕ("\n");
				ਲਘੂਗਣਕ("Type 'help <cell_type>' for more information on a cell type.\n");
				ਲਘੂਗਣਕ("\n");
				ਵਾਪਸ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (pass_ਰਜਿਸਟਰ.ਗਿਣਤੀ_2(ਦਲੀਲਾਂ_2[1])) {
				pass_ਰਜਿਸਟਰ.at(ਦਲੀਲਾਂ_2[1])->help();
				log_warning_flags(pass_ਰਜਿਸਟਰ.at(ਦਲੀਲਾਂ_2[1]));
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell_help_messages.ਸ਼ਾਮਲ_ਹੈ(ਦਲੀਲਾਂ_2[1])) {
				ਸਵੈਚਾਲਿਤ help_cell = cell_help_messages.get(ਦਲੀਲਾਂ_2[1]);
				ਜੇ (is_ਕੋਡ_getter(ਦਲੀਲਾਂ_2[1])) {
						ਲਘੂਗਣਕ("\n");
						ਲਘੂਗਣਕ("%s\n", help_cell.ਕੋਡ);
				} ਨਹੀਂਤਾਂ {
					ਲਘੂਗਣਕ("\n");
					ਲਘੂਗਣਕ("    %s %s\n", help_cell.name, help_cell.ports);
					ਲਘੂਗਣਕ("\n");
					ਜੇ (help_cell.ver == "2" || help_cell.ver == "2a") {
						ਜੇ (help_cell.title != "") ਲਘੂਗਣਕ("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						ਲਈ (std::string ਸਤਰ; std::getline(ss, ਸਤਰ, '\n');) {
							ਜੇ (ਸਤਰ != "::") ਲਘੂਗਣਕ("%s\n", ਸਤਰ);
						}
					} ਨਹੀਂਤਾਂ ਜੇ (help_cell.desc.ਲੰਮਾਈ()) {
						ਲਘੂਗਣਕ("%s\n", help_cell.desc);
					} ਨਹੀਂਤਾਂ {
						ਲਘੂਗਣਕ("No help message for this cell type found.\n");
					}
					ਲਘੂਗਣਕ("\n");
					ਲਘੂਗਣਕ("Run 'help %s+' to display the Verilog model for this cell type.\n", ਦਲੀਲਾਂ_2[1]);
					ਲਘੂਗਣਕ("\n");
				}
			}
			ਨਹੀਂਤਾਂ
				ਲਘੂਗਣਕ("No such command or cell type: %s\n", ਦਲੀਲਾਂ_2[1]);
			ਵਾਪਸ;
		} ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 3) {
			// this option is undocumented as it is for internal use only
			ਜੇ (ਦਲੀਲਾਂ_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				ਜੇ (!json.write_ਮੰਜ਼ਿਲ_file(ਦਲੀਲਾਂ_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", ਦਲੀਲਾਂ_2[2], strerror(errno));
				ਜੇ (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2[1] == "-dump-cells-json") {
				PrettyJson json;
				ਜੇ (!json.write_ਮੰਜ਼ਿਲ_file(ਦਲੀਲਾਂ_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", ਦਲੀਲਾਂ_2[2], strerror(errno));
				ਜੇ (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			ਨਹੀਂਤਾਂ
				ਲਘੂਗਣਕ("Unknown help command: `%s %s'\n", ਦਲੀਲਾਂ_2[1], ਦਲੀਲਾਂ_2[2]);
			ਵਾਪਸ;
		}

		help();
	}
} HelpPass;

ਬਣਤਰ EchoPass : ਜਨਤਕ Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	ਖਾਲੀ help() override
	{
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    echo on\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("Print all commands to log before executing them.\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    echo off\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("Do not print all commands to log before executing them. (default)\n");
		ਲਘੂਗਣਕ("\n");
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string> ਦਲੀਲਾਂ_2, RTLIL::Design*) override
	{
		ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() > 2)
			cmd_error(ਦਲੀਲਾਂ_2, 2, "Unexpected argument.");

		ਜੇ (ਦਲੀਲਾਂ_2.ਮਾਪ() == 2) {
			ਜੇ (ਦਲੀਲਾਂ_2[1] == "on")
				echo_ਬਹੁਲਕ = ਸੱਚ;
			ਨਹੀਂਤਾਂ ਜੇ (ਦਲੀਲਾਂ_2[1] == "off")
				echo_ਬਹੁਲਕ = ਝੂਠ;
			ਨਹੀਂਤਾਂ
				cmd_error(ਦਲੀਲਾਂ_2, 1, "Unexpected argument.");
		}

		ਲਘੂਗਣਕ("echo %s\n", echo_ਬਹੁਲਕ ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ਬਣਤਰ MinisatSatSolver : ਜਨਤਕ SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = ਇਹ;
	}
	ezSAT *create() override {
		ਵਾਪਸ ਨਵਾਂ ezMiniSAT();
	}
} MinisatSatSolver;

ਬਣਤਰ LicensePass : ਜਨਤਕ Pass {
	LicensePass() : Pass("license", "print license terms") { }
	ਖਾਲੀ help() override
	{
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("    license\n");
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ("This command produces the following notice.\n");
		notice();
	}
	ਖਾਲੀ execute(std::ਸਦਿਸ਼<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	ਖਾਲੀ notice()
	{
		ਲਘੂਗਣਕ("\n");
		ਲਘੂਗਣਕ(" /----------------------------------------------------------------------------\\\n");
		ਲਘੂਗਣਕ(" |                                                                            |\n");
		ਲਘੂਗਣਕ(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		ਲਘੂਗਣਕ(" |                                                                            |\n");
		ਲਘੂਗਣਕ(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		ਲਘੂਗਣਕ(" |                                                                            |\n");
		ਲਘੂਗਣਕ(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		ਲਘੂਗਣਕ(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		ਲਘੂਗਣਕ(" |  copyright notice and this permission notice appear in all copies.         |\n");
		ਲਘੂਗਣਕ(" |                                                                            |\n");
		ਲਘੂਗਣਕ(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		ਲਘੂਗਣਕ(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		ਲਘੂਗਣਕ(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		ਲਘੂਗਣਕ(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		ਲਘੂਗਣਕ(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		ਲਘੂਗਣਕ(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		ਲਘੂਗਣਕ(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		ਲਘੂਗਣਕ(" |                                                                            |\n");
		ਲਘੂਗਣਕ(" \\----------------------------------------------------------------------------/\n");
		ਲਘੂਗਣਕ("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_ਸਿਰਾ

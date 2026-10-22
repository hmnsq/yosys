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

#ضمن "kernel/yosys.h"
#ضمن "kernel/satgen.h"
#ضمن "kernel/json.h"
#ضمن "kernel/gzip.h"
#ضمن "kernel/log_help.h"
#ضمن "kernel/newcelltypes.h"

#ضمن <string.h>
#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <errno.h>
#ضمن <filesystem>

YOSYS_NAMESPACE_BEGIN

#وصف_ڪريو MAX_REG_ڳڻپ 1000

منطقي echo_گهڻائي = خطأ;
Pass *پهريون_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_رجسٽر;
std::map<std::string, Pass*> pass_رجسٽر;
std::map<std::string, Backend*> backend_رجسٽر;
std::map<std::string, هدف> هدف_رجسٽر;

std::ويڪٽر<std::string> Frontend::ايندڙ_دليلون;

منطقي GarbageCollectionGuard::is_enabled_ = سچ;

ساكن منطقي garbage_collection_requested = خطأ;

فراغ request_garbage_collection()
{
	garbage_collection_requested = سچ;
}

فراغ try_collect_garbage()
{
	إذا (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		أرجع;
	garbage_collection_requested = خطأ;
	إذا (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, ذريعو_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	ايندڙ_queued_pass = پهريون_queued_pass;
	پهريون_queued_pass = هذا;
	call_counter = 0;
	runtime_ns = 0;
}

فراغ Pass::لاڳيتو_رجسٽر()
{
	إذا (pass_رجسٽر.ڳڻپ(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٽر[pass_name] = هذا;
}

فراغ Pass::init_رجسٽر()
{
	ويڪٽر<Pass*> added_passes;
	طالما (پهريون_queued_pass) {
		added_passes.ڌڪو_پٺ(پهريون_queued_pass);
		پهريون_queued_pass->لاڳيتو_رجسٽر();
		پهريون_queued_pass = پهريون_queued_pass->ايندڙ_queued_pass;
	}
	لكل (خودڪار added_pass : added_passes)
		added_pass->on_رجسٽر();
}

فراغ Pass::پورو_رجسٽر()
{
	لكل (خودڪار &it : pass_رجسٽر)
		it.ٻيو->on_shutdown();

	frontend_رجسٽر.clear();
	pass_رجسٽر.clear();
	backend_رجسٽر.clear();
	log_assert(پهريون_queued_pass == عدم);
}

فراغ Pass::on_رجسٽر()
{
}

فراغ Pass::on_shutdown()
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
	state.والد_pass = current_pass;
	current_pass = هذا;
	clear_flags();
	أرجع state;
}

فراغ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t وقت_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += وقت_ns;
	current_pass = state.والد_pass;
	ڪٽيو_from_current_runtime_ns(وقت_ns);
}

فراغ Pass::ڪٽيو_from_current_runtime_ns(int64_t وقت_ns)
{
	إذا (current_pass)
		current_pass->runtime_ns -= وقت_ns;
}

فراغ Pass::help()
{
	خودڪار prettyHelp = PrettyHelp();
	إذا (formatted_help()) {
		prettyHelp.log_help();
	} وإلا {
		لاگرٿم("\n");
		لاگرٿم("No help message for command `%s'.\n", pass_name);
		لاگرٿم("\n");
	}
}

منطقي Pass::formatted_help()
{
	أرجع خطأ;
}

فراغ Pass::clear_flags()
{
}

فراغ Pass::cmd_log_دليلون(ثابت std::ويڪٽر<std::string> &دليلون_2)
{
	إذا (دليلون_2.سائيز() <= 1)
		أرجع;
	std::string پيغام = "Full command line:";
	لكل (سائيز_t i = 0; i < دليلون_2.سائيز(); i++)
		پيغام += stringf(" %s", دليلون_2[i]);
	لاگرٿم("%s\n", پيغام);
}

فراغ Pass::cmd_error(ثابت std::ويڪٽر<std::string> &دليلون_2, سائيز_t argidx, std::string نياپو)
{
	std::string command_متن;
	صحيح error_pos = 0;

	لكل (سائيز_t i = 0; i < دليلون_2.سائيز(); i++) {
		إذا (i < argidx)
			error_pos += دليلون_2[i].سائيز() + 1;
		command_متن = command_متن + (command_متن.خالي_جايون() ? "" : " ") + دليلون_2[i];
	}

	لاگرٿم("\n");
	لاگرٿم("Syntax error in command `%s':\n", command_متن);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			نياپو.c_str(), command_متن.c_str(), error_pos, "");
}

فراغ Pass::extra_دليلون(std::ويڪٽر<std::string> دليلون_2, سائيز_t argidx, RTLIL::Design *design, منطقي select)
{
	لكل (; argidx < دليلون_2.سائيز(); argidx++)
	{
		std::string arg = دليلون_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(دليلون_2, argidx, "Unknown option or option in arguments.");

		إذا (!select)
			cmd_error(دليلون_2, argidx, "Extra argument.");

		handle_extra_select_دليلون(هذا, دليلون_2, argidx, دليلون_2.سائيز(), design);
		اكسر;
	}
	// cmd_log_args(args);
}

فراغ Pass::call(RTLIL::Design *design, std::string command)
{
	std::ويڪٽر<std::string> دليلون_2;

	std::string cmd_buf = command;
	std::string tok = ايندڙ_token(cmd_buf, " \t\r\n", سچ);

	إذا (tok.خالي_جايون())
		أرجع;

	إذا (tok[0] == '!') {
#إذا defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.ڳوليو('!') + 1);
		طالما (!cmd_buf.خالي_جايون() && (cmd_buf.پٺ() == ' ' || cmd_buf.پٺ() == '\t' ||
				cmd_buf.پٺ() == '\r' || cmd_buf.پٺ() == '\n'))
			cmd_buf.resize(cmd_buf.سائيز()-1);
		log_سرو(design, "Shell command: %s\n", cmd_buf);
		صحيح ret_ڪوڊ = لاڳيتو_command(cmd_buf);
		إذا (ret_ڪوڊ != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_ڪوڊ);
		أرجع;
#وإلا
		log_cmd_error("Shell is not available.\n");
#جيڪڏهن_پورو
	}

	طالما (!tok.خالي_جايون()) {
		إذا (tok[0] == '#') {
			صحيح stop;
			لكل (stop = 0; stop < Get_سائيز(cmd_buf); stop++)
				إذا (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					اكسر;
			cmd_buf = cmd_buf.substr(stop);
		} وإلا
		إذا (tok.پٺ() == ';') {
			صحيح انگ_semikolon = 0;
			طالما (!tok.خالي_جايون() && tok.پٺ() == ';')
				tok.resize(tok.سائيز()-1), انگ_semikolon++;
			إذا (!tok.خالي_جايون())
				دليلون_2.ڌڪو_پٺ(tok);
			call(design, دليلون_2);
			دليلون_2.clear();
			إذا (انگ_semikolon == 2)
				call(design, "clean");
			إذا (انگ_semikolon == 3)
				call(design, "clean -purge");
		} وإلا
			دليلون_2.ڌڪو_پٺ(tok);
		منطقي مليو_nl = خطأ;
		لكل (خودڪار c : cmd_buf) {
			إذا (c == ' ' || c == '\t')
				استمر;
			إذا (c == '\r' || c == '\n')
				مليو_nl = سچ;
			اكسر;
		}
		إذا (مليو_nl) {
			call(design, دليلون_2);
			دليلون_2.clear();
		}
		tok = ايندڙ_token(cmd_buf, " \t\r\n", سچ);
	}

	call(design, دليلون_2);
}

فراغ Pass::call(RTLIL::Design *design, std::ويڪٽر<std::string> دليلون_2)
{
	إذا (دليلون_2.سائيز() == 0 || دليلون_2[0][0] == '#' || دليلون_2[0][0] == ':')
		أرجع;

	إذا (echo_گهڻائي) {
		std::string پيغام = stringf("%s", create_prompt(design, 0, خطأ));
		لكل (سائيز_t i = 0; i < دليلون_2.سائيز(); i++)
			پيغام += stringf("%s%s", i ? " " : "", دليلون_2[i]);
		لاگرٿم("\n");
		لاگرٿم("%s\n", پيغام);
	}

	إذا (pass_رجسٽر.ڳڻپ(دليلون_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", دليلون_2[0]);
	Pass *pass = pass_رجسٽر[دليلون_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	إذا (pass->experimental_flag)
		log_experimental(دليلون_2[0]);

	سائيز_t orig_sel_ڍير_pos = design->selection_ڍير.سائيز();
	خودڪار state = pass->pre_execute();
	pass->execute(دليلون_2, design);
	pass->post_execute(state);
	طالما (design->selection_ڍير.سائيز() > orig_sel_ڍير_pos)
		design->ڪڍو_selection();
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module.clear();
	design->ڌڪو_selection(selection);

	Pass::call(design, command);

	design->ڪڍو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::ويڪٽر<std::string> دليلون_2)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module.clear();
	design->ڌڪو_selection(selection);

	Pass::call(design, دليلون_2);

	design->ڪڍو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module = module->name;
	design->ڌڪو_خالي_جايون_selection();
	design->select(module);

	Pass::call(design, command);

	design->ڪڍو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ويڪٽر<std::string> دليلون_2)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module = module->name;
	design->ڌڪو_خالي_جايون_selection();
	design->select(module);

	Pass::call(design, دليلون_2);

	design->ڪڍو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

منطقي ScriptPass::check_label(std::string label, std::string info)
{
	إذا (سرگرم_design == خالي_اشارو) {
		لاگرٿم("\n");
		إذا (info.خالي_جايون())
			لاگرٿم("    %s:\n", label);
		وإلا
			لاگرٿم("    %s:    %s\n", label, info);
		أرجع سچ;
	} وإلا {
		إذا (!سرگرم_لاڳيتو_from.خالي_جايون() && سرگرم_لاڳيتو_from == سرگرم_لاڳيتو_منزل) {
			بلاڪ_سرگرم = (label == سرگرم_لاڳيتو_from);
		} وإلا {
			إذا (label == سرگرم_لاڳيتو_from)
				بلاڪ_سرگرم = سچ;
			إذا (label == سرگرم_لاڳيتو_منزل)
				بلاڪ_سرگرم = خطأ;
		}
		أرجع بلاڪ_سرگرم;
	}
}

فراغ ScriptPass::لاڳيتو(std::string command, std::string info)
{
	إذا (سرگرم_design == خالي_اشارو) {
		إذا (info.خالي_جايون())
			لاگرٿم("        %s\n", command);
		وإلا
			لاگرٿم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(سرگرم_design, command);
		سرگرم_design->check();
	}
}

فراغ ScriptPass::لاڳيتو_nocheck(std::string command, std::string info)
{
	إذا (سرگرم_design == خالي_اشارو) {
		إذا (info.خالي_جايون())
			لاگرٿم("        %s\n", command);
		وإلا
			لاگرٿم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(سرگرم_design, command);
	}
}

فراغ ScriptPass::لاڳيتو_script(RTLIL::Design *design, std::string لاڳيتو_from, std::string لاڳيتو_منزل)
{
	help_گهڻائي = خطأ;
	سرگرم_design = design;
	بلاڪ_سرگرم = لاڳيتو_from.خالي_جايون();
	سرگرم_لاڳيتو_from = لاڳيتو_from;
	سرگرم_لاڳيتو_منزل = لاڳيتو_منزل;
	script();
}

فراغ ScriptPass::help_script()
{
	clear_flags();
	help_گهڻائي = سچ;
	سرگرم_design = خالي_اشارو;
	بلاڪ_سرگرم = سچ;
	سرگرم_لاڳيتو_from.clear();
	سرگرم_لاڳيتو_منزل.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, ذريعو_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Frontend::لاڳيتو_رجسٽر()
{
	إذا (pass_رجسٽر.ڳڻپ(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٽر[pass_name] = هذا;

	إذا (frontend_رجسٽر.ڳڻپ(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_رجسٽر[frontend_name] = هذا;
}

Frontend::~Frontend()
{
}

فراغ Frontend::execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design)
{
	log_assert(ايندڙ_دليلون.خالي_جايون());
	افعل {
		std::istream *f = عدم;
		ايندڙ_دليلون.clear();
		خودڪار state = pre_execute();
		execute(f, std::string(), دليلون_2, design);
		post_execute(state);
		دليلون_2 = ايندڙ_دليلون;
		ميٽيو f;
	} طالما (!دليلون_2.خالي_جايون());
}

FILE *Frontend::current_script_file = عدم;
std::string Frontend::آخري_هتي_document;
std::string Frontend::current_script_filename;
صحيح Frontend::current_script_lineno = 0;

فراغ Frontend::extra_دليلون(std::istream *&f, std::string &filename, std::ويڪٽر<std::string> دليلون_2, سائيز_t argidx, منطقي bin_داخلا)
{
	منطقي called_with_fp = f != عدم;

	ايندڙ_دليلون.clear();

	إذا (argidx < دليلون_2.سائيز())
	{
		std::string arg = دليلون_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(دليلون_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(دليلون_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		إذا (filename == "<<" && argidx+1 < دليلون_2.سائيز())
			filename += دليلون_2[++argidx];
		إذا (filename.compare(0, 2, "<<") == 0) {
			إذا (filename.سائيز() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == خالي_اشارو) ?  "<stdin>" : "<inlined>";
			آخري_هتي_document.clear();
			طالما (1) {
				std::string بفر;
				حرف بلاڪ[4096];
				طالما (1) {
					إذا (fgets(بلاڪ, 4096, Frontend::current_script_file == خالي_اشارو? stdin : Frontend::current_script_file) == خالي_اشارو)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					بفر += بلاڪ;
					إذا (بفر.سائيز() > 0 && (بفر[بفر.سائيز() - 1] == '\n' || بفر[بفر.سائيز() - 1] == '\r'))
						اكسر;
				}
				Frontend::current_script_lineno++;
				سائيز_t indent = بفر.ڳوليو_پهريون_not_جو(" \t\r\n");
				إذا (indent != std::string::npos && بفر.compare(indent, eot_marker.سائيز(), eot_marker) == 0)
					اكسر;
				آخري_هتي_document += بفر;
			}
			f = جديد std::istringstream(آخري_هتي_document);
		} وإلا {
			rewrite_filename(filename);
			ويڪٽر<string> filenames = glob_filename(filename);
			filename = filenames.front();
			إذا (Get_سائيز(filenames) > 1) {
				ايندڙ_دليلون.وجهو(ايندڙ_دليلون.پڇاڙي(), دليلون_2.begin(), دليلون_2.begin()+argidx);
				ايندڙ_دليلون.وجهو(ايندڙ_دليلون.پڇاڙي(), filenames.begin()+1, filenames.پڇاڙي());
			}
			yosys_داخلا_files.وجهو(filename);
			f = uncompressed(filename, bin_داخلا ? std::ifstream::ٻٽو : std::ifstream::اندريون);
		}

		لكل (سائيز_t i = argidx+1; i < دليلون_2.سائيز(); i++)
			إذا (دليلون_2[i].compare(0, 1, "-") == 0)
				cmd_error(دليلون_2, i, "Found option, expected arguments.");

		إذا (argidx+1 < دليلون_2.سائيز()) {
			إذا (ايندڙ_دليلون.خالي_جايون())
				ايندڙ_دليلون.وجهو(ايندڙ_دليلون.پڇاڙي(), دليلون_2.begin(), دليلون_2.begin()+argidx);
			ايندڙ_دليلون.وجهو(ايندڙ_دليلون.پڇاڙي(), دليلون_2.begin()+argidx+1, دليلون_2.پڇاڙي());
			دليلون_2.erase(دليلون_2.begin()+argidx+1, دليلون_2.پڇاڙي());
		}
	}

	إذا (f == عدم)
		cmd_error(دليلون_2, argidx, "No filename given.");

	إذا (called_with_fp)
		دليلون_2.ڌڪو_پٺ(filename);
	دليلون_2[0] = pass_name;
	// cmd_log_args(args);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::ويڪٽر<std::string> دليلون_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		دليلون_2.ڌڪو_پٺ(p);
	حرر(s);
	frontend_call(design, f, filename, دليلون_2);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ويڪٽر<std::string> دليلون_2)
{
	إذا (دليلون_2.سائيز() == 0)
		أرجع;
	إذا (frontend_رجسٽر.ڳڻپ(دليلون_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", دليلون_2[0]);

	إذا (f != عدم) {
		خودڪار state = frontend_رجسٽر[دليلون_2[0]]->pre_execute();
		frontend_رجسٽر[دليلون_2[0]]->execute(f, filename, دليلون_2, design);
		frontend_رجسٽر[دليلون_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::istream *f_cin = &std::اندر;
		خودڪار state = frontend_رجسٽر[دليلون_2[0]]->pre_execute();
		frontend_رجسٽر[دليلون_2[0]]->execute(f_cin, "<stdin>", دليلون_2, design);
		frontend_رجسٽر[دليلون_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.خالي_جايون())
			دليلون_2.ڌڪو_پٺ(filename);
		frontend_رجسٽر[دليلون_2[0]]->execute(دليلون_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, ذريعو_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Backend::لاڳيتو_رجسٽر()
{
	إذا (pass_رجسٽر.ڳڻپ(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٽر[pass_name] = هذا;

	إذا (backend_رجسٽر.ڳڻپ(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_رجسٽر[backend_name] = هذا;
}

Backend::~Backend()
{
}

فراغ Backend::execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design *design)
{
	std::ostream *f = عدم;
	خودڪار state = pre_execute();
	execute(f, std::string(), دليلون_2, design);
	post_execute(state);
	إذا (f != &std::ٻاهر)
		ميٽيو f;
}

فراغ Backend::extra_دليلون(std::ostream *&f, std::string &filename, std::ويڪٽر<std::string> دليلون_2, سائيز_t argidx, منطقي bin_output)
{
	منطقي called_with_fp = f != عدم;

	لكل (; argidx < دليلون_2.سائيز(); argidx++)
	{
		std::string arg = دليلون_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(دليلون_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(دليلون_2, argidx, "Extra filename argument in direct file mode.");

		إذا (arg == "-") {
			filename = "<stdout>";
			f = &std::ٻاهر;
			استمر;
		}

		filename = arg;
		rewrite_filename(filename);
		إذا (filename.سائيز() > 3 && filename.compare(filename.سائيز()-3, std::string::npos, ".gz") == 0) {
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = جديد gzip_ostream;
			إذا (!gf->open(filename)) {
				ميٽيو gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.وجهو(filename);
			f = gf;
#وإلا
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#جيڪڏهن_پورو
		} وإلا {
			std::ofstream *ff = جديد std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ٻٽو) : std::ofstream::trunc);
			yosys_output_files.وجهو(filename);
			إذا (ff->fail()) {
				ميٽيو ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	إذا (called_with_fp)
		دليلون_2.ڌڪو_پٺ(filename);
	دليلون_2[0] = pass_name;
	// cmd_log_args(args);

	إذا (f == عدم) {
		filename = "<stdout>";
		f = &std::ٻاهر;
	}
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::ويڪٽر<std::string> دليلون_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		دليلون_2.ڌڪو_پٺ(p);
	حرر(s);
	backend_call(design, f, filename, دليلون_2);
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ويڪٽر<std::string> دليلون_2)
{
	إذا (دليلون_2.سائيز() == 0)
		أرجع;
	إذا (backend_رجسٽر.ڳڻپ(دليلون_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", دليلون_2[0]);

	سائيز_t orig_sel_ڍير_pos = design->selection_ڍير.سائيز();

	إذا (f != عدم) {
		خودڪار state = backend_رجسٽر[دليلون_2[0]]->pre_execute();
		backend_رجسٽر[دليلون_2[0]]->execute(f, filename, دليلون_2, design);
		backend_رجسٽر[دليلون_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::ostream *f_cout = &std::ٻاهر;
		خودڪار state = backend_رجسٽر[دليلون_2[0]]->pre_execute();
		backend_رجسٽر[دليلون_2[0]]->execute(f_cout, "<stdout>", دليلون_2, design);
		backend_رجسٽر[دليلون_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.خالي_جايون())
			دليلون_2.ڌڪو_پٺ(filename);
		backend_رجسٽر[دليلون_2[0]]->execute(دليلون_2, design);
	}

	طالما (design->selection_ڍير.سائيز() > orig_sel_ڍير_pos)
		design->ڪڍو_selection();
}

بنية SimHelper {
	string name;
	مضمن string filesafe_name() {
		إذا (name.at(0) == '$')
			إذا (name.at(1) == '_')
				أرجع "gate" + name.substr(1);
			وإلا
				أرجع "word_" + name.substr(1);
		وإلا
			أرجع name;
	}
	string title;
	string ports;
	string ذريعو;
	string desc;
	string ڪوڊ;
	string گروهه;
	string ver;
	string ٽيگ_فهرست;
};

ساكن منطقي is_ڪوڊ_getter(string name) {
	أرجع *(--(name.پڇاڙي())) == '+';
}

ساكن string get_cell_name(string name) {
	أرجع is_ڪوڊ_getter(name) ? name.substr(0, name.ڊيگهائي()-1) : name;
}

ساكن فراغ log_warning_flags(Pass *pass) {
	منطقي has_warnings = خطأ;
	ثابت string name = pass->pass_name;
	إذا (pass->experimental_flag) {
		إذا (!has_warnings) لاگرٿم("\n");
		has_warnings = سچ;
		لاگرٿم("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	إذا (pass->اندروني_flag) {
		إذا (!has_warnings) لاگرٿم("\n");
		has_warnings = سچ;
		لاگرٿم("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	إذا (has_warnings)
		لاگرٿم("\n");
}

ساكن بنية CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ضمن "kernel/simlib_help.inc"
#ضمن "kernel/simcells_help.inc"
		cell_help.ترتيب_ڏيو();
	}
	منطقي شامل_آهي(string name) { أرجع cell_help.ڳڻپ(get_cell_name(name)) > 0; }
	SimHelper get(string name) { أرجع cell_help[get_cell_name(name)]; }
} cell_help_messages;

بنية HelpPass : عام Pass {
	HelpPass() : Pass("help", "display help messages") { }
	فراغ help() override
	{
		لاگرٿم("\n");
		لاگرٿم("    help  ................  list all commands\n");
		لاگرٿم("    help <command>  ......  print help message for given command\n");
		لاگرٿم("    help -all  ...........  print complete command reference\n");
		لاگرٿم("\n");
		لاگرٿم("    help -cells ..........  list all cell types\n");
		لاگرٿم("    help <celltype>  .....  print help message for given cell type\n");
		لاگرٿم("    help <celltype>+  ....  print verilog code for given cell type\n");
		لاگرٿم("\n");
	}
	منطقي dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.داخلا_2("version", "Yosys command reference");
		json.داخلا_2("generator", yosys_نسخو_str);

		منطقي raise_error = خطأ;
		std::map<string, ويڪٽر<string>> groups;

		// get root path
		خودڪار this_رستو = std::filesystem::رستو_2(ذريعو_location::current().file_name());
		خودڪار ذريعو_پاڙ = this_رستو.والد_رستو().والد_رستو();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		لكل (خودڪار &it : pass_رجسٽر) {
			خودڪار name = it.پهريون;
			خودڪار pass = it.ٻيو;
			خودڪار title = pass->short_help;

			خودڪار cmd_help = PrettyHelp();
			خودڪار has_pretty_help = pass->formatted_help();

			إذا (!has_pretty_help) {
				تعداد PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				ذريعو_location null_ذريعو;
				string current_بفر = "";
				خودڪار پاڙ_listing_2 = cmd_help.get_پاڙ();
				خودڪار current_listing = پاڙ_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					خودڪار log_scope = logger().sink_scope();
					logger().جوڙيو_sink<وهڪرو_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				سائيز_t def_strip_ڳڻپ = 0;
				خودڪار current_state = PUState_none;
				خودڪار catch_verific = خطأ;
				خودڪار blank_سٽون = 2;
				لكل (string سٽ; std::getline(ss, سٽ, '\n');) {
					// find position of first non space character
					std::سائيز_t پهريون_pos = سٽ.ڳوليو_پهريون_not_جو(" \t");
					std::سائيز_t آخري_pos = سٽ.ڳوليو_آخري_not_جو(" \t");
					إذا (پهريون_pos == std::string::npos) {
						بدل (current_state)
						{
						حالة PUState_signature:
							پاڙ_listing_2->usage(current_بفر, null_ذريعو);
							current_listing = پاڙ_listing_2;
							current_state = PUState_none;
							current_بفر = "";
							اكسر;
						حالة PUState_none:
						حالة PUState_optionbody:
							blank_سٽون += 1;
							اكسر;
						افتراضي:
							اكسر;
						}
						// skip empty lines
						استمر;
					}

					// strip leading and trailing whitespace
					std::string stripped_سٽ = سٽ.substr(پهريون_pos, آخري_pos - پهريون_pos +1);
					منطقي IsDefinition = stripped_سٽ[0] == '-';
					IsDefinition &= stripped_سٽ[1] != ' ' && stripped_سٽ[1] != '>';
					منطقي IsDedent = def_strip_ڳڻپ && پهريون_pos < def_strip_ڳڻپ;
					منطقي IsIndent = def_strip_ڳڻپ < پهريون_pos;

					// line looks like a signature
					منطقي IsSignature = stripped_سٽ.ڳوليو(name) == 0 && (stripped_سٽ.ڊيگهائي() == name.ڊيگهائي() || stripped_سٽ.at(name.سائيز()) == ' ');

					إذا (IsSignature && پهريون_pos <= 4 && (blank_سٽون >= 2 || current_state == PUState_signature)) {
						إذا (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_بفر, "none", null_ذريعو);
							current_بفر = "";
						} وإلا إذا (current_state == PUState_signature) {
							پاڙ_listing_2->usage(current_بفر, null_ذريعو);
							current_بفر = "";
						} وإلا إذا (current_state == PUState_none && !current_بفر.خالي_جايون()) {
							current_listing->codeblock(current_بفر, "none", null_ذريعو);
							current_بفر = "";
						}
						current_listing = پاڙ_listing_2;
						current_state = PUState_signature;
						def_strip_ڳڻپ = پهريون_pos;
						catch_verific = خطأ;
					} وإلا إذا (IsDedent) {
						def_strip_ڳڻپ = پهريون_pos;
						إذا (current_state == PUState_optionbody) {
							إذا (!current_بفر.خالي_جايون()) {
								current_listing->codeblock(current_بفر, "none", null_ذريعو);
								current_بفر = "";
							}
							إذا (IsIndent) {
								current_state = PUState_options;
								current_listing = پاڙ_listing_2->پٺ();
							} وإلا {
								current_state = PUState_none;
								current_listing = پاڙ_listing_2;
							}
						} وإلا {
							current_state = PUState_none;
						}
					}

					إذا (IsDefinition && !catch_verific && current_state != PUState_signature) {
						إذا (!current_بفر.خالي_جايون()) {
							current_listing->codeblock(current_بفر, "none", null_ذريعو);
							current_بفر = "";
						}
						current_state = PUState_options;
						current_listing = پاڙ_listing_2->open_option(stripped_سٽ, null_ذريعو);
						def_strip_ڳڻپ = پهريون_pos;
					} وإلا {
						إذا (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						إذا (current_بفر.خالي_جايون())
							current_بفر = stripped_سٽ;
						وإلا إذا (current_state == PUState_signature && IsIndent)
							current_بفر += stripped_سٽ;
						وإلا إذا (current_state == PUState_none) {
							current_بفر += (blank_سٽون > 0 ? "\n\n" : "\n") + سٽ;
						} وإلا
							current_بفر += (blank_سٽون > 0 ? "\n\n" : "\n") + stripped_سٽ;
						إذا (stripped_سٽ.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = سچ;
					}
					blank_سٽون = 0;
				}

				إذا (!current_بفر.خالي_جايون()) {
					إذا (current_بفر.سائيز() > 64 && current_بفر.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_بفر.substr(0, 62), null_ذريعو);
						current_listing->codeblock(current_بفر.substr(64), "yoscrypt", null_ذريعو);
					} وإلا
						current_listing->codeblock(current_بفر, "none", null_ذريعو);
					current_بفر = "";
				}
			}

			// fix path
			string ذريعو_file = pass->location.file_name();
			منطقي has_ذريعو = ذريعو_file.compare("unknown") != 0;
			std::filesystem::رستو_2 ذريعو_رستو;
			خودڪار skip_ذريعو_گروهه = خطأ;

			إذا (has_ذريعو) {
				ذريعو_رستو = fixup_ذريعو_رستو(pass->location.file_name(), ذريعو_پاڙ, &skip_ذريعو_گروهه);
				ذريعو_file = ذريعو_رستو.string();
			}

			// attempt auto group
			إذا (!cmd_help.has_گروهه()) {
				إذا (pass->اندروني_flag)
					cmd_help.گروهه = "internal";
				وإلا إذا (ذريعو_file.ڳوليو("backends/") == 0 || (!has_ذريعو && name.ڳوليو("read_") == 0))
					cmd_help.گروهه = "backends";
				وإلا إذا (ذريعو_file.ڳوليو("frontends/") == 0 || (!has_ذريعو && name.ڳوليو("write_") == 0))
					cmd_help.گروهه = "frontends";
				وإلا إذا (has_ذريعو) {
					إذا (ذريعو_رستو.has_والد_رستو() && !skip_ذريعو_گروهه)
						cmd_help.گروهه = ذريعو_رستو.والد_رستو().string();
				}
				// implicit !has_source
				وإلا إذا (name.ڳوليو("equiv") == 0)
					cmd_help.گروهه = "passes/equiv";
				وإلا إذا (name.ڳوليو("fsm") == 0)
					cmd_help.گروهه = "passes/fsm";
				وإلا إذا (name.ڳوليو("memory") == 0)
					cmd_help.گروهه = "passes/memory";
				وإلا إذا (name.ڳوليو("opt") == 0)
					cmd_help.گروهه = "passes/opt";
				وإلا إذا (name.ڳوليو("proc") == 0)
					cmd_help.گروهه = "passes/proc";
			}

			إذا (groups.ڳڻپ(cmd_help.گروهه) == 0) {
				groups[cmd_help.گروهه] = ويڪٽر<string>();
			}
			groups[cmd_help.گروهه].ڌڪو_پٺ(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.داخلا_2("title", title);
			json.name("content"); json.begin_array();
			لكل (خودڪار &content : cmd_help)
				json.قدر(content.منزل_json(ذريعو_پاڙ));
			json.پڇاڙي_array();
			json.داخلا_2("group", cmd_help.گروهه);
			json.داخلا_2("source_file", ذريعو_file);
			json.داخلا_2("source_line", pass->location.سٽ());
			json.داخلا_2("source_func", pass->location.function_name());
			json.داخلا_2("experimental_flag", pass->experimental_flag);
			json.داخلا_2("internal_flag", pass->اندروني_flag);
			json.پڇاڙي_object();
		}
		json.پڇاڙي_object();

		json.داخلا_2("groups", groups);

		json.پڇاڙي_object();
		أرجع raise_error;
	}
	منطقي dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.داخلا_2("version", "Yosys internal cells");
		json.داخلا_2("generator", yosys_maybe_نسخو());

		dict<string, ويڪٽر<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>> cells;

		// iterate over cells
		منطقي raise_error = خطأ;
		لكل (خودڪار it : StaticCellTypes::builder.cells) {
			إذا (!StaticCellTypes::categories.is_معلوم(it.type))
				استمر;
			خودڪار name = ID::str(it.type);
			إذا (cell_help_messages.شامل_آهي(name)) {
				خودڪار cell_help = cell_help_messages.get(name);
				groups[cell_help.گروهه].emplace_پٺ(name);
				خودڪار cell_pair = pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} وإلا {
				لاگرٿم("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= سچ;
			}
		}
		لكل (خودڪار &it : cell_help_messages.cell_help) {
			إذا (cells.ڳڻپ(it.پهريون) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.پهريون);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.ترتيب_ڏيو();
		لكل (خودڪار &it : groups) {
			json.name(it.پهريون.c_str()); json.قدر(it.ٻيو);
		}
		json.پڇاڙي_object();

		json.name("cells"); json.begin_object();
		cells.ترتيب_ڏيو();
		لكل (خودڪار &it : cells) {
			خودڪار ch = it.ٻيو.پهريون;
			خودڪار ct = it.ٻيو.ٻيو;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.قدر(ch.title);
			json.name("ports"); json.قدر(ch.ports);
			json.name("source"); json.قدر(ch.ذريعو);
			json.name("desc"); json.قدر(ch.desc);
			json.name("code"); json.قدر(ch.ڪوڊ);
			ويڪٽر<string> inputs, outputs;
			لكل (خودڪار &داخلا : ct.inputs)
				inputs.ڌڪو_پٺ(ID::str(داخلا));
			json.name("inputs"); json.قدر(inputs);
			لكل (خودڪار &output : ct.outputs)
				outputs.ڌڪو_پٺ(ID::str(output));
			json.name("outputs"); json.قدر(outputs);
			ويڪٽر<string> properties;
			// CellType properties
			إذا (ct.features.is_evaluable) properties.ڌڪو_پٺ("is_evaluable");
			إذا (ct.features.is_combinatorial) properties.ڌڪو_پٺ("is_combinatorial");
			إذا (ct.features.is_synthesizable) properties.ڌڪو_پٺ("is_synthesizable");
			// SimHelper properties
			سائيز_t آخري = 0; سائيز_t ايندڙ = 0;
			طالما ((ايندڙ = ch.ٽيگ_فهرست.ڳوليو(", ", آخري)) != string::npos) {
				properties.ڌڪو_پٺ(ch.ٽيگ_فهرست.substr(آخري, ايندڙ-آخري));
				آخري = ايندڙ + 2;
			}
			خودڪار final_ٽيگ = ch.ٽيگ_فهرست.substr(آخري);
			إذا (final_ٽيگ.سائيز()) properties.ڌڪو_پٺ(final_ٽيگ);
			json.name("properties"); json.قدر(properties);
			json.پڇاڙي_object();
		}
		json.پڇاڙي_object();

		json.پڇاڙي_object();
		أرجع raise_error;
	}
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design*) override
	{
		إذا (دليلون_2.سائيز() == 1) {
			لاگرٿم("\n");
			لكل (خودڪار &it : pass_رجسٽر)
				لاگرٿم("    %-20s %s\n", it.پهريون, it.ٻيو->short_help);
			لاگرٿم("\n");
			لاگرٿم("Type 'help <command>' for more information on a command.\n");
			لاگرٿم("Type 'help -cells' for a list of all cell types.\n");
			لاگرٿم("\n");
			أرجع;
		}

		إذا (دليلون_2.سائيز() == 2) {
			إذا (دليلون_2[1] == "-all") {
				لكل (خودڪار &it : pass_رجسٽر) {
					لاگرٿم("\n");
					لاگرٿم("\n");
					لاگرٿم("%s  --  %s\n", it.پهريون, it.ٻيو->short_help);
					std::string نياپو;
					لكل (سائيز_t i = 0; i < it.پهريون.سائيز() + it.ٻيو->short_help.سائيز() + 6; i++)
						نياپو += '=';
					لاگرٿم("%s\n", نياپو);
					it.ٻيو->help();
					log_warning_flags(it.ٻيو);
				}
			}
			وإلا إذا (دليلون_2[1] == "-cells") {
				لاگرٿم("\n");
				لكل (خودڪار &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.ٻيو;
					لاگرٿم("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				لاگرٿم("\n");
				لاگرٿم("Type 'help <cell_type>' for more information on a cell type.\n");
				لاگرٿم("\n");
				أرجع;
			}
			وإلا إذا (pass_رجسٽر.ڳڻپ(دليلون_2[1])) {
				pass_رجسٽر.at(دليلون_2[1])->help();
				log_warning_flags(pass_رجسٽر.at(دليلون_2[1]));
			}
			وإلا إذا (cell_help_messages.شامل_آهي(دليلون_2[1])) {
				خودڪار help_cell = cell_help_messages.get(دليلون_2[1]);
				إذا (is_ڪوڊ_getter(دليلون_2[1])) {
						لاگرٿم("\n");
						لاگرٿم("%s\n", help_cell.ڪوڊ);
				} وإلا {
					لاگرٿم("\n");
					لاگرٿم("    %s %s\n", help_cell.name, help_cell.ports);
					لاگرٿم("\n");
					إذا (help_cell.ver == "2" || help_cell.ver == "2a") {
						إذا (help_cell.title != "") لاگرٿم("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						لكل (std::string سٽ; std::getline(ss, سٽ, '\n');) {
							إذا (سٽ != "::") لاگرٿم("%s\n", سٽ);
						}
					} وإلا إذا (help_cell.desc.ڊيگهائي()) {
						لاگرٿم("%s\n", help_cell.desc);
					} وإلا {
						لاگرٿم("No help message for this cell type found.\n");
					}
					لاگرٿم("\n");
					لاگرٿم("Run 'help %s+' to display the Verilog model for this cell type.\n", دليلون_2[1]);
					لاگرٿم("\n");
				}
			}
			وإلا
				لاگرٿم("No such command or cell type: %s\n", دليلون_2[1]);
			أرجع;
		} وإلا إذا (دليلون_2.سائيز() == 3) {
			// this option is undocumented as it is for internal use only
			إذا (دليلون_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				إذا (!json.write_منزل_file(دليلون_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دليلون_2[2], strerror(errno));
				إذا (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			وإلا إذا (دليلون_2[1] == "-dump-cells-json") {
				PrettyJson json;
				إذا (!json.write_منزل_file(دليلون_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دليلون_2[2], strerror(errno));
				إذا (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			وإلا
				لاگرٿم("Unknown help command: `%s %s'\n", دليلون_2[1], دليلون_2[2]);
			أرجع;
		}

		help();
	}
} HelpPass;

بنية EchoPass : عام Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	فراغ help() override
	{
		لاگرٿم("\n");
		لاگرٿم("    echo on\n");
		لاگرٿم("\n");
		لاگرٿم("Print all commands to log before executing them.\n");
		لاگرٿم("\n");
		لاگرٿم("\n");
		لاگرٿم("    echo off\n");
		لاگرٿم("\n");
		لاگرٿم("Do not print all commands to log before executing them. (default)\n");
		لاگرٿم("\n");
	}
	فراغ execute(std::ويڪٽر<std::string> دليلون_2, RTLIL::Design*) override
	{
		إذا (دليلون_2.سائيز() > 2)
			cmd_error(دليلون_2, 2, "Unexpected argument.");

		إذا (دليلون_2.سائيز() == 2) {
			إذا (دليلون_2[1] == "on")
				echo_گهڻائي = سچ;
			وإلا إذا (دليلون_2[1] == "off")
				echo_گهڻائي = خطأ;
			وإلا
				cmd_error(دليلون_2, 1, "Unexpected argument.");
		}

		لاگرٿم("echo %s\n", echo_گهڻائي ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

بنية MinisatSatSolver : عام SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = هذا;
	}
	ezSAT *create() override {
		أرجع جديد ezMiniSAT();
	}
} MinisatSatSolver;

بنية LicensePass : عام Pass {
	LicensePass() : Pass("license", "print license terms") { }
	فراغ help() override
	{
		لاگرٿم("\n");
		لاگرٿم("    license\n");
		لاگرٿم("\n");
		لاگرٿم("This command produces the following notice.\n");
		notice();
	}
	فراغ execute(std::ويڪٽر<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	فراغ notice()
	{
		لاگرٿم("\n");
		لاگرٿم(" /----------------------------------------------------------------------------\\\n");
		لاگرٿم(" |                                                                            |\n");
		لاگرٿم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		لاگرٿم(" |                                                                            |\n");
		لاگرٿم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		لاگرٿم(" |                                                                            |\n");
		لاگرٿم(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		لاگرٿم(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		لاگرٿم(" |  copyright notice and this permission notice appear in all copies.         |\n");
		لاگرٿم(" |                                                                            |\n");
		لاگرٿم(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		لاگرٿم(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		لاگرٿم(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		لاگرٿم(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		لاگرٿم(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		لاگرٿم(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		لاگرٿم(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		لاگرٿم(" |                                                                            |\n");
		لاگرٿم(" \\----------------------------------------------------------------------------/\n");
		لاگرٿم("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_پڇاڙي

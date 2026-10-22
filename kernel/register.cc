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

#تعریف_کړه MAX_REG_شمېر 1000

منطقي echo_ډېرپېښ = خطأ;
Pass *لومړی_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_ثبتوونکی;
std::map<std::string, Pass*> pass_ثبتوونکی;
std::map<std::string, Backend*> backend_ثبتوونکی;
std::map<std::string, موخه> موخه_ثبتوونکی;

std::ویکټور<std::string> Frontend::بل_دلیلونه;

منطقي GarbageCollectionGuard::is_enabled_ = ریښتیا;

ساكن منطقي garbage_collection_requested = خطأ;

فراغ request_garbage_collection()
{
	garbage_collection_requested = ریښتیا;
}

فراغ try_collect_garbage()
{
	إذا (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		أرجع;
	garbage_collection_requested = خطأ;
	إذا (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, سرچینه_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	بل_queued_pass = لومړی_queued_pass;
	لومړی_queued_pass = هذا;
	call_counter = 0;
	runtime_ns = 0;
}

فراغ Pass::پرلپسې_ثبتوونکی()
{
	إذا (pass_ثبتوونکی.شمېر(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ثبتوونکی[pass_name] = هذا;
}

فراغ Pass::init_ثبتوونکی()
{
	ویکټور<Pass*> added_passes;
	طالما (لومړی_queued_pass) {
		added_passes.ټېل_شا(لومړی_queued_pass);
		لومړی_queued_pass->پرلپسې_ثبتوونکی();
		لومړی_queued_pass = لومړی_queued_pass->بل_queued_pass;
	}
	لكل (خپلکاری added_pass : added_passes)
		added_pass->on_ثبتوونکی();
}

فراغ Pass::بشپړ_ثبتوونکی()
{
	لكل (خپلکاری &it : pass_ثبتوونکی)
		it.دوهم->on_shutdown();

	frontend_ثبتوونکی.clear();
	pass_ثبتوونکی.clear();
	backend_ثبتوونکی.clear();
	log_assert(لومړی_queued_pass == عدم);
}

فراغ Pass::on_ثبتوونکی()
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
	state.پلار_pass = current_pass;
	current_pass = هذا;
	clear_flags();
	أرجع state;
}

فراغ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t وخت_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += وخت_ns;
	current_pass = state.پلار_pass;
	کمول_from_current_runtime_ns(وخت_ns);
}

فراغ Pass::کمول_from_current_runtime_ns(int64_t وخت_ns)
{
	إذا (current_pass)
		current_pass->runtime_ns -= وخت_ns;
}

فراغ Pass::help()
{
	خپلکاری prettyHelp = PrettyHelp();
	إذا (formatted_help()) {
		prettyHelp.log_help();
	} وإلا {
		لوګاریتم("\n");
		لوګاریتم("No help message for command `%s'.\n", pass_name);
		لوګاریتم("\n");
	}
}

منطقي Pass::formatted_help()
{
	أرجع خطأ;
}

فراغ Pass::clear_flags()
{
}

فراغ Pass::cmd_log_دلیلونه(ثابت std::ویکټور<std::string> &دلیلونه_2)
{
	إذا (دلیلونه_2.اندازه() <= 1)
		أرجع;
	std::string پیغام_2 = "Full command line:";
	لكل (اندازه_t i = 0; i < دلیلونه_2.اندازه(); i++)
		پیغام_2 += stringf(" %s", دلیلونه_2[i]);
	لوګاریتم("%s\n", پیغام_2);
}

فراغ Pass::cmd_error(ثابت std::ویکټور<std::string> &دلیلونه_2, اندازه_t argidx, std::string پیغام)
{
	std::string command_متن;
	صحيح error_pos = 0;

	لكل (اندازه_t i = 0; i < دلیلونه_2.اندازه(); i++) {
		إذا (i < argidx)
			error_pos += دلیلونه_2[i].اندازه() + 1;
		command_متن = command_متن + (command_متن.تشه() ? "" : " ") + دلیلونه_2[i];
	}

	لوګاریتم("\n");
	لوګاریتم("Syntax error in command `%s':\n", command_متن);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			پیغام.c_str(), command_متن.c_str(), error_pos, "");
}

فراغ Pass::extra_دلیلونه(std::ویکټور<std::string> دلیلونه_2, اندازه_t argidx, RTLIL::Design *design, منطقي select)
{
	لكل (; argidx < دلیلونه_2.اندازه(); argidx++)
	{
		std::string arg = دلیلونه_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(دلیلونه_2, argidx, "Unknown option or option in arguments.");

		إذا (!select)
			cmd_error(دلیلونه_2, argidx, "Extra argument.");

		handle_extra_select_دلیلونه(هذا, دلیلونه_2, argidx, دلیلونه_2.اندازه(), design);
		اكسر;
	}
	// cmd_log_args(args);
}

فراغ Pass::call(RTLIL::Design *design, std::string command)
{
	std::ویکټور<std::string> دلیلونه_2;

	std::string cmd_buf = command;
	std::string tok = بل_token(cmd_buf, " \t\r\n", ریښتیا);

	إذا (tok.تشه())
		أرجع;

	إذا (tok[0] == '!') {
#إذا defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.پیدا_کړه('!') + 1);
		طالما (!cmd_buf.تشه() && (cmd_buf.شا() == ' ' || cmd_buf.شا() == '\t' ||
				cmd_buf.شا() == '\r' || cmd_buf.شا() == '\n'))
			cmd_buf.resize(cmd_buf.اندازه()-1);
		log_سرلیک(design, "Shell command: %s\n", cmd_buf);
		صحيح ret_کوډ = پرلپسې_command(cmd_buf);
		إذا (ret_کوډ != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_کوډ);
		أرجع;
#وإلا
		log_cmd_error("Shell is not available.\n");
#د_که_پای
	}

	طالما (!tok.تشه()) {
		إذا (tok[0] == '#') {
			صحيح stop;
			لكل (stop = 0; stop < Get_اندازه(cmd_buf); stop++)
				إذا (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					اكسر;
			cmd_buf = cmd_buf.substr(stop);
		} وإلا
		إذا (tok.شا() == ';') {
			صحيح عدد_semikolon = 0;
			طالما (!tok.تشه() && tok.شا() == ';')
				tok.resize(tok.اندازه()-1), عدد_semikolon++;
			إذا (!tok.تشه())
				دلیلونه_2.ټېل_شا(tok);
			call(design, دلیلونه_2);
			دلیلونه_2.clear();
			إذا (عدد_semikolon == 2)
				call(design, "clean");
			إذا (عدد_semikolon == 3)
				call(design, "clean -purge");
		} وإلا
			دلیلونه_2.ټېل_شا(tok);
		منطقي وموندل_nl = خطأ;
		لكل (خپلکاری c : cmd_buf) {
			إذا (c == ' ' || c == '\t')
				استمر;
			إذا (c == '\r' || c == '\n')
				وموندل_nl = ریښتیا;
			اكسر;
		}
		إذا (وموندل_nl) {
			call(design, دلیلونه_2);
			دلیلونه_2.clear();
		}
		tok = بل_token(cmd_buf, " \t\r\n", ریښتیا);
	}

	call(design, دلیلونه_2);
}

فراغ Pass::call(RTLIL::Design *design, std::ویکټور<std::string> دلیلونه_2)
{
	إذا (دلیلونه_2.اندازه() == 0 || دلیلونه_2[0][0] == '#' || دلیلونه_2[0][0] == ':')
		أرجع;

	إذا (echo_ډېرپېښ) {
		std::string پیغام_2 = stringf("%s", create_prompt(design, 0, خطأ));
		لكل (اندازه_t i = 0; i < دلیلونه_2.اندازه(); i++)
			پیغام_2 += stringf("%s%s", i ? " " : "", دلیلونه_2[i]);
		لوګاریتم("\n");
		لوګاریتم("%s\n", پیغام_2);
	}

	إذا (pass_ثبتوونکی.شمېر(دلیلونه_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", دلیلونه_2[0]);
	Pass *pass = pass_ثبتوونکی[دلیلونه_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	إذا (pass->experimental_flag)
		log_experimental(دلیلونه_2[0]);

	اندازه_t orig_sel_ډېری_pos = design->selection_ډېری.اندازه();
	خپلکاری state = pass->pre_execute();
	pass->execute(دلیلونه_2, design);
	pass->post_execute(state);
	طالما (design->selection_ډېری.اندازه() > orig_sel_ډېری_pos)
		design->راوباسه_selection();
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module.clear();
	design->ټېل_selection(selection);

	Pass::call(design, command);

	design->راوباسه_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::ویکټور<std::string> دلیلونه_2)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module.clear();
	design->ټېل_selection(selection);

	Pass::call(design, دلیلونه_2);

	design->راوباسه_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module = module->name;
	design->ټېل_تشه_selection();
	design->select(module);

	Pass::call(design, command);

	design->راوباسه_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::ویکټور<std::string> دلیلونه_2)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module = module->name;
	design->ټېل_تشه_selection();
	design->select(module);

	Pass::call(design, دلیلونه_2);

	design->راوباسه_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

منطقي ScriptPass::check_label(std::string label, std::string info)
{
	إذا (فعال_design == تش_اشاره) {
		لوګاریتم("\n");
		إذا (info.تشه())
			لوګاریتم("    %s:\n", label);
		وإلا
			لوګاریتم("    %s:    %s\n", label, info);
		أرجع ریښتیا;
	} وإلا {
		إذا (!فعال_پرلپسې_from.تشه() && فعال_پرلپسې_from == فعال_پرلپسې_منزل) {
			بلاک_فعال = (label == فعال_پرلپسې_from);
		} وإلا {
			إذا (label == فعال_پرلپسې_from)
				بلاک_فعال = ریښتیا;
			إذا (label == فعال_پرلپسې_منزل)
				بلاک_فعال = خطأ;
		}
		أرجع بلاک_فعال;
	}
}

فراغ ScriptPass::پرلپسې(std::string command, std::string info)
{
	إذا (فعال_design == تش_اشاره) {
		إذا (info.تشه())
			لوګاریتم("        %s\n", command);
		وإلا
			لوګاریتم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(فعال_design, command);
		فعال_design->check();
	}
}

فراغ ScriptPass::پرلپسې_nocheck(std::string command, std::string info)
{
	إذا (فعال_design == تش_اشاره) {
		إذا (info.تشه())
			لوګاریتم("        %s\n", command);
		وإلا
			لوګاریتم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(فعال_design, command);
	}
}

فراغ ScriptPass::پرلپسې_script(RTLIL::Design *design, std::string پرلپسې_from, std::string پرلپسې_منزل)
{
	help_ډېرپېښ = خطأ;
	فعال_design = design;
	بلاک_فعال = پرلپسې_from.تشه();
	فعال_پرلپسې_from = پرلپسې_from;
	فعال_پرلپسې_منزل = پرلپسې_منزل;
	script();
}

فراغ ScriptPass::help_script()
{
	clear_flags();
	help_ډېرپېښ = ریښتیا;
	فعال_design = تش_اشاره;
	بلاک_فعال = ریښتیا;
	فعال_پرلپسې_from.clear();
	فعال_پرلپسې_منزل.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, سرچینه_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Frontend::پرلپسې_ثبتوونکی()
{
	إذا (pass_ثبتوونکی.شمېر(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ثبتوونکی[pass_name] = هذا;

	إذا (frontend_ثبتوونکی.شمېر(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_ثبتوونکی[frontend_name] = هذا;
}

Frontend::~Frontend()
{
}

فراغ Frontend::execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design)
{
	log_assert(بل_دلیلونه.تشه());
	افعل {
		std::istream *f = عدم;
		بل_دلیلونه.clear();
		خپلکاری state = pre_execute();
		execute(f, std::string(), دلیلونه_2, design);
		post_execute(state);
		دلیلونه_2 = بل_دلیلونه;
		ړنګول f;
	} طالما (!دلیلونه_2.تشه());
}

FILE *Frontend::current_script_file = عدم;
std::string Frontend::وروستی_دلته_document;
std::string Frontend::current_script_filename;
صحيح Frontend::current_script_lineno = 0;

فراغ Frontend::extra_دلیلونه(std::istream *&f, std::string &filename, std::ویکټور<std::string> دلیلونه_2, اندازه_t argidx, منطقي bin_ننوت)
{
	منطقي called_with_fp = f != عدم;

	بل_دلیلونه.clear();

	إذا (argidx < دلیلونه_2.اندازه())
	{
		std::string arg = دلیلونه_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(دلیلونه_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(دلیلونه_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		إذا (filename == "<<" && argidx+1 < دلیلونه_2.اندازه())
			filename += دلیلونه_2[++argidx];
		إذا (filename.compare(0, 2, "<<") == 0) {
			إذا (filename.اندازه() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == تش_اشاره) ?  "<stdin>" : "<inlined>";
			وروستی_دلته_document.clear();
			طالما (1) {
				std::string بفر;
				حرف بلاک[4096];
				طالما (1) {
					إذا (fgets(بلاک, 4096, Frontend::current_script_file == تش_اشاره? stdin : Frontend::current_script_file) == تش_اشاره)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					بفر += بلاک;
					إذا (بفر.اندازه() > 0 && (بفر[بفر.اندازه() - 1] == '\n' || بفر[بفر.اندازه() - 1] == '\r'))
						اكسر;
				}
				Frontend::current_script_lineno++;
				اندازه_t indent = بفر.پیدا_کړه_لومړی_not_د(" \t\r\n");
				إذا (indent != std::string::npos && بفر.compare(indent, eot_marker.اندازه(), eot_marker) == 0)
					اكسر;
				وروستی_دلته_document += بفر;
			}
			f = جديد std::istringstream(وروستی_دلته_document);
		} وإلا {
			rewrite_filename(filename);
			ویکټور<string> filenames = glob_filename(filename);
			filename = filenames.front();
			إذا (Get_اندازه(filenames) > 1) {
				بل_دلیلونه.ورننباسه(بل_دلیلونه.پای_ټکی(), دلیلونه_2.begin(), دلیلونه_2.begin()+argidx);
				بل_دلیلونه.ورننباسه(بل_دلیلونه.پای_ټکی(), filenames.begin()+1, filenames.پای_ټکی());
			}
			yosys_ننوت_files.ورننباسه(filename);
			f = uncompressed(filename, bin_ننوت ? std::ifstream::دوګونی : std::ifstream::دننه);
		}

		لكل (اندازه_t i = argidx+1; i < دلیلونه_2.اندازه(); i++)
			إذا (دلیلونه_2[i].compare(0, 1, "-") == 0)
				cmd_error(دلیلونه_2, i, "Found option, expected arguments.");

		إذا (argidx+1 < دلیلونه_2.اندازه()) {
			إذا (بل_دلیلونه.تشه())
				بل_دلیلونه.ورننباسه(بل_دلیلونه.پای_ټکی(), دلیلونه_2.begin(), دلیلونه_2.begin()+argidx);
			بل_دلیلونه.ورننباسه(بل_دلیلونه.پای_ټکی(), دلیلونه_2.begin()+argidx+1, دلیلونه_2.پای_ټکی());
			دلیلونه_2.erase(دلیلونه_2.begin()+argidx+1, دلیلونه_2.پای_ټکی());
		}
	}

	إذا (f == عدم)
		cmd_error(دلیلونه_2, argidx, "No filename given.");

	إذا (called_with_fp)
		دلیلونه_2.ټېل_شا(filename);
	دلیلونه_2[0] = pass_name;
	// cmd_log_args(args);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::ویکټور<std::string> دلیلونه_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		دلیلونه_2.ټېل_شا(p);
	حرر(s);
	frontend_call(design, f, filename, دلیلونه_2);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::ویکټور<std::string> دلیلونه_2)
{
	إذا (دلیلونه_2.اندازه() == 0)
		أرجع;
	إذا (frontend_ثبتوونکی.شمېر(دلیلونه_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", دلیلونه_2[0]);

	إذا (f != عدم) {
		خپلکاری state = frontend_ثبتوونکی[دلیلونه_2[0]]->pre_execute();
		frontend_ثبتوونکی[دلیلونه_2[0]]->execute(f, filename, دلیلونه_2, design);
		frontend_ثبتوونکی[دلیلونه_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::istream *f_cin = &std::ننوتنه;
		خپلکاری state = frontend_ثبتوونکی[دلیلونه_2[0]]->pre_execute();
		frontend_ثبتوونکی[دلیلونه_2[0]]->execute(f_cin, "<stdin>", دلیلونه_2, design);
		frontend_ثبتوونکی[دلیلونه_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.تشه())
			دلیلونه_2.ټېل_شا(filename);
		frontend_ثبتوونکی[دلیلونه_2[0]]->execute(دلیلونه_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, سرچینه_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Backend::پرلپسې_ثبتوونکی()
{
	إذا (pass_ثبتوونکی.شمېر(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ثبتوونکی[pass_name] = هذا;

	إذا (backend_ثبتوونکی.شمېر(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_ثبتوونکی[backend_name] = هذا;
}

Backend::~Backend()
{
}

فراغ Backend::execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design *design)
{
	std::ostream *f = عدم;
	خپلکاری state = pre_execute();
	execute(f, std::string(), دلیلونه_2, design);
	post_execute(state);
	إذا (f != &std::وتنه)
		ړنګول f;
}

فراغ Backend::extra_دلیلونه(std::ostream *&f, std::string &filename, std::ویکټور<std::string> دلیلونه_2, اندازه_t argidx, منطقي bin_output)
{
	منطقي called_with_fp = f != عدم;

	لكل (; argidx < دلیلونه_2.اندازه(); argidx++)
	{
		std::string arg = دلیلونه_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(دلیلونه_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(دلیلونه_2, argidx, "Extra filename argument in direct file mode.");

		إذا (arg == "-") {
			filename = "<stdout>";
			f = &std::وتنه;
			استمر;
		}

		filename = arg;
		rewrite_filename(filename);
		إذا (filename.اندازه() > 3 && filename.compare(filename.اندازه()-3, std::string::npos, ".gz") == 0) {
#که_تعریف_شوی YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = جديد gzip_ostream;
			إذا (!gf->open(filename)) {
				ړنګول gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ورننباسه(filename);
			f = gf;
#وإلا
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#د_که_پای
		} وإلا {
			std::ofstream *ff = جديد std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::دوګونی) : std::ofstream::trunc);
			yosys_output_files.ورننباسه(filename);
			إذا (ff->fail()) {
				ړنګول ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	إذا (called_with_fp)
		دلیلونه_2.ټېل_شا(filename);
	دلیلونه_2[0] = pass_name;
	// cmd_log_args(args);

	إذا (f == عدم) {
		filename = "<stdout>";
		f = &std::وتنه;
	}
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::ویکټور<std::string> دلیلونه_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		دلیلونه_2.ټېل_شا(p);
	حرر(s);
	backend_call(design, f, filename, دلیلونه_2);
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::ویکټور<std::string> دلیلونه_2)
{
	إذا (دلیلونه_2.اندازه() == 0)
		أرجع;
	إذا (backend_ثبتوونکی.شمېر(دلیلونه_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", دلیلونه_2[0]);

	اندازه_t orig_sel_ډېری_pos = design->selection_ډېری.اندازه();

	إذا (f != عدم) {
		خپلکاری state = backend_ثبتوونکی[دلیلونه_2[0]]->pre_execute();
		backend_ثبتوونکی[دلیلونه_2[0]]->execute(f, filename, دلیلونه_2, design);
		backend_ثبتوونکی[دلیلونه_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::ostream *f_cout = &std::وتنه;
		خپلکاری state = backend_ثبتوونکی[دلیلونه_2[0]]->pre_execute();
		backend_ثبتوونکی[دلیلونه_2[0]]->execute(f_cout, "<stdout>", دلیلونه_2, design);
		backend_ثبتوونکی[دلیلونه_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.تشه())
			دلیلونه_2.ټېل_شا(filename);
		backend_ثبتوونکی[دلیلونه_2[0]]->execute(دلیلونه_2, design);
	}

	طالما (design->selection_ډېری.اندازه() > orig_sel_ډېری_pos)
		design->راوباسه_selection();
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
	string سرچینه;
	string desc;
	string کوډ;
	string ډله;
	string ver;
	string نښانونه;
};

ساكن منطقي is_کوډ_getter(string name) {
	أرجع *(--(name.پای_ټکی())) == '+';
}

ساكن string get_cell_name(string name) {
	أرجع is_کوډ_getter(name) ? name.substr(0, name.اوږدتیا()-1) : name;
}

ساكن فراغ log_warning_flags(Pass *pass) {
	منطقي has_warnings = خطأ;
	ثابت string name = pass->pass_name;
	إذا (pass->experimental_flag) {
		إذا (!has_warnings) لوګاریتم("\n");
		has_warnings = ریښتیا;
		لوګاریتم("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	إذا (pass->دننی_flag) {
		إذا (!has_warnings) لوګاریتم("\n");
		has_warnings = ریښتیا;
		لوګاریتم("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	إذا (has_warnings)
		لوګاریتم("\n");
}

ساكن بنية CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ضمن "kernel/simlib_help.inc"
#ضمن "kernel/simcells_help.inc"
		cell_help.ترتیبول();
	}
	منطقي لري(string name) { أرجع cell_help.شمېر(get_cell_name(name)) > 0; }
	SimHelper get(string name) { أرجع cell_help[get_cell_name(name)]; }
} cell_help_messages;

بنية HelpPass : عام Pass {
	HelpPass() : Pass("help", "display help messages") { }
	فراغ help() override
	{
		لوګاریتم("\n");
		لوګاریتم("    help  ................  list all commands\n");
		لوګاریتم("    help <command>  ......  print help message for given command\n");
		لوګاریتم("    help -all  ...........  print complete command reference\n");
		لوګاریتم("\n");
		لوګاریتم("    help -cells ..........  list all cell types\n");
		لوګاریتم("    help <celltype>  .....  print help message for given cell type\n");
		لوګاریتم("    help <celltype>+  ....  print verilog code for given cell type\n");
		لوګاریتم("\n");
	}
	منطقي dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.ثبت("version", "Yosys command reference");
		json.ثبت("generator", yosys_نسخه_str);

		منطقي raise_error = خطأ;
		std::map<string, ویکټور<string>> groups;

		// get root path
		خپلکاری this_لار = std::filesystem::لار_2(سرچینه_location::current().file_name());
		خپلکاری سرچینه_ریښه = this_لار.پلار_لار().پلار_لار();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		لكل (خپلکاری &it : pass_ثبتوونکی) {
			خپلکاری name = it.لومړی;
			خپلکاری pass = it.دوهم;
			خپلکاری title = pass->short_help;

			خپلکاری cmd_help = PrettyHelp();
			خپلکاری has_pretty_help = pass->formatted_help();

			إذا (!has_pretty_help) {
				تعداد PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				سرچینه_location null_سرچینه;
				string current_بفر = "";
				خپلکاری ریښه_listing_2 = cmd_help.get_ریښه();
				خپلکاری current_listing = ریښه_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					خپلکاری log_scope = logger().sink_scope();
					logger().جمع_کړه_sink<بهیر_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				اندازه_t def_strip_شمېر = 0;
				خپلکاری current_state = PUState_none;
				خپلکاری catch_verific = خطأ;
				خپلکاری blank_کرښې = 2;
				لكل (string کرښه; std::getline(ss, کرښه, '\n');) {
					// find position of first non space character
					std::اندازه_t لومړی_pos = کرښه.پیدا_کړه_لومړی_not_د(" \t");
					std::اندازه_t وروستی_pos = کرښه.پیدا_کړه_وروستی_not_د(" \t");
					إذا (لومړی_pos == std::string::npos) {
						بدل (current_state)
						{
						حالة PUState_signature:
							ریښه_listing_2->usage(current_بفر, null_سرچینه);
							current_listing = ریښه_listing_2;
							current_state = PUState_none;
							current_بفر = "";
							اكسر;
						حالة PUState_none:
						حالة PUState_optionbody:
							blank_کرښې += 1;
							اكسر;
						افتراضي:
							اكسر;
						}
						// skip empty lines
						استمر;
					}

					// strip leading and trailing whitespace
					std::string stripped_کرښه = کرښه.substr(لومړی_pos, وروستی_pos - لومړی_pos +1);
					منطقي IsDefinition = stripped_کرښه[0] == '-';
					IsDefinition &= stripped_کرښه[1] != ' ' && stripped_کرښه[1] != '>';
					منطقي IsDedent = def_strip_شمېر && لومړی_pos < def_strip_شمېر;
					منطقي IsIndent = def_strip_شمېر < لومړی_pos;

					// line looks like a signature
					منطقي IsSignature = stripped_کرښه.پیدا_کړه(name) == 0 && (stripped_کرښه.اوږدتیا() == name.اوږدتیا() || stripped_کرښه.at(name.اندازه()) == ' ');

					إذا (IsSignature && لومړی_pos <= 4 && (blank_کرښې >= 2 || current_state == PUState_signature)) {
						إذا (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_بفر, "none", null_سرچینه);
							current_بفر = "";
						} وإلا إذا (current_state == PUState_signature) {
							ریښه_listing_2->usage(current_بفر, null_سرچینه);
							current_بفر = "";
						} وإلا إذا (current_state == PUState_none && !current_بفر.تشه()) {
							current_listing->codeblock(current_بفر, "none", null_سرچینه);
							current_بفر = "";
						}
						current_listing = ریښه_listing_2;
						current_state = PUState_signature;
						def_strip_شمېر = لومړی_pos;
						catch_verific = خطأ;
					} وإلا إذا (IsDedent) {
						def_strip_شمېر = لومړی_pos;
						إذا (current_state == PUState_optionbody) {
							إذا (!current_بفر.تشه()) {
								current_listing->codeblock(current_بفر, "none", null_سرچینه);
								current_بفر = "";
							}
							إذا (IsIndent) {
								current_state = PUState_options;
								current_listing = ریښه_listing_2->شا();
							} وإلا {
								current_state = PUState_none;
								current_listing = ریښه_listing_2;
							}
						} وإلا {
							current_state = PUState_none;
						}
					}

					إذا (IsDefinition && !catch_verific && current_state != PUState_signature) {
						إذا (!current_بفر.تشه()) {
							current_listing->codeblock(current_بفر, "none", null_سرچینه);
							current_بفر = "";
						}
						current_state = PUState_options;
						current_listing = ریښه_listing_2->open_option(stripped_کرښه, null_سرچینه);
						def_strip_شمېر = لومړی_pos;
					} وإلا {
						إذا (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						إذا (current_بفر.تشه())
							current_بفر = stripped_کرښه;
						وإلا إذا (current_state == PUState_signature && IsIndent)
							current_بفر += stripped_کرښه;
						وإلا إذا (current_state == PUState_none) {
							current_بفر += (blank_کرښې > 0 ? "\n\n" : "\n") + کرښه;
						} وإلا
							current_بفر += (blank_کرښې > 0 ? "\n\n" : "\n") + stripped_کرښه;
						إذا (stripped_کرښه.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = ریښتیا;
					}
					blank_کرښې = 0;
				}

				إذا (!current_بفر.تشه()) {
					إذا (current_بفر.اندازه() > 64 && current_بفر.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_بفر.substr(0, 62), null_سرچینه);
						current_listing->codeblock(current_بفر.substr(64), "yoscrypt", null_سرچینه);
					} وإلا
						current_listing->codeblock(current_بفر, "none", null_سرچینه);
					current_بفر = "";
				}
			}

			// fix path
			string سرچینه_file = pass->location.file_name();
			منطقي has_سرچینه = سرچینه_file.compare("unknown") != 0;
			std::filesystem::لار_2 سرچینه_لار;
			خپلکاری skip_سرچینه_ډله = خطأ;

			إذا (has_سرچینه) {
				سرچینه_لار = fixup_سرچینه_لار(pass->location.file_name(), سرچینه_ریښه, &skip_سرچینه_ډله);
				سرچینه_file = سرچینه_لار.string();
			}

			// attempt auto group
			إذا (!cmd_help.has_ډله()) {
				إذا (pass->دننی_flag)
					cmd_help.ډله = "internal";
				وإلا إذا (سرچینه_file.پیدا_کړه("backends/") == 0 || (!has_سرچینه && name.پیدا_کړه("read_") == 0))
					cmd_help.ډله = "backends";
				وإلا إذا (سرچینه_file.پیدا_کړه("frontends/") == 0 || (!has_سرچینه && name.پیدا_کړه("write_") == 0))
					cmd_help.ډله = "frontends";
				وإلا إذا (has_سرچینه) {
					إذا (سرچینه_لار.has_پلار_لار() && !skip_سرچینه_ډله)
						cmd_help.ډله = سرچینه_لار.پلار_لار().string();
				}
				// implicit !has_source
				وإلا إذا (name.پیدا_کړه("equiv") == 0)
					cmd_help.ډله = "passes/equiv";
				وإلا إذا (name.پیدا_کړه("fsm") == 0)
					cmd_help.ډله = "passes/fsm";
				وإلا إذا (name.پیدا_کړه("memory") == 0)
					cmd_help.ډله = "passes/memory";
				وإلا إذا (name.پیدا_کړه("opt") == 0)
					cmd_help.ډله = "passes/opt";
				وإلا إذا (name.پیدا_کړه("proc") == 0)
					cmd_help.ډله = "passes/proc";
			}

			إذا (groups.شمېر(cmd_help.ډله) == 0) {
				groups[cmd_help.ډله] = ویکټور<string>();
			}
			groups[cmd_help.ډله].ټېل_شا(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.ثبت("title", title);
			json.name("content"); json.begin_array();
			لكل (خپلکاری &content : cmd_help)
				json.ارزښت(content.منزل_json(سرچینه_ریښه));
			json.پای_ټکی_array();
			json.ثبت("group", cmd_help.ډله);
			json.ثبت("source_file", سرچینه_file);
			json.ثبت("source_line", pass->location.کرښه());
			json.ثبت("source_func", pass->location.function_name());
			json.ثبت("experimental_flag", pass->experimental_flag);
			json.ثبت("internal_flag", pass->دننی_flag);
			json.پای_ټکی_object();
		}
		json.پای_ټکی_object();

		json.ثبت("groups", groups);

		json.پای_ټکی_object();
		أرجع raise_error;
	}
	منطقي dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.ثبت("version", "Yosys internal cells");
		json.ثبت("generator", yosys_maybe_نسخه());

		dict<string, ویکټور<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>> cells;

		// iterate over cells
		منطقي raise_error = خطأ;
		لكل (خپلکاری it : StaticCellTypes::builder.cells) {
			إذا (!StaticCellTypes::categories.is_معلوم(it.type))
				استمر;
			خپلکاری name = ID::str(it.type);
			إذا (cell_help_messages.لري(name)) {
				خپلکاری cell_help = cell_help_messages.get(name);
				groups[cell_help.ډله].emplace_شا(name);
				خپلکاری cell_pair = pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} وإلا {
				لوګاریتم("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= ریښتیا;
			}
		}
		لكل (خپلکاری &it : cell_help_messages.cell_help) {
			إذا (cells.شمېر(it.لومړی) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.لومړی);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.ترتیبول();
		لكل (خپلکاری &it : groups) {
			json.name(it.لومړی.c_str()); json.ارزښت(it.دوهم);
		}
		json.پای_ټکی_object();

		json.name("cells"); json.begin_object();
		cells.ترتیبول();
		لكل (خپلکاری &it : cells) {
			خپلکاری ch = it.دوهم.لومړی;
			خپلکاری ct = it.دوهم.دوهم;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.ارزښت(ch.title);
			json.name("ports"); json.ارزښت(ch.ports);
			json.name("source"); json.ارزښت(ch.سرچینه);
			json.name("desc"); json.ارزښت(ch.desc);
			json.name("code"); json.ارزښت(ch.کوډ);
			ویکټور<string> inputs, outputs;
			لكل (خپلکاری &ننوت : ct.inputs)
				inputs.ټېل_شا(ID::str(ننوت));
			json.name("inputs"); json.ارزښت(inputs);
			لكل (خپلکاری &output : ct.outputs)
				outputs.ټېل_شا(ID::str(output));
			json.name("outputs"); json.ارزښت(outputs);
			ویکټور<string> properties;
			// CellType properties
			إذا (ct.features.is_evaluable) properties.ټېل_شا("is_evaluable");
			إذا (ct.features.is_combinatorial) properties.ټېل_شا("is_combinatorial");
			إذا (ct.features.is_synthesizable) properties.ټېل_شا("is_synthesizable");
			// SimHelper properties
			اندازه_t وروستی = 0; اندازه_t بل = 0;
			طالما ((بل = ch.نښانونه.پیدا_کړه(", ", وروستی)) != string::npos) {
				properties.ټېل_شا(ch.نښانونه.substr(وروستی, بل-وروستی));
				وروستی = بل + 2;
			}
			خپلکاری final_نښان = ch.نښانونه.substr(وروستی);
			إذا (final_نښان.اندازه()) properties.ټېل_شا(final_نښان);
			json.name("properties"); json.ارزښت(properties);
			json.پای_ټکی_object();
		}
		json.پای_ټکی_object();

		json.پای_ټکی_object();
		أرجع raise_error;
	}
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design*) override
	{
		إذا (دلیلونه_2.اندازه() == 1) {
			لوګاریتم("\n");
			لكل (خپلکاری &it : pass_ثبتوونکی)
				لوګاریتم("    %-20s %s\n", it.لومړی, it.دوهم->short_help);
			لوګاریتم("\n");
			لوګاریتم("Type 'help <command>' for more information on a command.\n");
			لوګاریتم("Type 'help -cells' for a list of all cell types.\n");
			لوګاریتم("\n");
			أرجع;
		}

		إذا (دلیلونه_2.اندازه() == 2) {
			إذا (دلیلونه_2[1] == "-all") {
				لكل (خپلکاری &it : pass_ثبتوونکی) {
					لوګاریتم("\n");
					لوګاریتم("\n");
					لوګاریتم("%s  --  %s\n", it.لومړی, it.دوهم->short_help);
					std::string پیغام;
					لكل (اندازه_t i = 0; i < it.لومړی.اندازه() + it.دوهم->short_help.اندازه() + 6; i++)
						پیغام += '=';
					لوګاریتم("%s\n", پیغام);
					it.دوهم->help();
					log_warning_flags(it.دوهم);
				}
			}
			وإلا إذا (دلیلونه_2[1] == "-cells") {
				لوګاریتم("\n");
				لكل (خپلکاری &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.دوهم;
					لوګاریتم("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				لوګاریتم("\n");
				لوګاریتم("Type 'help <cell_type>' for more information on a cell type.\n");
				لوګاریتم("\n");
				أرجع;
			}
			وإلا إذا (pass_ثبتوونکی.شمېر(دلیلونه_2[1])) {
				pass_ثبتوونکی.at(دلیلونه_2[1])->help();
				log_warning_flags(pass_ثبتوونکی.at(دلیلونه_2[1]));
			}
			وإلا إذا (cell_help_messages.لري(دلیلونه_2[1])) {
				خپلکاری help_cell = cell_help_messages.get(دلیلونه_2[1]);
				إذا (is_کوډ_getter(دلیلونه_2[1])) {
						لوګاریتم("\n");
						لوګاریتم("%s\n", help_cell.کوډ);
				} وإلا {
					لوګاریتم("\n");
					لوګاریتم("    %s %s\n", help_cell.name, help_cell.ports);
					لوګاریتم("\n");
					إذا (help_cell.ver == "2" || help_cell.ver == "2a") {
						إذا (help_cell.title != "") لوګاریتم("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						لكل (std::string کرښه; std::getline(ss, کرښه, '\n');) {
							إذا (کرښه != "::") لوګاریتم("%s\n", کرښه);
						}
					} وإلا إذا (help_cell.desc.اوږدتیا()) {
						لوګاریتم("%s\n", help_cell.desc);
					} وإلا {
						لوګاریتم("No help message for this cell type found.\n");
					}
					لوګاریتم("\n");
					لوګاریتم("Run 'help %s+' to display the Verilog model for this cell type.\n", دلیلونه_2[1]);
					لوګاریتم("\n");
				}
			}
			وإلا
				لوګاریتم("No such command or cell type: %s\n", دلیلونه_2[1]);
			أرجع;
		} وإلا إذا (دلیلونه_2.اندازه() == 3) {
			// this option is undocumented as it is for internal use only
			إذا (دلیلونه_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				إذا (!json.write_منزل_file(دلیلونه_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دلیلونه_2[2], strerror(errno));
				إذا (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			وإلا إذا (دلیلونه_2[1] == "-dump-cells-json") {
				PrettyJson json;
				إذا (!json.write_منزل_file(دلیلونه_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دلیلونه_2[2], strerror(errno));
				إذا (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			وإلا
				لوګاریتم("Unknown help command: `%s %s'\n", دلیلونه_2[1], دلیلونه_2[2]);
			أرجع;
		}

		help();
	}
} HelpPass;

بنية EchoPass : عام Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	فراغ help() override
	{
		لوګاریتم("\n");
		لوګاریتم("    echo on\n");
		لوګاریتم("\n");
		لوګاریتم("Print all commands to log before executing them.\n");
		لوګاریتم("\n");
		لوګاریتم("\n");
		لوګاریتم("    echo off\n");
		لوګاریتم("\n");
		لوګاریتم("Do not print all commands to log before executing them. (default)\n");
		لوګاریتم("\n");
	}
	فراغ execute(std::ویکټور<std::string> دلیلونه_2, RTLIL::Design*) override
	{
		إذا (دلیلونه_2.اندازه() > 2)
			cmd_error(دلیلونه_2, 2, "Unexpected argument.");

		إذا (دلیلونه_2.اندازه() == 2) {
			إذا (دلیلونه_2[1] == "on")
				echo_ډېرپېښ = ریښتیا;
			وإلا إذا (دلیلونه_2[1] == "off")
				echo_ډېرپېښ = خطأ;
			وإلا
				cmd_error(دلیلونه_2, 1, "Unexpected argument.");
		}

		لوګاریتم("echo %s\n", echo_ډېرپېښ ? "on" : "off");
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
		لوګاریتم("\n");
		لوګاریتم("    license\n");
		لوګاریتم("\n");
		لوګاریتم("This command produces the following notice.\n");
		notice();
	}
	فراغ execute(std::ویکټور<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	فراغ notice()
	{
		لوګاریتم("\n");
		لوګاریتم(" /----------------------------------------------------------------------------\\\n");
		لوګاریتم(" |                                                                            |\n");
		لوګاریتم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		لوګاریتم(" |                                                                            |\n");
		لوګاریتم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		لوګاریتم(" |                                                                            |\n");
		لوګاریتم(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		لوګاریتم(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		لوګاریتم(" |  copyright notice and this permission notice appear in all copies.         |\n");
		لوګاریتم(" |                                                                            |\n");
		لوګاریتم(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		لوګاریتم(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		لوګاریتم(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		لوګاریتم(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		لوګاریتم(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		لوګاریتم(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		لوګاریتم(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		لوګاریتم(" |                                                                            |\n");
		لوګاریتم(" \\----------------------------------------------------------------------------/\n");
		لوګاریتم("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_پای_ټکی

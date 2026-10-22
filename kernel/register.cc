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

#تعریف_کرو MAX_REG_گنتی 1000

منطقي echo_منوال = خطأ;
Pass *پہلا_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_رجسٹر;
std::map<std::string, Pass*> pass_رجسٹر;
std::map<std::string, Backend*> backend_رجسٹر;
std::map<std::string, نشانہ> نشانہ_رجسٹر;

std::سمتیہ<std::string> Frontend::اگلا_دلیلاں;

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

Pass::Pass(std::string name, std::string short_help, سروت_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	اگلا_queued_pass = پہلا_queued_pass;
	پہلا_queued_pass = هذا;
	call_counter = 0;
	runtime_ns = 0;
}

فراغ Pass::لگاتار_رجسٹر()
{
	إذا (pass_رجسٹر.گنتی(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٹر[pass_name] = هذا;
}

فراغ Pass::init_رجسٹر()
{
	سمتیہ<Pass*> added_passes;
	طالما (پہلا_queued_pass) {
		added_passes.ڈالو_پٹھ(پہلا_queued_pass);
		پہلا_queued_pass->لگاتار_رجسٹر();
		پہلا_queued_pass = پہلا_queued_pass->اگلا_queued_pass;
	}
	لكل (خودکار added_pass : added_passes)
		added_pass->on_رجسٹر();
}

فراغ Pass::پورا_رجسٹر()
{
	لكل (خودکار &it : pass_رجسٹر)
		it.دوجا->on_shutdown();

	frontend_رجسٹر.clear();
	pass_رجسٹر.clear();
	backend_رجسٹر.clear();
	log_assert(پہلا_queued_pass == عدم);
}

فراغ Pass::on_رجسٹر()
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
	state.ماپا_pass = current_pass;
	current_pass = هذا;
	clear_flags();
	أرجع state;
}

فراغ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t ویلا_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += ویلا_ns;
	current_pass = state.ماپا_pass;
	گھٹاؤ_from_current_runtime_ns(ویلا_ns);
}

فراغ Pass::گھٹاؤ_from_current_runtime_ns(int64_t ویلا_ns)
{
	إذا (current_pass)
		current_pass->runtime_ns -= ویلا_ns;
}

فراغ Pass::help()
{
	خودکار prettyHelp = PrettyHelp();
	إذا (formatted_help()) {
		prettyHelp.log_help();
	} وإلا {
		لوگارتھم("\n");
		لوگارتھم("No help message for command `%s'.\n", pass_name);
		لوگارتھم("\n");
	}
}

منطقي Pass::formatted_help()
{
	أرجع خطأ;
}

فراغ Pass::clear_flags()
{
}

فراغ Pass::cmd_log_دلیلاں(ثابت std::سمتیہ<std::string> &دلیلاں_2)
{
	إذا (دلیلاں_2.ناپ() <= 1)
		أرجع;
	std::string سنیہا = "Full command line:";
	لكل (ناپ_t i = 0; i < دلیلاں_2.ناپ(); i++)
		سنیہا += stringf(" %s", دلیلاں_2[i]);
	لوگارتھم("%s\n", سنیہا);
}

فراغ Pass::cmd_error(ثابت std::سمتیہ<std::string> &دلیلاں_2, ناپ_t argidx, std::string پیغام)
{
	std::string command_متن;
	صحيح error_pos = 0;

	لكل (ناپ_t i = 0; i < دلیلاں_2.ناپ(); i++) {
		إذا (i < argidx)
			error_pos += دلیلاں_2[i].ناپ() + 1;
		command_متن = command_متن + (command_متن.خالی_تھاواں() ? "" : " ") + دلیلاں_2[i];
	}

	لوگارتھم("\n");
	لوگارتھم("Syntax error in command `%s':\n", command_متن);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			پیغام.c_str(), command_متن.c_str(), error_pos, "");
}

فراغ Pass::extra_دلیلاں(std::سمتیہ<std::string> دلیلاں_2, ناپ_t argidx, RTLIL::Design *design, منطقي select)
{
	لكل (; argidx < دلیلاں_2.ناپ(); argidx++)
	{
		std::string arg = دلیلاں_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(دلیلاں_2, argidx, "Unknown option or option in arguments.");

		إذا (!select)
			cmd_error(دلیلاں_2, argidx, "Extra argument.");

		handle_extra_select_دلیلاں(هذا, دلیلاں_2, argidx, دلیلاں_2.ناپ(), design);
		اكسر;
	}
	// cmd_log_args(args);
}

فراغ Pass::call(RTLIL::Design *design, std::string command)
{
	std::سمتیہ<std::string> دلیلاں_2;

	std::string cmd_buf = command;
	std::string tok = اگلا_token(cmd_buf, " \t\r\n", سچ);

	إذا (tok.خالی_تھاواں())
		أرجع;

	إذا (tok[0] == '!') {
#إذا defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.لبھو('!') + 1);
		طالما (!cmd_buf.خالی_تھاواں() && (cmd_buf.پٹھ() == ' ' || cmd_buf.پٹھ() == '\t' ||
				cmd_buf.پٹھ() == '\r' || cmd_buf.پٹھ() == '\n'))
			cmd_buf.resize(cmd_buf.ناپ()-1);
		log_سرخی(design, "Shell command: %s\n", cmd_buf);
		صحيح ret_کوڈ = لگاتار_command(cmd_buf);
		إذا (ret_کوڈ != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_کوڈ);
		أرجع;
#وإلا
		log_cmd_error("Shell is not available.\n");
#جے_انت
	}

	طالما (!tok.خالی_تھاواں()) {
		إذا (tok[0] == '#') {
			صحيح stop;
			لكل (stop = 0; stop < Get_ناپ(cmd_buf); stop++)
				إذا (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					اكسر;
			cmd_buf = cmd_buf.substr(stop);
		} وإلا
		إذا (tok.پٹھ() == ';') {
			صحيح عدد_semikolon = 0;
			طالما (!tok.خالی_تھاواں() && tok.پٹھ() == ';')
				tok.resize(tok.ناپ()-1), عدد_semikolon++;
			إذا (!tok.خالی_تھاواں())
				دلیلاں_2.ڈالو_پٹھ(tok);
			call(design, دلیلاں_2);
			دلیلاں_2.clear();
			إذا (عدد_semikolon == 2)
				call(design, "clean");
			إذا (عدد_semikolon == 3)
				call(design, "clean -purge");
		} وإلا
			دلیلاں_2.ڈالو_پٹھ(tok);
		منطقي لبھیا_nl = خطأ;
		لكل (خودکار c : cmd_buf) {
			إذا (c == ' ' || c == '\t')
				استمر;
			إذا (c == '\r' || c == '\n')
				لبھیا_nl = سچ;
			اكسر;
		}
		إذا (لبھیا_nl) {
			call(design, دلیلاں_2);
			دلیلاں_2.clear();
		}
		tok = اگلا_token(cmd_buf, " \t\r\n", سچ);
	}

	call(design, دلیلاں_2);
}

فراغ Pass::call(RTLIL::Design *design, std::سمتیہ<std::string> دلیلاں_2)
{
	إذا (دلیلاں_2.ناپ() == 0 || دلیلاں_2[0][0] == '#' || دلیلاں_2[0][0] == ':')
		أرجع;

	إذا (echo_منوال) {
		std::string سنیہا = stringf("%s", create_prompt(design, 0, خطأ));
		لكل (ناپ_t i = 0; i < دلیلاں_2.ناپ(); i++)
			سنیہا += stringf("%s%s", i ? " " : "", دلیلاں_2[i]);
		لوگارتھم("\n");
		لوگارتھم("%s\n", سنیہا);
	}

	إذا (pass_رجسٹر.گنتی(دلیلاں_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", دلیلاں_2[0]);
	Pass *pass = pass_رجسٹر[دلیلاں_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	إذا (pass->experimental_flag)
		log_experimental(دلیلاں_2[0]);

	ناپ_t orig_sel_ڈھیر_pos = design->selection_ڈھیر.ناپ();
	خودکار state = pass->pre_execute();
	pass->execute(دلیلاں_2, design);
	pass->post_execute(state);
	طالما (design->selection_ڈھیر.ناپ() > orig_sel_ڈھیر_pos)
		design->نکالو_selection();
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module.clear();
	design->ڈالو_selection(selection);

	Pass::call(design, command);

	design->نکالو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::سمتیہ<std::string> دلیلاں_2)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module.clear();
	design->ڈالو_selection(selection);

	Pass::call(design, دلیلاں_2);

	design->نکالو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module = module->name;
	design->ڈالو_خالی_تھاواں_selection();
	design->select(module);

	Pass::call(design, command);

	design->نکالو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::سمتیہ<std::string> دلیلاں_2)
{
	IdString backup_selected_سرگرم_module = design->selected_سرگرم_module;
	design->selected_سرگرم_module = module->name;
	design->ڈالو_خالی_تھاواں_selection();
	design->select(module);

	Pass::call(design, دلیلاں_2);

	design->نکالو_selection();
	design->selected_سرگرم_module = backup_selected_سرگرم_module;
}

منطقي ScriptPass::check_label(std::string label, std::string info)
{
	إذا (سرگرم_design == خالی_سوچک) {
		لوگارتھم("\n");
		إذا (info.خالی_تھاواں())
			لوگارتھم("    %s:\n", label);
		وإلا
			لوگارتھم("    %s:    %s\n", label, info);
		أرجع سچ;
	} وإلا {
		إذا (!سرگرم_لگاتار_from.خالی_تھاواں() && سرگرم_لگاتار_from == سرگرم_لگاتار_منزل) {
			بلاک_سرگرم = (label == سرگرم_لگاتار_from);
		} وإلا {
			إذا (label == سرگرم_لگاتار_from)
				بلاک_سرگرم = سچ;
			إذا (label == سرگرم_لگاتار_منزل)
				بلاک_سرگرم = خطأ;
		}
		أرجع بلاک_سرگرم;
	}
}

فراغ ScriptPass::لگاتار(std::string command, std::string info)
{
	إذا (سرگرم_design == خالی_سوچک) {
		إذا (info.خالی_تھاواں())
			لوگارتھم("        %s\n", command);
		وإلا
			لوگارتھم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(سرگرم_design, command);
		سرگرم_design->check();
	}
}

فراغ ScriptPass::لگاتار_nocheck(std::string command, std::string info)
{
	إذا (سرگرم_design == خالی_سوچک) {
		إذا (info.خالی_تھاواں())
			لوگارتھم("        %s\n", command);
		وإلا
			لوگارتھم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(سرگرم_design, command);
	}
}

فراغ ScriptPass::لگاتار_script(RTLIL::Design *design, std::string لگاتار_from, std::string لگاتار_منزل)
{
	help_منوال = خطأ;
	سرگرم_design = design;
	بلاک_سرگرم = لگاتار_from.خالی_تھاواں();
	سرگرم_لگاتار_from = لگاتار_from;
	سرگرم_لگاتار_منزل = لگاتار_منزل;
	script();
}

فراغ ScriptPass::help_script()
{
	clear_flags();
	help_منوال = سچ;
	سرگرم_design = خالی_سوچک;
	بلاک_سرگرم = سچ;
	سرگرم_لگاتار_from.clear();
	سرگرم_لگاتار_منزل.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, سروت_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Frontend::لگاتار_رجسٹر()
{
	إذا (pass_رجسٹر.گنتی(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٹر[pass_name] = هذا;

	إذا (frontend_رجسٹر.گنتی(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_رجسٹر[frontend_name] = هذا;
}

Frontend::~Frontend()
{
}

فراغ Frontend::execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design)
{
	log_assert(اگلا_دلیلاں.خالی_تھاواں());
	افعل {
		std::istream *f = عدم;
		اگلا_دلیلاں.clear();
		خودکار state = pre_execute();
		execute(f, std::string(), دلیلاں_2, design);
		post_execute(state);
		دلیلاں_2 = اگلا_دلیلاں;
		مٹاؤ f;
	} طالما (!دلیلاں_2.خالی_تھاواں());
}

FILE *Frontend::current_script_file = عدم;
std::string Frontend::آخری_ایتھے_document;
std::string Frontend::current_script_filename;
صحيح Frontend::current_script_lineno = 0;

فراغ Frontend::extra_دلیلاں(std::istream *&f, std::string &filename, std::سمتیہ<std::string> دلیلاں_2, ناپ_t argidx, منطقي bin_داخلہ)
{
	منطقي called_with_fp = f != عدم;

	اگلا_دلیلاں.clear();

	إذا (argidx < دلیلاں_2.ناپ())
	{
		std::string arg = دلیلاں_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(دلیلاں_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(دلیلاں_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		إذا (filename == "<<" && argidx+1 < دلیلاں_2.ناپ())
			filename += دلیلاں_2[++argidx];
		إذا (filename.compare(0, 2, "<<") == 0) {
			إذا (filename.ناپ() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == خالی_سوچک) ?  "<stdin>" : "<inlined>";
			آخری_ایتھے_document.clear();
			طالما (1) {
				std::string بفر;
				حرف بلاک[4096];
				طالما (1) {
					إذا (fgets(بلاک, 4096, Frontend::current_script_file == خالی_سوچک? stdin : Frontend::current_script_file) == خالی_سوچک)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					بفر += بلاک;
					إذا (بفر.ناپ() > 0 && (بفر[بفر.ناپ() - 1] == '\n' || بفر[بفر.ناپ() - 1] == '\r'))
						اكسر;
				}
				Frontend::current_script_lineno++;
				ناپ_t indent = بفر.لبھو_پہلا_not_دا(" \t\r\n");
				إذا (indent != std::string::npos && بفر.compare(indent, eot_marker.ناپ(), eot_marker) == 0)
					اكسر;
				آخری_ایتھے_document += بفر;
			}
			f = جديد std::istringstream(آخری_ایتھے_document);
		} وإلا {
			rewrite_filename(filename);
			سمتیہ<string> filenames = glob_filename(filename);
			filename = filenames.front();
			إذا (Get_ناپ(filenames) > 1) {
				اگلا_دلیلاں.داخل_کرو(اگلا_دلیلاں.سرا(), دلیلاں_2.begin(), دلیلاں_2.begin()+argidx);
				اگلا_دلیلاں.داخل_کرو(اگلا_دلیلاں.سرا(), filenames.begin()+1, filenames.سرا());
			}
			yosys_داخلہ_files.داخل_کرو(filename);
			f = uncompressed(filename, bin_داخلہ ? std::ifstream::ثنائی : std::ifstream::وچ);
		}

		لكل (ناپ_t i = argidx+1; i < دلیلاں_2.ناپ(); i++)
			إذا (دلیلاں_2[i].compare(0, 1, "-") == 0)
				cmd_error(دلیلاں_2, i, "Found option, expected arguments.");

		إذا (argidx+1 < دلیلاں_2.ناپ()) {
			إذا (اگلا_دلیلاں.خالی_تھاواں())
				اگلا_دلیلاں.داخل_کرو(اگلا_دلیلاں.سرا(), دلیلاں_2.begin(), دلیلاں_2.begin()+argidx);
			اگلا_دلیلاں.داخل_کرو(اگلا_دلیلاں.سرا(), دلیلاں_2.begin()+argidx+1, دلیلاں_2.سرا());
			دلیلاں_2.erase(دلیلاں_2.begin()+argidx+1, دلیلاں_2.سرا());
		}
	}

	إذا (f == عدم)
		cmd_error(دلیلاں_2, argidx, "No filename given.");

	إذا (called_with_fp)
		دلیلاں_2.ڈالو_پٹھ(filename);
	دلیلاں_2[0] = pass_name;
	// cmd_log_args(args);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::سمتیہ<std::string> دلیلاں_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		دلیلاں_2.ڈالو_پٹھ(p);
	حرر(s);
	frontend_call(design, f, filename, دلیلاں_2);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::سمتیہ<std::string> دلیلاں_2)
{
	إذا (دلیلاں_2.ناپ() == 0)
		أرجع;
	إذا (frontend_رجسٹر.گنتی(دلیلاں_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", دلیلاں_2[0]);

	إذا (f != عدم) {
		خودکار state = frontend_رجسٹر[دلیلاں_2[0]]->pre_execute();
		frontend_رجسٹر[دلیلاں_2[0]]->execute(f, filename, دلیلاں_2, design);
		frontend_رجسٹر[دلیلاں_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::istream *f_cin = &std::اندر;
		خودکار state = frontend_رجسٹر[دلیلاں_2[0]]->pre_execute();
		frontend_رجسٹر[دلیلاں_2[0]]->execute(f_cin, "<stdin>", دلیلاں_2, design);
		frontend_رجسٹر[دلیلاں_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.خالی_تھاواں())
			دلیلاں_2.ڈالو_پٹھ(filename);
		frontend_رجسٹر[دلیلاں_2[0]]->execute(دلیلاں_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, سروت_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Backend::لگاتار_رجسٹر()
{
	إذا (pass_رجسٹر.گنتی(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٹر[pass_name] = هذا;

	إذا (backend_رجسٹر.گنتی(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_رجسٹر[backend_name] = هذا;
}

Backend::~Backend()
{
}

فراغ Backend::execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design *design)
{
	std::ostream *f = عدم;
	خودکار state = pre_execute();
	execute(f, std::string(), دلیلاں_2, design);
	post_execute(state);
	إذا (f != &std::باہر)
		مٹاؤ f;
}

فراغ Backend::extra_دلیلاں(std::ostream *&f, std::string &filename, std::سمتیہ<std::string> دلیلاں_2, ناپ_t argidx, منطقي bin_output)
{
	منطقي called_with_fp = f != عدم;

	لكل (; argidx < دلیلاں_2.ناپ(); argidx++)
	{
		std::string arg = دلیلاں_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(دلیلاں_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(دلیلاں_2, argidx, "Extra filename argument in direct file mode.");

		إذا (arg == "-") {
			filename = "<stdout>";
			f = &std::باہر;
			استمر;
		}

		filename = arg;
		rewrite_filename(filename);
		إذا (filename.ناپ() > 3 && filename.compare(filename.ناپ()-3, std::string::npos, ".gz") == 0) {
#جے_تعریف_شدہ YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = جديد gzip_ostream;
			إذا (!gf->open(filename)) {
				مٹاؤ gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.داخل_کرو(filename);
			f = gf;
#وإلا
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#جے_انت
		} وإلا {
			std::ofstream *ff = جديد std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ثنائی) : std::ofstream::trunc);
			yosys_output_files.داخل_کرو(filename);
			إذا (ff->fail()) {
				مٹاؤ ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	إذا (called_with_fp)
		دلیلاں_2.ڈالو_پٹھ(filename);
	دلیلاں_2[0] = pass_name;
	// cmd_log_args(args);

	إذا (f == عدم) {
		filename = "<stdout>";
		f = &std::باہر;
	}
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::سمتیہ<std::string> دلیلاں_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		دلیلاں_2.ڈالو_پٹھ(p);
	حرر(s);
	backend_call(design, f, filename, دلیلاں_2);
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::سمتیہ<std::string> دلیلاں_2)
{
	إذا (دلیلاں_2.ناپ() == 0)
		أرجع;
	إذا (backend_رجسٹر.گنتی(دلیلاں_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", دلیلاں_2[0]);

	ناپ_t orig_sel_ڈھیر_pos = design->selection_ڈھیر.ناپ();

	إذا (f != عدم) {
		خودکار state = backend_رجسٹر[دلیلاں_2[0]]->pre_execute();
		backend_رجسٹر[دلیلاں_2[0]]->execute(f, filename, دلیلاں_2, design);
		backend_رجسٹر[دلیلاں_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::ostream *f_cout = &std::باہر;
		خودکار state = backend_رجسٹر[دلیلاں_2[0]]->pre_execute();
		backend_رجسٹر[دلیلاں_2[0]]->execute(f_cout, "<stdout>", دلیلاں_2, design);
		backend_رجسٹر[دلیلاں_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.خالی_تھاواں())
			دلیلاں_2.ڈالو_پٹھ(filename);
		backend_رجسٹر[دلیلاں_2[0]]->execute(دلیلاں_2, design);
	}

	طالما (design->selection_ڈھیر.ناپ() > orig_sel_ڈھیر_pos)
		design->نکالو_selection();
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
	string سروت;
	string desc;
	string کوڈ;
	string گروہ;
	string ver;
	string ٹیگ_سوچی;
};

ساكن منطقي is_کوڈ_getter(string name) {
	أرجع *(--(name.سرا())) == '+';
}

ساكن string get_cell_name(string name) {
	أرجع is_کوڈ_getter(name) ? name.substr(0, name.لمائی()-1) : name;
}

ساكن فراغ log_warning_flags(Pass *pass) {
	منطقي has_warnings = خطأ;
	ثابت string name = pass->pass_name;
	إذا (pass->experimental_flag) {
		إذا (!has_warnings) لوگارتھم("\n");
		has_warnings = سچ;
		لوگارتھم("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	إذا (pass->اندرونی_flag) {
		إذا (!has_warnings) لوگارتھم("\n");
		has_warnings = سچ;
		لوگارتھم("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	إذا (has_warnings)
		لوگارتھم("\n");
}

ساكن بنية CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ضمن "kernel/simlib_help.inc"
#ضمن "kernel/simcells_help.inc"
		cell_help.ترتیب_دیو();
	}
	منطقي شامل_اے(string name) { أرجع cell_help.گنتی(get_cell_name(name)) > 0; }
	SimHelper get(string name) { أرجع cell_help[get_cell_name(name)]; }
} cell_help_messages;

بنية HelpPass : عام Pass {
	HelpPass() : Pass("help", "display help messages") { }
	فراغ help() override
	{
		لوگارتھم("\n");
		لوگارتھم("    help  ................  list all commands\n");
		لوگارتھم("    help <command>  ......  print help message for given command\n");
		لوگارتھم("    help -all  ...........  print complete command reference\n");
		لوگارتھم("\n");
		لوگارتھم("    help -cells ..........  list all cell types\n");
		لوگارتھم("    help <celltype>  .....  print help message for given cell type\n");
		لوگارتھم("    help <celltype>+  ....  print verilog code for given cell type\n");
		لوگارتھم("\n");
	}
	منطقي dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.اندراج("version", "Yosys command reference");
		json.اندراج("generator", yosys_نسخہ_str);

		منطقي raise_error = خطأ;
		std::map<string, سمتیہ<string>> groups;

		// get root path
		خودکار this_رستہ = std::filesystem::رستہ_2(سروت_location::current().file_name());
		خودکار سروت_جڑ = this_رستہ.ماپا_رستہ().ماپا_رستہ();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		لكل (خودکار &it : pass_رجسٹر) {
			خودکار name = it.پہلا;
			خودکار pass = it.دوجا;
			خودکار title = pass->short_help;

			خودکار cmd_help = PrettyHelp();
			خودکار has_pretty_help = pass->formatted_help();

			إذا (!has_pretty_help) {
				تعداد PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				سروت_location null_سروت;
				string current_بفر = "";
				خودکار جڑ_listing_2 = cmd_help.get_جڑ();
				خودکار current_listing = جڑ_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					خودکار log_scope = logger().sink_scope();
					logger().جوڑو_sink<دھارا_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				ناپ_t def_strip_گنتی = 0;
				خودکار current_state = PUState_none;
				خودکار catch_verific = خطأ;
				خودکار blank_سطریں = 2;
				لكل (string سطر; std::getline(ss, سطر, '\n');) {
					// find position of first non space character
					std::ناپ_t پہلا_pos = سطر.لبھو_پہلا_not_دا(" \t");
					std::ناپ_t آخری_pos = سطر.لبھو_آخری_not_دا(" \t");
					إذا (پہلا_pos == std::string::npos) {
						بدل (current_state)
						{
						حالة PUState_signature:
							جڑ_listing_2->usage(current_بفر, null_سروت);
							current_listing = جڑ_listing_2;
							current_state = PUState_none;
							current_بفر = "";
							اكسر;
						حالة PUState_none:
						حالة PUState_optionbody:
							blank_سطریں += 1;
							اكسر;
						افتراضي:
							اكسر;
						}
						// skip empty lines
						استمر;
					}

					// strip leading and trailing whitespace
					std::string stripped_سطر = سطر.substr(پہلا_pos, آخری_pos - پہلا_pos +1);
					منطقي IsDefinition = stripped_سطر[0] == '-';
					IsDefinition &= stripped_سطر[1] != ' ' && stripped_سطر[1] != '>';
					منطقي IsDedent = def_strip_گنتی && پہلا_pos < def_strip_گنتی;
					منطقي IsIndent = def_strip_گنتی < پہلا_pos;

					// line looks like a signature
					منطقي IsSignature = stripped_سطر.لبھو(name) == 0 && (stripped_سطر.لمائی() == name.لمائی() || stripped_سطر.at(name.ناپ()) == ' ');

					إذا (IsSignature && پہلا_pos <= 4 && (blank_سطریں >= 2 || current_state == PUState_signature)) {
						إذا (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_بفر, "none", null_سروت);
							current_بفر = "";
						} وإلا إذا (current_state == PUState_signature) {
							جڑ_listing_2->usage(current_بفر, null_سروت);
							current_بفر = "";
						} وإلا إذا (current_state == PUState_none && !current_بفر.خالی_تھاواں()) {
							current_listing->codeblock(current_بفر, "none", null_سروت);
							current_بفر = "";
						}
						current_listing = جڑ_listing_2;
						current_state = PUState_signature;
						def_strip_گنتی = پہلا_pos;
						catch_verific = خطأ;
					} وإلا إذا (IsDedent) {
						def_strip_گنتی = پہلا_pos;
						إذا (current_state == PUState_optionbody) {
							إذا (!current_بفر.خالی_تھاواں()) {
								current_listing->codeblock(current_بفر, "none", null_سروت);
								current_بفر = "";
							}
							إذا (IsIndent) {
								current_state = PUState_options;
								current_listing = جڑ_listing_2->پٹھ();
							} وإلا {
								current_state = PUState_none;
								current_listing = جڑ_listing_2;
							}
						} وإلا {
							current_state = PUState_none;
						}
					}

					إذا (IsDefinition && !catch_verific && current_state != PUState_signature) {
						إذا (!current_بفر.خالی_تھاواں()) {
							current_listing->codeblock(current_بفر, "none", null_سروت);
							current_بفر = "";
						}
						current_state = PUState_options;
						current_listing = جڑ_listing_2->open_option(stripped_سطر, null_سروت);
						def_strip_گنتی = پہلا_pos;
					} وإلا {
						إذا (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						إذا (current_بفر.خالی_تھاواں())
							current_بفر = stripped_سطر;
						وإلا إذا (current_state == PUState_signature && IsIndent)
							current_بفر += stripped_سطر;
						وإلا إذا (current_state == PUState_none) {
							current_بفر += (blank_سطریں > 0 ? "\n\n" : "\n") + سطر;
						} وإلا
							current_بفر += (blank_سطریں > 0 ? "\n\n" : "\n") + stripped_سطر;
						إذا (stripped_سطر.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = سچ;
					}
					blank_سطریں = 0;
				}

				إذا (!current_بفر.خالی_تھاواں()) {
					إذا (current_بفر.ناپ() > 64 && current_بفر.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_بفر.substr(0, 62), null_سروت);
						current_listing->codeblock(current_بفر.substr(64), "yoscrypt", null_سروت);
					} وإلا
						current_listing->codeblock(current_بفر, "none", null_سروت);
					current_بفر = "";
				}
			}

			// fix path
			string سروت_file = pass->location.file_name();
			منطقي has_سروت = سروت_file.compare("unknown") != 0;
			std::filesystem::رستہ_2 سروت_رستہ;
			خودکار skip_سروت_گروہ = خطأ;

			إذا (has_سروت) {
				سروت_رستہ = fixup_سروت_رستہ(pass->location.file_name(), سروت_جڑ, &skip_سروت_گروہ);
				سروت_file = سروت_رستہ.string();
			}

			// attempt auto group
			إذا (!cmd_help.has_گروہ()) {
				إذا (pass->اندرونی_flag)
					cmd_help.گروہ = "internal";
				وإلا إذا (سروت_file.لبھو("backends/") == 0 || (!has_سروت && name.لبھو("read_") == 0))
					cmd_help.گروہ = "backends";
				وإلا إذا (سروت_file.لبھو("frontends/") == 0 || (!has_سروت && name.لبھو("write_") == 0))
					cmd_help.گروہ = "frontends";
				وإلا إذا (has_سروت) {
					إذا (سروت_رستہ.has_ماپا_رستہ() && !skip_سروت_گروہ)
						cmd_help.گروہ = سروت_رستہ.ماپا_رستہ().string();
				}
				// implicit !has_source
				وإلا إذا (name.لبھو("equiv") == 0)
					cmd_help.گروہ = "passes/equiv";
				وإلا إذا (name.لبھو("fsm") == 0)
					cmd_help.گروہ = "passes/fsm";
				وإلا إذا (name.لبھو("memory") == 0)
					cmd_help.گروہ = "passes/memory";
				وإلا إذا (name.لبھو("opt") == 0)
					cmd_help.گروہ = "passes/opt";
				وإلا إذا (name.لبھو("proc") == 0)
					cmd_help.گروہ = "passes/proc";
			}

			إذا (groups.گنتی(cmd_help.گروہ) == 0) {
				groups[cmd_help.گروہ] = سمتیہ<string>();
			}
			groups[cmd_help.گروہ].ڈالو_پٹھ(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.اندراج("title", title);
			json.name("content"); json.begin_array();
			لكل (خودکار &content : cmd_help)
				json.قدر(content.منزل_json(سروت_جڑ));
			json.سرا_array();
			json.اندراج("group", cmd_help.گروہ);
			json.اندراج("source_file", سروت_file);
			json.اندراج("source_line", pass->location.سطر());
			json.اندراج("source_func", pass->location.function_name());
			json.اندراج("experimental_flag", pass->experimental_flag);
			json.اندراج("internal_flag", pass->اندرونی_flag);
			json.سرا_object();
		}
		json.سرا_object();

		json.اندراج("groups", groups);

		json.سرا_object();
		أرجع raise_error;
	}
	منطقي dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.اندراج("version", "Yosys internal cells");
		json.اندراج("generator", yosys_maybe_نسخہ());

		dict<string, سمتیہ<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>> cells;

		// iterate over cells
		منطقي raise_error = خطأ;
		لكل (خودکار it : StaticCellTypes::builder.cells) {
			إذا (!StaticCellTypes::categories.is_معلوم(it.type))
				استمر;
			خودکار name = ID::str(it.type);
			إذا (cell_help_messages.شامل_اے(name)) {
				خودکار cell_help = cell_help_messages.get(name);
				groups[cell_help.گروہ].emplace_پٹھ(name);
				خودکار cell_pair = pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} وإلا {
				لوگارتھم("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= سچ;
			}
		}
		لكل (خودکار &it : cell_help_messages.cell_help) {
			إذا (cells.گنتی(it.پہلا) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.پہلا);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.ترتیب_دیو();
		لكل (خودکار &it : groups) {
			json.name(it.پہلا.c_str()); json.قدر(it.دوجا);
		}
		json.سرا_object();

		json.name("cells"); json.begin_object();
		cells.ترتیب_دیو();
		لكل (خودکار &it : cells) {
			خودکار ch = it.دوجا.پہلا;
			خودکار ct = it.دوجا.دوجا;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.قدر(ch.title);
			json.name("ports"); json.قدر(ch.ports);
			json.name("source"); json.قدر(ch.سروت);
			json.name("desc"); json.قدر(ch.desc);
			json.name("code"); json.قدر(ch.کوڈ);
			سمتیہ<string> inputs, outputs;
			لكل (خودکار &داخلہ : ct.inputs)
				inputs.ڈالو_پٹھ(ID::str(داخلہ));
			json.name("inputs"); json.قدر(inputs);
			لكل (خودکار &output : ct.outputs)
				outputs.ڈالو_پٹھ(ID::str(output));
			json.name("outputs"); json.قدر(outputs);
			سمتیہ<string> properties;
			// CellType properties
			إذا (ct.features.is_evaluable) properties.ڈالو_پٹھ("is_evaluable");
			إذا (ct.features.is_combinatorial) properties.ڈالو_پٹھ("is_combinatorial");
			إذا (ct.features.is_synthesizable) properties.ڈالو_پٹھ("is_synthesizable");
			// SimHelper properties
			ناپ_t آخری = 0; ناپ_t اگلا = 0;
			طالما ((اگلا = ch.ٹیگ_سوچی.لبھو(", ", آخری)) != string::npos) {
				properties.ڈالو_پٹھ(ch.ٹیگ_سوچی.substr(آخری, اگلا-آخری));
				آخری = اگلا + 2;
			}
			خودکار final_ٹیگ = ch.ٹیگ_سوچی.substr(آخری);
			إذا (final_ٹیگ.ناپ()) properties.ڈالو_پٹھ(final_ٹیگ);
			json.name("properties"); json.قدر(properties);
			json.سرا_object();
		}
		json.سرا_object();

		json.سرا_object();
		أرجع raise_error;
	}
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design*) override
	{
		إذا (دلیلاں_2.ناپ() == 1) {
			لوگارتھم("\n");
			لكل (خودکار &it : pass_رجسٹر)
				لوگارتھم("    %-20s %s\n", it.پہلا, it.دوجا->short_help);
			لوگارتھم("\n");
			لوگارتھم("Type 'help <command>' for more information on a command.\n");
			لوگارتھم("Type 'help -cells' for a list of all cell types.\n");
			لوگارتھم("\n");
			أرجع;
		}

		إذا (دلیلاں_2.ناپ() == 2) {
			إذا (دلیلاں_2[1] == "-all") {
				لكل (خودکار &it : pass_رجسٹر) {
					لوگارتھم("\n");
					لوگارتھم("\n");
					لوگارتھم("%s  --  %s\n", it.پہلا, it.دوجا->short_help);
					std::string پیغام;
					لكل (ناپ_t i = 0; i < it.پہلا.ناپ() + it.دوجا->short_help.ناپ() + 6; i++)
						پیغام += '=';
					لوگارتھم("%s\n", پیغام);
					it.دوجا->help();
					log_warning_flags(it.دوجا);
				}
			}
			وإلا إذا (دلیلاں_2[1] == "-cells") {
				لوگارتھم("\n");
				لكل (خودکار &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.دوجا;
					لوگارتھم("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				لوگارتھم("\n");
				لوگارتھم("Type 'help <cell_type>' for more information on a cell type.\n");
				لوگارتھم("\n");
				أرجع;
			}
			وإلا إذا (pass_رجسٹر.گنتی(دلیلاں_2[1])) {
				pass_رجسٹر.at(دلیلاں_2[1])->help();
				log_warning_flags(pass_رجسٹر.at(دلیلاں_2[1]));
			}
			وإلا إذا (cell_help_messages.شامل_اے(دلیلاں_2[1])) {
				خودکار help_cell = cell_help_messages.get(دلیلاں_2[1]);
				إذا (is_کوڈ_getter(دلیلاں_2[1])) {
						لوگارتھم("\n");
						لوگارتھم("%s\n", help_cell.کوڈ);
				} وإلا {
					لوگارتھم("\n");
					لوگارتھم("    %s %s\n", help_cell.name, help_cell.ports);
					لوگارتھم("\n");
					إذا (help_cell.ver == "2" || help_cell.ver == "2a") {
						إذا (help_cell.title != "") لوگارتھم("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						لكل (std::string سطر; std::getline(ss, سطر, '\n');) {
							إذا (سطر != "::") لوگارتھم("%s\n", سطر);
						}
					} وإلا إذا (help_cell.desc.لمائی()) {
						لوگارتھم("%s\n", help_cell.desc);
					} وإلا {
						لوگارتھم("No help message for this cell type found.\n");
					}
					لوگارتھم("\n");
					لوگارتھم("Run 'help %s+' to display the Verilog model for this cell type.\n", دلیلاں_2[1]);
					لوگارتھم("\n");
				}
			}
			وإلا
				لوگارتھم("No such command or cell type: %s\n", دلیلاں_2[1]);
			أرجع;
		} وإلا إذا (دلیلاں_2.ناپ() == 3) {
			// this option is undocumented as it is for internal use only
			إذا (دلیلاں_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				إذا (!json.write_منزل_file(دلیلاں_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دلیلاں_2[2], strerror(errno));
				إذا (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			وإلا إذا (دلیلاں_2[1] == "-dump-cells-json") {
				PrettyJson json;
				إذا (!json.write_منزل_file(دلیلاں_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دلیلاں_2[2], strerror(errno));
				إذا (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			وإلا
				لوگارتھم("Unknown help command: `%s %s'\n", دلیلاں_2[1], دلیلاں_2[2]);
			أرجع;
		}

		help();
	}
} HelpPass;

بنية EchoPass : عام Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	فراغ help() override
	{
		لوگارتھم("\n");
		لوگارتھم("    echo on\n");
		لوگارتھم("\n");
		لوگارتھم("Print all commands to log before executing them.\n");
		لوگارتھم("\n");
		لوگارتھم("\n");
		لوگارتھم("    echo off\n");
		لوگارتھم("\n");
		لوگارتھم("Do not print all commands to log before executing them. (default)\n");
		لوگارتھم("\n");
	}
	فراغ execute(std::سمتیہ<std::string> دلیلاں_2, RTLIL::Design*) override
	{
		إذا (دلیلاں_2.ناپ() > 2)
			cmd_error(دلیلاں_2, 2, "Unexpected argument.");

		إذا (دلیلاں_2.ناپ() == 2) {
			إذا (دلیلاں_2[1] == "on")
				echo_منوال = سچ;
			وإلا إذا (دلیلاں_2[1] == "off")
				echo_منوال = خطأ;
			وإلا
				cmd_error(دلیلاں_2, 1, "Unexpected argument.");
		}

		لوگارتھم("echo %s\n", echo_منوال ? "on" : "off");
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
		لوگارتھم("\n");
		لوگارتھم("    license\n");
		لوگارتھم("\n");
		لوگارتھم("This command produces the following notice.\n");
		notice();
	}
	فراغ execute(std::سمتیہ<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	فراغ notice()
	{
		لوگارتھم("\n");
		لوگارتھم(" /----------------------------------------------------------------------------\\\n");
		لوگارتھم(" |                                                                            |\n");
		لوگارتھم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		لوگارتھم(" |                                                                            |\n");
		لوگارتھم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		لوگارتھم(" |                                                                            |\n");
		لوگارتھم(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		لوگارتھم(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		لوگارتھم(" |  copyright notice and this permission notice appear in all copies.         |\n");
		لوگارتھم(" |                                                                            |\n");
		لوگارتھم(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		لوگارتھم(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		لوگارتھم(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		لوگارتھم(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		لوگارتھم(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		لوگارتھم(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		لوگارتھم(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		لوگارتھم(" |                                                                            |\n");
		لوگارتھم(" \\----------------------------------------------------------------------------/\n");
		لوگارتھم("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_سرا

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

#شامل "kernel/yosys.h"
#شامل "kernel/satgen.h"
#شامل "kernel/json.h"
#شامل "kernel/gzip.h"
#شامل "kernel/log_help.h"
#شامل "kernel/newcelltypes.h"

#شامل <string.h>
#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <errno.h>
#شامل <filesystem>

YOSYS_NAMESPACE_BEGIN

#تعریف_کرو MAX_REG_گنتی 1000

منطقی echo_منوال = جھوٹ;
Pass *پہلا_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_رجسٹر;
std::map<std::string, Pass*> pass_رجسٹر;
std::map<std::string, Backend*> backend_رجسٹر;
std::map<std::string, ہدف> ہدف_رجسٹر;

std::سمتیہ<std::string> Frontend::اگلا_دلائل;

منطقی GarbageCollectionGuard::is_enabled_ = سچ;

ساکن منطقی garbage_collection_requested = جھوٹ;

خالی request_garbage_collection()
{
	garbage_collection_requested = سچ;
}

خالی try_collect_garbage()
{
	اگر (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		واپس;
	garbage_collection_requested = جھوٹ;
	اگر (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, ماخذ_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	اگلا_queued_pass = پہلا_queued_pass;
	پہلا_queued_pass = یہ;
	call_شمار = 0;
	runtime_ns = 0;
}

خالی Pass::تسلسل_رجسٹر()
{
	اگر (pass_رجسٹر.گنتی(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٹر[pass_name] = یہ;
}

خالی Pass::init_رجسٹر()
{
	سمتیہ<Pass*> added_passes;
	جب (پہلا_queued_pass) {
		added_passes.ڈالو_عقب(پہلا_queued_pass);
		پہلا_queued_pass->تسلسل_رجسٹر();
		پہلا_queued_pass = پہلا_queued_pass->اگلا_queued_pass;
	}
	برائے (خودکار added_pass : added_passes)
		added_pass->on_رجسٹر();
}

خالی Pass::مکمل_رجسٹر()
{
	برائے (خودکار &it : pass_رجسٹر)
		it.دوسرا->on_shutdown();

	frontend_رجسٹر.clear();
	pass_رجسٹر.clear();
	backend_رجسٹر.clear();
	log_assert(پہلا_queued_pass == تہی);
}

خالی Pass::on_رجسٹر()
{
}

خالی Pass::on_shutdown()
{
}

Pass::~Pass()
{
}

Pass::pre_post_exec_state_t Pass::pre_execute()
{
	pre_post_exec_state_t state;
	call_شمار++;
	state.begin_ns = PerformanceTimer::query();
	state.والد_pass = current_pass;
	current_pass = یہ;
	clear_flags();
	واپس state;
}

خالی Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t وقت_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += وقت_ns;
	current_pass = state.والد_pass;
	تفریق_from_current_runtime_ns(وقت_ns);
}

خالی Pass::تفریق_from_current_runtime_ns(int64_t وقت_ns)
{
	اگر (current_pass)
		current_pass->runtime_ns -= وقت_ns;
}

خالی Pass::help()
{
	خودکار prettyHelp = PrettyHelp();
	اگر (formatted_help()) {
		prettyHelp.log_help();
	} ورنہ {
		لوگارتھم("\n");
		لوگارتھم("No help message for command `%s'.\n", pass_name);
		لوگارتھم("\n");
	}
}

منطقی Pass::formatted_help()
{
	واپس جھوٹ;
}

خالی Pass::clear_flags()
{
}

خالی Pass::cmd_log_دلائل(ثابت std::سمتیہ<std::string> &دلائل_2)
{
	اگر (دلائل_2.سائز() <= 1)
		واپس;
	std::string پیغام_2 = "Full command line:";
	برائے (سائز_t i = 0; i < دلائل_2.سائز(); i++)
		پیغام_2 += stringf(" %s", دلائل_2[i]);
	لوگارتھم("%s\n", پیغام_2);
}

خالی Pass::cmd_error(ثابت std::سمتیہ<std::string> &دلائل_2, سائز_t argidx, std::string پیغام)
{
	std::string command_متن;
	صحیح error_pos = 0;

	برائے (سائز_t i = 0; i < دلائل_2.سائز(); i++) {
		اگر (i < argidx)
			error_pos += دلائل_2[i].سائز() + 1;
		command_متن = command_متن + (command_متن.خالی_خانے() ? "" : " ") + دلائل_2[i];
	}

	لوگارتھم("\n");
	لوگارتھم("Syntax error in command `%s':\n", command_متن);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			پیغام.c_str(), command_متن.c_str(), error_pos, "");
}

خالی Pass::extra_دلائل(std::سمتیہ<std::string> دلائل_2, سائز_t argidx, RTLIL::Design *design, منطقی select)
{
	برائے (; argidx < دلائل_2.سائز(); argidx++)
	{
		std::string arg = دلائل_2[argidx];

		اگر (arg.compare(0, 1, "-") == 0)
			cmd_error(دلائل_2, argidx, "Unknown option or option in arguments.");

		اگر (!select)
			cmd_error(دلائل_2, argidx, "Extra argument.");

		handle_extra_select_دلائل(یہ, دلائل_2, argidx, دلائل_2.سائز(), design);
		توڑو;
	}
	// cmd_log_args(args);
}

خالی Pass::call(RTLIL::Design *design, std::string command)
{
	std::سمتیہ<std::string> دلائل_2;

	std::string cmd_buf = command;
	std::string tok = اگلا_token(cmd_buf, " \t\r\n", سچ);

	اگر (tok.خالی_خانے())
		واپس;

	اگر (tok[0] == '!') {
#اگر defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.ڈھونڈو('!') + 1);
		جب (!cmd_buf.خالی_خانے() && (cmd_buf.عقب() == ' ' || cmd_buf.عقب() == '\t' ||
				cmd_buf.عقب() == '\r' || cmd_buf.عقب() == '\n'))
			cmd_buf.resize(cmd_buf.سائز()-1);
		log_سرخی(design, "Shell command: %s\n", cmd_buf);
		صحیح ret_رمز = تسلسل_command(cmd_buf);
		اگر (ret_رمز != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_رمز);
		واپس;
#ورنہ
		log_cmd_error("Shell is not available.\n");
#اگر_ختم
	}

	جب (!tok.خالی_خانے()) {
		اگر (tok[0] == '#') {
			صحیح stop;
			برائے (stop = 0; stop < Get_سائز(cmd_buf); stop++)
				اگر (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					توڑو;
			cmd_buf = cmd_buf.substr(stop);
		} ورنہ
		اگر (tok.عقب() == ';') {
			صحیح عدد_semikolon = 0;
			جب (!tok.خالی_خانے() && tok.عقب() == ';')
				tok.resize(tok.سائز()-1), عدد_semikolon++;
			اگر (!tok.خالی_خانے())
				دلائل_2.ڈالو_عقب(tok);
			call(design, دلائل_2);
			دلائل_2.clear();
			اگر (عدد_semikolon == 2)
				call(design, "clean");
			اگر (عدد_semikolon == 3)
				call(design, "clean -purge");
		} ورنہ
			دلائل_2.ڈالو_عقب(tok);
		منطقی ملا_nl = جھوٹ;
		برائے (خودکار c : cmd_buf) {
			اگر (c == ' ' || c == '\t')
				جاری;
			اگر (c == '\r' || c == '\n')
				ملا_nl = سچ;
			توڑو;
		}
		اگر (ملا_nl) {
			call(design, دلائل_2);
			دلائل_2.clear();
		}
		tok = اگلا_token(cmd_buf, " \t\r\n", سچ);
	}

	call(design, دلائل_2);
}

خالی Pass::call(RTLIL::Design *design, std::سمتیہ<std::string> دلائل_2)
{
	اگر (دلائل_2.سائز() == 0 || دلائل_2[0][0] == '#' || دلائل_2[0][0] == ':')
		واپس;

	اگر (echo_منوال) {
		std::string پیغام_2 = stringf("%s", create_prompt(design, 0, جھوٹ));
		برائے (سائز_t i = 0; i < دلائل_2.سائز(); i++)
			پیغام_2 += stringf("%s%s", i ? " " : "", دلائل_2[i]);
		لوگارتھم("\n");
		لوگارتھم("%s\n", پیغام_2);
	}

	اگر (pass_رجسٹر.گنتی(دلائل_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", دلائل_2[0]);
	Pass *pass = pass_رجسٹر[دلائل_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	اگر (pass->experimental_flag)
		log_experimental(دلائل_2[0]);

	سائز_t orig_sel_ڈھیر_pos = design->selection_ڈھیر.سائز();
	خودکار state = pass->pre_execute();
	pass->execute(دلائل_2, design);
	pass->post_execute(state);
	جب (design->selection_ڈھیر.سائز() > orig_sel_ڈھیر_pos)
		design->نکالو_selection();
}

خالی Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module.clear();
	design->ڈالو_selection(selection);

	Pass::call(design, command);

	design->نکالو_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

خالی Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::سمتیہ<std::string> دلائل_2)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module.clear();
	design->ڈالو_selection(selection);

	Pass::call(design, دلائل_2);

	design->نکالو_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

خالی Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module = module->name;
	design->ڈالو_خالی_خانے_selection();
	design->select(module);

	Pass::call(design, command);

	design->نکالو_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

خالی Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::سمتیہ<std::string> دلائل_2)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module = module->name;
	design->ڈالو_خالی_خانے_selection();
	design->select(module);

	Pass::call(design, دلائل_2);

	design->نکالو_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

منطقی ScriptPass::check_label(std::string label, std::string info)
{
	اگر (فعال_design == خالی_اشاریہ) {
		لوگارتھم("\n");
		اگر (info.خالی_خانے())
			لوگارتھم("    %s:\n", label);
		ورنہ
			لوگارتھم("    %s:    %s\n", label, info);
		واپس سچ;
	} ورنہ {
		اگر (!فعال_تسلسل_from.خالی_خانے() && فعال_تسلسل_from == فعال_تسلسل_مقصد) {
			بلاک_فعال = (label == فعال_تسلسل_from);
		} ورنہ {
			اگر (label == فعال_تسلسل_from)
				بلاک_فعال = سچ;
			اگر (label == فعال_تسلسل_مقصد)
				بلاک_فعال = جھوٹ;
		}
		واپس بلاک_فعال;
	}
}

خالی ScriptPass::تسلسل(std::string command, std::string info)
{
	اگر (فعال_design == خالی_اشاریہ) {
		اگر (info.خالی_خانے())
			لوگارتھم("        %s\n", command);
		ورنہ
			لوگارتھم("        %s    %s\n", command, info);
	} ورنہ {
		Pass::call(فعال_design, command);
		فعال_design->check();
	}
}

خالی ScriptPass::تسلسل_nocheck(std::string command, std::string info)
{
	اگر (فعال_design == خالی_اشاریہ) {
		اگر (info.خالی_خانے())
			لوگارتھم("        %s\n", command);
		ورنہ
			لوگارتھم("        %s    %s\n", command, info);
	} ورنہ {
		Pass::call(فعال_design, command);
	}
}

خالی ScriptPass::تسلسل_script(RTLIL::Design *design, std::string تسلسل_from, std::string تسلسل_مقصد)
{
	help_منوال = جھوٹ;
	فعال_design = design;
	بلاک_فعال = تسلسل_from.خالی_خانے();
	فعال_تسلسل_from = تسلسل_from;
	فعال_تسلسل_مقصد = تسلسل_مقصد;
	script();
}

خالی ScriptPass::help_script()
{
	clear_flags();
	help_منوال = سچ;
	فعال_design = خالی_اشاریہ;
	بلاک_فعال = سچ;
	فعال_تسلسل_from.clear();
	فعال_تسلسل_مقصد.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, ماخذ_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

خالی Frontend::تسلسل_رجسٹر()
{
	اگر (pass_رجسٹر.گنتی(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٹر[pass_name] = یہ;

	اگر (frontend_رجسٹر.گنتی(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_رجسٹر[frontend_name] = یہ;
}

Frontend::~Frontend()
{
}

خالی Frontend::execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design)
{
	log_assert(اگلا_دلائل.خالی_خانے());
	کرو {
		std::istream *f = تہی;
		اگلا_دلائل.clear();
		خودکار state = pre_execute();
		execute(f, std::string(), دلائل_2, design);
		post_execute(state);
		دلائل_2 = اگلا_دلائل;
		حذف f;
	} جب (!دلائل_2.خالی_خانے());
}

FILE *Frontend::current_script_file = تہی;
std::string Frontend::آخری_یہاں_document;
std::string Frontend::current_script_filename;
صحیح Frontend::current_script_lineno = 0;

خالی Frontend::extra_دلائل(std::istream *&f, std::string &filename, std::سمتیہ<std::string> دلائل_2, سائز_t argidx, منطقی bin_داخلہ)
{
	منطقی called_with_fp = f != تہی;

	اگلا_دلائل.clear();

	اگر (argidx < دلائل_2.سائز())
	{
		std::string arg = دلائل_2[argidx];

		اگر (arg.compare(0, 1, "-") == 0)
			cmd_error(دلائل_2, argidx, "Unknown option or option in arguments.");
		اگر (f != تہی)
			cmd_error(دلائل_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		اگر (filename == "<<" && argidx+1 < دلائل_2.سائز())
			filename += دلائل_2[++argidx];
		اگر (filename.compare(0, 2, "<<") == 0) {
			اگر (filename.سائز() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == خالی_اشاریہ) ?  "<stdin>" : "<inlined>";
			آخری_یہاں_document.clear();
			جب (1) {
				std::string بفر;
				حرف بلاک[4096];
				جب (1) {
					اگر (fgets(بلاک, 4096, Frontend::current_script_file == خالی_اشاریہ? stdin : Frontend::current_script_file) == خالی_اشاریہ)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					بفر += بلاک;
					اگر (بفر.سائز() > 0 && (بفر[بفر.سائز() - 1] == '\n' || بفر[بفر.سائز() - 1] == '\r'))
						توڑو;
				}
				Frontend::current_script_lineno++;
				سائز_t indent = بفر.ڈھونڈو_پہلا_not_کا(" \t\r\n");
				اگر (indent != std::string::npos && بفر.compare(indent, eot_marker.سائز(), eot_marker) == 0)
					توڑو;
				آخری_یہاں_document += بفر;
			}
			f = نیا std::istringstream(آخری_یہاں_document);
		} ورنہ {
			rewrite_filename(filename);
			سمتیہ<string> filenames = glob_filename(filename);
			filename = filenames.front();
			اگر (Get_سائز(filenames) > 1) {
				اگلا_دلائل.داخل_کرو(اگلا_دلائل.انتہا(), دلائل_2.begin(), دلائل_2.begin()+argidx);
				اگلا_دلائل.داخل_کرو(اگلا_دلائل.انتہا(), filenames.begin()+1, filenames.انتہا());
			}
			yosys_داخلہ_files.داخل_کرو(filename);
			f = uncompressed(filename, bin_داخلہ ? std::ifstream::ثنائی : std::ifstream::اندر);
		}

		برائے (سائز_t i = argidx+1; i < دلائل_2.سائز(); i++)
			اگر (دلائل_2[i].compare(0, 1, "-") == 0)
				cmd_error(دلائل_2, i, "Found option, expected arguments.");

		اگر (argidx+1 < دلائل_2.سائز()) {
			اگر (اگلا_دلائل.خالی_خانے())
				اگلا_دلائل.داخل_کرو(اگلا_دلائل.انتہا(), دلائل_2.begin(), دلائل_2.begin()+argidx);
			اگلا_دلائل.داخل_کرو(اگلا_دلائل.انتہا(), دلائل_2.begin()+argidx+1, دلائل_2.انتہا());
			دلائل_2.erase(دلائل_2.begin()+argidx+1, دلائل_2.انتہا());
		}
	}

	اگر (f == تہی)
		cmd_error(دلائل_2, argidx, "No filename given.");

	اگر (called_with_fp)
		دلائل_2.ڈالو_عقب(filename);
	دلائل_2[0] = pass_name;
	// cmd_log_args(args);
}

خالی Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::سمتیہ<std::string> دلائل_2;
	حرف *s = strdup(command.c_str());
	برائے (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(تہی, " \t\r\n"))
		دلائل_2.ڈالو_عقب(p);
	آزاد(s);
	frontend_call(design, f, filename, دلائل_2);
}

خالی Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::سمتیہ<std::string> دلائل_2)
{
	اگر (دلائل_2.سائز() == 0)
		واپس;
	اگر (frontend_رجسٹر.گنتی(دلائل_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", دلائل_2[0]);

	اگر (f != تہی) {
		خودکار state = frontend_رجسٹر[دلائل_2[0]]->pre_execute();
		frontend_رجسٹر[دلائل_2[0]]->execute(f, filename, دلائل_2, design);
		frontend_رجسٹر[دلائل_2[0]]->post_execute(state);
	} ورنہ اگر (filename == "-") {
		std::istream *f_cin = &std::داخل;
		خودکار state = frontend_رجسٹر[دلائل_2[0]]->pre_execute();
		frontend_رجسٹر[دلائل_2[0]]->execute(f_cin, "<stdin>", دلائل_2, design);
		frontend_رجسٹر[دلائل_2[0]]->post_execute(state);
	} ورنہ {
		اگر (!filename.خالی_خانے())
			دلائل_2.ڈالو_عقب(filename);
		frontend_رجسٹر[دلائل_2[0]]->execute(دلائل_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, ماخذ_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

خالی Backend::تسلسل_رجسٹر()
{
	اگر (pass_رجسٹر.گنتی(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_رجسٹر[pass_name] = یہ;

	اگر (backend_رجسٹر.گنتی(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_رجسٹر[backend_name] = یہ;
}

Backend::~Backend()
{
}

خالی Backend::execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design *design)
{
	std::ostream *f = تہی;
	خودکار state = pre_execute();
	execute(f, std::string(), دلائل_2, design);
	post_execute(state);
	اگر (f != &std::خارج)
		حذف f;
}

خالی Backend::extra_دلائل(std::ostream *&f, std::string &filename, std::سمتیہ<std::string> دلائل_2, سائز_t argidx, منطقی bin_output)
{
	منطقی called_with_fp = f != تہی;

	برائے (; argidx < دلائل_2.سائز(); argidx++)
	{
		std::string arg = دلائل_2[argidx];

		اگر (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(دلائل_2, argidx, "Unknown option or option in arguments.");
		اگر (f != تہی)
			cmd_error(دلائل_2, argidx, "Extra filename argument in direct file mode.");

		اگر (arg == "-") {
			filename = "<stdout>";
			f = &std::خارج;
			جاری;
		}

		filename = arg;
		rewrite_filename(filename);
		اگر (filename.سائز() > 3 && filename.compare(filename.سائز()-3, std::string::npos, ".gz") == 0) {
#اگر_تعریف_شدہ YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = نیا gzip_ostream;
			اگر (!gf->open(filename)) {
				حذف gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.داخل_کرو(filename);
			f = gf;
#ورنہ
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#اگر_ختم
		} ورنہ {
			std::ofstream *ff = نیا std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ثنائی) : std::ofstream::trunc);
			yosys_output_files.داخل_کرو(filename);
			اگر (ff->fail()) {
				حذف ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	اگر (called_with_fp)
		دلائل_2.ڈالو_عقب(filename);
	دلائل_2[0] = pass_name;
	// cmd_log_args(args);

	اگر (f == تہی) {
		filename = "<stdout>";
		f = &std::خارج;
	}
}

خالی Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::سمتیہ<std::string> دلائل_2;
	حرف *s = strdup(command.c_str());
	برائے (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(تہی, " \t\r\n"))
		دلائل_2.ڈالو_عقب(p);
	آزاد(s);
	backend_call(design, f, filename, دلائل_2);
}

خالی Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::سمتیہ<std::string> دلائل_2)
{
	اگر (دلائل_2.سائز() == 0)
		واپس;
	اگر (backend_رجسٹر.گنتی(دلائل_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", دلائل_2[0]);

	سائز_t orig_sel_ڈھیر_pos = design->selection_ڈھیر.سائز();

	اگر (f != تہی) {
		خودکار state = backend_رجسٹر[دلائل_2[0]]->pre_execute();
		backend_رجسٹر[دلائل_2[0]]->execute(f, filename, دلائل_2, design);
		backend_رجسٹر[دلائل_2[0]]->post_execute(state);
	} ورنہ اگر (filename == "-") {
		std::ostream *f_cout = &std::خارج;
		خودکار state = backend_رجسٹر[دلائل_2[0]]->pre_execute();
		backend_رجسٹر[دلائل_2[0]]->execute(f_cout, "<stdout>", دلائل_2, design);
		backend_رجسٹر[دلائل_2[0]]->post_execute(state);
	} ورنہ {
		اگر (!filename.خالی_خانے())
			دلائل_2.ڈالو_عقب(filename);
		backend_رجسٹر[دلائل_2[0]]->execute(دلائل_2, design);
	}

	جب (design->selection_ڈھیر.سائز() > orig_sel_ڈھیر_pos)
		design->نکالو_selection();
}

ساخت SimHelper {
	string name;
	اندرونی string filesafe_name() {
		اگر (name.at(0) == '$')
			اگر (name.at(1) == '_')
				واپس "gate" + name.substr(1);
			ورنہ
				واپس "word_" + name.substr(1);
		ورنہ
			واپس name;
	}
	string title;
	string ports;
	string ماخذ;
	string desc;
	string رمز;
	string گروہ;
	string ver;
	string لیبل_فہرست;
};

ساکن منطقی is_رمز_getter(string name) {
	واپس *(--(name.انتہا())) == '+';
}

ساکن string get_cell_name(string name) {
	واپس is_رمز_getter(name) ? name.substr(0, name.لمبائی_2()-1) : name;
}

ساکن خالی log_warning_flags(Pass *pass) {
	منطقی has_warnings = جھوٹ;
	ثابت string name = pass->pass_name;
	اگر (pass->experimental_flag) {
		اگر (!has_warnings) لوگارتھم("\n");
		has_warnings = سچ;
		لوگارتھم("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	اگر (pass->اندرونی_flag) {
		اگر (!has_warnings) لوگارتھم("\n");
		has_warnings = سچ;
		لوگارتھم("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	اگر (has_warnings)
		لوگارتھم("\n");
}

ساکن ساخت CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#شامل "kernel/simlib_help.inc"
#شامل "kernel/simcells_help.inc"
		cell_help.ترتیب_دو();
	}
	منطقی شامل_ہے(string name) { واپس cell_help.گنتی(get_cell_name(name)) > 0; }
	SimHelper get(string name) { واپس cell_help[get_cell_name(name)]; }
} cell_help_messages;

ساخت HelpPass : عوامی Pass {
	HelpPass() : Pass("help", "display help messages") { }
	خالی help() override
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
	منطقی dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.اندراج_2("version", "Yosys command reference");
		json.اندراج_2("generator", yosys_نسخہ_str);

		منطقی raise_error = جھوٹ;
		std::map<string, سمتیہ<string>> groups;

		// get root path
		خودکار this_راستہ = std::filesystem::راستہ_2(ماخذ_location::current().file_name());
		خودکار ماخذ_جڑ = this_راستہ.والد_راستہ().والد_راستہ();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		برائے (خودکار &it : pass_رجسٹر) {
			خودکار name = it.پہلا;
			خودکار pass = it.دوسرا;
			خودکار title = pass->short_help;

			خودکار cmd_help = PrettyHelp();
			خودکار has_pretty_help = pass->formatted_help();

			اگر (!has_pretty_help) {
				شمار PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				ماخذ_location null_ماخذ;
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
				سائز_t def_strip_گنتی = 0;
				خودکار current_state = PUState_none;
				خودکار catch_verific = جھوٹ;
				خودکار blank_سطریں = 2;
				برائے (string سطر; std::getline(ss, سطر, '\n');) {
					// find position of first non space character
					std::سائز_t پہلا_pos = سطر.ڈھونڈو_پہلا_not_کا(" \t");
					std::سائز_t آخری_pos = سطر.ڈھونڈو_آخری_not_کا(" \t");
					اگر (پہلا_pos == std::string::npos) {
						انتخاب (current_state)
						{
						حالت PUState_signature:
							جڑ_listing_2->usage(current_بفر, null_ماخذ);
							current_listing = جڑ_listing_2;
							current_state = PUState_none;
							current_بفر = "";
							توڑو;
						حالت PUState_none:
						حالت PUState_optionbody:
							blank_سطریں += 1;
							توڑو;
						طےشدہ:
							توڑو;
						}
						// skip empty lines
						جاری;
					}

					// strip leading and trailing whitespace
					std::string stripped_سطر = سطر.substr(پہلا_pos, آخری_pos - پہلا_pos +1);
					منطقی IsDefinition = stripped_سطر[0] == '-';
					IsDefinition &= stripped_سطر[1] != ' ' && stripped_سطر[1] != '>';
					منطقی IsDedent = def_strip_گنتی && پہلا_pos < def_strip_گنتی;
					منطقی IsIndent = def_strip_گنتی < پہلا_pos;

					// line looks like a signature
					منطقی IsSignature = stripped_سطر.ڈھونڈو(name) == 0 && (stripped_سطر.لمبائی_2() == name.لمبائی_2() || stripped_سطر.at(name.سائز()) == ' ');

					اگر (IsSignature && پہلا_pos <= 4 && (blank_سطریں >= 2 || current_state == PUState_signature)) {
						اگر (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_بفر, "none", null_ماخذ);
							current_بفر = "";
						} ورنہ اگر (current_state == PUState_signature) {
							جڑ_listing_2->usage(current_بفر, null_ماخذ);
							current_بفر = "";
						} ورنہ اگر (current_state == PUState_none && !current_بفر.خالی_خانے()) {
							current_listing->codeblock(current_بفر, "none", null_ماخذ);
							current_بفر = "";
						}
						current_listing = جڑ_listing_2;
						current_state = PUState_signature;
						def_strip_گنتی = پہلا_pos;
						catch_verific = جھوٹ;
					} ورنہ اگر (IsDedent) {
						def_strip_گنتی = پہلا_pos;
						اگر (current_state == PUState_optionbody) {
							اگر (!current_بفر.خالی_خانے()) {
								current_listing->codeblock(current_بفر, "none", null_ماخذ);
								current_بفر = "";
							}
							اگر (IsIndent) {
								current_state = PUState_options;
								current_listing = جڑ_listing_2->عقب();
							} ورنہ {
								current_state = PUState_none;
								current_listing = جڑ_listing_2;
							}
						} ورنہ {
							current_state = PUState_none;
						}
					}

					اگر (IsDefinition && !catch_verific && current_state != PUState_signature) {
						اگر (!current_بفر.خالی_خانے()) {
							current_listing->codeblock(current_بفر, "none", null_ماخذ);
							current_بفر = "";
						}
						current_state = PUState_options;
						current_listing = جڑ_listing_2->open_option(stripped_سطر, null_ماخذ);
						def_strip_گنتی = پہلا_pos;
					} ورنہ {
						اگر (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						اگر (current_بفر.خالی_خانے())
							current_بفر = stripped_سطر;
						ورنہ اگر (current_state == PUState_signature && IsIndent)
							current_بفر += stripped_سطر;
						ورنہ اگر (current_state == PUState_none) {
							current_بفر += (blank_سطریں > 0 ? "\n\n" : "\n") + سطر;
						} ورنہ
							current_بفر += (blank_سطریں > 0 ? "\n\n" : "\n") + stripped_سطر;
						اگر (stripped_سطر.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = سچ;
					}
					blank_سطریں = 0;
				}

				اگر (!current_بفر.خالی_خانے()) {
					اگر (current_بفر.سائز() > 64 && current_بفر.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_بفر.substr(0, 62), null_ماخذ);
						current_listing->codeblock(current_بفر.substr(64), "yoscrypt", null_ماخذ);
					} ورنہ
						current_listing->codeblock(current_بفر, "none", null_ماخذ);
					current_بفر = "";
				}
			}

			// fix path
			string ماخذ_file = pass->location.file_name();
			منطقی has_ماخذ = ماخذ_file.compare("unknown") != 0;
			std::filesystem::راستہ_2 ماخذ_راستہ;
			خودکار skip_ماخذ_گروہ = جھوٹ;

			اگر (has_ماخذ) {
				ماخذ_راستہ = fixup_ماخذ_راستہ(pass->location.file_name(), ماخذ_جڑ, &skip_ماخذ_گروہ);
				ماخذ_file = ماخذ_راستہ.string();
			}

			// attempt auto group
			اگر (!cmd_help.has_گروہ()) {
				اگر (pass->اندرونی_flag)
					cmd_help.گروہ = "internal";
				ورنہ اگر (ماخذ_file.ڈھونڈو("backends/") == 0 || (!has_ماخذ && name.ڈھونڈو("read_") == 0))
					cmd_help.گروہ = "backends";
				ورنہ اگر (ماخذ_file.ڈھونڈو("frontends/") == 0 || (!has_ماخذ && name.ڈھونڈو("write_") == 0))
					cmd_help.گروہ = "frontends";
				ورنہ اگر (has_ماخذ) {
					اگر (ماخذ_راستہ.has_والد_راستہ() && !skip_ماخذ_گروہ)
						cmd_help.گروہ = ماخذ_راستہ.والد_راستہ().string();
				}
				// implicit !has_source
				ورنہ اگر (name.ڈھونڈو("equiv") == 0)
					cmd_help.گروہ = "passes/equiv";
				ورنہ اگر (name.ڈھونڈو("fsm") == 0)
					cmd_help.گروہ = "passes/fsm";
				ورنہ اگر (name.ڈھونڈو("memory") == 0)
					cmd_help.گروہ = "passes/memory";
				ورنہ اگر (name.ڈھونڈو("opt") == 0)
					cmd_help.گروہ = "passes/opt";
				ورنہ اگر (name.ڈھونڈو("proc") == 0)
					cmd_help.گروہ = "passes/proc";
			}

			اگر (groups.گنتی(cmd_help.گروہ) == 0) {
				groups[cmd_help.گروہ] = سمتیہ<string>();
			}
			groups[cmd_help.گروہ].ڈالو_عقب(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.اندراج_2("title", title);
			json.name("content"); json.begin_array();
			برائے (خودکار &content : cmd_help)
				json.قدر(content.مقصد_json(ماخذ_جڑ));
			json.انتہا_array();
			json.اندراج_2("group", cmd_help.گروہ);
			json.اندراج_2("source_file", ماخذ_file);
			json.اندراج_2("source_line", pass->location.سطر());
			json.اندراج_2("source_func", pass->location.function_name());
			json.اندراج_2("experimental_flag", pass->experimental_flag);
			json.اندراج_2("internal_flag", pass->اندرونی_flag);
			json.انتہا_object();
		}
		json.انتہا_object();

		json.اندراج_2("groups", groups);

		json.انتہا_object();
		واپس raise_error;
	}
	منطقی dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.اندراج_2("version", "Yosys internal cells");
		json.اندراج_2("generator", yosys_maybe_نسخہ());

		dict<string, سمتیہ<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>> cells;

		// iterate over cells
		منطقی raise_error = جھوٹ;
		برائے (خودکار it : StaticCellTypes::builder.cells) {
			اگر (!StaticCellTypes::categories.is_معلوم(it.type))
				جاری;
			خودکار name = ID::str(it.type);
			اگر (cell_help_messages.شامل_ہے(name)) {
				خودکار cell_help = cell_help_messages.get(name);
				groups[cell_help.گروہ].emplace_عقب(name);
				خودکار cell_pair = pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} ورنہ {
				لوگارتھم("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= سچ;
			}
		}
		برائے (خودکار &it : cell_help_messages.cell_help) {
			اگر (cells.گنتی(it.پہلا) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.پہلا);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.ترتیب_دو();
		برائے (خودکار &it : groups) {
			json.name(it.پہلا.c_str()); json.قدر(it.دوسرا);
		}
		json.انتہا_object();

		json.name("cells"); json.begin_object();
		cells.ترتیب_دو();
		برائے (خودکار &it : cells) {
			خودکار ch = it.دوسرا.پہلا;
			خودکار ct = it.دوسرا.دوسرا;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.قدر(ch.title);
			json.name("ports"); json.قدر(ch.ports);
			json.name("source"); json.قدر(ch.ماخذ);
			json.name("desc"); json.قدر(ch.desc);
			json.name("code"); json.قدر(ch.رمز);
			سمتیہ<string> inputs, outputs;
			برائے (خودکار &داخلہ : ct.inputs)
				inputs.ڈالو_عقب(ID::str(داخلہ));
			json.name("inputs"); json.قدر(inputs);
			برائے (خودکار &output : ct.outputs)
				outputs.ڈالو_عقب(ID::str(output));
			json.name("outputs"); json.قدر(outputs);
			سمتیہ<string> properties;
			// CellType properties
			اگر (ct.features.is_evaluable) properties.ڈالو_عقب("is_evaluable");
			اگر (ct.features.is_combinatorial) properties.ڈالو_عقب("is_combinatorial");
			اگر (ct.features.is_synthesizable) properties.ڈالو_عقب("is_synthesizable");
			// SimHelper properties
			سائز_t آخری = 0; سائز_t اگلا = 0;
			جب ((اگلا = ch.لیبل_فہرست.ڈھونڈو(", ", آخری)) != string::npos) {
				properties.ڈالو_عقب(ch.لیبل_فہرست.substr(آخری, اگلا-آخری));
				آخری = اگلا + 2;
			}
			خودکار final_لیبل = ch.لیبل_فہرست.substr(آخری);
			اگر (final_لیبل.سائز()) properties.ڈالو_عقب(final_لیبل);
			json.name("properties"); json.قدر(properties);
			json.انتہا_object();
		}
		json.انتہا_object();

		json.انتہا_object();
		واپس raise_error;
	}
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design*) override
	{
		اگر (دلائل_2.سائز() == 1) {
			لوگارتھم("\n");
			برائے (خودکار &it : pass_رجسٹر)
				لوگارتھم("    %-20s %s\n", it.پہلا, it.دوسرا->short_help);
			لوگارتھم("\n");
			لوگارتھم("Type 'help <command>' for more information on a command.\n");
			لوگارتھم("Type 'help -cells' for a list of all cell types.\n");
			لوگارتھم("\n");
			واپس;
		}

		اگر (دلائل_2.سائز() == 2) {
			اگر (دلائل_2[1] == "-all") {
				برائے (خودکار &it : pass_رجسٹر) {
					لوگارتھم("\n");
					لوگارتھم("\n");
					لوگارتھم("%s  --  %s\n", it.پہلا, it.دوسرا->short_help);
					std::string پیغام;
					برائے (سائز_t i = 0; i < it.پہلا.سائز() + it.دوسرا->short_help.سائز() + 6; i++)
						پیغام += '=';
					لوگارتھم("%s\n", پیغام);
					it.دوسرا->help();
					log_warning_flags(it.دوسرا);
				}
			}
			ورنہ اگر (دلائل_2[1] == "-cells") {
				لوگارتھم("\n");
				برائے (خودکار &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.دوسرا;
					لوگارتھم("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				لوگارتھم("\n");
				لوگارتھم("Type 'help <cell_type>' for more information on a cell type.\n");
				لوگارتھم("\n");
				واپس;
			}
			ورنہ اگر (pass_رجسٹر.گنتی(دلائل_2[1])) {
				pass_رجسٹر.at(دلائل_2[1])->help();
				log_warning_flags(pass_رجسٹر.at(دلائل_2[1]));
			}
			ورنہ اگر (cell_help_messages.شامل_ہے(دلائل_2[1])) {
				خودکار help_cell = cell_help_messages.get(دلائل_2[1]);
				اگر (is_رمز_getter(دلائل_2[1])) {
						لوگارتھم("\n");
						لوگارتھم("%s\n", help_cell.رمز);
				} ورنہ {
					لوگارتھم("\n");
					لوگارتھم("    %s %s\n", help_cell.name, help_cell.ports);
					لوگارتھم("\n");
					اگر (help_cell.ver == "2" || help_cell.ver == "2a") {
						اگر (help_cell.title != "") لوگارتھم("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						برائے (std::string سطر; std::getline(ss, سطر, '\n');) {
							اگر (سطر != "::") لوگارتھم("%s\n", سطر);
						}
					} ورنہ اگر (help_cell.desc.لمبائی_2()) {
						لوگارتھم("%s\n", help_cell.desc);
					} ورنہ {
						لوگارتھم("No help message for this cell type found.\n");
					}
					لوگارتھم("\n");
					لوگارتھم("Run 'help %s+' to display the Verilog model for this cell type.\n", دلائل_2[1]);
					لوگارتھم("\n");
				}
			}
			ورنہ
				لوگارتھم("No such command or cell type: %s\n", دلائل_2[1]);
			واپس;
		} ورنہ اگر (دلائل_2.سائز() == 3) {
			// this option is undocumented as it is for internal use only
			اگر (دلائل_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				اگر (!json.write_مقصد_file(دلائل_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دلائل_2[2], strerror(errno));
				اگر (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			ورنہ اگر (دلائل_2[1] == "-dump-cells-json") {
				PrettyJson json;
				اگر (!json.write_مقصد_file(دلائل_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", دلائل_2[2], strerror(errno));
				اگر (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			ورنہ
				لوگارتھم("Unknown help command: `%s %s'\n", دلائل_2[1], دلائل_2[2]);
			واپس;
		}

		help();
	}
} HelpPass;

ساخت EchoPass : عوامی Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	خالی help() override
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
	خالی execute(std::سمتیہ<std::string> دلائل_2, RTLIL::Design*) override
	{
		اگر (دلائل_2.سائز() > 2)
			cmd_error(دلائل_2, 2, "Unexpected argument.");

		اگر (دلائل_2.سائز() == 2) {
			اگر (دلائل_2[1] == "on")
				echo_منوال = سچ;
			ورنہ اگر (دلائل_2[1] == "off")
				echo_منوال = جھوٹ;
			ورنہ
				cmd_error(دلائل_2, 1, "Unexpected argument.");
		}

		لوگارتھم("echo %s\n", echo_منوال ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ساخت MinisatSatSolver : عوامی SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = یہ;
	}
	ezSAT *create() override {
		واپس نیا ezMiniSAT();
	}
} MinisatSatSolver;

ساخت LicensePass : عوامی Pass {
	LicensePass() : Pass("license", "print license terms") { }
	خالی help() override
	{
		لوگارتھم("\n");
		لوگارتھم("    license\n");
		لوگارتھم("\n");
		لوگارتھم("This command produces the following notice.\n");
		notice();
	}
	خالی execute(std::سمتیہ<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	خالی notice()
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

YOSYS_NAMESPACE_انتہا

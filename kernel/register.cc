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

#تعریف_کن MAX_REG_شمار 1000

منطقي echo_نما = خطأ;
Pass *اولی_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_ثبات;
std::map<std::string, Pass*> pass_ثبات;
std::map<std::string, Backend*> backend_ثبات;
std::map<std::string, هدف> هدف_ثبات;

std::وکتور<std::string> Frontend::بعدی_آرگومانها;

منطقي GarbageCollectionGuard::is_enabled_ = درست;

ساكن منطقي garbage_collection_requested = خطأ;

فراغ request_garbage_collection()
{
	garbage_collection_requested = درست;
}

فراغ try_collect_garbage()
{
	إذا (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		أرجع;
	garbage_collection_requested = خطأ;
	إذا (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, منبع_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	بعدی_queued_pass = اولی_queued_pass;
	اولی_queued_pass = هذا;
	call_counter = 0;
	runtime_ns = 0;
}

فراغ Pass::پیاپی_ثبات()
{
	إذا (pass_ثبات.شمار(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ثبات[pass_name] = هذا;
}

فراغ Pass::init_ثبات()
{
	وکتور<Pass*> added_passes;
	طالما (اولی_queued_pass) {
		added_passes.افزودن_پشت(اولی_queued_pass);
		اولی_queued_pass->پیاپی_ثبات();
		اولی_queued_pass = اولی_queued_pass->بعدی_queued_pass;
	}
	لكل (خودکار added_pass : added_passes)
		added_pass->on_ثبات();
}

فراغ Pass::انجامشده_ثبات()
{
	لكل (خودکار &it : pass_ثبات)
		it.دومی->on_shutdown();

	frontend_ثبات.clear();
	pass_ثبات.clear();
	backend_ثبات.clear();
	log_assert(اولی_queued_pass == عدم);
}

فراغ Pass::on_ثبات()
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

	int64_t زمان_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += زمان_ns;
	current_pass = state.والد_pass;
	تفریق_from_current_runtime_ns(زمان_ns);
}

فراغ Pass::تفریق_from_current_runtime_ns(int64_t زمان_ns)
{
	إذا (current_pass)
		current_pass->runtime_ns -= زمان_ns;
}

فراغ Pass::help()
{
	خودکار prettyHelp = PrettyHelp();
	إذا (formatted_help()) {
		prettyHelp.log_help();
	} وإلا {
		لگاریتم("\n");
		لگاریتم("No help message for command `%s'.\n", pass_name);
		لگاریتم("\n");
	}
}

منطقي Pass::formatted_help()
{
	أرجع خطأ;
}

فراغ Pass::clear_flags()
{
}

فراغ Pass::cmd_log_آرگومانها(ثابت std::وکتور<std::string> &آرگومانها_2)
{
	إذا (آرگومانها_2.اندازه() <= 1)
		أرجع;
	std::string پیام_2 = "Full command line:";
	لكل (اندازه_t i = 0; i < آرگومانها_2.اندازه(); i++)
		پیام_2 += stringf(" %s", آرگومانها_2[i]);
	لگاریتم("%s\n", پیام_2);
}

فراغ Pass::cmd_error(ثابت std::وکتور<std::string> &آرگومانها_2, اندازه_t argidx, std::string پیام)
{
	std::string command_متن;
	صحيح error_pos = 0;

	لكل (اندازه_t i = 0; i < آرگومانها_2.اندازه(); i++) {
		إذا (i < argidx)
			error_pos += آرگومانها_2[i].اندازه() + 1;
		command_متن = command_متن + (command_متن.خالی() ? "" : " ") + آرگومانها_2[i];
	}

	لگاریتم("\n");
	لگاریتم("Syntax error in command `%s':\n", command_متن);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			پیام.c_str(), command_متن.c_str(), error_pos, "");
}

فراغ Pass::extra_آرگومانها(std::وکتور<std::string> آرگومانها_2, اندازه_t argidx, RTLIL::Design *design, منطقي select)
{
	لكل (; argidx < آرگومانها_2.اندازه(); argidx++)
	{
		std::string arg = آرگومانها_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(آرگومانها_2, argidx, "Unknown option or option in arguments.");

		إذا (!select)
			cmd_error(آرگومانها_2, argidx, "Extra argument.");

		handle_extra_select_آرگومانها(هذا, آرگومانها_2, argidx, آرگومانها_2.اندازه(), design);
		اكسر;
	}
	// cmd_log_args(args);
}

فراغ Pass::call(RTLIL::Design *design, std::string command)
{
	std::وکتور<std::string> آرگومانها_2;

	std::string cmd_buf = command;
	std::string tok = بعدی_token(cmd_buf, " \t\r\n", درست);

	إذا (tok.خالی())
		أرجع;

	إذا (tok[0] == '!') {
#إذا defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.پیدا_کن('!') + 1);
		طالما (!cmd_buf.خالی() && (cmd_buf.پشت() == ' ' || cmd_buf.پشت() == '\t' ||
				cmd_buf.پشت() == '\r' || cmd_buf.پشت() == '\n'))
			cmd_buf.resize(cmd_buf.اندازه()-1);
		log_سرآیند(design, "Shell command: %s\n", cmd_buf);
		صحيح ret_کد = پیاپی_command(cmd_buf);
		إذا (ret_کد != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_کد);
		أرجع;
#وإلا
		log_cmd_error("Shell is not available.\n");
#پایان_اگر
	}

	طالما (!tok.خالی()) {
		إذا (tok[0] == '#') {
			صحيح stop;
			لكل (stop = 0; stop < Get_اندازه(cmd_buf); stop++)
				إذا (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					اكسر;
			cmd_buf = cmd_buf.substr(stop);
		} وإلا
		إذا (tok.پشت() == ';') {
			صحيح رقم_semikolon = 0;
			طالما (!tok.خالی() && tok.پشت() == ';')
				tok.resize(tok.اندازه()-1), رقم_semikolon++;
			إذا (!tok.خالی())
				آرگومانها_2.افزودن_پشت(tok);
			call(design, آرگومانها_2);
			آرگومانها_2.clear();
			إذا (رقم_semikolon == 2)
				call(design, "clean");
			إذا (رقم_semikolon == 3)
				call(design, "clean -purge");
		} وإلا
			آرگومانها_2.افزودن_پشت(tok);
		منطقي یافت_nl = خطأ;
		لكل (خودکار c : cmd_buf) {
			إذا (c == ' ' || c == '\t')
				استمر;
			إذا (c == '\r' || c == '\n')
				یافت_nl = درست;
			اكسر;
		}
		إذا (یافت_nl) {
			call(design, آرگومانها_2);
			آرگومانها_2.clear();
		}
		tok = بعدی_token(cmd_buf, " \t\r\n", درست);
	}

	call(design, آرگومانها_2);
}

فراغ Pass::call(RTLIL::Design *design, std::وکتور<std::string> آرگومانها_2)
{
	إذا (آرگومانها_2.اندازه() == 0 || آرگومانها_2[0][0] == '#' || آرگومانها_2[0][0] == ':')
		أرجع;

	إذا (echo_نما) {
		std::string پیام_2 = stringf("%s", create_prompt(design, 0, خطأ));
		لكل (اندازه_t i = 0; i < آرگومانها_2.اندازه(); i++)
			پیام_2 += stringf("%s%s", i ? " " : "", آرگومانها_2[i]);
		لگاریتم("\n");
		لگاریتم("%s\n", پیام_2);
	}

	إذا (pass_ثبات.شمار(آرگومانها_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", آرگومانها_2[0]);
	Pass *pass = pass_ثبات[آرگومانها_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	إذا (pass->experimental_flag)
		log_experimental(آرگومانها_2[0]);

	اندازه_t orig_sel_پشته_pos = design->selection_پشته.اندازه();
	خودکار state = pass->pre_execute();
	pass->execute(آرگومانها_2, design);
	pass->post_execute(state);
	طالما (design->selection_پشته.اندازه() > orig_sel_پشته_pos)
		design->برداشت_selection();
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module.clear();
	design->افزودن_selection(selection);

	Pass::call(design, command);

	design->برداشت_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::وکتور<std::string> آرگومانها_2)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module.clear();
	design->افزودن_selection(selection);

	Pass::call(design, آرگومانها_2);

	design->برداشت_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module = module->name;
	design->افزودن_خالی_selection();
	design->select(module);

	Pass::call(design, command);

	design->برداشت_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::وکتور<std::string> آرگومانها_2)
{
	IdString backup_selected_فعال_module = design->selected_فعال_module;
	design->selected_فعال_module = module->name;
	design->افزودن_خالی_selection();
	design->select(module);

	Pass::call(design, آرگومانها_2);

	design->برداشت_selection();
	design->selected_فعال_module = backup_selected_فعال_module;
}

منطقي ScriptPass::check_label(std::string label, std::string info)
{
	إذا (فعال_design == اشاره_تهی) {
		لگاریتم("\n");
		إذا (info.خالی())
			لگاریتم("    %s:\n", label);
		وإلا
			لگاریتم("    %s:    %s\n", label, info);
		أرجع درست;
	} وإلا {
		إذا (!فعال_پیاپی_from.خالی() && فعال_پیاپی_from == فعال_پیاپی_مقصد) {
			بلوک_فعال = (label == فعال_پیاپی_from);
		} وإلا {
			إذا (label == فعال_پیاپی_from)
				بلوک_فعال = درست;
			إذا (label == فعال_پیاپی_مقصد)
				بلوک_فعال = خطأ;
		}
		أرجع بلوک_فعال;
	}
}

فراغ ScriptPass::پیاپی(std::string command, std::string info)
{
	إذا (فعال_design == اشاره_تهی) {
		إذا (info.خالی())
			لگاریتم("        %s\n", command);
		وإلا
			لگاریتم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(فعال_design, command);
		فعال_design->check();
	}
}

فراغ ScriptPass::پیاپی_nocheck(std::string command, std::string info)
{
	إذا (فعال_design == اشاره_تهی) {
		إذا (info.خالی())
			لگاریتم("        %s\n", command);
		وإلا
			لگاریتم("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(فعال_design, command);
	}
}

فراغ ScriptPass::پیاپی_script(RTLIL::Design *design, std::string پیاپی_from, std::string پیاپی_مقصد)
{
	help_نما = خطأ;
	فعال_design = design;
	بلوک_فعال = پیاپی_from.خالی();
	فعال_پیاپی_from = پیاپی_from;
	فعال_پیاپی_مقصد = پیاپی_مقصد;
	script();
}

فراغ ScriptPass::help_script()
{
	clear_flags();
	help_نما = درست;
	فعال_design = اشاره_تهی;
	بلوک_فعال = درست;
	فعال_پیاپی_from.clear();
	فعال_پیاپی_مقصد.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, منبع_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Frontend::پیاپی_ثبات()
{
	إذا (pass_ثبات.شمار(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ثبات[pass_name] = هذا;

	إذا (frontend_ثبات.شمار(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_ثبات[frontend_name] = هذا;
}

Frontend::~Frontend()
{
}

فراغ Frontend::execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design)
{
	log_assert(بعدی_آرگومانها.خالی());
	افعل {
		std::istream *f = عدم;
		بعدی_آرگومانها.clear();
		خودکار state = pre_execute();
		execute(f, std::string(), آرگومانها_2, design);
		post_execute(state);
		آرگومانها_2 = بعدی_آرگومانها;
		حذف f;
	} طالما (!آرگومانها_2.خالی());
}

FILE *Frontend::current_script_file = عدم;
std::string Frontend::آخرین_اینجا_document;
std::string Frontend::current_script_filename;
صحيح Frontend::current_script_lineno = 0;

فراغ Frontend::extra_آرگومانها(std::istream *&f, std::string &filename, std::وکتور<std::string> آرگومانها_2, اندازه_t argidx, منطقي bin_درون_داد)
{
	منطقي called_with_fp = f != عدم;

	بعدی_آرگومانها.clear();

	إذا (argidx < آرگومانها_2.اندازه())
	{
		std::string arg = آرگومانها_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(آرگومانها_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(آرگومانها_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		إذا (filename == "<<" && argidx+1 < آرگومانها_2.اندازه())
			filename += آرگومانها_2[++argidx];
		إذا (filename.compare(0, 2, "<<") == 0) {
			إذا (filename.اندازه() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == اشاره_تهی) ?  "<stdin>" : "<inlined>";
			آخرین_اینجا_document.clear();
			طالما (1) {
				std::string میانگیر;
				حرف بلوک[4096];
				طالما (1) {
					إذا (fgets(بلوک, 4096, Frontend::current_script_file == اشاره_تهی? stdin : Frontend::current_script_file) == اشاره_تهی)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					میانگیر += بلوک;
					إذا (میانگیر.اندازه() > 0 && (میانگیر[میانگیر.اندازه() - 1] == '\n' || میانگیر[میانگیر.اندازه() - 1] == '\r'))
						اكسر;
				}
				Frontend::current_script_lineno++;
				اندازه_t indent = میانگیر.پیدا_کن_اولی_not_از(" \t\r\n");
				إذا (indent != std::string::npos && میانگیر.compare(indent, eot_marker.اندازه(), eot_marker) == 0)
					اكسر;
				آخرین_اینجا_document += میانگیر;
			}
			f = جديد std::istringstream(آخرین_اینجا_document);
		} وإلا {
			rewrite_filename(filename);
			وکتور<string> filenames = glob_filename(filename);
			filename = filenames.front();
			إذا (Get_اندازه(filenames) > 1) {
				بعدی_آرگومانها.درج(بعدی_آرگومانها.انتها(), آرگومانها_2.begin(), آرگومانها_2.begin()+argidx);
				بعدی_آرگومانها.درج(بعدی_آرگومانها.انتها(), filenames.begin()+1, filenames.انتها());
			}
			yosys_درون_داد_files.درج(filename);
			f = uncompressed(filename, bin_درون_داد ? std::ifstream::دودویی : std::ifstream::درون);
		}

		لكل (اندازه_t i = argidx+1; i < آرگومانها_2.اندازه(); i++)
			إذا (آرگومانها_2[i].compare(0, 1, "-") == 0)
				cmd_error(آرگومانها_2, i, "Found option, expected arguments.");

		إذا (argidx+1 < آرگومانها_2.اندازه()) {
			إذا (بعدی_آرگومانها.خالی())
				بعدی_آرگومانها.درج(بعدی_آرگومانها.انتها(), آرگومانها_2.begin(), آرگومانها_2.begin()+argidx);
			بعدی_آرگومانها.درج(بعدی_آرگومانها.انتها(), آرگومانها_2.begin()+argidx+1, آرگومانها_2.انتها());
			آرگومانها_2.erase(آرگومانها_2.begin()+argidx+1, آرگومانها_2.انتها());
		}
	}

	إذا (f == عدم)
		cmd_error(آرگومانها_2, argidx, "No filename given.");

	إذا (called_with_fp)
		آرگومانها_2.افزودن_پشت(filename);
	آرگومانها_2[0] = pass_name;
	// cmd_log_args(args);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::وکتور<std::string> آرگومانها_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		آرگومانها_2.افزودن_پشت(p);
	حرر(s);
	frontend_call(design, f, filename, آرگومانها_2);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::وکتور<std::string> آرگومانها_2)
{
	إذا (آرگومانها_2.اندازه() == 0)
		أرجع;
	إذا (frontend_ثبات.شمار(آرگومانها_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", آرگومانها_2[0]);

	إذا (f != عدم) {
		خودکار state = frontend_ثبات[آرگومانها_2[0]]->pre_execute();
		frontend_ثبات[آرگومانها_2[0]]->execute(f, filename, آرگومانها_2, design);
		frontend_ثبات[آرگومانها_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::istream *f_cin = &std::ورودی;
		خودکار state = frontend_ثبات[آرگومانها_2[0]]->pre_execute();
		frontend_ثبات[آرگومانها_2[0]]->execute(f_cin, "<stdin>", آرگومانها_2, design);
		frontend_ثبات[آرگومانها_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.خالی())
			آرگومانها_2.افزودن_پشت(filename);
		frontend_ثبات[آرگومانها_2[0]]->execute(آرگومانها_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, منبع_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Backend::پیاپی_ثبات()
{
	إذا (pass_ثبات.شمار(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_ثبات[pass_name] = هذا;

	إذا (backend_ثبات.شمار(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_ثبات[backend_name] = هذا;
}

Backend::~Backend()
{
}

فراغ Backend::execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design *design)
{
	std::ostream *f = عدم;
	خودکار state = pre_execute();
	execute(f, std::string(), آرگومانها_2, design);
	post_execute(state);
	إذا (f != &std::خروجی)
		حذف f;
}

فراغ Backend::extra_آرگومانها(std::ostream *&f, std::string &filename, std::وکتور<std::string> آرگومانها_2, اندازه_t argidx, منطقي bin_output)
{
	منطقي called_with_fp = f != عدم;

	لكل (; argidx < آرگومانها_2.اندازه(); argidx++)
	{
		std::string arg = آرگومانها_2[argidx];

		إذا (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(آرگومانها_2, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(آرگومانها_2, argidx, "Extra filename argument in direct file mode.");

		إذا (arg == "-") {
			filename = "<stdout>";
			f = &std::خروجی;
			استمر;
		}

		filename = arg;
		rewrite_filename(filename);
		إذا (filename.اندازه() > 3 && filename.compare(filename.اندازه()-3, std::string::npos, ".gz") == 0) {
#اگر_تعریف_شده YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = جديد gzip_ostream;
			إذا (!gf->open(filename)) {
				حذف gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.درج(filename);
			f = gf;
#وإلا
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#پایان_اگر
		} وإلا {
			std::ofstream *ff = جديد std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::دودویی) : std::ofstream::trunc);
			yosys_output_files.درج(filename);
			إذا (ff->fail()) {
				حذف ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	إذا (called_with_fp)
		آرگومانها_2.افزودن_پشت(filename);
	آرگومانها_2[0] = pass_name;
	// cmd_log_args(args);

	إذا (f == عدم) {
		filename = "<stdout>";
		f = &std::خروجی;
	}
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::وکتور<std::string> آرگومانها_2;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		آرگومانها_2.افزودن_پشت(p);
	حرر(s);
	backend_call(design, f, filename, آرگومانها_2);
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::وکتور<std::string> آرگومانها_2)
{
	إذا (آرگومانها_2.اندازه() == 0)
		أرجع;
	إذا (backend_ثبات.شمار(آرگومانها_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", آرگومانها_2[0]);

	اندازه_t orig_sel_پشته_pos = design->selection_پشته.اندازه();

	إذا (f != عدم) {
		خودکار state = backend_ثبات[آرگومانها_2[0]]->pre_execute();
		backend_ثبات[آرگومانها_2[0]]->execute(f, filename, آرگومانها_2, design);
		backend_ثبات[آرگومانها_2[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::ostream *f_cout = &std::خروجی;
		خودکار state = backend_ثبات[آرگومانها_2[0]]->pre_execute();
		backend_ثبات[آرگومانها_2[0]]->execute(f_cout, "<stdout>", آرگومانها_2, design);
		backend_ثبات[آرگومانها_2[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.خالی())
			آرگومانها_2.افزودن_پشت(filename);
		backend_ثبات[آرگومانها_2[0]]->execute(آرگومانها_2, design);
	}

	طالما (design->selection_پشته.اندازه() > orig_sel_پشته_pos)
		design->برداشت_selection();
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
	string منبع;
	string desc;
	string کد;
	string گروه;
	string ver;
	string برچسبها;
};

ساكن منطقي is_کد_getter(string name) {
	أرجع *(--(name.انتها())) == '+';
}

ساكن string get_cell_name(string name) {
	أرجع is_کد_getter(name) ? name.substr(0, name.درازی()-1) : name;
}

ساكن فراغ log_warning_flags(Pass *pass) {
	منطقي has_warnings = خطأ;
	ثابت string name = pass->pass_name;
	إذا (pass->experimental_flag) {
		إذا (!has_warnings) لگاریتم("\n");
		has_warnings = درست;
		لگاریتم("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	إذا (pass->داخلی_flag) {
		إذا (!has_warnings) لگاریتم("\n");
		has_warnings = درست;
		لگاریتم("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	إذا (has_warnings)
		لگاریتم("\n");
}

ساكن بنية CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ضمن "kernel/simlib_help.inc"
#ضمن "kernel/simcells_help.inc"
		cell_help.مرتب_سازی();
	}
	منطقي شامل_است(string name) { أرجع cell_help.شمار(get_cell_name(name)) > 0; }
	SimHelper get(string name) { أرجع cell_help[get_cell_name(name)]; }
} cell_help_messages;

بنية HelpPass : عام Pass {
	HelpPass() : Pass("help", "display help messages") { }
	فراغ help() override
	{
		لگاریتم("\n");
		لگاریتم("    help  ................  list all commands\n");
		لگاریتم("    help <command>  ......  print help message for given command\n");
		لگاریتم("    help -all  ...........  print complete command reference\n");
		لگاریتم("\n");
		لگاریتم("    help -cells ..........  list all cell types\n");
		لگاریتم("    help <celltype>  .....  print help message for given cell type\n");
		لگاریتم("    help <celltype>+  ....  print verilog code for given cell type\n");
		لگاریتم("\n");
	}
	منطقي dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.مدخل("version", "Yosys command reference");
		json.مدخل("generator", yosys_نسخه_str);

		منطقي raise_error = خطأ;
		std::map<string, وکتور<string>> groups;

		// get root path
		خودکار this_مسیر = std::filesystem::مسیر_2(منبع_location::current().file_name());
		خودکار منبع_ریشه = this_مسیر.والد_مسیر().والد_مسیر();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		لكل (خودکار &it : pass_ثبات) {
			خودکار name = it.اولی;
			خودکار pass = it.دومی;
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

				منبع_location null_منبع;
				string current_میانگیر = "";
				خودکار ریشه_listing_2 = cmd_help.get_ریشه();
				خودکار current_listing = ریشه_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					خودکار log_scope = logger().sink_scope();
					logger().جمع_کن_sink<جریان_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				اندازه_t def_strip_شمار = 0;
				خودکار current_state = PUState_none;
				خودکار catch_verific = خطأ;
				خودکار blank_سطرها = 2;
				لكل (string سطر; std::getline(ss, سطر, '\n');) {
					// find position of first non space character
					std::اندازه_t اولی_pos = سطر.پیدا_کن_اولی_not_از(" \t");
					std::اندازه_t آخرین_pos = سطر.پیدا_کن_آخرین_not_از(" \t");
					إذا (اولی_pos == std::string::npos) {
						بدل (current_state)
						{
						حالة PUState_signature:
							ریشه_listing_2->usage(current_میانگیر, null_منبع);
							current_listing = ریشه_listing_2;
							current_state = PUState_none;
							current_میانگیر = "";
							اكسر;
						حالة PUState_none:
						حالة PUState_optionbody:
							blank_سطرها += 1;
							اكسر;
						افتراضي:
							اكسر;
						}
						// skip empty lines
						استمر;
					}

					// strip leading and trailing whitespace
					std::string stripped_سطر = سطر.substr(اولی_pos, آخرین_pos - اولی_pos +1);
					منطقي IsDefinition = stripped_سطر[0] == '-';
					IsDefinition &= stripped_سطر[1] != ' ' && stripped_سطر[1] != '>';
					منطقي IsDedent = def_strip_شمار && اولی_pos < def_strip_شمار;
					منطقي IsIndent = def_strip_شمار < اولی_pos;

					// line looks like a signature
					منطقي IsSignature = stripped_سطر.پیدا_کن(name) == 0 && (stripped_سطر.درازی() == name.درازی() || stripped_سطر.at(name.اندازه()) == ' ');

					إذا (IsSignature && اولی_pos <= 4 && (blank_سطرها >= 2 || current_state == PUState_signature)) {
						إذا (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_میانگیر, "none", null_منبع);
							current_میانگیر = "";
						} وإلا إذا (current_state == PUState_signature) {
							ریشه_listing_2->usage(current_میانگیر, null_منبع);
							current_میانگیر = "";
						} وإلا إذا (current_state == PUState_none && !current_میانگیر.خالی()) {
							current_listing->codeblock(current_میانگیر, "none", null_منبع);
							current_میانگیر = "";
						}
						current_listing = ریشه_listing_2;
						current_state = PUState_signature;
						def_strip_شمار = اولی_pos;
						catch_verific = خطأ;
					} وإلا إذا (IsDedent) {
						def_strip_شمار = اولی_pos;
						إذا (current_state == PUState_optionbody) {
							إذا (!current_میانگیر.خالی()) {
								current_listing->codeblock(current_میانگیر, "none", null_منبع);
								current_میانگیر = "";
							}
							إذا (IsIndent) {
								current_state = PUState_options;
								current_listing = ریشه_listing_2->پشت();
							} وإلا {
								current_state = PUState_none;
								current_listing = ریشه_listing_2;
							}
						} وإلا {
							current_state = PUState_none;
						}
					}

					إذا (IsDefinition && !catch_verific && current_state != PUState_signature) {
						إذا (!current_میانگیر.خالی()) {
							current_listing->codeblock(current_میانگیر, "none", null_منبع);
							current_میانگیر = "";
						}
						current_state = PUState_options;
						current_listing = ریشه_listing_2->open_option(stripped_سطر, null_منبع);
						def_strip_شمار = اولی_pos;
					} وإلا {
						إذا (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						إذا (current_میانگیر.خالی())
							current_میانگیر = stripped_سطر;
						وإلا إذا (current_state == PUState_signature && IsIndent)
							current_میانگیر += stripped_سطر;
						وإلا إذا (current_state == PUState_none) {
							current_میانگیر += (blank_سطرها > 0 ? "\n\n" : "\n") + سطر;
						} وإلا
							current_میانگیر += (blank_سطرها > 0 ? "\n\n" : "\n") + stripped_سطر;
						إذا (stripped_سطر.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = درست;
					}
					blank_سطرها = 0;
				}

				إذا (!current_میانگیر.خالی()) {
					إذا (current_میانگیر.اندازه() > 64 && current_میانگیر.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_میانگیر.substr(0, 62), null_منبع);
						current_listing->codeblock(current_میانگیر.substr(64), "yoscrypt", null_منبع);
					} وإلا
						current_listing->codeblock(current_میانگیر, "none", null_منبع);
					current_میانگیر = "";
				}
			}

			// fix path
			string منبع_file = pass->location.file_name();
			منطقي has_منبع = منبع_file.compare("unknown") != 0;
			std::filesystem::مسیر_2 منبع_مسیر;
			خودکار skip_منبع_گروه = خطأ;

			إذا (has_منبع) {
				منبع_مسیر = fixup_منبع_مسیر(pass->location.file_name(), منبع_ریشه, &skip_منبع_گروه);
				منبع_file = منبع_مسیر.string();
			}

			// attempt auto group
			إذا (!cmd_help.has_گروه()) {
				إذا (pass->داخلی_flag)
					cmd_help.گروه = "internal";
				وإلا إذا (منبع_file.پیدا_کن("backends/") == 0 || (!has_منبع && name.پیدا_کن("read_") == 0))
					cmd_help.گروه = "backends";
				وإلا إذا (منبع_file.پیدا_کن("frontends/") == 0 || (!has_منبع && name.پیدا_کن("write_") == 0))
					cmd_help.گروه = "frontends";
				وإلا إذا (has_منبع) {
					إذا (منبع_مسیر.has_والد_مسیر() && !skip_منبع_گروه)
						cmd_help.گروه = منبع_مسیر.والد_مسیر().string();
				}
				// implicit !has_source
				وإلا إذا (name.پیدا_کن("equiv") == 0)
					cmd_help.گروه = "passes/equiv";
				وإلا إذا (name.پیدا_کن("fsm") == 0)
					cmd_help.گروه = "passes/fsm";
				وإلا إذا (name.پیدا_کن("memory") == 0)
					cmd_help.گروه = "passes/memory";
				وإلا إذا (name.پیدا_کن("opt") == 0)
					cmd_help.گروه = "passes/opt";
				وإلا إذا (name.پیدا_کن("proc") == 0)
					cmd_help.گروه = "passes/proc";
			}

			إذا (groups.شمار(cmd_help.گروه) == 0) {
				groups[cmd_help.گروه] = وکتور<string>();
			}
			groups[cmd_help.گروه].افزودن_پشت(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.مدخل("title", title);
			json.name("content"); json.begin_array();
			لكل (خودکار &content : cmd_help)
				json.مقدار(content.مقصد_json(منبع_ریشه));
			json.انتها_array();
			json.مدخل("group", cmd_help.گروه);
			json.مدخل("source_file", منبع_file);
			json.مدخل("source_line", pass->location.سطر());
			json.مدخل("source_func", pass->location.function_name());
			json.مدخل("experimental_flag", pass->experimental_flag);
			json.مدخل("internal_flag", pass->داخلی_flag);
			json.انتها_object();
		}
		json.انتها_object();

		json.مدخل("groups", groups);

		json.انتها_object();
		أرجع raise_error;
	}
	منطقي dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.مدخل("version", "Yosys internal cells");
		json.مدخل("generator", yosys_maybe_نسخه());

		dict<string, وکتور<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>> cells;

		// iterate over cells
		منطقي raise_error = خطأ;
		لكل (خودکار it : StaticCellTypes::builder.cells) {
			إذا (!StaticCellTypes::categories.is_معلوم(it.type))
				استمر;
			خودکار name = ID::str(it.type);
			إذا (cell_help_messages.شامل_است(name)) {
				خودکار cell_help = cell_help_messages.get(name);
				groups[cell_help.گروه].emplace_پشت(name);
				خودکار cell_pair = pair<SimHelper, StaticCellTypes::Cell_جدول_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} وإلا {
				لگاریتم("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= درست;
			}
		}
		لكل (خودکار &it : cell_help_messages.cell_help) {
			إذا (cells.شمار(it.اولی) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.اولی);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.مرتب_سازی();
		لكل (خودکار &it : groups) {
			json.name(it.اولی.c_str()); json.مقدار(it.دومی);
		}
		json.انتها_object();

		json.name("cells"); json.begin_object();
		cells.مرتب_سازی();
		لكل (خودکار &it : cells) {
			خودکار ch = it.دومی.اولی;
			خودکار ct = it.دومی.دومی;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.مقدار(ch.title);
			json.name("ports"); json.مقدار(ch.ports);
			json.name("source"); json.مقدار(ch.منبع);
			json.name("desc"); json.مقدار(ch.desc);
			json.name("code"); json.مقدار(ch.کد);
			وکتور<string> inputs, outputs;
			لكل (خودکار &درون_داد : ct.inputs)
				inputs.افزودن_پشت(ID::str(درون_داد));
			json.name("inputs"); json.مقدار(inputs);
			لكل (خودکار &output : ct.outputs)
				outputs.افزودن_پشت(ID::str(output));
			json.name("outputs"); json.مقدار(outputs);
			وکتور<string> properties;
			// CellType properties
			إذا (ct.features.is_evaluable) properties.افزودن_پشت("is_evaluable");
			إذا (ct.features.is_combinatorial) properties.افزودن_پشت("is_combinatorial");
			إذا (ct.features.is_synthesizable) properties.افزودن_پشت("is_synthesizable");
			// SimHelper properties
			اندازه_t آخرین = 0; اندازه_t بعدی = 0;
			طالما ((بعدی = ch.برچسبها.پیدا_کن(", ", آخرین)) != string::npos) {
				properties.افزودن_پشت(ch.برچسبها.substr(آخرین, بعدی-آخرین));
				آخرین = بعدی + 2;
			}
			خودکار final_برچسب = ch.برچسبها.substr(آخرین);
			إذا (final_برچسب.اندازه()) properties.افزودن_پشت(final_برچسب);
			json.name("properties"); json.مقدار(properties);
			json.انتها_object();
		}
		json.انتها_object();

		json.انتها_object();
		أرجع raise_error;
	}
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design*) override
	{
		إذا (آرگومانها_2.اندازه() == 1) {
			لگاریتم("\n");
			لكل (خودکار &it : pass_ثبات)
				لگاریتم("    %-20s %s\n", it.اولی, it.دومی->short_help);
			لگاریتم("\n");
			لگاریتم("Type 'help <command>' for more information on a command.\n");
			لگاریتم("Type 'help -cells' for a list of all cell types.\n");
			لگاریتم("\n");
			أرجع;
		}

		إذا (آرگومانها_2.اندازه() == 2) {
			إذا (آرگومانها_2[1] == "-all") {
				لكل (خودکار &it : pass_ثبات) {
					لگاریتم("\n");
					لگاریتم("\n");
					لگاریتم("%s  --  %s\n", it.اولی, it.دومی->short_help);
					std::string پیام;
					لكل (اندازه_t i = 0; i < it.اولی.اندازه() + it.دومی->short_help.اندازه() + 6; i++)
						پیام += '=';
					لگاریتم("%s\n", پیام);
					it.دومی->help();
					log_warning_flags(it.دومی);
				}
			}
			وإلا إذا (آرگومانها_2[1] == "-cells") {
				لگاریتم("\n");
				لكل (خودکار &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.دومی;
					لگاریتم("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				لگاریتم("\n");
				لگاریتم("Type 'help <cell_type>' for more information on a cell type.\n");
				لگاریتم("\n");
				أرجع;
			}
			وإلا إذا (pass_ثبات.شمار(آرگومانها_2[1])) {
				pass_ثبات.at(آرگومانها_2[1])->help();
				log_warning_flags(pass_ثبات.at(آرگومانها_2[1]));
			}
			وإلا إذا (cell_help_messages.شامل_است(آرگومانها_2[1])) {
				خودکار help_cell = cell_help_messages.get(آرگومانها_2[1]);
				إذا (is_کد_getter(آرگومانها_2[1])) {
						لگاریتم("\n");
						لگاریتم("%s\n", help_cell.کد);
				} وإلا {
					لگاریتم("\n");
					لگاریتم("    %s %s\n", help_cell.name, help_cell.ports);
					لگاریتم("\n");
					إذا (help_cell.ver == "2" || help_cell.ver == "2a") {
						إذا (help_cell.title != "") لگاریتم("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						لكل (std::string سطر; std::getline(ss, سطر, '\n');) {
							إذا (سطر != "::") لگاریتم("%s\n", سطر);
						}
					} وإلا إذا (help_cell.desc.درازی()) {
						لگاریتم("%s\n", help_cell.desc);
					} وإلا {
						لگاریتم("No help message for this cell type found.\n");
					}
					لگاریتم("\n");
					لگاریتم("Run 'help %s+' to display the Verilog model for this cell type.\n", آرگومانها_2[1]);
					لگاریتم("\n");
				}
			}
			وإلا
				لگاریتم("No such command or cell type: %s\n", آرگومانها_2[1]);
			أرجع;
		} وإلا إذا (آرگومانها_2.اندازه() == 3) {
			// this option is undocumented as it is for internal use only
			إذا (آرگومانها_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				إذا (!json.write_مقصد_file(آرگومانها_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", آرگومانها_2[2], strerror(errno));
				إذا (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			وإلا إذا (آرگومانها_2[1] == "-dump-cells-json") {
				PrettyJson json;
				إذا (!json.write_مقصد_file(آرگومانها_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", آرگومانها_2[2], strerror(errno));
				إذا (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			وإلا
				لگاریتم("Unknown help command: `%s %s'\n", آرگومانها_2[1], آرگومانها_2[2]);
			أرجع;
		}

		help();
	}
} HelpPass;

بنية EchoPass : عام Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	فراغ help() override
	{
		لگاریتم("\n");
		لگاریتم("    echo on\n");
		لگاریتم("\n");
		لگاریتم("Print all commands to log before executing them.\n");
		لگاریتم("\n");
		لگاریتم("\n");
		لگاریتم("    echo off\n");
		لگاریتم("\n");
		لگاریتم("Do not print all commands to log before executing them. (default)\n");
		لگاریتم("\n");
	}
	فراغ execute(std::وکتور<std::string> آرگومانها_2, RTLIL::Design*) override
	{
		إذا (آرگومانها_2.اندازه() > 2)
			cmd_error(آرگومانها_2, 2, "Unexpected argument.");

		إذا (آرگومانها_2.اندازه() == 2) {
			إذا (آرگومانها_2[1] == "on")
				echo_نما = درست;
			وإلا إذا (آرگومانها_2[1] == "off")
				echo_نما = خطأ;
			وإلا
				cmd_error(آرگومانها_2, 1, "Unexpected argument.");
		}

		لگاریتم("echo %s\n", echo_نما ? "on" : "off");
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
		لگاریتم("\n");
		لگاریتم("    license\n");
		لگاریتم("\n");
		لگاریتم("This command produces the following notice.\n");
		notice();
	}
	فراغ execute(std::وکتور<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	فراغ notice()
	{
		لگاریتم("\n");
		لگاریتم(" /----------------------------------------------------------------------------\\\n");
		لگاریتم(" |                                                                            |\n");
		لگاریتم(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		لگاریتم(" |                                                                            |\n");
		لگاریتم(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		لگاریتم(" |                                                                            |\n");
		لگاریتم(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		لگاریتم(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		لگاریتم(" |  copyright notice and this permission notice appear in all copies.         |\n");
		لگاریتم(" |                                                                            |\n");
		لگاریتم(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		لگاریتم(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		لگاریتم(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		لگاریتم(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		لگاریتم(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		لگاریتم(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		لگاریتم(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		لگاریتم(" |                                                                            |\n");
		لگاریتم(" \\----------------------------------------------------------------------------/\n");
		لگاریتم("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_انتها

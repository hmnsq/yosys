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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/satgen.h"
#అన్తర్భావ "kernel/json.h"
#అన్తర్భావ "kernel/gzip.h"
#అన్తర్భావ "kernel/log_help.h"
#అన్తర్భావ "kernel/newcelltypes.h"

#అన్తర్భావ <string.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <filesystem>

YOSYS_NAMESPACE_BEGIN

#నిర్వచించు MAX_REG_లెక్క 1000

తార్కిక echo_బహుళకం = అబద్ధం;
Pass *మొదటి_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_రిజిస్టరు;
std::map<std::string, Pass*> pass_రిజిస్టరు;
std::map<std::string, Backend*> backend_రిజిస్టరు;
std::map<std::string, లక్ష్యం> లక్ష్యం_రిజిస్టరు;

std::సదిశ<std::string> Frontend::తదుపరి_వాదనలు;

తార్కిక GarbageCollectionGuard::is_enabled_ = నిజం;

స్థిర తార్కిక garbage_collection_requested = అబద్ధం;

శూన్య request_garbage_collection()
{
	garbage_collection_requested = నిజం;
}

శూన్య try_collect_garbage()
{
	ఒకవేళ (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		తిరిగి;
	garbage_collection_requested = అబద్ధం;
	ఒకవేళ (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, ఆధారం_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	తదుపరి_queued_pass = మొదటి_queued_pass;
	మొదటి_queued_pass = ఇది;
	call_counter = 0;
	runtime_ns = 0;
}

శూన్య Pass::నిరంతరం_రిజిస్టరు()
{
	ఒకవేళ (pass_రిజిస్టరు.లెక్క(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_రిజిస్టరు[pass_name] = ఇది;
}

శూన్య Pass::init_రిజిస్టరు()
{
	సదిశ<Pass*> added_passes;
	అయితే (మొదటి_queued_pass) {
		added_passes.నెట్టు_వెనుక(మొదటి_queued_pass);
		మొదటి_queued_pass->నిరంతరం_రిజిస్టరు();
		మొదటి_queued_pass = మొదటి_queued_pass->తదుపరి_queued_pass;
	}
	కొరకు (స్వయంచాలక added_pass : added_passes)
		added_pass->on_రిజిస్టరు();
}

శూన్య Pass::పూర్తి_రిజిస్టరు()
{
	కొరకు (స్వయంచాలక &it : pass_రిజిస్టరు)
		it.రెండవ->on_shutdown();

	frontend_రిజిస్టరు.clear();
	pass_రిజిస్టరు.clear();
	backend_రిజిస్టరు.clear();
	log_assert(మొదటి_queued_pass == ఖాళీ);
}

శూన్య Pass::on_రిజిస్టరు()
{
}

శూన్య Pass::on_shutdown()
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
	state.మాతృ_pass = current_pass;
	current_pass = ఇది;
	clear_flags();
	తిరిగి state;
}

శూన్య Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t సమయం_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += సమయం_ns;
	current_pass = state.మాతృ_pass;
	వ్యవకలనం_from_current_runtime_ns(సమయం_ns);
}

శూన్య Pass::వ్యవకలనం_from_current_runtime_ns(int64_t సమయం_ns)
{
	ఒకవేళ (current_pass)
		current_pass->runtime_ns -= సమయం_ns;
}

శూన్య Pass::help()
{
	స్వయంచాలక prettyHelp = PrettyHelp();
	ఒకవేళ (formatted_help()) {
		prettyHelp.log_help();
	} లేకపోతే {
		సంవర్గమానం("\n");
		సంవర్గమానం("No help message for command `%s'.\n", pass_name);
		సంవర్గమానం("\n");
	}
}

తార్కిక Pass::formatted_help()
{
	తిరిగి అబద్ధం;
}

శూన్య Pass::clear_flags()
{
}

శూన్య Pass::cmd_log_వాదనలు(స్థిరరాశి std::సదిశ<std::string> &వాదనలు_2)
{
	ఒకవేళ (వాదనలు_2.కొలత() <= 1)
		తిరిగి;
	std::string సందేశం_2 = "Full command line:";
	కొరకు (కొలత_t i = 0; i < వాదనలు_2.కొలత(); i++)
		సందేశం_2 += stringf(" %s", వాదనలు_2[i]);
	సంవర్గమానం("%s\n", సందేశం_2);
}

శూన్య Pass::cmd_error(స్థిరరాశి std::సదిశ<std::string> &వాదనలు_2, కొలత_t argidx, std::string సందేశం)
{
	std::string command_పాఠ్యం;
	పూర్ణాంకం error_pos = 0;

	కొరకు (కొలత_t i = 0; i < వాదనలు_2.కొలత(); i++) {
		ఒకవేళ (i < argidx)
			error_pos += వాదనలు_2[i].కొలత() + 1;
		command_పాఠ్యం = command_పాఠ్యం + (command_పాఠ్యం.ఖాళీలు() ? "" : " ") + వాదనలు_2[i];
	}

	సంవర్గమానం("\n");
	సంవర్గమానం("Syntax error in command `%s':\n", command_పాఠ్యం);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			సందేశం.c_str(), command_పాఠ్యం.c_str(), error_pos, "");
}

శూన్య Pass::extra_వాదనలు(std::సదిశ<std::string> వాదనలు_2, కొలత_t argidx, RTLIL::Design *design, తార్కిక select)
{
	కొరకు (; argidx < వాదనలు_2.కొలత(); argidx++)
	{
		std::string arg = వాదనలు_2[argidx];

		ఒకవేళ (arg.compare(0, 1, "-") == 0)
			cmd_error(వాదనలు_2, argidx, "Unknown option or option in arguments.");

		ఒకవేళ (!select)
			cmd_error(వాదనలు_2, argidx, "Extra argument.");

		handle_extra_select_వాదనలు(ఇది, వాదనలు_2, argidx, వాదనలు_2.కొలత(), design);
		ఆపు;
	}
	// cmd_log_args(args);
}

శూన్య Pass::call(RTLIL::Design *design, std::string command)
{
	std::సదిశ<std::string> వాదనలు_2;

	std::string cmd_buf = command;
	std::string tok = తదుపరి_token(cmd_buf, " \t\r\n", నిజం);

	ఒకవేళ (tok.ఖాళీలు())
		తిరిగి;

	ఒకవేళ (tok[0] == '!') {
#ఒకవేళ defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.కనుగొను('!') + 1);
		అయితే (!cmd_buf.ఖాళీలు() && (cmd_buf.వెనుక() == ' ' || cmd_buf.వెనుక() == '\t' ||
				cmd_buf.వెనుక() == '\r' || cmd_buf.వెనుక() == '\n'))
			cmd_buf.resize(cmd_buf.కొలత()-1);
		log_శీర్షిక(design, "Shell command: %s\n", cmd_buf);
		పూర్ణాంకం ret_సంకేతం = నిరంతరం_command(cmd_buf);
		ఒకవేళ (ret_సంకేతం != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_సంకేతం);
		తిరిగి;
#లేకపోతే
		log_cmd_error("Shell is not available.\n");
#అయితే_ముగింపు
	}

	అయితే (!tok.ఖాళీలు()) {
		ఒకవేళ (tok[0] == '#') {
			పూర్ణాంకం stop;
			కొరకు (stop = 0; stop < Get_కొలత(cmd_buf); stop++)
				ఒకవేళ (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					ఆపు;
			cmd_buf = cmd_buf.substr(stop);
		} లేకపోతే
		ఒకవేళ (tok.వెనుక() == ';') {
			పూర్ణాంకం అంకె_semikolon = 0;
			అయితే (!tok.ఖాళీలు() && tok.వెనుక() == ';')
				tok.resize(tok.కొలత()-1), అంకె_semikolon++;
			ఒకవేళ (!tok.ఖాళీలు())
				వాదనలు_2.నెట్టు_వెనుక(tok);
			call(design, వాదనలు_2);
			వాదనలు_2.clear();
			ఒకవేళ (అంకె_semikolon == 2)
				call(design, "clean");
			ఒకవేళ (అంకె_semikolon == 3)
				call(design, "clean -purge");
		} లేకపోతే
			వాదనలు_2.నెట్టు_వెనుక(tok);
		తార్కిక దొరికింది_nl = అబద్ధం;
		కొరకు (స్వయంచాలక c : cmd_buf) {
			ఒకవేళ (c == ' ' || c == '\t')
				కొనసాగు;
			ఒకవేళ (c == '\r' || c == '\n')
				దొరికింది_nl = నిజం;
			ఆపు;
		}
		ఒకవేళ (దొరికింది_nl) {
			call(design, వాదనలు_2);
			వాదనలు_2.clear();
		}
		tok = తదుపరి_token(cmd_buf, " \t\r\n", నిజం);
	}

	call(design, వాదనలు_2);
}

శూన్య Pass::call(RTLIL::Design *design, std::సదిశ<std::string> వాదనలు_2)
{
	ఒకవేళ (వాదనలు_2.కొలత() == 0 || వాదనలు_2[0][0] == '#' || వాదనలు_2[0][0] == ':')
		తిరిగి;

	ఒకవేళ (echo_బహుళకం) {
		std::string సందేశం_2 = stringf("%s", create_prompt(design, 0, అబద్ధం));
		కొరకు (కొలత_t i = 0; i < వాదనలు_2.కొలత(); i++)
			సందేశం_2 += stringf("%s%s", i ? " " : "", వాదనలు_2[i]);
		సంవర్గమానం("\n");
		సంవర్గమానం("%s\n", సందేశం_2);
	}

	ఒకవేళ (pass_రిజిస్టరు.లెక్క(వాదనలు_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", వాదనలు_2[0]);
	Pass *pass = pass_రిజిస్టరు[వాదనలు_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	ఒకవేళ (pass->experimental_flag)
		log_experimental(వాదనలు_2[0]);

	కొలత_t orig_sel_దొంతర_pos = design->selection_దొంతర.కొలత();
	స్వయంచాలక state = pass->pre_execute();
	pass->execute(వాదనలు_2, design);
	pass->post_execute(state);
	అయితే (design->selection_దొంతర.కొలత() > orig_sel_దొంతర_pos)
		design->తీయి_selection();
}

శూన్య Pass::call_on_selection(RTLIL::Design *design, స్థిరరాశి RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_చురుకైన_module = design->selected_చురుకైన_module;
	design->selected_చురుకైన_module.clear();
	design->నెట్టు_selection(selection);

	Pass::call(design, command);

	design->తీయి_selection();
	design->selected_చురుకైన_module = backup_selected_చురుకైన_module;
}

శూన్య Pass::call_on_selection(RTLIL::Design *design, స్థిరరాశి RTLIL::Selection &selection, std::సదిశ<std::string> వాదనలు_2)
{
	IdString backup_selected_చురుకైన_module = design->selected_చురుకైన_module;
	design->selected_చురుకైన_module.clear();
	design->నెట్టు_selection(selection);

	Pass::call(design, వాదనలు_2);

	design->తీయి_selection();
	design->selected_చురుకైన_module = backup_selected_చురుకైన_module;
}

శూన్య Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_చురుకైన_module = design->selected_చురుకైన_module;
	design->selected_చురుకైన_module = module->name;
	design->నెట్టు_ఖాళీలు_selection();
	design->select(module);

	Pass::call(design, command);

	design->తీయి_selection();
	design->selected_చురుకైన_module = backup_selected_చురుకైన_module;
}

శూన్య Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::సదిశ<std::string> వాదనలు_2)
{
	IdString backup_selected_చురుకైన_module = design->selected_చురుకైన_module;
	design->selected_చురుకైన_module = module->name;
	design->నెట్టు_ఖాళీలు_selection();
	design->select(module);

	Pass::call(design, వాదనలు_2);

	design->తీయి_selection();
	design->selected_చురుకైన_module = backup_selected_చురుకైన_module;
}

తార్కిక ScriptPass::check_label(std::string label, std::string info)
{
	ఒకవేళ (చురుకైన_design == శూన్యసూచిక) {
		సంవర్గమానం("\n");
		ఒకవేళ (info.ఖాళీలు())
			సంవర్గమానం("    %s:\n", label);
		లేకపోతే
			సంవర్గమానం("    %s:    %s\n", label, info);
		తిరిగి నిజం;
	} లేకపోతే {
		ఒకవేళ (!చురుకైన_నిరంతరం_from.ఖాళీలు() && చురుకైన_నిరంతరం_from == చురుకైన_నిరంతరం_గమ్యం) {
			ఖండం_చురుకైన = (label == చురుకైన_నిరంతరం_from);
		} లేకపోతే {
			ఒకవేళ (label == చురుకైన_నిరంతరం_from)
				ఖండం_చురుకైన = నిజం;
			ఒకవేళ (label == చురుకైన_నిరంతరం_గమ్యం)
				ఖండం_చురుకైన = అబద్ధం;
		}
		తిరిగి ఖండం_చురుకైన;
	}
}

శూన్య ScriptPass::నిరంతరం(std::string command, std::string info)
{
	ఒకవేళ (చురుకైన_design == శూన్యసూచిక) {
		ఒకవేళ (info.ఖాళీలు())
			సంవర్గమానం("        %s\n", command);
		లేకపోతే
			సంవర్గమానం("        %s    %s\n", command, info);
	} లేకపోతే {
		Pass::call(చురుకైన_design, command);
		చురుకైన_design->check();
	}
}

శూన్య ScriptPass::నిరంతరం_nocheck(std::string command, std::string info)
{
	ఒకవేళ (చురుకైన_design == శూన్యసూచిక) {
		ఒకవేళ (info.ఖాళీలు())
			సంవర్గమానం("        %s\n", command);
		లేకపోతే
			సంవర్గమానం("        %s    %s\n", command, info);
	} లేకపోతే {
		Pass::call(చురుకైన_design, command);
	}
}

శూన్య ScriptPass::నిరంతరం_script(RTLIL::Design *design, std::string నిరంతరం_from, std::string నిరంతరం_గమ్యం)
{
	help_బహుళకం = అబద్ధం;
	చురుకైన_design = design;
	ఖండం_చురుకైన = నిరంతరం_from.ఖాళీలు();
	చురుకైన_నిరంతరం_from = నిరంతరం_from;
	చురుకైన_నిరంతరం_గమ్యం = నిరంతరం_గమ్యం;
	script();
}

శూన్య ScriptPass::help_script()
{
	clear_flags();
	help_బహుళకం = నిజం;
	చురుకైన_design = శూన్యసూచిక;
	ఖండం_చురుకైన = నిజం;
	చురుకైన_నిరంతరం_from.clear();
	చురుకైన_నిరంతరం_గమ్యం.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, ఆధారం_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

శూన్య Frontend::నిరంతరం_రిజిస్టరు()
{
	ఒకవేళ (pass_రిజిస్టరు.లెక్క(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_రిజిస్టరు[pass_name] = ఇది;

	ఒకవేళ (frontend_రిజిస్టరు.లెక్క(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_రిజిస్టరు[frontend_name] = ఇది;
}

Frontend::~Frontend()
{
}

శూన్య Frontend::execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design)
{
	log_assert(తదుపరి_వాదనలు.ఖాళీలు());
	చేయి {
		std::istream *f = ఖాళీ;
		తదుపరి_వాదనలు.clear();
		స్వయంచాలక state = pre_execute();
		execute(f, std::string(), వాదనలు_2, design);
		post_execute(state);
		వాదనలు_2 = తదుపరి_వాదనలు;
		తొలగించు f;
	} అయితే (!వాదనలు_2.ఖాళీలు());
}

FILE *Frontend::current_script_file = ఖాళీ;
std::string Frontend::చివరి_ఇక్కడ_document;
std::string Frontend::current_script_filename;
పూర్ణాంకం Frontend::current_script_lineno = 0;

శూన్య Frontend::extra_వాదనలు(std::istream *&f, std::string &filename, std::సదిశ<std::string> వాదనలు_2, కొలత_t argidx, తార్కిక bin_ఆదానం)
{
	తార్కిక called_with_fp = f != ఖాళీ;

	తదుపరి_వాదనలు.clear();

	ఒకవేళ (argidx < వాదనలు_2.కొలత())
	{
		std::string arg = వాదనలు_2[argidx];

		ఒకవేళ (arg.compare(0, 1, "-") == 0)
			cmd_error(వాదనలు_2, argidx, "Unknown option or option in arguments.");
		ఒకవేళ (f != ఖాళీ)
			cmd_error(వాదనలు_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		ఒకవేళ (filename == "<<" && argidx+1 < వాదనలు_2.కొలత())
			filename += వాదనలు_2[++argidx];
		ఒకవేళ (filename.compare(0, 2, "<<") == 0) {
			ఒకవేళ (filename.కొలత() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == శూన్యసూచిక) ?  "<stdin>" : "<inlined>";
			చివరి_ఇక్కడ_document.clear();
			అయితే (1) {
				std::string బఫరు;
				అక్షరం ఖండం[4096];
				అయితే (1) {
					ఒకవేళ (fgets(ఖండం, 4096, Frontend::current_script_file == శూన్యసూచిక? stdin : Frontend::current_script_file) == శూన్యసూచిక)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					బఫరు += ఖండం;
					ఒకవేళ (బఫరు.కొలత() > 0 && (బఫరు[బఫరు.కొలత() - 1] == '\n' || బఫరు[బఫరు.కొలత() - 1] == '\r'))
						ఆపు;
				}
				Frontend::current_script_lineno++;
				కొలత_t indent = బఫరు.కనుగొను_మొదటి_not_యొక్క(" \t\r\n");
				ఒకవేళ (indent != std::string::npos && బఫరు.compare(indent, eot_marker.కొలత(), eot_marker) == 0)
					ఆపు;
				చివరి_ఇక్కడ_document += బఫరు;
			}
			f = కొత్త std::istringstream(చివరి_ఇక్కడ_document);
		} లేకపోతే {
			rewrite_filename(filename);
			సదిశ<string> filenames = glob_filename(filename);
			filename = filenames.front();
			ఒకవేళ (Get_కొలత(filenames) > 1) {
				తదుపరి_వాదనలు.చొప్పించు(తదుపరి_వాదనలు.చివర(), వాదనలు_2.begin(), వాదనలు_2.begin()+argidx);
				తదుపరి_వాదనలు.చొప్పించు(తదుపరి_వాదనలు.చివర(), filenames.begin()+1, filenames.చివర());
			}
			yosys_ఆదానం_files.చొప్పించు(filename);
			f = uncompressed(filename, bin_ఆదానం ? std::ifstream::ద్విమాన : std::ifstream::లోపల);
		}

		కొరకు (కొలత_t i = argidx+1; i < వాదనలు_2.కొలత(); i++)
			ఒకవేళ (వాదనలు_2[i].compare(0, 1, "-") == 0)
				cmd_error(వాదనలు_2, i, "Found option, expected arguments.");

		ఒకవేళ (argidx+1 < వాదనలు_2.కొలత()) {
			ఒకవేళ (తదుపరి_వాదనలు.ఖాళీలు())
				తదుపరి_వాదనలు.చొప్పించు(తదుపరి_వాదనలు.చివర(), వాదనలు_2.begin(), వాదనలు_2.begin()+argidx);
			తదుపరి_వాదనలు.చొప్పించు(తదుపరి_వాదనలు.చివర(), వాదనలు_2.begin()+argidx+1, వాదనలు_2.చివర());
			వాదనలు_2.erase(వాదనలు_2.begin()+argidx+1, వాదనలు_2.చివర());
		}
	}

	ఒకవేళ (f == ఖాళీ)
		cmd_error(వాదనలు_2, argidx, "No filename given.");

	ఒకవేళ (called_with_fp)
		వాదనలు_2.నెట్టు_వెనుక(filename);
	వాదనలు_2[0] = pass_name;
	// cmd_log_args(args);
}

శూన్య Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::సదిశ<std::string> వాదనలు_2;
	అక్షరం *s = strdup(command.c_str());
	కొరకు (అక్షరం *p = strtok(s, " \t\r\n"); p; p = strtok(ఖాళీ, " \t\r\n"))
		వాదనలు_2.నెట్టు_వెనుక(p);
	విడుదల(s);
	frontend_call(design, f, filename, వాదనలు_2);
}

శూన్య Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::సదిశ<std::string> వాదనలు_2)
{
	ఒకవేళ (వాదనలు_2.కొలత() == 0)
		తిరిగి;
	ఒకవేళ (frontend_రిజిస్టరు.లెక్క(వాదనలు_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", వాదనలు_2[0]);

	ఒకవేళ (f != ఖాళీ) {
		స్వయంచాలక state = frontend_రిజిస్టరు[వాదనలు_2[0]]->pre_execute();
		frontend_రిజిస్టరు[వాదనలు_2[0]]->execute(f, filename, వాదనలు_2, design);
		frontend_రిజిస్టరు[వాదనలు_2[0]]->post_execute(state);
	} లేకపోతే ఒకవేళ (filename == "-") {
		std::istream *f_cin = &std::లోపలికి;
		స్వయంచాలక state = frontend_రిజిస్టరు[వాదనలు_2[0]]->pre_execute();
		frontend_రిజిస్టరు[వాదనలు_2[0]]->execute(f_cin, "<stdin>", వాదనలు_2, design);
		frontend_రిజిస్టరు[వాదనలు_2[0]]->post_execute(state);
	} లేకపోతే {
		ఒకవేళ (!filename.ఖాళీలు())
			వాదనలు_2.నెట్టు_వెనుక(filename);
		frontend_రిజిస్టరు[వాదనలు_2[0]]->execute(వాదనలు_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, ఆధారం_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

శూన్య Backend::నిరంతరం_రిజిస్టరు()
{
	ఒకవేళ (pass_రిజిస్టరు.లెక్క(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_రిజిస్టరు[pass_name] = ఇది;

	ఒకవేళ (backend_రిజిస్టరు.లెక్క(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_రిజిస్టరు[backend_name] = ఇది;
}

Backend::~Backend()
{
}

శూన్య Backend::execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design *design)
{
	std::ostream *f = ఖాళీ;
	స్వయంచాలక state = pre_execute();
	execute(f, std::string(), వాదనలు_2, design);
	post_execute(state);
	ఒకవేళ (f != &std::బయటకు)
		తొలగించు f;
}

శూన్య Backend::extra_వాదనలు(std::ostream *&f, std::string &filename, std::సదిశ<std::string> వాదనలు_2, కొలత_t argidx, తార్కిక bin_output)
{
	తార్కిక called_with_fp = f != ఖాళీ;

	కొరకు (; argidx < వాదనలు_2.కొలత(); argidx++)
	{
		std::string arg = వాదనలు_2[argidx];

		ఒకవేళ (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(వాదనలు_2, argidx, "Unknown option or option in arguments.");
		ఒకవేళ (f != ఖాళీ)
			cmd_error(వాదనలు_2, argidx, "Extra filename argument in direct file mode.");

		ఒకవేళ (arg == "-") {
			filename = "<stdout>";
			f = &std::బయటకు;
			కొనసాగు;
		}

		filename = arg;
		rewrite_filename(filename);
		ఒకవేళ (filename.కొలత() > 3 && filename.compare(filename.కొలత()-3, std::string::npos, ".gz") == 0) {
#నిర్వచితమైతే YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = కొత్త gzip_ostream;
			ఒకవేళ (!gf->open(filename)) {
				తొలగించు gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.చొప్పించు(filename);
			f = gf;
#లేకపోతే
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#అయితే_ముగింపు
		} లేకపోతే {
			std::ofstream *ff = కొత్త std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ద్విమాన) : std::ofstream::trunc);
			yosys_output_files.చొప్పించు(filename);
			ఒకవేళ (ff->fail()) {
				తొలగించు ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	ఒకవేళ (called_with_fp)
		వాదనలు_2.నెట్టు_వెనుక(filename);
	వాదనలు_2[0] = pass_name;
	// cmd_log_args(args);

	ఒకవేళ (f == ఖాళీ) {
		filename = "<stdout>";
		f = &std::బయటకు;
	}
}

శూన్య Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::సదిశ<std::string> వాదనలు_2;
	అక్షరం *s = strdup(command.c_str());
	కొరకు (అక్షరం *p = strtok(s, " \t\r\n"); p; p = strtok(ఖాళీ, " \t\r\n"))
		వాదనలు_2.నెట్టు_వెనుక(p);
	విడుదల(s);
	backend_call(design, f, filename, వాదనలు_2);
}

శూన్య Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::సదిశ<std::string> వాదనలు_2)
{
	ఒకవేళ (వాదనలు_2.కొలత() == 0)
		తిరిగి;
	ఒకవేళ (backend_రిజిస్టరు.లెక్క(వాదనలు_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", వాదనలు_2[0]);

	కొలత_t orig_sel_దొంతర_pos = design->selection_దొంతర.కొలత();

	ఒకవేళ (f != ఖాళీ) {
		స్వయంచాలక state = backend_రిజిస్టరు[వాదనలు_2[0]]->pre_execute();
		backend_రిజిస్టరు[వాదనలు_2[0]]->execute(f, filename, వాదనలు_2, design);
		backend_రిజిస్టరు[వాదనలు_2[0]]->post_execute(state);
	} లేకపోతే ఒకవేళ (filename == "-") {
		std::ostream *f_cout = &std::బయటకు;
		స్వయంచాలక state = backend_రిజిస్టరు[వాదనలు_2[0]]->pre_execute();
		backend_రిజిస్టరు[వాదనలు_2[0]]->execute(f_cout, "<stdout>", వాదనలు_2, design);
		backend_రిజిస్టరు[వాదనలు_2[0]]->post_execute(state);
	} లేకపోతే {
		ఒకవేళ (!filename.ఖాళీలు())
			వాదనలు_2.నెట్టు_వెనుక(filename);
		backend_రిజిస్టరు[వాదనలు_2[0]]->execute(వాదనలు_2, design);
	}

	అయితే (design->selection_దొంతర.కొలత() > orig_sel_దొంతర_pos)
		design->తీయి_selection();
}

నిర్మాణం SimHelper {
	string name;
	అంతఃవరుస string filesafe_name() {
		ఒకవేళ (name.at(0) == '$')
			ఒకవేళ (name.at(1) == '_')
				తిరిగి "gate" + name.substr(1);
			లేకపోతే
				తిరిగి "word_" + name.substr(1);
		లేకపోతే
			తిరిగి name;
	}
	string title;
	string ports;
	string ఆధారం;
	string desc;
	string సంకేతం_2;
	string సమూహం;
	string ver;
	string ట్యాగులు;
};

స్థిర తార్కిక is_సంకేతం_getter(string name) {
	తిరిగి *(--(name.చివర())) == '+';
}

స్థిర string get_cell_name(string name) {
	తిరిగి is_సంకేతం_getter(name) ? name.substr(0, name.నిడివి()-1) : name;
}

స్థిర శూన్య log_warning_flags(Pass *pass) {
	తార్కిక has_warnings = అబద్ధం;
	స్థిరరాశి string name = pass->pass_name;
	ఒకవేళ (pass->experimental_flag) {
		ఒకవేళ (!has_warnings) సంవర్గమానం("\n");
		has_warnings = నిజం;
		సంవర్గమానం("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	ఒకవేళ (pass->అంతర్గత_flag) {
		ఒకవేళ (!has_warnings) సంవర్గమానం("\n");
		has_warnings = నిజం;
		సంవర్గమానం("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	ఒకవేళ (has_warnings)
		సంవర్గమానం("\n");
}

స్థిర నిర్మాణం CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#అన్తర్భావ "kernel/simlib_help.inc"
#అన్తర్భావ "kernel/simcells_help.inc"
		cell_help.క్రమీకరించు();
	}
	తార్కిక కలిగియుంది(string name) { తిరిగి cell_help.లెక్క(get_cell_name(name)) > 0; }
	SimHelper get(string name) { తిరిగి cell_help[get_cell_name(name)]; }
} cell_help_messages;

నిర్మాణం HelpPass : బహిరంగ Pass {
	HelpPass() : Pass("help", "display help messages") { }
	శూన్య help() override
	{
		సంవర్గమానం("\n");
		సంవర్గమానం("    help  ................  list all commands\n");
		సంవర్గమానం("    help <command>  ......  print help message for given command\n");
		సంవర్గమానం("    help -all  ...........  print complete command reference\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("    help -cells ..........  list all cell types\n");
		సంవర్గమానం("    help <celltype>  .....  print help message for given cell type\n");
		సంవర్గమానం("    help <celltype>+  ....  print verilog code for given cell type\n");
		సంవర్గమానం("\n");
	}
	తార్కిక dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.నమోదు_2("version", "Yosys command reference");
		json.నమోదు_2("generator", yosys_సంస్కరణ_str);

		తార్కిక raise_error = అబద్ధం;
		std::map<string, సదిశ<string>> groups;

		// get root path
		స్వయంచాలక this_మార్గం = std::filesystem::మార్గం_2(ఆధారం_location::current().file_name());
		స్వయంచాలక ఆధారం_మూలం = this_మార్గం.మాతృ_మార్గం().మాతృ_మార్గం();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		కొరకు (స్వయంచాలక &it : pass_రిజిస్టరు) {
			స్వయంచాలక name = it.మొదటి;
			స్వయంచాలక pass = it.రెండవ;
			స్వయంచాలక title = pass->short_help;

			స్వయంచాలక cmd_help = PrettyHelp();
			స్వయంచాలక has_pretty_help = pass->formatted_help();

			ఒకవేళ (!has_pretty_help) {
				గణన PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				ఆధారం_location null_ఆధారం;
				string current_బఫరు = "";
				స్వయంచాలక మూలం_listing_2 = cmd_help.get_మూలం();
				స్వయంచాలక current_listing = మూలం_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					స్వయంచాలక log_scope = logger().sink_scope();
					logger().కూడు_sink<ప్రవాహం_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				కొలత_t def_strip_లెక్క = 0;
				స్వయంచాలక current_state = PUState_none;
				స్వయంచాలక catch_verific = అబద్ధం;
				స్వయంచాలక blank_పంక్తులు = 2;
				కొరకు (string పంక్తి; std::getline(ss, పంక్తి, '\n');) {
					// find position of first non space character
					std::కొలత_t మొదటి_pos = పంక్తి.కనుగొను_మొదటి_not_యొక్క(" \t");
					std::కొలత_t చివరి_pos = పంక్తి.కనుగొను_చివరి_not_యొక్క(" \t");
					ఒకవేళ (మొదటి_pos == std::string::npos) {
						ఎంపిక (current_state)
						{
						స్థితి PUState_signature:
							మూలం_listing_2->usage(current_బఫరు, null_ఆధారం);
							current_listing = మూలం_listing_2;
							current_state = PUState_none;
							current_బఫరు = "";
							ఆపు;
						స్థితి PUState_none:
						స్థితి PUState_optionbody:
							blank_పంక్తులు += 1;
							ఆపు;
						అప్రమేయ:
							ఆపు;
						}
						// skip empty lines
						కొనసాగు;
					}

					// strip leading and trailing whitespace
					std::string stripped_పంక్తి = పంక్తి.substr(మొదటి_pos, చివరి_pos - మొదటి_pos +1);
					తార్కిక IsDefinition = stripped_పంక్తి[0] == '-';
					IsDefinition &= stripped_పంక్తి[1] != ' ' && stripped_పంక్తి[1] != '>';
					తార్కిక IsDedent = def_strip_లెక్క && మొదటి_pos < def_strip_లెక్క;
					తార్కిక IsIndent = def_strip_లెక్క < మొదటి_pos;

					// line looks like a signature
					తార్కిక IsSignature = stripped_పంక్తి.కనుగొను(name) == 0 && (stripped_పంక్తి.నిడివి() == name.నిడివి() || stripped_పంక్తి.at(name.కొలత()) == ' ');

					ఒకవేళ (IsSignature && మొదటి_pos <= 4 && (blank_పంక్తులు >= 2 || current_state == PUState_signature)) {
						ఒకవేళ (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_బఫరు, "none", null_ఆధారం);
							current_బఫరు = "";
						} లేకపోతే ఒకవేళ (current_state == PUState_signature) {
							మూలం_listing_2->usage(current_బఫరు, null_ఆధారం);
							current_బఫరు = "";
						} లేకపోతే ఒకవేళ (current_state == PUState_none && !current_బఫరు.ఖాళీలు()) {
							current_listing->codeblock(current_బఫరు, "none", null_ఆధారం);
							current_బఫరు = "";
						}
						current_listing = మూలం_listing_2;
						current_state = PUState_signature;
						def_strip_లెక్క = మొదటి_pos;
						catch_verific = అబద్ధం;
					} లేకపోతే ఒకవేళ (IsDedent) {
						def_strip_లెక్క = మొదటి_pos;
						ఒకవేళ (current_state == PUState_optionbody) {
							ఒకవేళ (!current_బఫరు.ఖాళీలు()) {
								current_listing->codeblock(current_బఫరు, "none", null_ఆధారం);
								current_బఫరు = "";
							}
							ఒకవేళ (IsIndent) {
								current_state = PUState_options;
								current_listing = మూలం_listing_2->వెనుక();
							} లేకపోతే {
								current_state = PUState_none;
								current_listing = మూలం_listing_2;
							}
						} లేకపోతే {
							current_state = PUState_none;
						}
					}

					ఒకవేళ (IsDefinition && !catch_verific && current_state != PUState_signature) {
						ఒకవేళ (!current_బఫరు.ఖాళీలు()) {
							current_listing->codeblock(current_బఫరు, "none", null_ఆధారం);
							current_బఫరు = "";
						}
						current_state = PUState_options;
						current_listing = మూలం_listing_2->open_option(stripped_పంక్తి, null_ఆధారం);
						def_strip_లెక్క = మొదటి_pos;
					} లేకపోతే {
						ఒకవేళ (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						ఒకవేళ (current_బఫరు.ఖాళీలు())
							current_బఫరు = stripped_పంక్తి;
						లేకపోతే ఒకవేళ (current_state == PUState_signature && IsIndent)
							current_బఫరు += stripped_పంక్తి;
						లేకపోతే ఒకవేళ (current_state == PUState_none) {
							current_బఫరు += (blank_పంక్తులు > 0 ? "\n\n" : "\n") + పంక్తి;
						} లేకపోతే
							current_బఫరు += (blank_పంక్తులు > 0 ? "\n\n" : "\n") + stripped_పంక్తి;
						ఒకవేళ (stripped_పంక్తి.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = నిజం;
					}
					blank_పంక్తులు = 0;
				}

				ఒకవేళ (!current_బఫరు.ఖాళీలు()) {
					ఒకవేళ (current_బఫరు.కొలత() > 64 && current_బఫరు.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_బఫరు.substr(0, 62), null_ఆధారం);
						current_listing->codeblock(current_బఫరు.substr(64), "yoscrypt", null_ఆధారం);
					} లేకపోతే
						current_listing->codeblock(current_బఫరు, "none", null_ఆధారం);
					current_బఫరు = "";
				}
			}

			// fix path
			string ఆధారం_file = pass->location.file_name();
			తార్కిక has_ఆధారం = ఆధారం_file.compare("unknown") != 0;
			std::filesystem::మార్గం_2 ఆధారం_మార్గం;
			స్వయంచాలక skip_ఆధారం_సమూహం = అబద్ధం;

			ఒకవేళ (has_ఆధారం) {
				ఆధారం_మార్గం = fixup_ఆధారం_మార్గం(pass->location.file_name(), ఆధారం_మూలం, &skip_ఆధారం_సమూహం);
				ఆధారం_file = ఆధారం_మార్గం.string();
			}

			// attempt auto group
			ఒకవేళ (!cmd_help.has_సమూహం()) {
				ఒకవేళ (pass->అంతర్గత_flag)
					cmd_help.సమూహం = "internal";
				లేకపోతే ఒకవేళ (ఆధారం_file.కనుగొను("backends/") == 0 || (!has_ఆధారం && name.కనుగొను("read_") == 0))
					cmd_help.సమూహం = "backends";
				లేకపోతే ఒకవేళ (ఆధారం_file.కనుగొను("frontends/") == 0 || (!has_ఆధారం && name.కనుగొను("write_") == 0))
					cmd_help.సమూహం = "frontends";
				లేకపోతే ఒకవేళ (has_ఆధారం) {
					ఒకవేళ (ఆధారం_మార్గం.has_మాతృ_మార్గం() && !skip_ఆధారం_సమూహం)
						cmd_help.సమూహం = ఆధారం_మార్గం.మాతృ_మార్గం().string();
				}
				// implicit !has_source
				లేకపోతే ఒకవేళ (name.కనుగొను("equiv") == 0)
					cmd_help.సమూహం = "passes/equiv";
				లేకపోతే ఒకవేళ (name.కనుగొను("fsm") == 0)
					cmd_help.సమూహం = "passes/fsm";
				లేకపోతే ఒకవేళ (name.కనుగొను("memory") == 0)
					cmd_help.సమూహం = "passes/memory";
				లేకపోతే ఒకవేళ (name.కనుగొను("opt") == 0)
					cmd_help.సమూహం = "passes/opt";
				లేకపోతే ఒకవేళ (name.కనుగొను("proc") == 0)
					cmd_help.సమూహం = "passes/proc";
			}

			ఒకవేళ (groups.లెక్క(cmd_help.సమూహం) == 0) {
				groups[cmd_help.సమూహం] = సదిశ<string>();
			}
			groups[cmd_help.సమూహం].నెట్టు_వెనుక(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.నమోదు_2("title", title);
			json.name("content"); json.begin_array();
			కొరకు (స్వయంచాలక &content : cmd_help)
				json.విలువ(content.గమ్యం_json(ఆధారం_మూలం));
			json.చివర_array();
			json.నమోదు_2("group", cmd_help.సమూహం);
			json.నమోదు_2("source_file", ఆధారం_file);
			json.నమోదు_2("source_line", pass->location.పంక్తి());
			json.నమోదు_2("source_func", pass->location.function_name());
			json.నమోదు_2("experimental_flag", pass->experimental_flag);
			json.నమోదు_2("internal_flag", pass->అంతర్గత_flag);
			json.చివర_object();
		}
		json.చివర_object();

		json.నమోదు_2("groups", groups);

		json.చివర_object();
		తిరిగి raise_error;
	}
	తార్కిక dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.నమోదు_2("version", "Yosys internal cells");
		json.నమోదు_2("generator", yosys_maybe_సంస్కరణ());

		dict<string, సదిశ<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_పట్టిక_Builder::CellInfo>> cells;

		// iterate over cells
		తార్కిక raise_error = అబద్ధం;
		కొరకు (స్వయంచాలక it : StaticCellTypes::builder.cells) {
			ఒకవేళ (!StaticCellTypes::categories.is_తెలిసిన(it.type))
				కొనసాగు;
			స్వయంచాలక name = ID::str(it.type);
			ఒకవేళ (cell_help_messages.కలిగియుంది(name)) {
				స్వయంచాలక cell_help = cell_help_messages.get(name);
				groups[cell_help.సమూహం].emplace_వెనుక(name);
				స్వయంచాలక cell_pair = pair<SimHelper, StaticCellTypes::Cell_పట్టిక_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} లేకపోతే {
				సంవర్గమానం("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= నిజం;
			}
		}
		కొరకు (స్వయంచాలక &it : cell_help_messages.cell_help) {
			ఒకవేళ (cells.లెక్క(it.మొదటి) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.మొదటి);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.క్రమీకరించు();
		కొరకు (స్వయంచాలక &it : groups) {
			json.name(it.మొదటి.c_str()); json.విలువ(it.రెండవ);
		}
		json.చివర_object();

		json.name("cells"); json.begin_object();
		cells.క్రమీకరించు();
		కొరకు (స్వయంచాలక &it : cells) {
			స్వయంచాలక ch = it.రెండవ.మొదటి;
			స్వయంచాలక ct = it.రెండవ.రెండవ;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.విలువ(ch.title);
			json.name("ports"); json.విలువ(ch.ports);
			json.name("source"); json.విలువ(ch.ఆధారం);
			json.name("desc"); json.విలువ(ch.desc);
			json.name("code"); json.విలువ(ch.సంకేతం_2);
			సదిశ<string> inputs, outputs;
			కొరకు (స్వయంచాలక &ఆదానం : ct.inputs)
				inputs.నెట్టు_వెనుక(ID::str(ఆదానం));
			json.name("inputs"); json.విలువ(inputs);
			కొరకు (స్వయంచాలక &output : ct.outputs)
				outputs.నెట్టు_వెనుక(ID::str(output));
			json.name("outputs"); json.విలువ(outputs);
			సదిశ<string> properties;
			// CellType properties
			ఒకవేళ (ct.features.is_evaluable) properties.నెట్టు_వెనుక("is_evaluable");
			ఒకవేళ (ct.features.is_combinatorial) properties.నెట్టు_వెనుక("is_combinatorial");
			ఒకవేళ (ct.features.is_synthesizable) properties.నెట్టు_వెనుక("is_synthesizable");
			// SimHelper properties
			కొలత_t చివరి = 0; కొలత_t తదుపరి = 0;
			అయితే ((తదుపరి = ch.ట్యాగులు.కనుగొను(", ", చివరి)) != string::npos) {
				properties.నెట్టు_వెనుక(ch.ట్యాగులు.substr(చివరి, తదుపరి-చివరి));
				చివరి = తదుపరి + 2;
			}
			స్వయంచాలక final_ట్యాగ్ = ch.ట్యాగులు.substr(చివరి);
			ఒకవేళ (final_ట్యాగ్.కొలత()) properties.నెట్టు_వెనుక(final_ట్యాగ్);
			json.name("properties"); json.విలువ(properties);
			json.చివర_object();
		}
		json.చివర_object();

		json.చివర_object();
		తిరిగి raise_error;
	}
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design*) override
	{
		ఒకవేళ (వాదనలు_2.కొలత() == 1) {
			సంవర్గమానం("\n");
			కొరకు (స్వయంచాలక &it : pass_రిజిస్టరు)
				సంవర్గమానం("    %-20s %s\n", it.మొదటి, it.రెండవ->short_help);
			సంవర్గమానం("\n");
			సంవర్గమానం("Type 'help <command>' for more information on a command.\n");
			సంవర్గమానం("Type 'help -cells' for a list of all cell types.\n");
			సంవర్గమానం("\n");
			తిరిగి;
		}

		ఒకవేళ (వాదనలు_2.కొలత() == 2) {
			ఒకవేళ (వాదనలు_2[1] == "-all") {
				కొరకు (స్వయంచాలక &it : pass_రిజిస్టరు) {
					సంవర్గమానం("\n");
					సంవర్గమానం("\n");
					సంవర్గమానం("%s  --  %s\n", it.మొదటి, it.రెండవ->short_help);
					std::string సందేశం;
					కొరకు (కొలత_t i = 0; i < it.మొదటి.కొలత() + it.రెండవ->short_help.కొలత() + 6; i++)
						సందేశం += '=';
					సంవర్గమానం("%s\n", సందేశం);
					it.రెండవ->help();
					log_warning_flags(it.రెండవ);
				}
			}
			లేకపోతే ఒకవేళ (వాదనలు_2[1] == "-cells") {
				సంవర్గమానం("\n");
				కొరకు (స్వయంచాలక &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.రెండవ;
					సంవర్గమానం("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				సంవర్గమానం("\n");
				సంవర్గమానం("Type 'help <cell_type>' for more information on a cell type.\n");
				సంవర్గమానం("\n");
				తిరిగి;
			}
			లేకపోతే ఒకవేళ (pass_రిజిస్టరు.లెక్క(వాదనలు_2[1])) {
				pass_రిజిస్టరు.at(వాదనలు_2[1])->help();
				log_warning_flags(pass_రిజిస్టరు.at(వాదనలు_2[1]));
			}
			లేకపోతే ఒకవేళ (cell_help_messages.కలిగియుంది(వాదనలు_2[1])) {
				స్వయంచాలక help_cell = cell_help_messages.get(వాదనలు_2[1]);
				ఒకవేళ (is_సంకేతం_getter(వాదనలు_2[1])) {
						సంవర్గమానం("\n");
						సంవర్గమానం("%s\n", help_cell.సంకేతం_2);
				} లేకపోతే {
					సంవర్గమానం("\n");
					సంవర్గమానం("    %s %s\n", help_cell.name, help_cell.ports);
					సంవర్గమానం("\n");
					ఒకవేళ (help_cell.ver == "2" || help_cell.ver == "2a") {
						ఒకవేళ (help_cell.title != "") సంవర్గమానం("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						కొరకు (std::string పంక్తి; std::getline(ss, పంక్తి, '\n');) {
							ఒకవేళ (పంక్తి != "::") సంవర్గమానం("%s\n", పంక్తి);
						}
					} లేకపోతే ఒకవేళ (help_cell.desc.నిడివి()) {
						సంవర్గమానం("%s\n", help_cell.desc);
					} లేకపోతే {
						సంవర్గమానం("No help message for this cell type found.\n");
					}
					సంవర్గమానం("\n");
					సంవర్గమానం("Run 'help %s+' to display the Verilog model for this cell type.\n", వాదనలు_2[1]);
					సంవర్గమానం("\n");
				}
			}
			లేకపోతే
				సంవర్గమానం("No such command or cell type: %s\n", వాదనలు_2[1]);
			తిరిగి;
		} లేకపోతే ఒకవేళ (వాదనలు_2.కొలత() == 3) {
			// this option is undocumented as it is for internal use only
			ఒకవేళ (వాదనలు_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				ఒకవేళ (!json.write_గమ్యం_file(వాదనలు_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", వాదనలు_2[2], strerror(errno));
				ఒకవేళ (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			లేకపోతే ఒకవేళ (వాదనలు_2[1] == "-dump-cells-json") {
				PrettyJson json;
				ఒకవేళ (!json.write_గమ్యం_file(వాదనలు_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", వాదనలు_2[2], strerror(errno));
				ఒకవేళ (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			లేకపోతే
				సంవర్గమానం("Unknown help command: `%s %s'\n", వాదనలు_2[1], వాదనలు_2[2]);
			తిరిగి;
		}

		help();
	}
} HelpPass;

నిర్మాణం EchoPass : బహిరంగ Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	శూన్య help() override
	{
		సంవర్గమానం("\n");
		సంవర్గమానం("    echo on\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("Print all commands to log before executing them.\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("    echo off\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("Do not print all commands to log before executing them. (default)\n");
		సంవర్గమానం("\n");
	}
	శూన్య execute(std::సదిశ<std::string> వాదనలు_2, RTLIL::Design*) override
	{
		ఒకవేళ (వాదనలు_2.కొలత() > 2)
			cmd_error(వాదనలు_2, 2, "Unexpected argument.");

		ఒకవేళ (వాదనలు_2.కొలత() == 2) {
			ఒకవేళ (వాదనలు_2[1] == "on")
				echo_బహుళకం = నిజం;
			లేకపోతే ఒకవేళ (వాదనలు_2[1] == "off")
				echo_బహుళకం = అబద్ధం;
			లేకపోతే
				cmd_error(వాదనలు_2, 1, "Unexpected argument.");
		}

		సంవర్గమానం("echo %s\n", echo_బహుళకం ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

నిర్మాణం MinisatSatSolver : బహిరంగ SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = ఇది;
	}
	ezSAT *create() override {
		తిరిగి కొత్త ezMiniSAT();
	}
} MinisatSatSolver;

నిర్మాణం LicensePass : బహిరంగ Pass {
	LicensePass() : Pass("license", "print license terms") { }
	శూన్య help() override
	{
		సంవర్గమానం("\n");
		సంవర్గమానం("    license\n");
		సంవర్గమానం("\n");
		సంవర్గమానం("This command produces the following notice.\n");
		notice();
	}
	శూన్య execute(std::సదిశ<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	శూన్య notice()
	{
		సంవర్గమానం("\n");
		సంవర్గమానం(" /----------------------------------------------------------------------------\\\n");
		సంవర్గమానం(" |                                                                            |\n");
		సంవర్గమానం(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		సంవర్గమానం(" |                                                                            |\n");
		సంవర్గమానం(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		సంవర్గమానం(" |                                                                            |\n");
		సంవర్గమానం(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		సంవర్గమానం(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		సంవర్గమానం(" |  copyright notice and this permission notice appear in all copies.         |\n");
		సంవర్గమానం(" |                                                                            |\n");
		సంవర్గమానం(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		సంవర్గమానం(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		సంవర్గమానం(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		సంవర్గమానం(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		సంవర్గమానం(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		సంవర్గమానం(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		సంవర్గమానం(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		సంవర్గమానం(" |                                                                            |\n");
		సంవర్గమానం(" \\----------------------------------------------------------------------------/\n");
		సంవర్గమానం("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_చివర

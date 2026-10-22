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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/satgen.h"
#അന്തര്ഭാവ "kernel/json.h"
#അന്തര്ഭാവ "kernel/gzip.h"
#അന്തര്ഭാവ "kernel/log_help.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <filesystem>

YOSYS_NAMESPACE_BEGIN

#define MAX_REG_എണ്ണം 1000

തർക്കം echo_mode = അസത്യം;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

തർക്കം GarbageCollectionGuard::is_enabled_ = സത്യം;

സ്ഥിതം തർക്കം garbage_collection_requested = അസത്യം;

ശൂന്യം request_garbage_collection()
{
	garbage_collection_requested = സത്യം;
}

ശൂന്യം try_collect_garbage()
{
	എങ്കിൽ (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		തിരികെ;
	garbage_collection_requested = അസത്യം;
	എങ്കിൽ (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = ഇത്;
	call_counter = 0;
	runtime_ns = 0;
}

ശൂന്യം Pass::run_register()
{
	എങ്കിൽ (pass_register.എണ്ണം(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ഇത്;
}

ശൂന്യം Pass::init_register()
{
	vector<Pass*> added_passes;
	ആകുമ്പോൾ (first_queued_pass) {
		added_passes.തള്ളുക_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	വേണ്ടി (auto added_pass : added_passes)
		added_pass->on_register();
}

ശൂന്യം Pass::done_register()
{
	വേണ്ടി (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == ശൂന്യ);
}

ശൂന്യം Pass::on_register()
{
}

ശൂന്യം Pass::on_shutdown()
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
	state.parent_pass = current_pass;
	current_pass = ഇത്;
	clear_flags();
	തിരികെ state;
}

ശൂന്യം Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

ശൂന്യം Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	എങ്കിൽ (current_pass)
		current_pass->runtime_ns -= time_ns;
}

ശൂന്യം Pass::help()
{
	auto prettyHelp = PrettyHelp();
	എങ്കിൽ (formatted_help()) {
		prettyHelp.log_help();
	} അല്ലെങ്കിൽ {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

തർക്കം Pass::formatted_help()
{
	തിരികെ അസത്യം;
}

ശൂന്യം Pass::clear_flags()
{
}

ശൂന്യം Pass::cmd_log_args(സ്ഥിരം std::vector<std::string> &args)
{
	എങ്കിൽ (args.size() <= 1)
		തിരികെ;
	std::string message = "Full command line:";
	വേണ്ടി (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

ശൂന്യം Pass::cmd_error(സ്ഥിരം std::vector<std::string> &args, size_t argidx, std::string സന്ദേശം)
{
	std::string command_text;
	പൂർണ്ണസംഖ്യ error_pos = 0;

	വേണ്ടി (size_t i = 0; i < args.size(); i++) {
		എങ്കിൽ (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			സന്ദേശം.c_str(), command_text.c_str(), error_pos, "");
}

ശൂന്യം Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, തർക്കം select)
{
	വേണ്ടി (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		എങ്കിൽ (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		എങ്കിൽ (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(ഇത്, args, argidx, args.size(), design);
		നിർത്തുക;
	}
	// cmd_log_args(args);
}

ശൂന്യം Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", സത്യം);

	എങ്കിൽ (tok.empty())
		തിരികെ;

	എങ്കിൽ (tok[0] == '!') {
#എങ്കിൽ defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		ആകുമ്പോൾ (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		പൂർണ്ണസംഖ്യ retCode = run_command(cmd_buf);
		എങ്കിൽ (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		തിരികെ;
#അല്ലെങ്കിൽ
		log_cmd_error("Shell is not available.\n");
#endif
	}

	ആകുമ്പോൾ (!tok.empty()) {
		എങ്കിൽ (tok[0] == '#') {
			പൂർണ്ണസംഖ്യ stop;
			വേണ്ടി (stop = 0; stop < GetSize(cmd_buf); stop++)
				എങ്കിൽ (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					നിർത്തുക;
			cmd_buf = cmd_buf.substr(stop);
		} അല്ലെങ്കിൽ
		എങ്കിൽ (tok.back() == ';') {
			പൂർണ്ണസംഖ്യ അക്കം_semikolon = 0;
			ആകുമ്പോൾ (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), അക്കം_semikolon++;
			എങ്കിൽ (!tok.empty())
				args.തള്ളുക_back(tok);
			call(design, args);
			args.clear();
			എങ്കിൽ (അക്കം_semikolon == 2)
				call(design, "clean");
			എങ്കിൽ (അക്കം_semikolon == 3)
				call(design, "clean -purge");
		} അല്ലെങ്കിൽ
			args.തള്ളുക_back(tok);
		തർക്കം found_nl = അസത്യം;
		വേണ്ടി (auto c : cmd_buf) {
			എങ്കിൽ (c == ' ' || c == '\t')
				തുടരുക;
			എങ്കിൽ (c == '\r' || c == '\n')
				found_nl = സത്യം;
			നിർത്തുക;
		}
		എങ്കിൽ (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", സത്യം);
	}

	call(design, args);
}

ശൂന്യം Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	എങ്കിൽ (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		തിരികെ;

	എങ്കിൽ (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, അസത്യം));
		വേണ്ടി (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	എങ്കിൽ (pass_register.എണ്ണം(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	എങ്കിൽ (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_അട്ടി_pos = design->selection_അട്ടി.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	ആകുമ്പോൾ (design->selection_അട്ടി.size() > orig_sel_അട്ടി_pos)
		design->എടുക്കുക_selection();
}

ശൂന്യം Pass::call_on_selection(RTLIL::Design *design, സ്ഥിരം RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->തള്ളുക_selection(selection);

	Pass::call(design, command);

	design->എടുക്കുക_selection();
	design->selected_active_module = backup_selected_active_module;
}

ശൂന്യം Pass::call_on_selection(RTLIL::Design *design, സ്ഥിരം RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->തള്ളുക_selection(selection);

	Pass::call(design, args);

	design->എടുക്കുക_selection();
	design->selected_active_module = backup_selected_active_module;
}

ശൂന്യം Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->തള്ളുക_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->എടുക്കുക_selection();
	design->selected_active_module = backup_selected_active_module;
}

ശൂന്യം Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->തള്ളുക_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->എടുക്കുക_selection();
	design->selected_active_module = backup_selected_active_module;
}

തർക്കം ScriptPass::check_label(std::string label, std::string info)
{
	എങ്കിൽ (active_design == nullptr) {
		log("\n");
		എങ്കിൽ (info.empty())
			log("    %s:\n", label);
		അല്ലെങ്കിൽ
			log("    %s:    %s\n", label, info);
		തിരികെ സത്യം;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (label == active_run_from)
				block_active = സത്യം;
			എങ്കിൽ (label == active_run_to)
				block_active = അസത്യം;
		}
		തിരികെ block_active;
	}
}

ശൂന്യം ScriptPass::run(std::string command, std::string info)
{
	എങ്കിൽ (active_design == nullptr) {
		എങ്കിൽ (info.empty())
			log("        %s\n", command);
		അല്ലെങ്കിൽ
			log("        %s    %s\n", command, info);
	} അല്ലെങ്കിൽ {
		Pass::call(active_design, command);
		active_design->check();
	}
}

ശൂന്യം ScriptPass::run_nocheck(std::string command, std::string info)
{
	എങ്കിൽ (active_design == nullptr) {
		എങ്കിൽ (info.empty())
			log("        %s\n", command);
		അല്ലെങ്കിൽ
			log("        %s    %s\n", command, info);
	} അല്ലെങ്കിൽ {
		Pass::call(active_design, command);
	}
}

ശൂന്യം ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = അസത്യം;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

ശൂന്യം ScriptPass::help_script()
{
	clear_flags();
	help_mode = സത്യം;
	active_design = nullptr;
	block_active = സത്യം;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ശൂന്യം Frontend::run_register()
{
	എങ്കിൽ (pass_register.എണ്ണം(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ഇത്;

	എങ്കിൽ (frontend_register.എണ്ണം(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = ഇത്;
}

Frontend::~Frontend()
{
}

ശൂന്യം Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	ചെയ്യുക {
		std::istream *f = ശൂന്യ;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} ആകുമ്പോൾ (!args.empty());
}

FILE *Frontend::current_script_file = ശൂന്യ;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
പൂർണ്ണസംഖ്യ Frontend::current_script_lineno = 0;

ശൂന്യം Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, തർക്കം bin_നിവേശം)
{
	തർക്കം called_with_fp = f != ശൂന്യ;

	next_args.clear();

	എങ്കിൽ (argidx < args.size())
	{
		std::string arg = args[argidx];

		എങ്കിൽ (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		എങ്കിൽ (f != ശൂന്യ)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		എങ്കിൽ (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		എങ്കിൽ (filename.compare(0, 2, "<<") == 0) {
			എങ്കിൽ (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			ആകുമ്പോൾ (1) {
				std::string buffer;
				അക്ഷരം block[4096];
				ആകുമ്പോൾ (1) {
					എങ്കിൽ (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					എങ്കിൽ (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						നിർത്തുക;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				എങ്കിൽ (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					നിർത്തുക;
				last_here_document += buffer;
			}
			f = പുതിയ std::istringstream(last_here_document);
		} അല്ലെങ്കിൽ {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			എങ്കിൽ (GetSize(filenames) > 1) {
				next_args.തിരുകുക(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.തിരുകുക(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_നിവേശം_files.തിരുകുക(filename);
			f = uncompressed(filename, bin_നിവേശം ? std::ifstream::ദ്വിമാന : std::ifstream::in);
		}

		വേണ്ടി (size_t i = argidx+1; i < args.size(); i++)
			എങ്കിൽ (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		എങ്കിൽ (argidx+1 < args.size()) {
			എങ്കിൽ (next_args.empty())
				next_args.തിരുകുക(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.തിരുകുക(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	എങ്കിൽ (f == ശൂന്യ)
		cmd_error(args, argidx, "No filename given.");

	എങ്കിൽ (called_with_fp)
		args.തള്ളുക_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

ശൂന്യം Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	അക്ഷരം *s = strdup(command.c_str());
	വേണ്ടി (അക്ഷരം *p = strtok(s, " \t\r\n"); p; p = strtok(ശൂന്യ, " \t\r\n"))
		args.തള്ളുക_back(p);
	സ്വതന്ത്രം(s);
	frontend_call(design, f, filename, args);
}

ശൂന്യം Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	എങ്കിൽ (args.size() == 0)
		തിരികെ;
	എങ്കിൽ (frontend_register.എണ്ണം(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	എങ്കിൽ (f != ശൂന്യ) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} അല്ലെങ്കിൽ എങ്കിൽ (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!filename.empty())
			args.തള്ളുക_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ശൂന്യം Backend::run_register()
{
	എങ്കിൽ (pass_register.എണ്ണം(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ഇത്;

	എങ്കിൽ (backend_register.എണ്ണം(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = ഇത്;
}

Backend::~Backend()
{
}

ശൂന്യം Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = ശൂന്യ;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	എങ്കിൽ (f != &std::cout)
		delete f;
}

ശൂന്യം Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, തർക്കം bin_output)
{
	തർക്കം called_with_fp = f != ശൂന്യ;

	വേണ്ടി (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		എങ്കിൽ (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		എങ്കിൽ (f != ശൂന്യ)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		എങ്കിൽ (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			തുടരുക;
		}

		filename = arg;
		rewrite_filename(filename);
		എങ്കിൽ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = പുതിയ gzip_ostream;
			എങ്കിൽ (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.തിരുകുക(filename);
			f = gf;
#അല്ലെങ്കിൽ
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} അല്ലെങ്കിൽ {
			std::ofstream *ff = പുതിയ std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ദ്വിമാന) : std::ofstream::trunc);
			yosys_output_files.തിരുകുക(filename);
			എങ്കിൽ (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	എങ്കിൽ (called_with_fp)
		args.തള്ളുക_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	എങ്കിൽ (f == ശൂന്യ) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

ശൂന്യം Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	അക്ഷരം *s = strdup(command.c_str());
	വേണ്ടി (അക്ഷരം *p = strtok(s, " \t\r\n"); p; p = strtok(ശൂന്യ, " \t\r\n"))
		args.തള്ളുക_back(p);
	സ്വതന്ത്രം(s);
	backend_call(design, f, filename, args);
}

ശൂന്യം Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	എങ്കിൽ (args.size() == 0)
		തിരികെ;
	എങ്കിൽ (backend_register.എണ്ണം(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_അട്ടി_pos = design->selection_അട്ടി.size();

	എങ്കിൽ (f != ശൂന്യ) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} അല്ലെങ്കിൽ എങ്കിൽ (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (!filename.empty())
			args.തള്ളുക_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	ആകുമ്പോൾ (design->selection_അട്ടി.size() > orig_sel_അട്ടി_pos)
		design->എടുക്കുക_selection();
}

ഘടന SimHelper {
	string name;
	വരിയിൽ string filesafe_name() {
		എങ്കിൽ (name.at(0) == '$')
			എങ്കിൽ (name.at(1) == '_')
				തിരികെ "gate" + name.substr(1);
			അല്ലെങ്കിൽ
				തിരികെ "word_" + name.substr(1);
		അല്ലെങ്കിൽ
			തിരികെ name;
	}
	string title;
	string ports;
	string source;
	string desc;
	string code;
	string group;
	string ver;
	string tags;
};

സ്ഥിതം തർക്കം is_code_getter(string name) {
	തിരികെ *(--(name.end())) == '+';
}

സ്ഥിതം string get_cell_name(string name) {
	തിരികെ is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

സ്ഥിതം ശൂന്യം log_warning_flags(Pass *pass) {
	തർക്കം has_warnings = അസത്യം;
	സ്ഥിരം string name = pass->pass_name;
	എങ്കിൽ (pass->experimental_flag) {
		എങ്കിൽ (!has_warnings) log("\n");
		has_warnings = സത്യം;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	എങ്കിൽ (pass->internal_flag) {
		എങ്കിൽ (!has_warnings) log("\n");
		has_warnings = സത്യം;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	എങ്കിൽ (has_warnings)
		log("\n");
}

സ്ഥിതം ഘടന CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#അന്തര്ഭാവ "kernel/simlib_help.inc"
#അന്തര്ഭാവ "kernel/simcells_help.inc"
		cell_help.sort();
	}
	തർക്കം contains(string name) { തിരികെ cell_help.എണ്ണം(get_cell_name(name)) > 0; }
	SimHelper get(string name) { തിരികെ cell_help[get_cell_name(name)]; }
} cell_help_messages;

ഘടന HelpPass : പൊതു Pass {
	HelpPass() : Pass("help", "display help messages") { }
	ശൂന്യം help() override
	{
		log("\n");
		log("    help  ................  list all commands\n");
		log("    help <command>  ......  print help message for given command\n");
		log("    help -all  ...........  print complete command reference\n");
		log("\n");
		log("    help -cells ..........  list all cell types\n");
		log("    help <celltype>  .....  print help message for given cell type\n");
		log("    help <celltype>+  ....  print verilog code for given cell type\n");
		log("\n");
	}
	തർക്കം dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		തർക്കം raise_error = അസത്യം;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_വേര് = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		വേണ്ടി (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			എങ്കിൽ (!has_pretty_help) {
				എണ്ണൽ PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto വേര്_listing_2 = cmd_help.get_വേര്();
				auto current_listing = വേര്_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					auto log_scope = logger().sink_scope();
					logger().add_sink<StreamLogSink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				size_t def_strip_എണ്ണം = 0;
				auto current_state = PUState_none;
				auto catch_verific = അസത്യം;
				auto blank_വരികൾ = 2;
				വേണ്ടി (string വരി; std::getline(ss, വരി, '\n');) {
					// find position of first non space character
					std::size_t first_pos = വരി.find_first_not_of(" \t");
					std::size_t last_pos = വരി.find_last_not_of(" \t");
					എങ്കിൽ (first_pos == std::string::npos) {
						തിരഞ്ഞെടുപ്പ് (current_state)
						{
						അവസ്ഥ PUState_signature:
							വേര്_listing_2->usage(current_buffer, null_source);
							current_listing = വേര്_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							നിർത്തുക;
						അവസ്ഥ PUState_none:
						അവസ്ഥ PUState_optionbody:
							blank_വരികൾ += 1;
							നിർത്തുക;
						സ്ഥിരസ്ഥിതി:
							നിർത്തുക;
						}
						// skip empty lines
						തുടരുക;
					}

					// strip leading and trailing whitespace
					std::string stripped_വരി = വരി.substr(first_pos, last_pos - first_pos +1);
					തർക്കം IsDefinition = stripped_വരി[0] == '-';
					IsDefinition &= stripped_വരി[1] != ' ' && stripped_വരി[1] != '>';
					തർക്കം IsDedent = def_strip_എണ്ണം && first_pos < def_strip_എണ്ണം;
					തർക്കം IsIndent = def_strip_എണ്ണം < first_pos;

					// line looks like a signature
					തർക്കം IsSignature = stripped_വരി.find(name) == 0 && (stripped_വരി.length() == name.length() || stripped_വരി.at(name.size()) == ' ');

					എങ്കിൽ (IsSignature && first_pos <= 4 && (blank_വരികൾ >= 2 || current_state == PUState_signature)) {
						എങ്കിൽ (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} അല്ലെങ്കിൽ എങ്കിൽ (current_state == PUState_signature) {
							വേര്_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} അല്ലെങ്കിൽ എങ്കിൽ (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = വേര്_listing_2;
						current_state = PUState_signature;
						def_strip_എണ്ണം = first_pos;
						catch_verific = അസത്യം;
					} അല്ലെങ്കിൽ എങ്കിൽ (IsDedent) {
						def_strip_എണ്ണം = first_pos;
						എങ്കിൽ (current_state == PUState_optionbody) {
							എങ്കിൽ (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							എങ്കിൽ (IsIndent) {
								current_state = PUState_options;
								current_listing = വേര്_listing_2->back();
							} അല്ലെങ്കിൽ {
								current_state = PUState_none;
								current_listing = വേര്_listing_2;
							}
						} അല്ലെങ്കിൽ {
							current_state = PUState_none;
						}
					}

					എങ്കിൽ (IsDefinition && !catch_verific && current_state != PUState_signature) {
						എങ്കിൽ (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = വേര്_listing_2->open_option(stripped_വരി, null_source);
						def_strip_എണ്ണം = first_pos;
					} അല്ലെങ്കിൽ {
						എങ്കിൽ (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						എങ്കിൽ (current_buffer.empty())
							current_buffer = stripped_വരി;
						അല്ലെങ്കിൽ എങ്കിൽ (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_വരി;
						അല്ലെങ്കിൽ എങ്കിൽ (current_state == PUState_none) {
							current_buffer += (blank_വരികൾ > 0 ? "\n\n" : "\n") + വരി;
						} അല്ലെങ്കിൽ
							current_buffer += (blank_വരികൾ > 0 ? "\n\n" : "\n") + stripped_വരി;
						എങ്കിൽ (stripped_വരി.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = സത്യം;
					}
					blank_വരികൾ = 0;
				}

				എങ്കിൽ (!current_buffer.empty()) {
					എങ്കിൽ (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} അല്ലെങ്കിൽ
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			തർക്കം has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = അസത്യം;

			എങ്കിൽ (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_വേര്, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			എങ്കിൽ (!cmd_help.has_group()) {
				എങ്കിൽ (pass->internal_flag)
					cmd_help.group = "internal";
				അല്ലെങ്കിൽ എങ്കിൽ (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				അല്ലെങ്കിൽ എങ്കിൽ (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				അല്ലെങ്കിൽ എങ്കിൽ (has_source) {
					എങ്കിൽ (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				അല്ലെങ്കിൽ എങ്കിൽ (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				അല്ലെങ്കിൽ എങ്കിൽ (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				അല്ലെങ്കിൽ എങ്കിൽ (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				അല്ലെങ്കിൽ എങ്കിൽ (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				അല്ലെങ്കിൽ എങ്കിൽ (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			എങ്കിൽ (groups.എണ്ണം(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].തള്ളുക_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			വേണ്ടി (auto &content : cmd_help)
				json.മൂല്യം(content.to_json(source_വേര്));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.വരി());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		തിരികെ raise_error;
	}
	തർക്കം dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		തർക്കം raise_error = അസത്യം;
		വേണ്ടി (auto it : StaticCellTypes::builder.cells) {
			എങ്കിൽ (!StaticCellTypes::categories.is_known(it.type))
				തുടരുക;
			auto name = ID::str(it.type);
			എങ്കിൽ (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} അല്ലെങ്കിൽ {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= സത്യം;
			}
		}
		വേണ്ടി (auto &it : cell_help_messages.cell_help) {
			എങ്കിൽ (cells.എണ്ണം(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		വേണ്ടി (auto &it : groups) {
			json.name(it.first.c_str()); json.മൂല്യം(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		വേണ്ടി (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.മൂല്യം(ch.title);
			json.name("ports"); json.മൂല്യം(ch.ports);
			json.name("source"); json.മൂല്യം(ch.source);
			json.name("desc"); json.മൂല്യം(ch.desc);
			json.name("code"); json.മൂല്യം(ch.code);
			vector<string> inputs, outputs;
			വേണ്ടി (auto &നിവേശം : ct.inputs)
				inputs.തള്ളുക_back(ID::str(നിവേശം));
			json.name("inputs"); json.മൂല്യം(inputs);
			വേണ്ടി (auto &output : ct.outputs)
				outputs.തള്ളുക_back(ID::str(output));
			json.name("outputs"); json.മൂല്യം(outputs);
			vector<string> properties;
			// CellType properties
			എങ്കിൽ (ct.features.is_evaluable) properties.തള്ളുക_back("is_evaluable");
			എങ്കിൽ (ct.features.is_combinatorial) properties.തള്ളുക_back("is_combinatorial");
			എങ്കിൽ (ct.features.is_synthesizable) properties.തള്ളുക_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			ആകുമ്പോൾ ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.തള്ളുക_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			എങ്കിൽ (final_tag.size()) properties.തള്ളുക_back(final_tag);
			json.name("properties"); json.മൂല്യം(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		തിരികെ raise_error;
	}
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		എങ്കിൽ (args.size() == 1) {
			log("\n");
			വേണ്ടി (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			തിരികെ;
		}

		എങ്കിൽ (args.size() == 2) {
			എങ്കിൽ (args[1] == "-all") {
				വേണ്ടി (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string സന്ദേശം;
					വേണ്ടി (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						സന്ദേശം += '=';
					log("%s\n", സന്ദേശം);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			അല്ലെങ്കിൽ എങ്കിൽ (args[1] == "-cells") {
				log("\n");
				വേണ്ടി (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				തിരികെ;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (pass_register.എണ്ണം(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				എങ്കിൽ (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} അല്ലെങ്കിൽ {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					എങ്കിൽ (help_cell.ver == "2" || help_cell.ver == "2a") {
						എങ്കിൽ (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						വേണ്ടി (std::string വരി; std::getline(ss, വരി, '\n');) {
							എങ്കിൽ (വരി != "::") log("%s\n", വരി);
						}
					} അല്ലെങ്കിൽ എങ്കിൽ (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} അല്ലെങ്കിൽ {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			അല്ലെങ്കിൽ
				log("No such command or cell type: %s\n", args[1]);
			തിരികെ;
		} അല്ലെങ്കിൽ എങ്കിൽ (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			എങ്കിൽ (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				എങ്കിൽ (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				എങ്കിൽ (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			അല്ലെങ്കിൽ എങ്കിൽ (args[1] == "-dump-cells-json") {
				PrettyJson json;
				എങ്കിൽ (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				എങ്കിൽ (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			അല്ലെങ്കിൽ
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			തിരികെ;
		}

		help();
	}
} HelpPass;

ഘടന EchoPass : പൊതു Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	ശൂന്യം help() override
	{
		log("\n");
		log("    echo on\n");
		log("\n");
		log("Print all commands to log before executing them.\n");
		log("\n");
		log("\n");
		log("    echo off\n");
		log("\n");
		log("Do not print all commands to log before executing them. (default)\n");
		log("\n");
	}
	ശൂന്യം execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		എങ്കിൽ (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		എങ്കിൽ (args.size() == 2) {
			എങ്കിൽ (args[1] == "on")
				echo_mode = സത്യം;
			അല്ലെങ്കിൽ എങ്കിൽ (args[1] == "off")
				echo_mode = അസത്യം;
			അല്ലെങ്കിൽ
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ഘടന MinisatSatSolver : പൊതു SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = ഇത്;
	}
	ezSAT *create() override {
		തിരികെ പുതിയ ezMiniSAT();
	}
} MinisatSatSolver;

ഘടന LicensePass : പൊതു Pass {
	LicensePass() : Pass("license", "print license terms") { }
	ശൂന്യം help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	ശൂന്യം execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	ശൂന്യം notice()
	{
		log("\n");
		log(" /----------------------------------------------------------------------------\\\n");
		log(" |                                                                            |\n");
		log(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		log(" |                                                                            |\n");
		log(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		log(" |                                                                            |\n");
		log(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		log(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		log(" |  copyright notice and this permission notice appear in all copies.         |\n");
		log(" |                                                                            |\n");
		log(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		log(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		log(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		log(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		log(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		log(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		log(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		log(" |                                                                            |\n");
		log(" \\----------------------------------------------------------------------------/\n");
		log("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_END

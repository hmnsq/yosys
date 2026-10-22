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

#define MAX_REG_ಎಣಿಕೆ 1000

ತಾರ್ಕಿಕ echo_mode = ಅಸತ್ಯ;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

ತಾರ್ಕಿಕ GarbageCollectionGuard::is_enabled_ = ಸತ್ಯ;

ಸ್ಥಿರ ತಾರ್ಕಿಕ garbage_collection_requested = ಅಸತ್ಯ;

ಶೂನ್ಯ request_garbage_collection()
{
	garbage_collection_requested = ಸತ್ಯ;
}

ಶೂನ್ಯ try_collect_garbage()
{
	ಯದಿ (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		ಪ್ರತ್ಯಾಗಮ;
	garbage_collection_requested = ಅಸತ್ಯ;
	ಯದಿ (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = ಏತತ್;
	call_counter = 0;
	runtime_ns = 0;
}

ಶೂನ್ಯ Pass::run_register()
{
	ಯದಿ (pass_register.ಎಣಿಕೆ(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ಏತತ್;
}

ಶೂನ್ಯ Pass::init_register()
{
	vector<Pass*> added_passes;
	ಯಾವತ್ (first_queued_pass) {
		added_passes.ತಳ್ಳು_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	ಪ್ರತಿ (auto added_pass : added_passes)
		added_pass->on_register();
}

ಶೂನ್ಯ Pass::done_register()
{
	ಪ್ರತಿ (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == ರಿಕ್ತ);
}

ಶೂನ್ಯ Pass::on_register()
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
	state.parent_pass = current_pass;
	current_pass = ಏತತ್;
	clear_flags();
	ಪ್ರತ್ಯಾಗಮ state;
}

ಶೂನ್ಯ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

ಶೂನ್ಯ Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	ಯದಿ (current_pass)
		current_pass->runtime_ns -= time_ns;
}

ಶೂನ್ಯ Pass::help()
{
	auto prettyHelp = PrettyHelp();
	ಯದಿ (formatted_help()) {
		prettyHelp.log_help();
	} ಅನ್ಯಥಾ {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

ತಾರ್ಕಿಕ Pass::formatted_help()
{
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ Pass::clear_flags()
{
}

ಶೂನ್ಯ Pass::cmd_log_args(ಧ್ರುವ std::vector<std::string> &args)
{
	ಯದಿ (args.size() <= 1)
		ಪ್ರತ್ಯಾಗಮ;
	std::string message = "Full command line:";
	ಪ್ರತಿ (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

ಶೂನ್ಯ Pass::cmd_error(ಧ್ರುವ std::vector<std::string> &args, size_t argidx, std::string ಸಂದೇಶ)
{
	std::string command_text;
	ಪೂರ್ಣಾಙ್ಕ error_pos = 0;

	ಪ್ರತಿ (size_t i = 0; i < args.size(); i++) {
		ಯದಿ (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			ಸಂದೇಶ.c_str(), command_text.c_str(), error_pos, "");
}

ಶೂನ್ಯ Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, ತಾರ್ಕಿಕ select)
{
	ಪ್ರತಿ (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		ಯದಿ (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		ಯದಿ (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(ಏತತ್, args, argidx, args.size(), design);
		ಭಙ್ಗ;
	}
	// cmd_log_args(args);
}

ಶೂನ್ಯ Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", ಸತ್ಯ);

	ಯದಿ (tok.empty())
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (tok[0] == '!') {
#ಯದಿ defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		ಯಾವತ್ (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		ಪೂರ್ಣಾಙ್ಕ retCode = run_command(cmd_buf);
		ಯದಿ (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		ಪ್ರತ್ಯಾಗಮ;
#ಅನ್ಯಥಾ
		log_cmd_error("Shell is not available.\n");
#endif
	}

	ಯಾವತ್ (!tok.empty()) {
		ಯದಿ (tok[0] == '#') {
			ಪೂರ್ಣಾಙ್ಕ stop;
			ಪ್ರತಿ (stop = 0; stop < GetSize(cmd_buf); stop++)
				ಯದಿ (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					ಭಙ್ಗ;
			cmd_buf = cmd_buf.substr(stop);
		} ಅನ್ಯಥಾ
		ಯದಿ (tok.back() == ';') {
			ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_semikolon = 0;
			ಯಾವತ್ (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), ಅಂಕೆ_semikolon++;
			ಯದಿ (!tok.empty())
				args.ತಳ್ಳು_back(tok);
			call(design, args);
			args.clear();
			ಯದಿ (ಅಂಕೆ_semikolon == 2)
				call(design, "clean");
			ಯದಿ (ಅಂಕೆ_semikolon == 3)
				call(design, "clean -purge");
		} ಅನ್ಯಥಾ
			args.ತಳ್ಳು_back(tok);
		ತಾರ್ಕಿಕ found_nl = ಅಸತ್ಯ;
		ಪ್ರತಿ (auto c : cmd_buf) {
			ಯದಿ (c == ' ' || c == '\t')
				ಅನುವರ್ತ;
			ಯದಿ (c == '\r' || c == '\n')
				found_nl = ಸತ್ಯ;
			ಭಙ್ಗ;
		}
		ಯದಿ (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", ಸತ್ಯ);
	}

	call(design, args);
}

ಶೂನ್ಯ Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	ಯದಿ (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, ಅಸತ್ಯ));
		ಪ್ರತಿ (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	ಯದಿ (pass_register.ಎಣಿಕೆ(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	ಯದಿ (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_ರಾಶಿ_pos = design->selection_ರಾಶಿ.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	ಯಾವತ್ (design->selection_ರಾಶಿ.size() > orig_sel_ರಾಶಿ_pos)
		design->ತೆಗೆ_selection();
}

ಶೂನ್ಯ Pass::call_on_selection(RTLIL::Design *design, ಧ್ರುವ RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ತಳ್ಳು_selection(selection);

	Pass::call(design, command);

	design->ತೆಗೆ_selection();
	design->selected_active_module = backup_selected_active_module;
}

ಶೂನ್ಯ Pass::call_on_selection(RTLIL::Design *design, ಧ್ರುವ RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ತಳ್ಳು_selection(selection);

	Pass::call(design, args);

	design->ತೆಗೆ_selection();
	design->selected_active_module = backup_selected_active_module;
}

ಶೂನ್ಯ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ತಳ್ಳು_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->ತೆಗೆ_selection();
	design->selected_active_module = backup_selected_active_module;
}

ಶೂನ್ಯ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ತಳ್ಳು_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->ತೆಗೆ_selection();
	design->selected_active_module = backup_selected_active_module;
}

ತಾರ್ಕಿಕ ScriptPass::check_label(std::string label, std::string info)
{
	ಯದಿ (active_design == nullptr) {
		log("\n");
		ಯದಿ (info.empty())
			log("    %s:\n", label);
		ಅನ್ಯಥಾ
			log("    %s:    %s\n", label, info);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	} ಅನ್ಯಥಾ {
		ಯದಿ (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} ಅನ್ಯಥಾ {
			ಯದಿ (label == active_run_from)
				block_active = ಸತ್ಯ;
			ಯದಿ (label == active_run_to)
				block_active = ಅಸತ್ಯ;
		}
		ಪ್ರತ್ಯಾಗಮ block_active;
	}
}

ಶೂನ್ಯ ScriptPass::run(std::string command, std::string info)
{
	ಯದಿ (active_design == nullptr) {
		ಯದಿ (info.empty())
			log("        %s\n", command);
		ಅನ್ಯಥಾ
			log("        %s    %s\n", command, info);
	} ಅನ್ಯಥಾ {
		Pass::call(active_design, command);
		active_design->check();
	}
}

ಶೂನ್ಯ ScriptPass::run_nocheck(std::string command, std::string info)
{
	ಯದಿ (active_design == nullptr) {
		ಯದಿ (info.empty())
			log("        %s\n", command);
		ಅನ್ಯಥಾ
			log("        %s    %s\n", command, info);
	} ಅನ್ಯಥಾ {
		Pass::call(active_design, command);
	}
}

ಶೂನ್ಯ ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = ಅಸತ್ಯ;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

ಶೂನ್ಯ ScriptPass::help_script()
{
	clear_flags();
	help_mode = ಸತ್ಯ;
	active_design = nullptr;
	block_active = ಸತ್ಯ;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ಶೂನ್ಯ Frontend::run_register()
{
	ಯದಿ (pass_register.ಎಣಿಕೆ(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ಏತತ್;

	ಯದಿ (frontend_register.ಎಣಿಕೆ(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = ಏತತ್;
}

Frontend::~Frontend()
{
}

ಶೂನ್ಯ Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	ಕುರು {
		std::istream *f = ರಿಕ್ತ;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} ಯಾವತ್ (!args.empty());
}

FILE *Frontend::current_script_file = ರಿಕ್ತ;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
ಪೂರ್ಣಾಙ್ಕ Frontend::current_script_lineno = 0;

ಶೂನ್ಯ Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, ತಾರ್ಕಿಕ bin_ಒಳಹರಿವು)
{
	ತಾರ್ಕಿಕ called_with_fp = f != ರಿಕ್ತ;

	next_args.clear();

	ಯದಿ (argidx < args.size())
	{
		std::string arg = args[argidx];

		ಯದಿ (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		ಯದಿ (f != ರಿಕ್ತ)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		ಯದಿ (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		ಯದಿ (filename.compare(0, 2, "<<") == 0) {
			ಯದಿ (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			ಯಾವತ್ (1) {
				std::string buffer;
				ವರ್ಣ block[4096];
				ಯಾವತ್ (1) {
					ಯದಿ (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					ಯದಿ (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						ಭಙ್ಗ;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				ಯದಿ (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					ಭಙ್ಗ;
				last_here_document += buffer;
			}
			f = ನೂತನ std::istringstream(last_here_document);
		} ಅನ್ಯಥಾ {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			ಯದಿ (GetSize(filenames) > 1) {
				next_args.ಸೇರಿಸು(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.ಸೇರಿಸು(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_ಒಳಹರಿವು_files.ಸೇರಿಸು(filename);
			f = uncompressed(filename, bin_ಒಳಹರಿವು ? std::ifstream::ದ್ವಿಮಾನ : std::ifstream::in);
		}

		ಪ್ರತಿ (size_t i = argidx+1; i < args.size(); i++)
			ಯದಿ (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		ಯದಿ (argidx+1 < args.size()) {
			ಯದಿ (next_args.empty())
				next_args.ಸೇರಿಸು(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.ಸೇರಿಸು(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	ಯದಿ (f == ರಿಕ್ತ)
		cmd_error(args, argidx, "No filename given.");

	ಯದಿ (called_with_fp)
		args.ತಳ್ಳು_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

ಶೂನ್ಯ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	ವರ್ಣ *s = strdup(command.c_str());
	ಪ್ರತಿ (ವರ್ಣ *p = strtok(s, " \t\r\n"); p; p = strtok(ರಿಕ್ತ, " \t\r\n"))
		args.ತಳ್ಳು_back(p);
	ಮುಕ್ತ(s);
	frontend_call(design, f, filename, args);
}

ಶೂನ್ಯ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	ಯದಿ (args.size() == 0)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (frontend_register.ಎಣಿಕೆ(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	ಯದಿ (f != ರಿಕ್ತ) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} ಅನ್ಯಥಾ ಯದಿ (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} ಅನ್ಯಥಾ {
		ಯದಿ (!filename.empty())
			args.ತಳ್ಳು_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ಶೂನ್ಯ Backend::run_register()
{
	ಯದಿ (pass_register.ಎಣಿಕೆ(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ಏತತ್;

	ಯದಿ (backend_register.ಎಣಿಕೆ(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = ಏತತ್;
}

Backend::~Backend()
{
}

ಶೂನ್ಯ Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = ರಿಕ್ತ;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	ಯದಿ (f != &std::cout)
		delete f;
}

ಶೂನ್ಯ Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, ತಾರ್ಕಿಕ bin_output)
{
	ತಾರ್ಕಿಕ called_with_fp = f != ರಿಕ್ತ;

	ಪ್ರತಿ (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		ಯದಿ (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		ಯದಿ (f != ರಿಕ್ತ)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		ಯದಿ (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			ಅನುವರ್ತ;
		}

		filename = arg;
		rewrite_filename(filename);
		ಯದಿ (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = ನೂತನ gzip_ostream;
			ಯದಿ (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ಸೇರಿಸು(filename);
			f = gf;
#ಅನ್ಯಥಾ
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} ಅನ್ಯಥಾ {
			std::ofstream *ff = ನೂತನ std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ದ್ವಿಮಾನ) : std::ofstream::trunc);
			yosys_output_files.ಸೇರಿಸು(filename);
			ಯದಿ (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	ಯದಿ (called_with_fp)
		args.ತಳ್ಳು_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	ಯದಿ (f == ರಿಕ್ತ) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

ಶೂನ್ಯ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	ವರ್ಣ *s = strdup(command.c_str());
	ಪ್ರತಿ (ವರ್ಣ *p = strtok(s, " \t\r\n"); p; p = strtok(ರಿಕ್ತ, " \t\r\n"))
		args.ತಳ್ಳು_back(p);
	ಮುಕ್ತ(s);
	backend_call(design, f, filename, args);
}

ಶೂನ್ಯ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	ಯದಿ (args.size() == 0)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (backend_register.ಎಣಿಕೆ(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_ರಾಶಿ_pos = design->selection_ರಾಶಿ.size();

	ಯದಿ (f != ರಿಕ್ತ) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} ಅನ್ಯಥಾ ಯದಿ (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} ಅನ್ಯಥಾ {
		ಯದಿ (!filename.empty())
			args.ತಳ್ಳು_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	ಯಾವತ್ (design->selection_ರಾಶಿ.size() > orig_sel_ರಾಶಿ_pos)
		design->ತೆಗೆ_selection();
}

ಸಂರಚನಾ SimHelper {
	string name;
	ಅನ್ತಃಸ್ಥ string filesafe_name() {
		ಯದಿ (name.at(0) == '$')
			ಯದಿ (name.at(1) == '_')
				ಪ್ರತ್ಯಾಗಮ "gate" + name.substr(1);
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ "word_" + name.substr(1);
		ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ name;
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

ಸ್ಥಿರ ತಾರ್ಕಿಕ is_code_getter(string name) {
	ಪ್ರತ್ಯಾಗಮ *(--(name.end())) == '+';
}

ಸ್ಥಿರ string get_cell_name(string name) {
	ಪ್ರತ್ಯಾಗಮ is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_warning_flags(Pass *pass) {
	ತಾರ್ಕಿಕ has_warnings = ಅಸತ್ಯ;
	ಧ್ರುವ string name = pass->pass_name;
	ಯದಿ (pass->experimental_flag) {
		ಯದಿ (!has_warnings) log("\n");
		has_warnings = ಸತ್ಯ;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	ಯದಿ (pass->internal_flag) {
		ಯದಿ (!has_warnings) log("\n");
		has_warnings = ಸತ್ಯ;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	ಯದಿ (has_warnings)
		log("\n");
}

ಸ್ಥಿರ ಸಂರಚನಾ CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ಅನ್ತರ್ಭಾವ "kernel/simlib_help.inc"
#ಅನ್ತರ್ಭಾವ "kernel/simcells_help.inc"
		cell_help.sort();
	}
	ತಾರ್ಕಿಕ contains(string name) { ಪ್ರತ್ಯಾಗಮ cell_help.ಎಣಿಕೆ(get_cell_name(name)) > 0; }
	SimHelper get(string name) { ಪ್ರತ್ಯಾಗಮ cell_help[get_cell_name(name)]; }
} cell_help_messages;

ಸಂರಚನಾ HelpPass : ಸಾರ್ವಜನಿಕ Pass {
	HelpPass() : Pass("help", "display help messages") { }
	ಶೂನ್ಯ help() override
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
	ತಾರ್ಕಿಕ dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		ತಾರ್ಕಿಕ raise_error = ಅಸತ್ಯ;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_ಬೇರು = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		ಪ್ರತಿ (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			ಯದಿ (!has_pretty_help) {
				ಗಣನಾ PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto ಬೇರು_listing_2 = cmd_help.get_ಬೇರು();
				auto current_listing = ಬೇರು_listing_2;

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
				size_t def_strip_ಎಣಿಕೆ = 0;
				auto current_state = PUState_none;
				auto catch_verific = ಅಸತ್ಯ;
				auto blank_ಸಾಲುಗಳು = 2;
				ಪ್ರತಿ (string ಸಾಲು; std::getline(ss, ಸಾಲು, '\n');) {
					// find position of first non space character
					std::size_t first_pos = ಸಾಲು.find_first_not_of(" \t");
					std::size_t last_pos = ಸಾಲು.find_last_not_of(" \t");
					ಯದಿ (first_pos == std::string::npos) {
						ವಿಕಲ್ಪ (current_state)
						{
						ಪಕ್ಷ PUState_signature:
							ಬೇರು_listing_2->usage(current_buffer, null_source);
							current_listing = ಬೇರು_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							ಭಙ್ಗ;
						ಪಕ್ಷ PUState_none:
						ಪಕ್ಷ PUState_optionbody:
							blank_ಸಾಲುಗಳು += 1;
							ಭಙ್ಗ;
						ಸ್ವಭಾವ:
							ಭಙ್ಗ;
						}
						// skip empty lines
						ಅನುವರ್ತ;
					}

					// strip leading and trailing whitespace
					std::string stripped_ಸಾಲು = ಸಾಲು.substr(first_pos, last_pos - first_pos +1);
					ತಾರ್ಕಿಕ IsDefinition = stripped_ಸಾಲು[0] == '-';
					IsDefinition &= stripped_ಸಾಲು[1] != ' ' && stripped_ಸಾಲು[1] != '>';
					ತಾರ್ಕಿಕ IsDedent = def_strip_ಎಣಿಕೆ && first_pos < def_strip_ಎಣಿಕೆ;
					ತಾರ್ಕಿಕ IsIndent = def_strip_ಎಣಿಕೆ < first_pos;

					// line looks like a signature
					ತಾರ್ಕಿಕ IsSignature = stripped_ಸಾಲು.find(name) == 0 && (stripped_ಸಾಲು.length() == name.length() || stripped_ಸಾಲು.at(name.size()) == ' ');

					ಯದಿ (IsSignature && first_pos <= 4 && (blank_ಸಾಲುಗಳು >= 2 || current_state == PUState_signature)) {
						ಯದಿ (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} ಅನ್ಯಥಾ ಯದಿ (current_state == PUState_signature) {
							ಬೇರು_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} ಅನ್ಯಥಾ ಯದಿ (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = ಬೇರು_listing_2;
						current_state = PUState_signature;
						def_strip_ಎಣಿಕೆ = first_pos;
						catch_verific = ಅಸತ್ಯ;
					} ಅನ್ಯಥಾ ಯದಿ (IsDedent) {
						def_strip_ಎಣಿಕೆ = first_pos;
						ಯದಿ (current_state == PUState_optionbody) {
							ಯದಿ (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							ಯದಿ (IsIndent) {
								current_state = PUState_options;
								current_listing = ಬೇರು_listing_2->back();
							} ಅನ್ಯಥಾ {
								current_state = PUState_none;
								current_listing = ಬೇರು_listing_2;
							}
						} ಅನ್ಯಥಾ {
							current_state = PUState_none;
						}
					}

					ಯದಿ (IsDefinition && !catch_verific && current_state != PUState_signature) {
						ಯದಿ (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = ಬೇರು_listing_2->open_option(stripped_ಸಾಲು, null_source);
						def_strip_ಎಣಿಕೆ = first_pos;
					} ಅನ್ಯಥಾ {
						ಯದಿ (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						ಯದಿ (current_buffer.empty())
							current_buffer = stripped_ಸಾಲು;
						ಅನ್ಯಥಾ ಯದಿ (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_ಸಾಲು;
						ಅನ್ಯಥಾ ಯದಿ (current_state == PUState_none) {
							current_buffer += (blank_ಸಾಲುಗಳು > 0 ? "\n\n" : "\n") + ಸಾಲು;
						} ಅನ್ಯಥಾ
							current_buffer += (blank_ಸಾಲುಗಳು > 0 ? "\n\n" : "\n") + stripped_ಸಾಲು;
						ಯದಿ (stripped_ಸಾಲು.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = ಸತ್ಯ;
					}
					blank_ಸಾಲುಗಳು = 0;
				}

				ಯದಿ (!current_buffer.empty()) {
					ಯದಿ (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} ಅನ್ಯಥಾ
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			ತಾರ್ಕಿಕ has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = ಅಸತ್ಯ;

			ಯದಿ (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_ಬೇರು, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			ಯದಿ (!cmd_help.has_group()) {
				ಯದಿ (pass->internal_flag)
					cmd_help.group = "internal";
				ಅನ್ಯಥಾ ಯದಿ (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				ಅನ್ಯಥಾ ಯದಿ (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				ಅನ್ಯಥಾ ಯದಿ (has_source) {
					ಯದಿ (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				ಅನ್ಯಥಾ ಯದಿ (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				ಅನ್ಯಥಾ ಯದಿ (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				ಅನ್ಯಥಾ ಯದಿ (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				ಅನ್ಯಥಾ ಯದಿ (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				ಅನ್ಯಥಾ ಯದಿ (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			ಯದಿ (groups.ಎಣಿಕೆ(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].ತಳ್ಳು_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			ಪ್ರತಿ (auto &content : cmd_help)
				json.ಮೌಲ್ಯ(content.to_json(source_ಬೇರು));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.ಸಾಲು());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		ಪ್ರತ್ಯಾಗಮ raise_error;
	}
	ತಾರ್ಕಿಕ dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		ತಾರ್ಕಿಕ raise_error = ಅಸತ್ಯ;
		ಪ್ರತಿ (auto it : StaticCellTypes::builder.cells) {
			ಯದಿ (!StaticCellTypes::categories.is_known(it.type))
				ಅನುವರ್ತ;
			auto name = ID::str(it.type);
			ಯದಿ (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} ಅನ್ಯಥಾ {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= ಸತ್ಯ;
			}
		}
		ಪ್ರತಿ (auto &it : cell_help_messages.cell_help) {
			ಯದಿ (cells.ಎಣಿಕೆ(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		ಪ್ರತಿ (auto &it : groups) {
			json.name(it.first.c_str()); json.ಮೌಲ್ಯ(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		ಪ್ರತಿ (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.ಮೌಲ್ಯ(ch.title);
			json.name("ports"); json.ಮೌಲ್ಯ(ch.ports);
			json.name("source"); json.ಮೌಲ್ಯ(ch.source);
			json.name("desc"); json.ಮೌಲ್ಯ(ch.desc);
			json.name("code"); json.ಮೌಲ್ಯ(ch.code);
			vector<string> inputs, outputs;
			ಪ್ರತಿ (auto &ಒಳಹರಿವು : ct.inputs)
				inputs.ತಳ್ಳು_back(ID::str(ಒಳಹರಿವು));
			json.name("inputs"); json.ಮೌಲ್ಯ(inputs);
			ಪ್ರತಿ (auto &output : ct.outputs)
				outputs.ತಳ್ಳು_back(ID::str(output));
			json.name("outputs"); json.ಮೌಲ್ಯ(outputs);
			vector<string> properties;
			// CellType properties
			ಯದಿ (ct.features.is_evaluable) properties.ತಳ್ಳು_back("is_evaluable");
			ಯದಿ (ct.features.is_combinatorial) properties.ತಳ್ಳು_back("is_combinatorial");
			ಯದಿ (ct.features.is_synthesizable) properties.ತಳ್ಳು_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			ಯಾವತ್ ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.ತಳ್ಳು_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			ಯದಿ (final_tag.size()) properties.ತಳ್ಳು_back(final_tag);
			json.name("properties"); json.ಮೌಲ್ಯ(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		ಪ್ರತ್ಯಾಗಮ raise_error;
	}
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		ಯದಿ (args.size() == 1) {
			log("\n");
			ಪ್ರತಿ (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			ಪ್ರತ್ಯಾಗಮ;
		}

		ಯದಿ (args.size() == 2) {
			ಯದಿ (args[1] == "-all") {
				ಪ್ರತಿ (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string ಸಂದೇಶ;
					ಪ್ರತಿ (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						ಸಂದೇಶ += '=';
					log("%s\n", ಸಂದೇಶ);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			ಅನ್ಯಥಾ ಯದಿ (args[1] == "-cells") {
				log("\n");
				ಪ್ರತಿ (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				ಪ್ರತ್ಯಾಗಮ;
			}
			ಅನ್ಯಥಾ ಯದಿ (pass_register.ಎಣಿಕೆ(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			ಅನ್ಯಥಾ ಯದಿ (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				ಯದಿ (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} ಅನ್ಯಥಾ {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					ಯದಿ (help_cell.ver == "2" || help_cell.ver == "2a") {
						ಯದಿ (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						ಪ್ರತಿ (std::string ಸಾಲು; std::getline(ss, ಸಾಲು, '\n');) {
							ಯದಿ (ಸಾಲು != "::") log("%s\n", ಸಾಲು);
						}
					} ಅನ್ಯಥಾ ಯದಿ (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} ಅನ್ಯಥಾ {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			ಅನ್ಯಥಾ
				log("No such command or cell type: %s\n", args[1]);
			ಪ್ರತ್ಯಾಗಮ;
		} ಅನ್ಯಥಾ ಯದಿ (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			ಯದಿ (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				ಯದಿ (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				ಯದಿ (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			ಅನ್ಯಥಾ ಯದಿ (args[1] == "-dump-cells-json") {
				PrettyJson json;
				ಯದಿ (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				ಯದಿ (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			ಅನ್ಯಥಾ
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			ಪ್ರತ್ಯಾಗಮ;
		}

		help();
	}
} HelpPass;

ಸಂರಚನಾ EchoPass : ಸಾರ್ವಜನಿಕ Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	ಶೂನ್ಯ help() override
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
	ಶೂನ್ಯ execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		ಯದಿ (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		ಯದಿ (args.size() == 2) {
			ಯದಿ (args[1] == "on")
				echo_mode = ಸತ್ಯ;
			ಅನ್ಯಥಾ ಯದಿ (args[1] == "off")
				echo_mode = ಅಸತ್ಯ;
			ಅನ್ಯಥಾ
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ಸಂರಚನಾ MinisatSatSolver : ಸಾರ್ವಜನಿಕ SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = ಏತತ್;
	}
	ezSAT *create() override {
		ಪ್ರತ್ಯಾಗಮ ನೂತನ ezMiniSAT();
	}
} MinisatSatSolver;

ಸಂರಚನಾ LicensePass : ಸಾರ್ವಜನಿಕ Pass {
	LicensePass() : Pass("license", "print license terms") { }
	ಶೂನ್ಯ help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	ಶೂನ್ಯ execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	ಶೂನ್ಯ notice()
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

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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/satgen.h"
#ଅନ୍ତର୍ଭାଵ "kernel/json.h"
#ଅନ୍ତର୍ଭାଵ "kernel/gzip.h"
#ଅନ୍ତର୍ଭାଵ "kernel/log_help.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <filesystem>

YOSYS_NAMESPACE_BEGIN

#define MAX_REG_ଗଣନା 1000

ତାର୍କିକ echo_mode = ମିଥ୍ୟା;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

ତାର୍କିକ GarbageCollectionGuard::is_enabled_ = ସତ୍ୟ;

ସ୍ଥିର ତାର୍କିକ garbage_collection_requested = ମିଥ୍ୟା;

ଶୂନ୍ୟ request_garbage_collection()
{
	garbage_collection_requested = ସତ୍ୟ;
}

ଶୂନ୍ୟ try_collect_garbage()
{
	ଯଦି (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		ଫେରନ୍ତୁ;
	garbage_collection_requested = ମିଥ୍ୟା;
	ଯଦି (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = ଏହି;
	call_counter = 0;
	runtime_ns = 0;
}

ଶୂନ୍ୟ Pass::run_register()
{
	ଯଦି (pass_register.ଗଣନା_2(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ଏହି;
}

ଶୂନ୍ୟ Pass::init_register()
{
	vector<Pass*> added_passes;
	ଯେତେବେଳେ (first_queued_pass) {
		added_passes.ଠେଲ_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	ପାଇଁ (auto added_pass : added_passes)
		added_pass->on_register();
}

ଶୂନ୍ୟ Pass::done_register()
{
	ପାଇଁ (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == ଖାଲି);
}

ଶୂନ୍ୟ Pass::on_register()
{
}

ଶୂନ୍ୟ Pass::on_shutdown()
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
	current_pass = ଏହି;
	clear_flags();
	ଫେରନ୍ତୁ state;
}

ଶୂନ୍ୟ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

ଶୂନ୍ୟ Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	ଯଦି (current_pass)
		current_pass->runtime_ns -= time_ns;
}

ଶୂନ୍ୟ Pass::help()
{
	auto prettyHelp = PrettyHelp();
	ଯଦି (formatted_help()) {
		prettyHelp.log_help();
	} ଅନ୍ୟଥା {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

ତାର୍କିକ Pass::formatted_help()
{
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Pass::clear_flags()
{
}

ଶୂନ୍ୟ Pass::cmd_log_args(ଅଚଳ std::vector<std::string> &args)
{
	ଯଦି (args.size() <= 1)
		ଫେରନ୍ତୁ;
	std::string message = "Full command line:";
	ପାଇଁ (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

ଶୂନ୍ୟ Pass::cmd_error(ଅଚଳ std::vector<std::string> &args, size_t argidx, std::string ସନ୍ଦେଶ)
{
	std::string command_text;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_pos = 0;

	ପାଇଁ (size_t i = 0; i < args.size(); i++) {
		ଯଦି (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			ସନ୍ଦେଶ.c_str(), command_text.c_str(), error_pos, "");
}

ଶୂନ୍ୟ Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, ତାର୍କିକ select)
{
	ପାଇଁ (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		ଯଦି (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		ଯଦି (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(ଏହି, args, argidx, args.size(), design);
		ଭାଙ୍ଗ;
	}
	// cmd_log_args(args);
}

ଶୂନ୍ୟ Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", ସତ୍ୟ);

	ଯଦି (tok.empty())
		ଫେରନ୍ତୁ;

	ଯଦି (tok[0] == '!') {
#ଯଦି defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		ଯେତେବେଳେ (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା retCode = run_command(cmd_buf);
		ଯଦି (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		ଫେରନ୍ତୁ;
#ଅନ୍ୟଥା
		log_cmd_error("Shell is not available.\n");
#endif
	}

	ଯେତେବେଳେ (!tok.empty()) {
		ଯଦି (tok[0] == '#') {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା stop;
			ପାଇଁ (stop = 0; stop < GetSize(cmd_buf); stop++)
				ଯଦି (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					ଭାଙ୍ଗ;
			cmd_buf = cmd_buf.substr(stop);
		} ଅନ୍ୟଥା
		ଯଦି (tok.back() == ';') {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_semikolon = 0;
			ଯେତେବେଳେ (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), ଅଙ୍କ_semikolon++;
			ଯଦି (!tok.empty())
				args.ଠେଲ_back(tok);
			call(design, args);
			args.clear();
			ଯଦି (ଅଙ୍କ_semikolon == 2)
				call(design, "clean");
			ଯଦି (ଅଙ୍କ_semikolon == 3)
				call(design, "clean -purge");
		} ଅନ୍ୟଥା
			args.ଠେଲ_back(tok);
		ତାର୍କିକ found_nl = ମିଥ୍ୟା;
		ପାଇଁ (auto c : cmd_buf) {
			ଯଦି (c == ' ' || c == '\t')
				ଜାରି;
			ଯଦି (c == '\r' || c == '\n')
				found_nl = ସତ୍ୟ;
			ଭାଙ୍ଗ;
		}
		ଯଦି (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", ସତ୍ୟ);
	}

	call(design, args);
}

ଶୂନ୍ୟ Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	ଯଦି (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		ଫେରନ୍ତୁ;

	ଯଦି (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, ମିଥ୍ୟା));
		ପାଇଁ (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	ଯଦି (pass_register.ଗଣନା_2(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	ଯଦି (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_ଗଦା_pos = design->selection_ଗଦା.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	ଯେତେବେଳେ (design->selection_ଗଦା.size() > orig_sel_ଗଦା_pos)
		design->କାଢ଼_selection();
}

ଶୂନ୍ୟ Pass::call_on_selection(RTLIL::Design *design, ଅଚଳ RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ଠେଲ_selection(selection);

	Pass::call(design, command);

	design->କାଢ଼_selection();
	design->selected_active_module = backup_selected_active_module;
}

ଶୂନ୍ୟ Pass::call_on_selection(RTLIL::Design *design, ଅଚଳ RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ଠେଲ_selection(selection);

	Pass::call(design, args);

	design->କାଢ଼_selection();
	design->selected_active_module = backup_selected_active_module;
}

ଶୂନ୍ୟ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ଠେଲ_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->କାଢ଼_selection();
	design->selected_active_module = backup_selected_active_module;
}

ଶୂନ୍ୟ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ଠେଲ_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->କାଢ଼_selection();
	design->selected_active_module = backup_selected_active_module;
}

ତାର୍କିକ ScriptPass::check_label(std::string label, std::string info)
{
	ଯଦି (active_design == nullptr) {
		log("\n");
		ଯଦି (info.empty())
			log("    %s:\n", label);
		ଅନ୍ୟଥା
			log("    %s:    %s\n", label, info);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	} ଅନ୍ୟଥା {
		ଯଦି (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} ଅନ୍ୟଥା {
			ଯଦି (label == active_run_from)
				block_active = ସତ୍ୟ;
			ଯଦି (label == active_run_to)
				block_active = ମିଥ୍ୟା;
		}
		ଫେରନ୍ତୁ block_active;
	}
}

ଶୂନ୍ୟ ScriptPass::run(std::string command, std::string info)
{
	ଯଦି (active_design == nullptr) {
		ଯଦି (info.empty())
			log("        %s\n", command);
		ଅନ୍ୟଥା
			log("        %s    %s\n", command, info);
	} ଅନ୍ୟଥା {
		Pass::call(active_design, command);
		active_design->check();
	}
}

ଶୂନ୍ୟ ScriptPass::run_nocheck(std::string command, std::string info)
{
	ଯଦି (active_design == nullptr) {
		ଯଦି (info.empty())
			log("        %s\n", command);
		ଅନ୍ୟଥା
			log("        %s    %s\n", command, info);
	} ଅନ୍ୟଥା {
		Pass::call(active_design, command);
	}
}

ଶୂନ୍ୟ ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = ମିଥ୍ୟା;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

ଶୂନ୍ୟ ScriptPass::help_script()
{
	clear_flags();
	help_mode = ସତ୍ୟ;
	active_design = nullptr;
	block_active = ସତ୍ୟ;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ଶୂନ୍ୟ Frontend::run_register()
{
	ଯଦି (pass_register.ଗଣନା_2(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ଏହି;

	ଯଦି (frontend_register.ଗଣନା_2(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = ଏହି;
}

Frontend::~Frontend()
{
}

ଶୂନ୍ୟ Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	କର {
		std::istream *f = ଖାଲି;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} ଯେତେବେଳେ (!args.empty());
}

FILE *Frontend::current_script_file = ଖାଲି;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Frontend::current_script_lineno = 0;

ଶୂନ୍ୟ Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, ତାର୍କିକ bin_ନିବେଶ)
{
	ତାର୍କିକ called_with_fp = f != ଖାଲି;

	next_args.clear();

	ଯଦି (argidx < args.size())
	{
		std::string arg = args[argidx];

		ଯଦି (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		ଯଦି (f != ଖାଲି)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		ଯଦି (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		ଯଦି (filename.compare(0, 2, "<<") == 0) {
			ଯଦି (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			ଯେତେବେଳେ (1) {
				std::string buffer;
				ଅକ୍ଷର block[4096];
				ଯେତେବେଳେ (1) {
					ଯଦି (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					ଯଦି (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						ଭାଙ୍ଗ;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				ଯଦି (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					ଭାଙ୍ଗ;
				last_here_document += buffer;
			}
			f = ନୂତନ std::istringstream(last_here_document);
		} ଅନ୍ୟଥା {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			ଯଦି (GetSize(filenames) > 1) {
				next_args.ଭର୍ତ୍ତି(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.ଭର୍ତ୍ତି(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_ନିବେଶ_files.ଭର୍ତ୍ତି(filename);
			f = uncompressed(filename, bin_ନିବେଶ ? std::ifstream::ଦ୍ୱିମିକ : std::ifstream::in);
		}

		ପାଇଁ (size_t i = argidx+1; i < args.size(); i++)
			ଯଦି (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		ଯଦି (argidx+1 < args.size()) {
			ଯଦି (next_args.empty())
				next_args.ଭର୍ତ୍ତି(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.ଭର୍ତ୍ତି(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	ଯଦି (f == ଖାଲି)
		cmd_error(args, argidx, "No filename given.");

	ଯଦି (called_with_fp)
		args.ଠେଲ_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

ଶୂନ୍ୟ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	ଅକ୍ଷର *s = strdup(command.c_str());
	ପାଇଁ (ଅକ୍ଷର *p = strtok(s, " \t\r\n"); p; p = strtok(ଖାଲି, " \t\r\n"))
		args.ଠେଲ_back(p);
	ମୁକ୍ତ(s);
	frontend_call(design, f, filename, args);
}

ଶୂନ୍ୟ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	ଯଦି (args.size() == 0)
		ଫେରନ୍ତୁ;
	ଯଦି (frontend_register.ଗଣନା_2(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	ଯଦି (f != ଖାଲି) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} ଅନ୍ୟଥା ଯଦି (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} ଅନ୍ୟଥା {
		ଯଦି (!filename.empty())
			args.ଠେଲ_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

ଶୂନ୍ୟ Backend::run_register()
{
	ଯଦି (pass_register.ଗଣନା_2(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = ଏହି;

	ଯଦି (backend_register.ଗଣନା_2(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = ଏହି;
}

Backend::~Backend()
{
}

ଶୂନ୍ୟ Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = ଖାଲି;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	ଯଦି (f != &std::cout)
		delete f;
}

ଶୂନ୍ୟ Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, ତାର୍କିକ bin_output)
{
	ତାର୍କିକ called_with_fp = f != ଖାଲି;

	ପାଇଁ (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		ଯଦି (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		ଯଦି (f != ଖାଲି)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		ଯଦି (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			ଜାରି;
		}

		filename = arg;
		rewrite_filename(filename);
		ଯଦି (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = ନୂତନ gzip_ostream;
			ଯଦି (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ଭର୍ତ୍ତି(filename);
			f = gf;
#ଅନ୍ୟଥା
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} ଅନ୍ୟଥା {
			std::ofstream *ff = ନୂତନ std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ଦ୍ୱିମିକ) : std::ofstream::trunc);
			yosys_output_files.ଭର୍ତ୍ତି(filename);
			ଯଦି (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	ଯଦି (called_with_fp)
		args.ଠେଲ_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	ଯଦି (f == ଖାଲି) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

ଶୂନ୍ୟ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	ଅକ୍ଷର *s = strdup(command.c_str());
	ପାଇଁ (ଅକ୍ଷର *p = strtok(s, " \t\r\n"); p; p = strtok(ଖାଲି, " \t\r\n"))
		args.ଠେଲ_back(p);
	ମୁକ୍ତ(s);
	backend_call(design, f, filename, args);
}

ଶୂନ୍ୟ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	ଯଦି (args.size() == 0)
		ଫେରନ୍ତୁ;
	ଯଦି (backend_register.ଗଣନା_2(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_ଗଦା_pos = design->selection_ଗଦା.size();

	ଯଦି (f != ଖାଲି) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} ଅନ୍ୟଥା ଯଦି (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} ଅନ୍ୟଥା {
		ଯଦି (!filename.empty())
			args.ଠେଲ_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	ଯେତେବେଳେ (design->selection_ଗଦା.size() > orig_sel_ଗଦା_pos)
		design->କାଢ଼_selection();
}

ସଂରଚନା SimHelper {
	string name;
	ଧାଡ଼ିଭିତରେ string filesafe_name() {
		ଯଦି (name.at(0) == '$')
			ଯଦି (name.at(1) == '_')
				ଫେରନ୍ତୁ "gate" + name.substr(1);
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ "word_" + name.substr(1);
		ଅନ୍ୟଥା
			ଫେରନ୍ତୁ name;
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

ସ୍ଥିର ତାର୍କିକ is_code_getter(string name) {
	ଫେରନ୍ତୁ *(--(name.end())) == '+';
}

ସ୍ଥିର string get_cell_name(string name) {
	ଫେରନ୍ତୁ is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

ସ୍ଥିର ଶୂନ୍ୟ log_warning_flags(Pass *pass) {
	ତାର୍କିକ has_warnings = ମିଥ୍ୟା;
	ଅଚଳ string name = pass->pass_name;
	ଯଦି (pass->experimental_flag) {
		ଯଦି (!has_warnings) log("\n");
		has_warnings = ସତ୍ୟ;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	ଯଦି (pass->internal_flag) {
		ଯଦି (!has_warnings) log("\n");
		has_warnings = ସତ୍ୟ;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	ଯଦି (has_warnings)
		log("\n");
}

ସ୍ଥିର ସଂରଚନା CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ଅନ୍ତର୍ଭାଵ "kernel/simlib_help.inc"
#ଅନ୍ତର୍ଭାଵ "kernel/simcells_help.inc"
		cell_help.sort();
	}
	ତାର୍କିକ contains(string name) { ଫେରନ୍ତୁ cell_help.ଗଣନା_2(get_cell_name(name)) > 0; }
	SimHelper get(string name) { ଫେରନ୍ତୁ cell_help[get_cell_name(name)]; }
} cell_help_messages;

ସଂରଚନା HelpPass : ସର୍ବସାଧାରଣ Pass {
	HelpPass() : Pass("help", "display help messages") { }
	ଶୂନ୍ୟ help() override
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
	ତାର୍କିକ dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		ତାର୍କିକ raise_error = ମିଥ୍ୟା;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_ମୂଳ = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		ପାଇଁ (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			ଯଦି (!has_pretty_help) {
				ଗଣନା PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto ମୂଳ_listing_2 = cmd_help.get_ମୂଳ();
				auto current_listing = ମୂଳ_listing_2;

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
				size_t def_strip_ଗଣନା = 0;
				auto current_state = PUState_none;
				auto catch_verific = ମିଥ୍ୟା;
				auto blank_ଧାଡ଼ିଗୁଡ଼ିକ = 2;
				ପାଇଁ (string ଧାଡ଼ି; std::getline(ss, ଧାଡ଼ି, '\n');) {
					// find position of first non space character
					std::size_t first_pos = ଧାଡ଼ି.find_first_not_of(" \t");
					std::size_t last_pos = ଧାଡ଼ି.find_last_not_of(" \t");
					ଯଦି (first_pos == std::string::npos) {
						ବାଛ (current_state)
						{
						ସ୍ଥିତି PUState_signature:
							ମୂଳ_listing_2->usage(current_buffer, null_source);
							current_listing = ମୂଳ_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							ଭାଙ୍ଗ;
						ସ୍ଥିତି PUState_none:
						ସ୍ଥିତି PUState_optionbody:
							blank_ଧାଡ଼ିଗୁଡ଼ିକ += 1;
							ଭାଙ୍ଗ;
						ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
							ଭାଙ୍ଗ;
						}
						// skip empty lines
						ଜାରି;
					}

					// strip leading and trailing whitespace
					std::string stripped_ଧାଡ଼ି = ଧାଡ଼ି.substr(first_pos, last_pos - first_pos +1);
					ତାର୍କିକ IsDefinition = stripped_ଧାଡ଼ି[0] == '-';
					IsDefinition &= stripped_ଧାଡ଼ି[1] != ' ' && stripped_ଧାଡ଼ି[1] != '>';
					ତାର୍କିକ IsDedent = def_strip_ଗଣନା && first_pos < def_strip_ଗଣନା;
					ତାର୍କିକ IsIndent = def_strip_ଗଣନା < first_pos;

					// line looks like a signature
					ତାର୍କିକ IsSignature = stripped_ଧାଡ଼ି.find(name) == 0 && (stripped_ଧାଡ଼ି.length() == name.length() || stripped_ଧାଡ଼ି.at(name.size()) == ' ');

					ଯଦି (IsSignature && first_pos <= 4 && (blank_ଧାଡ଼ିଗୁଡ଼ିକ >= 2 || current_state == PUState_signature)) {
						ଯଦି (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} ଅନ୍ୟଥା ଯଦି (current_state == PUState_signature) {
							ମୂଳ_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} ଅନ୍ୟଥା ଯଦି (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = ମୂଳ_listing_2;
						current_state = PUState_signature;
						def_strip_ଗଣନା = first_pos;
						catch_verific = ମିଥ୍ୟା;
					} ଅନ୍ୟଥା ଯଦି (IsDedent) {
						def_strip_ଗଣନା = first_pos;
						ଯଦି (current_state == PUState_optionbody) {
							ଯଦି (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							ଯଦି (IsIndent) {
								current_state = PUState_options;
								current_listing = ମୂଳ_listing_2->back();
							} ଅନ୍ୟଥା {
								current_state = PUState_none;
								current_listing = ମୂଳ_listing_2;
							}
						} ଅନ୍ୟଥା {
							current_state = PUState_none;
						}
					}

					ଯଦି (IsDefinition && !catch_verific && current_state != PUState_signature) {
						ଯଦି (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = ମୂଳ_listing_2->open_option(stripped_ଧାଡ଼ି, null_source);
						def_strip_ଗଣନା = first_pos;
					} ଅନ୍ୟଥା {
						ଯଦି (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						ଯଦି (current_buffer.empty())
							current_buffer = stripped_ଧାଡ଼ି;
						ଅନ୍ୟଥା ଯଦି (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_ଧାଡ଼ି;
						ଅନ୍ୟଥା ଯଦି (current_state == PUState_none) {
							current_buffer += (blank_ଧାଡ଼ିଗୁଡ଼ିକ > 0 ? "\n\n" : "\n") + ଧାଡ଼ି;
						} ଅନ୍ୟଥା
							current_buffer += (blank_ଧାଡ଼ିଗୁଡ଼ିକ > 0 ? "\n\n" : "\n") + stripped_ଧାଡ଼ି;
						ଯଦି (stripped_ଧାଡ଼ି.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = ସତ୍ୟ;
					}
					blank_ଧାଡ଼ିଗୁଡ଼ିକ = 0;
				}

				ଯଦି (!current_buffer.empty()) {
					ଯଦି (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} ଅନ୍ୟଥା
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			ତାର୍କିକ has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = ମିଥ୍ୟା;

			ଯଦି (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_ମୂଳ, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			ଯଦି (!cmd_help.has_group()) {
				ଯଦି (pass->internal_flag)
					cmd_help.group = "internal";
				ଅନ୍ୟଥା ଯଦି (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				ଅନ୍ୟଥା ଯଦି (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				ଅନ୍ୟଥା ଯଦି (has_source) {
					ଯଦି (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				ଅନ୍ୟଥା ଯଦି (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				ଅନ୍ୟଥା ଯଦି (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				ଅନ୍ୟଥା ଯଦି (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				ଅନ୍ୟଥା ଯଦି (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				ଅନ୍ୟଥା ଯଦି (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			ଯଦି (groups.ଗଣନା_2(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].ଠେଲ_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			ପାଇଁ (auto &content : cmd_help)
				json.ମୂଲ୍ୟ(content.to_json(source_ମୂଳ));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.ଧାଡ଼ି());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		ଫେରନ୍ତୁ raise_error;
	}
	ତାର୍କିକ dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		ତାର୍କିକ raise_error = ମିଥ୍ୟା;
		ପାଇଁ (auto it : StaticCellTypes::builder.cells) {
			ଯଦି (!StaticCellTypes::categories.is_known(it.type))
				ଜାରି;
			auto name = ID::str(it.type);
			ଯଦି (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} ଅନ୍ୟଥା {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= ସତ୍ୟ;
			}
		}
		ପାଇଁ (auto &it : cell_help_messages.cell_help) {
			ଯଦି (cells.ଗଣନା_2(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		ପାଇଁ (auto &it : groups) {
			json.name(it.first.c_str()); json.ମୂଲ୍ୟ(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		ପାଇଁ (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.ମୂଲ୍ୟ(ch.title);
			json.name("ports"); json.ମୂଲ୍ୟ(ch.ports);
			json.name("source"); json.ମୂଲ୍ୟ(ch.source);
			json.name("desc"); json.ମୂଲ୍ୟ(ch.desc);
			json.name("code"); json.ମୂଲ୍ୟ(ch.code);
			vector<string> inputs, outputs;
			ପାଇଁ (auto &ନିବେଶ : ct.inputs)
				inputs.ଠେଲ_back(ID::str(ନିବେଶ));
			json.name("inputs"); json.ମୂଲ୍ୟ(inputs);
			ପାଇଁ (auto &output : ct.outputs)
				outputs.ଠେଲ_back(ID::str(output));
			json.name("outputs"); json.ମୂଲ୍ୟ(outputs);
			vector<string> properties;
			// CellType properties
			ଯଦି (ct.features.is_evaluable) properties.ଠେଲ_back("is_evaluable");
			ଯଦି (ct.features.is_combinatorial) properties.ଠେଲ_back("is_combinatorial");
			ଯଦି (ct.features.is_synthesizable) properties.ଠେଲ_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			ଯେତେବେଳେ ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.ଠେଲ_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			ଯଦି (final_tag.size()) properties.ଠେଲ_back(final_tag);
			json.name("properties"); json.ମୂଲ୍ୟ(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		ଫେରନ୍ତୁ raise_error;
	}
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		ଯଦି (args.size() == 1) {
			log("\n");
			ପାଇଁ (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			ଫେରନ୍ତୁ;
		}

		ଯଦି (args.size() == 2) {
			ଯଦି (args[1] == "-all") {
				ପାଇଁ (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string ସନ୍ଦେଶ;
					ପାଇଁ (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						ସନ୍ଦେଶ += '=';
					log("%s\n", ସନ୍ଦେଶ);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			ଅନ୍ୟଥା ଯଦି (args[1] == "-cells") {
				log("\n");
				ପାଇଁ (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				ଫେରନ୍ତୁ;
			}
			ଅନ୍ୟଥା ଯଦି (pass_register.ଗଣନା_2(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			ଅନ୍ୟଥା ଯଦି (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				ଯଦି (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} ଅନ୍ୟଥା {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					ଯଦି (help_cell.ver == "2" || help_cell.ver == "2a") {
						ଯଦି (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						ପାଇଁ (std::string ଧାଡ଼ି; std::getline(ss, ଧାଡ଼ି, '\n');) {
							ଯଦି (ଧାଡ଼ି != "::") log("%s\n", ଧାଡ଼ି);
						}
					} ଅନ୍ୟଥା ଯଦି (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} ଅନ୍ୟଥା {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			ଅନ୍ୟଥା
				log("No such command or cell type: %s\n", args[1]);
			ଫେରନ୍ତୁ;
		} ଅନ୍ୟଥା ଯଦି (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			ଯଦି (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				ଯଦି (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				ଯଦି (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			ଅନ୍ୟଥା ଯଦି (args[1] == "-dump-cells-json") {
				PrettyJson json;
				ଯଦି (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				ଯଦି (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			ଅନ୍ୟଥା
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			ଫେରନ୍ତୁ;
		}

		help();
	}
} HelpPass;

ସଂରଚନା EchoPass : ସର୍ବସାଧାରଣ Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	ଶୂନ୍ୟ help() override
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
	ଶୂନ୍ୟ execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		ଯଦି (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		ଯଦି (args.size() == 2) {
			ଯଦି (args[1] == "on")
				echo_mode = ସତ୍ୟ;
			ଅନ୍ୟଥା ଯଦି (args[1] == "off")
				echo_mode = ମିଥ୍ୟା;
			ଅନ୍ୟଥା
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ସଂରଚନା MinisatSatSolver : ସର୍ବସାଧାରଣ SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = ଏହି;
	}
	ezSAT *create() override {
		ଫେରନ୍ତୁ ନୂତନ ezMiniSAT();
	}
} MinisatSatSolver;

ସଂରଚନା LicensePass : ସର୍ବସାଧାରଣ Pass {
	LicensePass() : Pass("license", "print license terms") { }
	ଶୂନ୍ୟ help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	ଶୂନ୍ୟ execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	ଶୂନ୍ୟ notice()
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

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

#define MAX_REG_سانى 1000

منطقي echo_mode = خطأ;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

منطقي GarbageCollectionGuard::is_enabled_ = true;

ساكن منطقي garbage_collection_requested = خطأ;

فراغ request_garbage_collection()
{
	garbage_collection_requested = true;
}

فراغ try_collect_garbage()
{
	إذا (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		أرجع;
	garbage_collection_requested = خطأ;
	إذا (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = هذا;
	call_counter = 0;
	runtime_ns = 0;
}

فراغ Pass::run_register()
{
	إذا (pass_register.سانى(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = هذا;
}

فراغ Pass::init_register()
{
	vector<Pass*> added_passes;
	طالما (first_queued_pass) {
		added_passes.ئىتتىرىش_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	لكل (auto added_pass : added_passes)
		added_pass->on_register();
}

فراغ Pass::done_register()
{
	لكل (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == عدم);
}

فراغ Pass::on_register()
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
	state.parent_pass = current_pass;
	current_pass = هذا;
	clear_flags();
	أرجع state;
}

فراغ Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

فراغ Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	إذا (current_pass)
		current_pass->runtime_ns -= time_ns;
}

فراغ Pass::help()
{
	auto prettyHelp = PrettyHelp();
	إذا (formatted_help()) {
		prettyHelp.log_help();
	} وإلا {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

منطقي Pass::formatted_help()
{
	أرجع خطأ;
}

فراغ Pass::clear_flags()
{
}

فراغ Pass::cmd_log_args(ثابت std::vector<std::string> &args)
{
	إذا (args.size() <= 1)
		أرجع;
	std::string message = "Full command line:";
	لكل (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

فراغ Pass::cmd_error(ثابت std::vector<std::string> &args, size_t argidx, std::string ئۇچۇر)
{
	std::string command_text;
	صحيح error_pos = 0;

	لكل (size_t i = 0; i < args.size(); i++) {
		إذا (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			ئۇچۇر.c_str(), command_text.c_str(), error_pos, "");
}

فراغ Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, منطقي select)
{
	لكل (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		إذا (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(هذا, args, argidx, args.size(), design);
		اكسر;
	}
	// cmd_log_args(args);
}

فراغ Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", true);

	إذا (tok.empty())
		أرجع;

	إذا (tok[0] == '!') {
#إذا defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		طالما (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		صحيح retCode = run_command(cmd_buf);
		إذا (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		أرجع;
#وإلا
		log_cmd_error("Shell is not available.\n");
#endif
	}

	طالما (!tok.empty()) {
		إذا (tok[0] == '#') {
			صحيح stop;
			لكل (stop = 0; stop < GetSize(cmd_buf); stop++)
				إذا (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					اكسر;
			cmd_buf = cmd_buf.substr(stop);
		} وإلا
		إذا (tok.back() == ';') {
			صحيح رەقەم_semikolon = 0;
			طالما (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), رەقەم_semikolon++;
			إذا (!tok.empty())
				args.ئىتتىرىش_back(tok);
			call(design, args);
			args.clear();
			إذا (رەقەم_semikolon == 2)
				call(design, "clean");
			إذا (رەقەم_semikolon == 3)
				call(design, "clean -purge");
		} وإلا
			args.ئىتتىرىش_back(tok);
		منطقي found_nl = خطأ;
		لكل (auto c : cmd_buf) {
			إذا (c == ' ' || c == '\t')
				استمر;
			إذا (c == '\r' || c == '\n')
				found_nl = true;
			اكسر;
		}
		إذا (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", true);
	}

	call(design, args);
}

فراغ Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	إذا (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		أرجع;

	إذا (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, خطأ));
		لكل (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	إذا (pass_register.سانى(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	إذا (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_دۆۋە_pos = design->selection_دۆۋە.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	طالما (design->selection_دۆۋە.size() > orig_sel_دۆۋە_pos)
		design->چىقىرىش_selection();
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ئىتتىرىش_selection(selection);

	Pass::call(design, command);

	design->چىقىرىش_selection();
	design->selected_active_module = backup_selected_active_module;
}

فراغ Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ئىتتىرىش_selection(selection);

	Pass::call(design, args);

	design->چىقىرىش_selection();
	design->selected_active_module = backup_selected_active_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ئىتتىرىش_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->چىقىرىش_selection();
	design->selected_active_module = backup_selected_active_module;
}

فراغ Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ئىتتىرىش_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->چىقىرىش_selection();
	design->selected_active_module = backup_selected_active_module;
}

منطقي ScriptPass::check_label(std::string label, std::string info)
{
	إذا (active_design == nullptr) {
		log("\n");
		إذا (info.empty())
			log("    %s:\n", label);
		وإلا
			log("    %s:    %s\n", label, info);
		أرجع true;
	} وإلا {
		إذا (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} وإلا {
			إذا (label == active_run_from)
				block_active = true;
			إذا (label == active_run_to)
				block_active = خطأ;
		}
		أرجع block_active;
	}
}

فراغ ScriptPass::run(std::string command, std::string info)
{
	إذا (active_design == nullptr) {
		إذا (info.empty())
			log("        %s\n", command);
		وإلا
			log("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(active_design, command);
		active_design->check();
	}
}

فراغ ScriptPass::run_nocheck(std::string command, std::string info)
{
	إذا (active_design == nullptr) {
		إذا (info.empty())
			log("        %s\n", command);
		وإلا
			log("        %s    %s\n", command, info);
	} وإلا {
		Pass::call(active_design, command);
	}
}

فراغ ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = خطأ;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

فراغ ScriptPass::help_script()
{
	clear_flags();
	help_mode = true;
	active_design = nullptr;
	block_active = true;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Frontend::run_register()
{
	إذا (pass_register.سانى(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = هذا;

	إذا (frontend_register.سانى(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = هذا;
}

Frontend::~Frontend()
{
}

فراغ Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	افعل {
		std::istream *f = عدم;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} طالما (!args.empty());
}

FILE *Frontend::current_script_file = عدم;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
صحيح Frontend::current_script_lineno = 0;

فراغ Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, منطقي bin_كىرگۈزۈش)
{
	منطقي called_with_fp = f != عدم;

	next_args.clear();

	إذا (argidx < args.size())
	{
		std::string arg = args[argidx];

		إذا (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		إذا (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		إذا (filename.compare(0, 2, "<<") == 0) {
			إذا (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			طالما (1) {
				std::string buffer;
				حرف block[4096];
				طالما (1) {
					إذا (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					إذا (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						اكسر;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				إذا (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					اكسر;
				last_here_document += buffer;
			}
			f = جديد std::istringstream(last_here_document);
		} وإلا {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			إذا (GetSize(filenames) > 1) {
				next_args.قىستۇرۇش(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.قىستۇرۇش(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_كىرگۈزۈش_files.قىستۇرۇش(filename);
			f = uncompressed(filename, bin_كىرگۈزۈش ? std::ifstream::ئىككىلىك : std::ifstream::in);
		}

		لكل (size_t i = argidx+1; i < args.size(); i++)
			إذا (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		إذا (argidx+1 < args.size()) {
			إذا (next_args.empty())
				next_args.قىستۇرۇش(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.قىستۇرۇش(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	إذا (f == عدم)
		cmd_error(args, argidx, "No filename given.");

	إذا (called_with_fp)
		args.ئىتتىرىش_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		args.ئىتتىرىش_back(p);
	حرر(s);
	frontend_call(design, f, filename, args);
}

فراغ Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	إذا (args.size() == 0)
		أرجع;
	إذا (frontend_register.سانى(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	إذا (f != عدم) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.empty())
			args.ئىتتىرىش_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

فراغ Backend::run_register()
{
	إذا (pass_register.سانى(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = هذا;

	إذا (backend_register.سانى(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = هذا;
}

Backend::~Backend()
{
}

فراغ Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = عدم;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	إذا (f != &std::cout)
		delete f;
}

فراغ Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, منطقي bin_output)
{
	منطقي called_with_fp = f != عدم;

	لكل (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		إذا (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		إذا (f != عدم)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		إذا (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			استمر;
		}

		filename = arg;
		rewrite_filename(filename);
		إذا (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = جديد gzip_ostream;
			إذا (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.قىستۇرۇش(filename);
			f = gf;
#وإلا
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} وإلا {
			std::ofstream *ff = جديد std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::ئىككىلىك) : std::ofstream::trunc);
			yosys_output_files.قىستۇرۇش(filename);
			إذا (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	إذا (called_with_fp)
		args.ئىتتىرىش_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	إذا (f == عدم) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	حرف *s = strdup(command.c_str());
	لكل (حرف *p = strtok(s, " \t\r\n"); p; p = strtok(عدم, " \t\r\n"))
		args.ئىتتىرىش_back(p);
	حرر(s);
	backend_call(design, f, filename, args);
}

فراغ Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	إذا (args.size() == 0)
		أرجع;
	إذا (backend_register.سانى(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_دۆۋە_pos = design->selection_دۆۋە.size();

	إذا (f != عدم) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} وإلا إذا (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} وإلا {
		إذا (!filename.empty())
			args.ئىتتىرىش_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	طالما (design->selection_دۆۋە.size() > orig_sel_دۆۋە_pos)
		design->چىقىرىش_selection();
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
	string source;
	string desc;
	string code;
	string group;
	string ver;
	string tags;
};

ساكن منطقي is_code_getter(string name) {
	أرجع *(--(name.end())) == '+';
}

ساكن string get_cell_name(string name) {
	أرجع is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

ساكن فراغ log_warning_flags(Pass *pass) {
	منطقي has_warnings = خطأ;
	ثابت string name = pass->pass_name;
	إذا (pass->experimental_flag) {
		إذا (!has_warnings) log("\n");
		has_warnings = true;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	إذا (pass->internal_flag) {
		إذا (!has_warnings) log("\n");
		has_warnings = true;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	إذا (has_warnings)
		log("\n");
}

ساكن بنية CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#ضمن "kernel/simlib_help.inc"
#ضمن "kernel/simcells_help.inc"
		cell_help.sort();
	}
	منطقي contains(string name) { أرجع cell_help.سانى(get_cell_name(name)) > 0; }
	SimHelper get(string name) { أرجع cell_help[get_cell_name(name)]; }
} cell_help_messages;

بنية HelpPass : عام Pass {
	HelpPass() : Pass("help", "display help messages") { }
	فراغ help() override
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
	منطقي dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		منطقي raise_error = خطأ;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_يىلتىز = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		لكل (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			إذا (!has_pretty_help) {
				تعداد PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto يىلتىز_listing_2 = cmd_help.get_يىلتىز();
				auto current_listing = يىلتىز_listing_2;

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
				size_t def_strip_سانى = 0;
				auto current_state = PUState_none;
				auto catch_verific = خطأ;
				auto blank_قۇرلار = 2;
				لكل (string قۇر; std::getline(ss, قۇر, '\n');) {
					// find position of first non space character
					std::size_t first_pos = قۇر.find_first_not_of(" \t");
					std::size_t last_pos = قۇر.find_last_not_of(" \t");
					إذا (first_pos == std::string::npos) {
						بدل (current_state)
						{
						حالة PUState_signature:
							يىلتىز_listing_2->usage(current_buffer, null_source);
							current_listing = يىلتىز_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							اكسر;
						حالة PUState_none:
						حالة PUState_optionbody:
							blank_قۇرلار += 1;
							اكسر;
						افتراضي:
							اكسر;
						}
						// skip empty lines
						استمر;
					}

					// strip leading and trailing whitespace
					std::string stripped_قۇر = قۇر.substr(first_pos, last_pos - first_pos +1);
					منطقي IsDefinition = stripped_قۇر[0] == '-';
					IsDefinition &= stripped_قۇر[1] != ' ' && stripped_قۇر[1] != '>';
					منطقي IsDedent = def_strip_سانى && first_pos < def_strip_سانى;
					منطقي IsIndent = def_strip_سانى < first_pos;

					// line looks like a signature
					منطقي IsSignature = stripped_قۇر.find(name) == 0 && (stripped_قۇر.length() == name.length() || stripped_قۇر.at(name.size()) == ' ');

					إذا (IsSignature && first_pos <= 4 && (blank_قۇرلار >= 2 || current_state == PUState_signature)) {
						إذا (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} وإلا إذا (current_state == PUState_signature) {
							يىلتىز_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} وإلا إذا (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = يىلتىز_listing_2;
						current_state = PUState_signature;
						def_strip_سانى = first_pos;
						catch_verific = خطأ;
					} وإلا إذا (IsDedent) {
						def_strip_سانى = first_pos;
						إذا (current_state == PUState_optionbody) {
							إذا (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							إذا (IsIndent) {
								current_state = PUState_options;
								current_listing = يىلتىز_listing_2->back();
							} وإلا {
								current_state = PUState_none;
								current_listing = يىلتىز_listing_2;
							}
						} وإلا {
							current_state = PUState_none;
						}
					}

					إذا (IsDefinition && !catch_verific && current_state != PUState_signature) {
						إذا (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = يىلتىز_listing_2->open_option(stripped_قۇر, null_source);
						def_strip_سانى = first_pos;
					} وإلا {
						إذا (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						إذا (current_buffer.empty())
							current_buffer = stripped_قۇر;
						وإلا إذا (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_قۇر;
						وإلا إذا (current_state == PUState_none) {
							current_buffer += (blank_قۇرلار > 0 ? "\n\n" : "\n") + قۇر;
						} وإلا
							current_buffer += (blank_قۇرلار > 0 ? "\n\n" : "\n") + stripped_قۇر;
						إذا (stripped_قۇر.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = true;
					}
					blank_قۇرلار = 0;
				}

				إذا (!current_buffer.empty()) {
					إذا (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} وإلا
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			منطقي has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = خطأ;

			إذا (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_يىلتىز, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			إذا (!cmd_help.has_group()) {
				إذا (pass->internal_flag)
					cmd_help.group = "internal";
				وإلا إذا (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				وإلا إذا (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				وإلا إذا (has_source) {
					إذا (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				وإلا إذا (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				وإلا إذا (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				وإلا إذا (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				وإلا إذا (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				وإلا إذا (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			إذا (groups.سانى(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].ئىتتىرىش_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			لكل (auto &content : cmd_help)
				json.قىممەت(content.to_json(source_يىلتىز));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.قۇر());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		أرجع raise_error;
	}
	منطقي dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		منطقي raise_error = خطأ;
		لكل (auto it : StaticCellTypes::builder.cells) {
			إذا (!StaticCellTypes::categories.is_known(it.type))
				استمر;
			auto name = ID::str(it.type);
			إذا (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} وإلا {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= true;
			}
		}
		لكل (auto &it : cell_help_messages.cell_help) {
			إذا (cells.سانى(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		لكل (auto &it : groups) {
			json.name(it.first.c_str()); json.قىممەت(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		لكل (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.قىممەت(ch.title);
			json.name("ports"); json.قىممەت(ch.ports);
			json.name("source"); json.قىممەت(ch.source);
			json.name("desc"); json.قىممەت(ch.desc);
			json.name("code"); json.قىممەت(ch.code);
			vector<string> inputs, outputs;
			لكل (auto &كىرگۈزۈش : ct.inputs)
				inputs.ئىتتىرىش_back(ID::str(كىرگۈزۈش));
			json.name("inputs"); json.قىممەت(inputs);
			لكل (auto &output : ct.outputs)
				outputs.ئىتتىرىش_back(ID::str(output));
			json.name("outputs"); json.قىممەت(outputs);
			vector<string> properties;
			// CellType properties
			إذا (ct.features.is_evaluable) properties.ئىتتىرىش_back("is_evaluable");
			إذا (ct.features.is_combinatorial) properties.ئىتتىرىش_back("is_combinatorial");
			إذا (ct.features.is_synthesizable) properties.ئىتتىرىش_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			طالما ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.ئىتتىرىش_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			إذا (final_tag.size()) properties.ئىتتىرىش_back(final_tag);
			json.name("properties"); json.قىممەت(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		أرجع raise_error;
	}
	فراغ execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		إذا (args.size() == 1) {
			log("\n");
			لكل (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			أرجع;
		}

		إذا (args.size() == 2) {
			إذا (args[1] == "-all") {
				لكل (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string ئۇچۇر;
					لكل (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						ئۇچۇر += '=';
					log("%s\n", ئۇچۇر);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			وإلا إذا (args[1] == "-cells") {
				log("\n");
				لكل (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				أرجع;
			}
			وإلا إذا (pass_register.سانى(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			وإلا إذا (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				إذا (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} وإلا {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					إذا (help_cell.ver == "2" || help_cell.ver == "2a") {
						إذا (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						لكل (std::string قۇر; std::getline(ss, قۇر, '\n');) {
							إذا (قۇر != "::") log("%s\n", قۇر);
						}
					} وإلا إذا (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} وإلا {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			وإلا
				log("No such command or cell type: %s\n", args[1]);
			أرجع;
		} وإلا إذا (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			إذا (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				إذا (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				إذا (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			وإلا إذا (args[1] == "-dump-cells-json") {
				PrettyJson json;
				إذا (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				إذا (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			وإلا
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			أرجع;
		}

		help();
	}
} HelpPass;

بنية EchoPass : عام Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	فراغ help() override
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
	فراغ execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		إذا (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		إذا (args.size() == 2) {
			إذا (args[1] == "on")
				echo_mode = true;
			وإلا إذا (args[1] == "off")
				echo_mode = خطأ;
			وإلا
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
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
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	فراغ execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	فراغ notice()
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

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

#define MAX_REG_شمار 1000

منطقی echo_mode = نادرست;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

منطقی GarbageCollectionGuard::is_enabled_ = درست;

ایستا منطقی garbage_collection_requested = نادرست;

تهی request_garbage_collection()
{
	garbage_collection_requested = درست;
}

تهی try_collect_garbage()
{
	اگر (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		بازگشت;
	garbage_collection_requested = نادرست;
	اگر (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = این;
	call_counter = 0;
	runtime_ns = 0;
}

تهی Pass::run_register()
{
	اگر (pass_register.شمار(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = این;
}

تهی Pass::init_register()
{
	vector<Pass*> added_passes;
	وقتی (first_queued_pass) {
		added_passes.افزودن_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	برای (auto added_pass : added_passes)
		added_pass->on_register();
}

تهی Pass::done_register()
{
	برای (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == پوچ);
}

تهی Pass::on_register()
{
}

تهی Pass::on_shutdown()
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
	current_pass = این;
	clear_flags();
	بازگشت state;
}

تهی Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

تهی Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	اگر (current_pass)
		current_pass->runtime_ns -= time_ns;
}

تهی Pass::help()
{
	auto prettyHelp = PrettyHelp();
	اگر (formatted_help()) {
		prettyHelp.log_help();
	} وگرنه {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

منطقی Pass::formatted_help()
{
	بازگشت نادرست;
}

تهی Pass::clear_flags()
{
}

تهی Pass::cmd_log_args(ثابت std::vector<std::string> &args)
{
	اگر (args.size() <= 1)
		بازگشت;
	std::string message = "Full command line:";
	برای (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

تهی Pass::cmd_error(ثابت std::vector<std::string> &args, size_t argidx, std::string پیام)
{
	std::string command_text;
	صحیح error_pos = 0;

	برای (size_t i = 0; i < args.size(); i++) {
		اگر (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			پیام.c_str(), command_text.c_str(), error_pos, "");
}

تهی Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, منطقی select)
{
	برای (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		اگر (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		اگر (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(این, args, argidx, args.size(), design);
		بشکن;
	}
	// cmd_log_args(args);
}

تهی Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", درست);

	اگر (tok.empty())
		بازگشت;

	اگر (tok[0] == '!') {
#اگر defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		وقتی (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		صحیح retCode = run_command(cmd_buf);
		اگر (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		بازگشت;
#وگرنه
		log_cmd_error("Shell is not available.\n");
#endif
	}

	وقتی (!tok.empty()) {
		اگر (tok[0] == '#') {
			صحیح stop;
			برای (stop = 0; stop < GetSize(cmd_buf); stop++)
				اگر (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					بشکن;
			cmd_buf = cmd_buf.substr(stop);
		} وگرنه
		اگر (tok.back() == ';') {
			صحیح رقم_semikolon = 0;
			وقتی (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), رقم_semikolon++;
			اگر (!tok.empty())
				args.افزودن_back(tok);
			call(design, args);
			args.clear();
			اگر (رقم_semikolon == 2)
				call(design, "clean");
			اگر (رقم_semikolon == 3)
				call(design, "clean -purge");
		} وگرنه
			args.افزودن_back(tok);
		منطقی found_nl = نادرست;
		برای (auto c : cmd_buf) {
			اگر (c == ' ' || c == '\t')
				ادامه;
			اگر (c == '\r' || c == '\n')
				found_nl = درست;
			بشکن;
		}
		اگر (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", درست);
	}

	call(design, args);
}

تهی Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	اگر (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		بازگشت;

	اگر (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, نادرست));
		برای (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	اگر (pass_register.شمار(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	اگر (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_پشته_pos = design->selection_پشته.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	وقتی (design->selection_پشته.size() > orig_sel_پشته_pos)
		design->برداشت_selection();
}

تهی Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->افزودن_selection(selection);

	Pass::call(design, command);

	design->برداشت_selection();
	design->selected_active_module = backup_selected_active_module;
}

تهی Pass::call_on_selection(RTLIL::Design *design, ثابت RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->افزودن_selection(selection);

	Pass::call(design, args);

	design->برداشت_selection();
	design->selected_active_module = backup_selected_active_module;
}

تهی Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->افزودن_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->برداشت_selection();
	design->selected_active_module = backup_selected_active_module;
}

تهی Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->افزودن_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->برداشت_selection();
	design->selected_active_module = backup_selected_active_module;
}

منطقی ScriptPass::check_label(std::string label, std::string info)
{
	اگر (active_design == nullptr) {
		log("\n");
		اگر (info.empty())
			log("    %s:\n", label);
		وگرنه
			log("    %s:    %s\n", label, info);
		بازگشت درست;
	} وگرنه {
		اگر (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} وگرنه {
			اگر (label == active_run_from)
				block_active = درست;
			اگر (label == active_run_to)
				block_active = نادرست;
		}
		بازگشت block_active;
	}
}

تهی ScriptPass::run(std::string command, std::string info)
{
	اگر (active_design == nullptr) {
		اگر (info.empty())
			log("        %s\n", command);
		وگرنه
			log("        %s    %s\n", command, info);
	} وگرنه {
		Pass::call(active_design, command);
		active_design->check();
	}
}

تهی ScriptPass::run_nocheck(std::string command, std::string info)
{
	اگر (active_design == nullptr) {
		اگر (info.empty())
			log("        %s\n", command);
		وگرنه
			log("        %s    %s\n", command, info);
	} وگرنه {
		Pass::call(active_design, command);
	}
}

تهی ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = نادرست;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

تهی ScriptPass::help_script()
{
	clear_flags();
	help_mode = درست;
	active_design = nullptr;
	block_active = درست;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

تهی Frontend::run_register()
{
	اگر (pass_register.شمار(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = این;

	اگر (frontend_register.شمار(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = این;
}

Frontend::~Frontend()
{
}

تهی Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	انجامبده {
		std::istream *f = پوچ;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} وقتی (!args.empty());
}

FILE *Frontend::current_script_file = پوچ;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
صحیح Frontend::current_script_lineno = 0;

تهی Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, منطقی bin_درون_داد)
{
	منطقی called_with_fp = f != پوچ;

	next_args.clear();

	اگر (argidx < args.size())
	{
		std::string arg = args[argidx];

		اگر (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		اگر (f != پوچ)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		اگر (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		اگر (filename.compare(0, 2, "<<") == 0) {
			اگر (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			وقتی (1) {
				std::string buffer;
				نویسه block[4096];
				وقتی (1) {
					اگر (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					اگر (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						بشکن;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				اگر (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					بشکن;
				last_here_document += buffer;
			}
			f = جدید std::istringstream(last_here_document);
		} وگرنه {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			اگر (GetSize(filenames) > 1) {
				next_args.درج(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.درج(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_درون_داد_files.درج(filename);
			f = uncompressed(filename, bin_درون_داد ? std::ifstream::دودویی : std::ifstream::in);
		}

		برای (size_t i = argidx+1; i < args.size(); i++)
			اگر (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		اگر (argidx+1 < args.size()) {
			اگر (next_args.empty())
				next_args.درج(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.درج(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	اگر (f == پوچ)
		cmd_error(args, argidx, "No filename given.");

	اگر (called_with_fp)
		args.افزودن_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

تهی Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	نویسه *s = strdup(command.c_str());
	برای (نویسه *p = strtok(s, " \t\r\n"); p; p = strtok(پوچ, " \t\r\n"))
		args.افزودن_back(p);
	آزاد(s);
	frontend_call(design, f, filename, args);
}

تهی Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	اگر (args.size() == 0)
		بازگشت;
	اگر (frontend_register.شمار(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	اگر (f != پوچ) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} وگرنه اگر (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} وگرنه {
		اگر (!filename.empty())
			args.افزودن_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

تهی Backend::run_register()
{
	اگر (pass_register.شمار(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = این;

	اگر (backend_register.شمار(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = این;
}

Backend::~Backend()
{
}

تهی Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = پوچ;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	اگر (f != &std::cout)
		delete f;
}

تهی Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, منطقی bin_output)
{
	منطقی called_with_fp = f != پوچ;

	برای (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		اگر (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		اگر (f != پوچ)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		اگر (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			ادامه;
		}

		filename = arg;
		rewrite_filename(filename);
		اگر (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = جدید gzip_ostream;
			اگر (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.درج(filename);
			f = gf;
#وگرنه
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} وگرنه {
			std::ofstream *ff = جدید std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::دودویی) : std::ofstream::trunc);
			yosys_output_files.درج(filename);
			اگر (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	اگر (called_with_fp)
		args.افزودن_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	اگر (f == پوچ) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

تهی Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	نویسه *s = strdup(command.c_str());
	برای (نویسه *p = strtok(s, " \t\r\n"); p; p = strtok(پوچ, " \t\r\n"))
		args.افزودن_back(p);
	آزاد(s);
	backend_call(design, f, filename, args);
}

تهی Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	اگر (args.size() == 0)
		بازگشت;
	اگر (backend_register.شمار(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_پشته_pos = design->selection_پشته.size();

	اگر (f != پوچ) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} وگرنه اگر (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} وگرنه {
		اگر (!filename.empty())
			args.افزودن_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	وقتی (design->selection_پشته.size() > orig_sel_پشته_pos)
		design->برداشت_selection();
}

ساختار SimHelper {
	string name;
	درونخطی string filesafe_name() {
		اگر (name.at(0) == '$')
			اگر (name.at(1) == '_')
				بازگشت "gate" + name.substr(1);
			وگرنه
				بازگشت "word_" + name.substr(1);
		وگرنه
			بازگشت name;
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

ایستا منطقی is_code_getter(string name) {
	بازگشت *(--(name.end())) == '+';
}

ایستا string get_cell_name(string name) {
	بازگشت is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

ایستا تهی log_warning_flags(Pass *pass) {
	منطقی has_warnings = نادرست;
	ثابت string name = pass->pass_name;
	اگر (pass->experimental_flag) {
		اگر (!has_warnings) log("\n");
		has_warnings = درست;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	اگر (pass->internal_flag) {
		اگر (!has_warnings) log("\n");
		has_warnings = درست;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	اگر (has_warnings)
		log("\n");
}

ایستا ساختار CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#شامل "kernel/simlib_help.inc"
#شامل "kernel/simcells_help.inc"
		cell_help.sort();
	}
	منطقی contains(string name) { بازگشت cell_help.شمار(get_cell_name(name)) > 0; }
	SimHelper get(string name) { بازگشت cell_help[get_cell_name(name)]; }
} cell_help_messages;

ساختار HelpPass : عمومی Pass {
	HelpPass() : Pass("help", "display help messages") { }
	تهی help() override
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
	منطقی dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		منطقی raise_error = نادرست;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_ریشه = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		برای (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			اگر (!has_pretty_help) {
				شمارش PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto ریشه_listing_2 = cmd_help.get_ریشه();
				auto current_listing = ریشه_listing_2;

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
				size_t def_strip_شمار = 0;
				auto current_state = PUState_none;
				auto catch_verific = نادرست;
				auto blank_سطرها = 2;
				برای (string سطر; std::getline(ss, سطر, '\n');) {
					// find position of first non space character
					std::size_t first_pos = سطر.find_first_not_of(" \t");
					std::size_t last_pos = سطر.find_last_not_of(" \t");
					اگر (first_pos == std::string::npos) {
						انتخاب (current_state)
						{
						حالت PUState_signature:
							ریشه_listing_2->usage(current_buffer, null_source);
							current_listing = ریشه_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							بشکن;
						حالت PUState_none:
						حالت PUState_optionbody:
							blank_سطرها += 1;
							بشکن;
						پیشفرض:
							بشکن;
						}
						// skip empty lines
						ادامه;
					}

					// strip leading and trailing whitespace
					std::string stripped_سطر = سطر.substr(first_pos, last_pos - first_pos +1);
					منطقی IsDefinition = stripped_سطر[0] == '-';
					IsDefinition &= stripped_سطر[1] != ' ' && stripped_سطر[1] != '>';
					منطقی IsDedent = def_strip_شمار && first_pos < def_strip_شمار;
					منطقی IsIndent = def_strip_شمار < first_pos;

					// line looks like a signature
					منطقی IsSignature = stripped_سطر.find(name) == 0 && (stripped_سطر.length() == name.length() || stripped_سطر.at(name.size()) == ' ');

					اگر (IsSignature && first_pos <= 4 && (blank_سطرها >= 2 || current_state == PUState_signature)) {
						اگر (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} وگرنه اگر (current_state == PUState_signature) {
							ریشه_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} وگرنه اگر (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = ریشه_listing_2;
						current_state = PUState_signature;
						def_strip_شمار = first_pos;
						catch_verific = نادرست;
					} وگرنه اگر (IsDedent) {
						def_strip_شمار = first_pos;
						اگر (current_state == PUState_optionbody) {
							اگر (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							اگر (IsIndent) {
								current_state = PUState_options;
								current_listing = ریشه_listing_2->back();
							} وگرنه {
								current_state = PUState_none;
								current_listing = ریشه_listing_2;
							}
						} وگرنه {
							current_state = PUState_none;
						}
					}

					اگر (IsDefinition && !catch_verific && current_state != PUState_signature) {
						اگر (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = ریشه_listing_2->open_option(stripped_سطر, null_source);
						def_strip_شمار = first_pos;
					} وگرنه {
						اگر (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						اگر (current_buffer.empty())
							current_buffer = stripped_سطر;
						وگرنه اگر (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_سطر;
						وگرنه اگر (current_state == PUState_none) {
							current_buffer += (blank_سطرها > 0 ? "\n\n" : "\n") + سطر;
						} وگرنه
							current_buffer += (blank_سطرها > 0 ? "\n\n" : "\n") + stripped_سطر;
						اگر (stripped_سطر.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = درست;
					}
					blank_سطرها = 0;
				}

				اگر (!current_buffer.empty()) {
					اگر (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} وگرنه
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			منطقی has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = نادرست;

			اگر (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_ریشه, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			اگر (!cmd_help.has_group()) {
				اگر (pass->internal_flag)
					cmd_help.group = "internal";
				وگرنه اگر (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				وگرنه اگر (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				وگرنه اگر (has_source) {
					اگر (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				وگرنه اگر (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				وگرنه اگر (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				وگرنه اگر (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				وگرنه اگر (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				وگرنه اگر (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			اگر (groups.شمار(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].افزودن_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			برای (auto &content : cmd_help)
				json.مقدار(content.to_json(source_ریشه));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.سطر());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		بازگشت raise_error;
	}
	منطقی dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		منطقی raise_error = نادرست;
		برای (auto it : StaticCellTypes::builder.cells) {
			اگر (!StaticCellTypes::categories.is_known(it.type))
				ادامه;
			auto name = ID::str(it.type);
			اگر (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} وگرنه {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= درست;
			}
		}
		برای (auto &it : cell_help_messages.cell_help) {
			اگر (cells.شمار(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		برای (auto &it : groups) {
			json.name(it.first.c_str()); json.مقدار(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		برای (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.مقدار(ch.title);
			json.name("ports"); json.مقدار(ch.ports);
			json.name("source"); json.مقدار(ch.source);
			json.name("desc"); json.مقدار(ch.desc);
			json.name("code"); json.مقدار(ch.code);
			vector<string> inputs, outputs;
			برای (auto &درون_داد : ct.inputs)
				inputs.افزودن_back(ID::str(درون_داد));
			json.name("inputs"); json.مقدار(inputs);
			برای (auto &output : ct.outputs)
				outputs.افزودن_back(ID::str(output));
			json.name("outputs"); json.مقدار(outputs);
			vector<string> properties;
			// CellType properties
			اگر (ct.features.is_evaluable) properties.افزودن_back("is_evaluable");
			اگر (ct.features.is_combinatorial) properties.افزودن_back("is_combinatorial");
			اگر (ct.features.is_synthesizable) properties.افزودن_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			وقتی ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.افزودن_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			اگر (final_tag.size()) properties.افزودن_back(final_tag);
			json.name("properties"); json.مقدار(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		بازگشت raise_error;
	}
	تهی execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		اگر (args.size() == 1) {
			log("\n");
			برای (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			بازگشت;
		}

		اگر (args.size() == 2) {
			اگر (args[1] == "-all") {
				برای (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string پیام;
					برای (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						پیام += '=';
					log("%s\n", پیام);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			وگرنه اگر (args[1] == "-cells") {
				log("\n");
				برای (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				بازگشت;
			}
			وگرنه اگر (pass_register.شمار(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			وگرنه اگر (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				اگر (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} وگرنه {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					اگر (help_cell.ver == "2" || help_cell.ver == "2a") {
						اگر (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						برای (std::string سطر; std::getline(ss, سطر, '\n');) {
							اگر (سطر != "::") log("%s\n", سطر);
						}
					} وگرنه اگر (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} وگرنه {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			وگرنه
				log("No such command or cell type: %s\n", args[1]);
			بازگشت;
		} وگرنه اگر (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			اگر (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				اگر (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				اگر (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			وگرنه اگر (args[1] == "-dump-cells-json") {
				PrettyJson json;
				اگر (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				اگر (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			وگرنه
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			بازگشت;
		}

		help();
	}
} HelpPass;

ساختار EchoPass : عمومی Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	تهی help() override
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
	تهی execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		اگر (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		اگر (args.size() == 2) {
			اگر (args[1] == "on")
				echo_mode = درست;
			وگرنه اگر (args[1] == "off")
				echo_mode = نادرست;
			وگرنه
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

ساختار MinisatSatSolver : عمومی SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = این;
	}
	ezSAT *create() override {
		بازگشت جدید ezMiniSAT();
	}
} MinisatSatSolver;

ساختار LicensePass : عمومی Pass {
	LicensePass() : Pass("license", "print license terms") { }
	تهی help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	تهی execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	تهی notice()
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

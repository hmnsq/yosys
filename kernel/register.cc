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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/satgen.h"
#অন্তর্ভাব "kernel/json.h"
#অন্তর্ভাব "kernel/gzip.h"
#অন্তর্ভাব "kernel/log_help.h"
#অন্তর্ভাব "kernel/newcelltypes.h"

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <filesystem>

YOSYS_NAMESPACE_BEGIN

#define MAX_REG_গুনতি 1000

যৌক্তিক echo_mode = মিথ্যা;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::পরবর্তী_args;

যৌক্তিক GarbageCollectionGuard::is_enabled_ = সত্য;

স্থির যৌক্তিক garbage_collection_requested = মিথ্যা;

শূন্য request_garbage_collection()
{
	garbage_collection_requested = সত্য;
}

শূন্য try_collect_garbage()
{
	যদি (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		ফেরত;
	garbage_collection_requested = মিথ্যা;
	যদি (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	পরবর্তী_queued_pass = first_queued_pass;
	first_queued_pass = এই;
	call_গণক = 0;
	runtime_ns = 0;
}

শূন্য Pass::run_register()
{
	যদি (pass_register.গুনতি(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = এই;
}

শূন্য Pass::init_register()
{
	vector<Pass*> added_passes;
	যখন (first_queued_pass) {
		added_passes.ঠেলো_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->পরবর্তী_queued_pass;
	}
	জন্য (auto added_pass : added_passes)
		added_pass->on_register();
}

শূন্য Pass::done_register()
{
	জন্য (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == ফাঁকা);
}

শূন্য Pass::on_register()
{
}

শূন্য Pass::on_shutdown()
{
}

Pass::~Pass()
{
}

Pass::pre_post_exec_state_t Pass::pre_execute()
{
	pre_post_exec_state_t state;
	call_গণক++;
	state.begin_ns = PerformanceTimer::query();
	state.parent_pass = current_pass;
	current_pass = এই;
	clear_flags();
	ফেরত state;
}

শূন্য Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

শূন্য Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	যদি (current_pass)
		current_pass->runtime_ns -= time_ns;
}

শূন্য Pass::help()
{
	auto prettyHelp = PrettyHelp();
	যদি (formatted_help()) {
		prettyHelp.log_help();
	} অন্যথা {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

যৌক্তিক Pass::formatted_help()
{
	ফেরত মিথ্যা;
}

শূন্য Pass::clear_flags()
{
}

শূন্য Pass::cmd_log_args(ধ্রুবক std::vector<std::string> &args)
{
	যদি (args.আকার_2() <= 1)
		ফেরত;
	std::string message = "Full command line:";
	জন্য (আকার_t i = 0; i < args.আকার_2(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

শূন্য Pass::cmd_error(ধ্রুবক std::vector<std::string> &args, আকার_t argidx, std::string বার্তা)
{
	std::string command_text;
	পূর্ণসংখ্যা error_pos = 0;

	জন্য (আকার_t i = 0; i < args.আকার_2(); i++) {
		যদি (i < argidx)
			error_pos += args[i].আকার_2() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			বার্তা.c_str(), command_text.c_str(), error_pos, "");
}

শূন্য Pass::extra_args(std::vector<std::string> args, আকার_t argidx, RTLIL::Design *design, যৌক্তিক select)
{
	জন্য (; argidx < args.আকার_2(); argidx++)
	{
		std::string arg = args[argidx];

		যদি (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		যদি (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(এই, args, argidx, args.আকার_2(), design);
		বিরতি;
	}
	// cmd_log_args(args);
}

শূন্য Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = পরবর্তী_token(cmd_buf, " \t\r\n", সত্য);

	যদি (tok.empty())
		ফেরত;

	যদি (tok[0] == '!') {
#যদি defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		যখন (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.আকার_2()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		পূর্ণসংখ্যা retCode = run_command(cmd_buf);
		যদি (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		ফেরত;
#অন্যথা
		log_cmd_error("Shell is not available.\n");
#endif
	}

	যখন (!tok.empty()) {
		যদি (tok[0] == '#') {
			পূর্ণসংখ্যা stop;
			জন্য (stop = 0; stop < Get_আকার(cmd_buf); stop++)
				যদি (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					বিরতি;
			cmd_buf = cmd_buf.substr(stop);
		} অন্যথা
		যদি (tok.back() == ';') {
			পূর্ণসংখ্যা অঙ্ক_semikolon = 0;
			যখন (!tok.empty() && tok.back() == ';')
				tok.resize(tok.আকার_2()-1), অঙ্ক_semikolon++;
			যদি (!tok.empty())
				args.ঠেলো_back(tok);
			call(design, args);
			args.clear();
			যদি (অঙ্ক_semikolon == 2)
				call(design, "clean");
			যদি (অঙ্ক_semikolon == 3)
				call(design, "clean -purge");
		} অন্যথা
			args.ঠেলো_back(tok);
		যৌক্তিক found_nl = মিথ্যা;
		জন্য (auto c : cmd_buf) {
			যদি (c == ' ' || c == '\t')
				চালু;
			যদি (c == '\r' || c == '\n')
				found_nl = সত্য;
			বিরতি;
		}
		যদি (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = পরবর্তী_token(cmd_buf, " \t\r\n", সত্য);
	}

	call(design, args);
}

শূন্য Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	যদি (args.আকার_2() == 0 || args[0][0] == '#' || args[0][0] == ':')
		ফেরত;

	যদি (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, মিথ্যা));
		জন্য (আকার_t i = 0; i < args.আকার_2(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	যদি (pass_register.গুনতি(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	যদি (pass->experimental_flag)
		log_experimental(args[0]);

	আকার_t orig_sel_স্তূপ_pos = design->selection_স্তূপ.আকার_2();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	যখন (design->selection_স্তূপ.আকার_2() > orig_sel_স্তূপ_pos)
		design->বের_করো_selection();
}

শূন্য Pass::call_on_selection(RTLIL::Design *design, ধ্রুবক RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ঠেলো_selection(selection);

	Pass::call(design, command);

	design->বের_করো_selection();
	design->selected_active_module = backup_selected_active_module;
}

শূন্য Pass::call_on_selection(RTLIL::Design *design, ধ্রুবক RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ঠেলো_selection(selection);

	Pass::call(design, args);

	design->বের_করো_selection();
	design->selected_active_module = backup_selected_active_module;
}

শূন্য Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ঠেলো_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->বের_করো_selection();
	design->selected_active_module = backup_selected_active_module;
}

শূন্য Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ঠেলো_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->বের_করো_selection();
	design->selected_active_module = backup_selected_active_module;
}

যৌক্তিক ScriptPass::check_label(std::string label, std::string info)
{
	যদি (active_design == nullptr) {
		log("\n");
		যদি (info.empty())
			log("    %s:\n", label);
		অন্যথা
			log("    %s:    %s\n", label, info);
		ফেরত সত্য;
	} অন্যথা {
		যদি (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} অন্যথা {
			যদি (label == active_run_from)
				block_active = সত্য;
			যদি (label == active_run_to)
				block_active = মিথ্যা;
		}
		ফেরত block_active;
	}
}

শূন্য ScriptPass::run(std::string command, std::string info)
{
	যদি (active_design == nullptr) {
		যদি (info.empty())
			log("        %s\n", command);
		অন্যথা
			log("        %s    %s\n", command, info);
	} অন্যথা {
		Pass::call(active_design, command);
		active_design->check();
	}
}

শূন্য ScriptPass::run_nocheck(std::string command, std::string info)
{
	যদি (active_design == nullptr) {
		যদি (info.empty())
			log("        %s\n", command);
		অন্যথা
			log("        %s    %s\n", command, info);
	} অন্যথা {
		Pass::call(active_design, command);
	}
}

শূন্য ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = মিথ্যা;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

শূন্য ScriptPass::help_script()
{
	clear_flags();
	help_mode = সত্য;
	active_design = nullptr;
	block_active = সত্য;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

শূন্য Frontend::run_register()
{
	যদি (pass_register.গুনতি(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = এই;

	যদি (frontend_register.গুনতি(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = এই;
}

Frontend::~Frontend()
{
}

শূন্য Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(পরবর্তী_args.empty());
	করো {
		std::istream *f = ফাঁকা;
		পরবর্তী_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = পরবর্তী_args;
		delete f;
	} যখন (!args.empty());
}

FILE *Frontend::current_script_file = ফাঁকা;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
পূর্ণসংখ্যা Frontend::current_script_lineno = 0;

শূন্য Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, আকার_t argidx, যৌক্তিক bin_প্রবেশ)
{
	যৌক্তিক called_with_fp = f != ফাঁকা;

	পরবর্তী_args.clear();

	যদি (argidx < args.আকার_2())
	{
		std::string arg = args[argidx];

		যদি (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		যদি (f != ফাঁকা)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		যদি (filename == "<<" && argidx+1 < args.আকার_2())
			filename += args[++argidx];
		যদি (filename.compare(0, 2, "<<") == 0) {
			যদি (filename.আকার_2() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			যখন (1) {
				std::string বাফার;
				অক্ষর block[4096];
				যখন (1) {
					যদি (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					বাফার += block;
					যদি (বাফার.আকার_2() > 0 && (বাফার[বাফার.আকার_2() - 1] == '\n' || বাফার[বাফার.আকার_2() - 1] == '\r'))
						বিরতি;
				}
				Frontend::current_script_lineno++;
				আকার_t indent = বাফার.find_first_not_of(" \t\r\n");
				যদি (indent != std::string::npos && বাফার.compare(indent, eot_marker.আকার_2(), eot_marker) == 0)
					বিরতি;
				last_here_document += বাফার;
			}
			f = নতুন std::istringstream(last_here_document);
		} অন্যথা {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			যদি (Get_আকার(filenames) > 1) {
				পরবর্তী_args.ঢোকাও(পরবর্তী_args.end(), args.begin(), args.begin()+argidx);
				পরবর্তী_args.ঢোকাও(পরবর্তী_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_প্রবেশ_files.ঢোকাও(filename);
			f = uncompressed(filename, bin_প্রবেশ ? std::ifstream::দ্বিমিক : std::ifstream::in);
		}

		জন্য (আকার_t i = argidx+1; i < args.আকার_2(); i++)
			যদি (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		যদি (argidx+1 < args.আকার_2()) {
			যদি (পরবর্তী_args.empty())
				পরবর্তী_args.ঢোকাও(পরবর্তী_args.end(), args.begin(), args.begin()+argidx);
			পরবর্তী_args.ঢোকাও(পরবর্তী_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	যদি (f == ফাঁকা)
		cmd_error(args, argidx, "No filename given.");

	যদি (called_with_fp)
		args.ঠেলো_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

শূন্য Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	অক্ষর *s = strdup(command.c_str());
	জন্য (অক্ষর *p = strtok(s, " \t\r\n"); p; p = strtok(ফাঁকা, " \t\r\n"))
		args.ঠেলো_back(p);
	মুক্ত(s);
	frontend_call(design, f, filename, args);
}

শূন্য Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	যদি (args.আকার_2() == 0)
		ফেরত;
	যদি (frontend_register.গুনতি(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	যদি (f != ফাঁকা) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} অন্যথা যদি (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} অন্যথা {
		যদি (!filename.empty())
			args.ঠেলো_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

শূন্য Backend::run_register()
{
	যদি (pass_register.গুনতি(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = এই;

	যদি (backend_register.গুনতি(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = এই;
}

Backend::~Backend()
{
}

শূন্য Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = ফাঁকা;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	যদি (f != &std::cout)
		delete f;
}

শূন্য Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, আকার_t argidx, যৌক্তিক bin_output)
{
	যৌক্তিক called_with_fp = f != ফাঁকা;

	জন্য (; argidx < args.আকার_2(); argidx++)
	{
		std::string arg = args[argidx];

		যদি (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		যদি (f != ফাঁকা)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		যদি (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			চালু;
		}

		filename = arg;
		rewrite_filename(filename);
		যদি (filename.আকার_2() > 3 && filename.compare(filename.আকার_2()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = নতুন gzip_ostream;
			যদি (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ঢোকাও(filename);
			f = gf;
#অন্যথা
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} অন্যথা {
			std::ofstream *ff = নতুন std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::দ্বিমিক) : std::ofstream::trunc);
			yosys_output_files.ঢোকাও(filename);
			যদি (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	যদি (called_with_fp)
		args.ঠেলো_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	যদি (f == ফাঁকা) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

শূন্য Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	অক্ষর *s = strdup(command.c_str());
	জন্য (অক্ষর *p = strtok(s, " \t\r\n"); p; p = strtok(ফাঁকা, " \t\r\n"))
		args.ঠেলো_back(p);
	মুক্ত(s);
	backend_call(design, f, filename, args);
}

শূন্য Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	যদি (args.আকার_2() == 0)
		ফেরত;
	যদি (backend_register.গুনতি(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	আকার_t orig_sel_স্তূপ_pos = design->selection_স্তূপ.আকার_2();

	যদি (f != ফাঁকা) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} অন্যথা যদি (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} অন্যথা {
		যদি (!filename.empty())
			args.ঠেলো_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	যখন (design->selection_স্তূপ.আকার_2() > orig_sel_স্তূপ_pos)
		design->বের_করো_selection();
}

কাঠামো SimHelper {
	string name;
	অন্তঃরেখ string filesafe_name() {
		যদি (name.at(0) == '$')
			যদি (name.at(1) == '_')
				ফেরত "gate" + name.substr(1);
			অন্যথা
				ফেরত "word_" + name.substr(1);
		অন্যথা
			ফেরত name;
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

স্থির যৌক্তিক is_code_getter(string name) {
	ফেরত *(--(name.end())) == '+';
}

স্থির string get_cell_name(string name) {
	ফেরত is_code_getter(name) ? name.substr(0, name.দৈর্ঘ্য()-1) : name;
}

স্থির শূন্য log_warning_flags(Pass *pass) {
	যৌক্তিক has_warnings = মিথ্যা;
	ধ্রুবক string name = pass->pass_name;
	যদি (pass->experimental_flag) {
		যদি (!has_warnings) log("\n");
		has_warnings = সত্য;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	যদি (pass->internal_flag) {
		যদি (!has_warnings) log("\n");
		has_warnings = সত্য;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	যদি (has_warnings)
		log("\n");
}

স্থির কাঠামো CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#অন্তর্ভাব "kernel/simlib_help.inc"
#অন্তর্ভাব "kernel/simcells_help.inc"
		cell_help.sort();
	}
	যৌক্তিক contains(string name) { ফেরত cell_help.গুনতি(get_cell_name(name)) > 0; }
	SimHelper get(string name) { ফেরত cell_help[get_cell_name(name)]; }
} cell_help_messages;

কাঠামো HelpPass : সর্বজনীন Pass {
	HelpPass() : Pass("help", "display help messages") { }
	শূন্য help() override
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
	যৌক্তিক dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		যৌক্তিক raise_error = মিথ্যা;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_মূল = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		জন্য (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			যদি (!has_pretty_help) {
				গণনা PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_বাফার = "";
				auto মূল_listing_2 = cmd_help.get_মূল();
				auto current_listing = মূল_listing_2;

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
				আকার_t def_strip_গুনতি = 0;
				auto current_state = PUState_none;
				auto catch_verific = মিথ্যা;
				auto blank_সারিগুলি = 2;
				জন্য (string সারি; std::getline(ss, সারি, '\n');) {
					// find position of first non space character
					std::আকার_t first_pos = সারি.find_first_not_of(" \t");
					std::আকার_t last_pos = সারি.find_last_not_of(" \t");
					যদি (first_pos == std::string::npos) {
						নির্বাচন (current_state)
						{
						ক্ষেত্র PUState_signature:
							মূল_listing_2->usage(current_বাফার, null_source);
							current_listing = মূল_listing_2;
							current_state = PUState_none;
							current_বাফার = "";
							বিরতি;
						ক্ষেত্র PUState_none:
						ক্ষেত্র PUState_optionbody:
							blank_সারিগুলি += 1;
							বিরতি;
						পূর্বনির্ধারিত:
							বিরতি;
						}
						// skip empty lines
						চালু;
					}

					// strip leading and trailing whitespace
					std::string stripped_সারি = সারি.substr(first_pos, last_pos - first_pos +1);
					যৌক্তিক IsDefinition = stripped_সারি[0] == '-';
					IsDefinition &= stripped_সারি[1] != ' ' && stripped_সারি[1] != '>';
					যৌক্তিক IsDedent = def_strip_গুনতি && first_pos < def_strip_গুনতি;
					যৌক্তিক IsIndent = def_strip_গুনতি < first_pos;

					// line looks like a signature
					যৌক্তিক IsSignature = stripped_সারি.find(name) == 0 && (stripped_সারি.দৈর্ঘ্য() == name.দৈর্ঘ্য() || stripped_সারি.at(name.আকার_2()) == ' ');

					যদি (IsSignature && first_pos <= 4 && (blank_সারিগুলি >= 2 || current_state == PUState_signature)) {
						যদি (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_বাফার, "none", null_source);
							current_বাফার = "";
						} অন্যথা যদি (current_state == PUState_signature) {
							মূল_listing_2->usage(current_বাফার, null_source);
							current_বাফার = "";
						} অন্যথা যদি (current_state == PUState_none && !current_বাফার.empty()) {
							current_listing->codeblock(current_বাফার, "none", null_source);
							current_বাফার = "";
						}
						current_listing = মূল_listing_2;
						current_state = PUState_signature;
						def_strip_গুনতি = first_pos;
						catch_verific = মিথ্যা;
					} অন্যথা যদি (IsDedent) {
						def_strip_গুনতি = first_pos;
						যদি (current_state == PUState_optionbody) {
							যদি (!current_বাফার.empty()) {
								current_listing->codeblock(current_বাফার, "none", null_source);
								current_বাফার = "";
							}
							যদি (IsIndent) {
								current_state = PUState_options;
								current_listing = মূল_listing_2->back();
							} অন্যথা {
								current_state = PUState_none;
								current_listing = মূল_listing_2;
							}
						} অন্যথা {
							current_state = PUState_none;
						}
					}

					যদি (IsDefinition && !catch_verific && current_state != PUState_signature) {
						যদি (!current_বাফার.empty()) {
							current_listing->codeblock(current_বাফার, "none", null_source);
							current_বাফার = "";
						}
						current_state = PUState_options;
						current_listing = মূল_listing_2->open_option(stripped_সারি, null_source);
						def_strip_গুনতি = first_pos;
					} অন্যথা {
						যদি (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						যদি (current_বাফার.empty())
							current_বাফার = stripped_সারি;
						অন্যথা যদি (current_state == PUState_signature && IsIndent)
							current_বাফার += stripped_সারি;
						অন্যথা যদি (current_state == PUState_none) {
							current_বাফার += (blank_সারিগুলি > 0 ? "\n\n" : "\n") + সারি;
						} অন্যথা
							current_বাফার += (blank_সারিগুলি > 0 ? "\n\n" : "\n") + stripped_সারি;
						যদি (stripped_সারি.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = সত্য;
					}
					blank_সারিগুলি = 0;
				}

				যদি (!current_বাফার.empty()) {
					যদি (current_বাফার.আকার_2() > 64 && current_বাফার.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_বাফার.substr(0, 62), null_source);
						current_listing->codeblock(current_বাফার.substr(64), "yoscrypt", null_source);
					} অন্যথা
						current_listing->codeblock(current_বাফার, "none", null_source);
					current_বাফার = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			যৌক্তিক has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = মিথ্যা;

			যদি (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_মূল, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			যদি (!cmd_help.has_group()) {
				যদি (pass->internal_flag)
					cmd_help.group = "internal";
				অন্যথা যদি (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				অন্যথা যদি (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				অন্যথা যদি (has_source) {
					যদি (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				অন্যথা যদি (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				অন্যথা যদি (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				অন্যথা যদি (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				অন্যথা যদি (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				অন্যথা যদি (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			যদি (groups.গুনতি(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].ঠেলো_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			জন্য (auto &content : cmd_help)
				json.মান(content.to_json(source_মূল));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.সারি());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		ফেরত raise_error;
	}
	যৌক্তিক dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		যৌক্তিক raise_error = মিথ্যা;
		জন্য (auto it : StaticCellTypes::builder.cells) {
			যদি (!StaticCellTypes::categories.is_known(it.type))
				চালু;
			auto name = ID::str(it.type);
			যদি (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} অন্যথা {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= সত্য;
			}
		}
		জন্য (auto &it : cell_help_messages.cell_help) {
			যদি (cells.গুনতি(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		জন্য (auto &it : groups) {
			json.name(it.first.c_str()); json.মান(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		জন্য (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.মান(ch.title);
			json.name("ports"); json.মান(ch.ports);
			json.name("source"); json.মান(ch.source);
			json.name("desc"); json.মান(ch.desc);
			json.name("code"); json.মান(ch.code);
			vector<string> inputs, outputs;
			জন্য (auto &প্রবেশ : ct.inputs)
				inputs.ঠেলো_back(ID::str(প্রবেশ));
			json.name("inputs"); json.মান(inputs);
			জন্য (auto &output : ct.outputs)
				outputs.ঠেলো_back(ID::str(output));
			json.name("outputs"); json.মান(outputs);
			vector<string> properties;
			// CellType properties
			যদি (ct.features.is_evaluable) properties.ঠেলো_back("is_evaluable");
			যদি (ct.features.is_combinatorial) properties.ঠেলো_back("is_combinatorial");
			যদি (ct.features.is_synthesizable) properties.ঠেলো_back("is_synthesizable");
			// SimHelper properties
			আকার_t last = 0; আকার_t পরবর্তী = 0;
			যখন ((পরবর্তী = ch.tags.find(", ", last)) != string::npos) {
				properties.ঠেলো_back(ch.tags.substr(last, পরবর্তী-last));
				last = পরবর্তী + 2;
			}
			auto final_tag = ch.tags.substr(last);
			যদি (final_tag.আকার_2()) properties.ঠেলো_back(final_tag);
			json.name("properties"); json.মান(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		ফেরত raise_error;
	}
	শূন্য execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		যদি (args.আকার_2() == 1) {
			log("\n");
			জন্য (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			ফেরত;
		}

		যদি (args.আকার_2() == 2) {
			যদি (args[1] == "-all") {
				জন্য (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string বার্তা;
					জন্য (আকার_t i = 0; i < it.first.আকার_2() + it.second->short_help.আকার_2() + 6; i++)
						বার্তা += '=';
					log("%s\n", বার্তা);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			অন্যথা যদি (args[1] == "-cells") {
				log("\n");
				জন্য (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				ফেরত;
			}
			অন্যথা যদি (pass_register.গুনতি(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			অন্যথা যদি (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				যদি (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} অন্যথা {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					যদি (help_cell.ver == "2" || help_cell.ver == "2a") {
						যদি (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						জন্য (std::string সারি; std::getline(ss, সারি, '\n');) {
							যদি (সারি != "::") log("%s\n", সারি);
						}
					} অন্যথা যদি (help_cell.desc.দৈর্ঘ্য()) {
						log("%s\n", help_cell.desc);
					} অন্যথা {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			অন্যথা
				log("No such command or cell type: %s\n", args[1]);
			ফেরত;
		} অন্যথা যদি (args.আকার_2() == 3) {
			// this option is undocumented as it is for internal use only
			যদি (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				যদি (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				যদি (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			অন্যথা যদি (args[1] == "-dump-cells-json") {
				PrettyJson json;
				যদি (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				যদি (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			অন্যথা
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			ফেরত;
		}

		help();
	}
} HelpPass;

কাঠামো EchoPass : সর্বজনীন Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	শূন্য help() override
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
	শূন্য execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		যদি (args.আকার_2() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		যদি (args.আকার_2() == 2) {
			যদি (args[1] == "on")
				echo_mode = সত্য;
			অন্যথা যদি (args[1] == "off")
				echo_mode = মিথ্যা;
			অন্যথা
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

কাঠামো MinisatSatSolver : সর্বজনীন SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = এই;
	}
	ezSAT *create() override {
		ফেরত নতুন ezMiniSAT();
	}
} MinisatSatSolver;

কাঠামো LicensePass : সর্বজনীন Pass {
	LicensePass() : Pass("license", "print license terms") { }
	শূন্য help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	শূন্য execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	শূন্য notice()
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

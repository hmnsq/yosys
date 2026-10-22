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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/satgen.h"
#અન્તર્ભાવ "kernel/json.h"
#અન્તર્ભાવ "kernel/gzip.h"
#અન્તર્ભાવ "kernel/log_help.h"
#અન્તર્ભાવ "kernel/newcelltypes.h"

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <filesystem>

YOSYS_NAMESPACE_BEGIN

#define MAX_REG_ગણતરી 1000

તાર્કિક echo_mode = ખોટું;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

તાર્કિક GarbageCollectionGuard::is_enabled_ = સાચું;

સ્થિર તાર્કિક garbage_collection_requested = ખોટું;

શૂન્ય request_garbage_collection()
{
	garbage_collection_requested = સાચું;
}

શૂન્ય try_collect_garbage()
{
	જો (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		પાછા;
	garbage_collection_requested = ખોટું;
	જો (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = આ;
	call_counter = 0;
	runtime_ns = 0;
}

શૂન્ય Pass::run_register()
{
	જો (pass_register.ગણતરી(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = આ;
}

શૂન્ય Pass::init_register()
{
	vector<Pass*> added_passes;
	જ્યારે (first_queued_pass) {
		added_passes.ધકેલો_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	માટે (auto added_pass : added_passes)
		added_pass->on_register();
}

શૂન્ય Pass::done_register()
{
	માટે (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == ખાલી);
}

શૂન્ય Pass::on_register()
{
}

શૂન્ય Pass::on_shutdown()
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
	current_pass = આ;
	clear_flags();
	પાછા state;
}

શૂન્ય Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

શૂન્ય Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	જો (current_pass)
		current_pass->runtime_ns -= time_ns;
}

શૂન્ય Pass::help()
{
	auto prettyHelp = PrettyHelp();
	જો (formatted_help()) {
		prettyHelp.log_help();
	} નહીંતર {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

તાર્કિક Pass::formatted_help()
{
	પાછા ખોટું;
}

શૂન્ય Pass::clear_flags()
{
}

શૂન્ય Pass::cmd_log_args(અચલ std::vector<std::string> &args)
{
	જો (args.size() <= 1)
		પાછા;
	std::string message = "Full command line:";
	માટે (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

શૂન્ય Pass::cmd_error(અચલ std::vector<std::string> &args, size_t argidx, std::string સંદેશ)
{
	std::string command_text;
	પૂર્ણાંક error_pos = 0;

	માટે (size_t i = 0; i < args.size(); i++) {
		જો (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			સંદેશ.c_str(), command_text.c_str(), error_pos, "");
}

શૂન્ય Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, તાર્કિક select)
{
	માટે (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		જો (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		જો (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(આ, args, argidx, args.size(), design);
		તોડો;
	}
	// cmd_log_args(args);
}

શૂન્ય Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", સાચું);

	જો (tok.empty())
		પાછા;

	જો (tok[0] == '!') {
#જો defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		જ્યારે (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		પૂર્ણાંક retCode = run_command(cmd_buf);
		જો (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		પાછા;
#નહીંતર
		log_cmd_error("Shell is not available.\n");
#endif
	}

	જ્યારે (!tok.empty()) {
		જો (tok[0] == '#') {
			પૂર્ણાંક stop;
			માટે (stop = 0; stop < GetSize(cmd_buf); stop++)
				જો (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					તોડો;
			cmd_buf = cmd_buf.substr(stop);
		} નહીંતર
		જો (tok.back() == ';') {
			પૂર્ણાંક અંક_semikolon = 0;
			જ્યારે (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), અંક_semikolon++;
			જો (!tok.empty())
				args.ધકેલો_back(tok);
			call(design, args);
			args.clear();
			જો (અંક_semikolon == 2)
				call(design, "clean");
			જો (અંક_semikolon == 3)
				call(design, "clean -purge");
		} નહીંતર
			args.ધકેલો_back(tok);
		તાર્કિક found_nl = ખોટું;
		માટે (auto c : cmd_buf) {
			જો (c == ' ' || c == '\t')
				ચાલુ;
			જો (c == '\r' || c == '\n')
				found_nl = સાચું;
			તોડો;
		}
		જો (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", સાચું);
	}

	call(design, args);
}

શૂન્ય Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	જો (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		પાછા;

	જો (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, ખોટું));
		માટે (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	જો (pass_register.ગણતરી(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	જો (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_ઢગલો_pos = design->selection_ઢગલો.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	જ્યારે (design->selection_ઢગલો.size() > orig_sel_ઢગલો_pos)
		design->કાઢો_selection();
}

શૂન્ય Pass::call_on_selection(RTLIL::Design *design, અચલ RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ધકેલો_selection(selection);

	Pass::call(design, command);

	design->કાઢો_selection();
	design->selected_active_module = backup_selected_active_module;
}

શૂન્ય Pass::call_on_selection(RTLIL::Design *design, અચલ RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->ધકેલો_selection(selection);

	Pass::call(design, args);

	design->કાઢો_selection();
	design->selected_active_module = backup_selected_active_module;
}

શૂન્ય Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ધકેલો_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->કાઢો_selection();
	design->selected_active_module = backup_selected_active_module;
}

શૂન્ય Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->ધકેલો_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->કાઢો_selection();
	design->selected_active_module = backup_selected_active_module;
}

તાર્કિક ScriptPass::check_label(std::string label, std::string info)
{
	જો (active_design == nullptr) {
		log("\n");
		જો (info.empty())
			log("    %s:\n", label);
		નહીંતર
			log("    %s:    %s\n", label, info);
		પાછા સાચું;
	} નહીંતર {
		જો (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} નહીંતર {
			જો (label == active_run_from)
				block_active = સાચું;
			જો (label == active_run_to)
				block_active = ખોટું;
		}
		પાછા block_active;
	}
}

શૂન્ય ScriptPass::run(std::string command, std::string info)
{
	જો (active_design == nullptr) {
		જો (info.empty())
			log("        %s\n", command);
		નહીંતર
			log("        %s    %s\n", command, info);
	} નહીંતર {
		Pass::call(active_design, command);
		active_design->check();
	}
}

શૂન્ય ScriptPass::run_nocheck(std::string command, std::string info)
{
	જો (active_design == nullptr) {
		જો (info.empty())
			log("        %s\n", command);
		નહીંતર
			log("        %s    %s\n", command, info);
	} નહીંતર {
		Pass::call(active_design, command);
	}
}

શૂન્ય ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = ખોટું;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

શૂન્ય ScriptPass::help_script()
{
	clear_flags();
	help_mode = સાચું;
	active_design = nullptr;
	block_active = સાચું;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

શૂન્ય Frontend::run_register()
{
	જો (pass_register.ગણતરી(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = આ;

	જો (frontend_register.ગણતરી(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = આ;
}

Frontend::~Frontend()
{
}

શૂન્ય Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	કરો {
		std::istream *f = ખાલી;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} જ્યારે (!args.empty());
}

FILE *Frontend::current_script_file = ખાલી;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
પૂર્ણાંક Frontend::current_script_lineno = 0;

શૂન્ય Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, તાર્કિક bin_આગત)
{
	તાર્કિક called_with_fp = f != ખાલી;

	next_args.clear();

	જો (argidx < args.size())
	{
		std::string arg = args[argidx];

		જો (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		જો (f != ખાલી)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		જો (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		જો (filename.compare(0, 2, "<<") == 0) {
			જો (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			જ્યારે (1) {
				std::string buffer;
				અક્ષર block[4096];
				જ્યારે (1) {
					જો (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					જો (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						તોડો;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				જો (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					તોડો;
				last_here_document += buffer;
			}
			f = નવું std::istringstream(last_here_document);
		} નહીંતર {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			જો (GetSize(filenames) > 1) {
				next_args.ઉમેરો(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.ઉમેરો(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_આગત_files.ઉમેરો(filename);
			f = uncompressed(filename, bin_આગત ? std::ifstream::દ્વિઅંકી : std::ifstream::in);
		}

		માટે (size_t i = argidx+1; i < args.size(); i++)
			જો (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		જો (argidx+1 < args.size()) {
			જો (next_args.empty())
				next_args.ઉમેરો(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.ઉમેરો(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	જો (f == ખાલી)
		cmd_error(args, argidx, "No filename given.");

	જો (called_with_fp)
		args.ધકેલો_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

શૂન્ય Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	અક્ષર *s = strdup(command.c_str());
	માટે (અક્ષર *p = strtok(s, " \t\r\n"); p; p = strtok(ખાલી, " \t\r\n"))
		args.ધકેલો_back(p);
	મુક્ત(s);
	frontend_call(design, f, filename, args);
}

શૂન્ય Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	જો (args.size() == 0)
		પાછા;
	જો (frontend_register.ગણતરી(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	જો (f != ખાલી) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} નહીંતર જો (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} નહીંતર {
		જો (!filename.empty())
			args.ધકેલો_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

શૂન્ય Backend::run_register()
{
	જો (pass_register.ગણતરી(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = આ;

	જો (backend_register.ગણતરી(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = આ;
}

Backend::~Backend()
{
}

શૂન્ય Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = ખાલી;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	જો (f != &std::cout)
		delete f;
}

શૂન્ય Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, તાર્કિક bin_output)
{
	તાર્કિક called_with_fp = f != ખાલી;

	માટે (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		જો (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		જો (f != ખાલી)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		જો (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			ચાલુ;
		}

		filename = arg;
		rewrite_filename(filename);
		જો (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = નવું gzip_ostream;
			જો (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.ઉમેરો(filename);
			f = gf;
#નહીંતર
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} નહીંતર {
			std::ofstream *ff = નવું std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::દ્વિઅંકી) : std::ofstream::trunc);
			yosys_output_files.ઉમેરો(filename);
			જો (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	જો (called_with_fp)
		args.ધકેલો_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	જો (f == ખાલી) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

શૂન્ય Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	અક્ષર *s = strdup(command.c_str());
	માટે (અક્ષર *p = strtok(s, " \t\r\n"); p; p = strtok(ખાલી, " \t\r\n"))
		args.ધકેલો_back(p);
	મુક્ત(s);
	backend_call(design, f, filename, args);
}

શૂન્ય Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	જો (args.size() == 0)
		પાછા;
	જો (backend_register.ગણતરી(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_ઢગલો_pos = design->selection_ઢગલો.size();

	જો (f != ખાલી) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} નહીંતર જો (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} નહીંતર {
		જો (!filename.empty())
			args.ધકેલો_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	જ્યારે (design->selection_ઢગલો.size() > orig_sel_ઢગલો_pos)
		design->કાઢો_selection();
}

સંરચના SimHelper {
	string name;
	અંતઃરેખ string filesafe_name() {
		જો (name.at(0) == '$')
			જો (name.at(1) == '_')
				પાછા "gate" + name.substr(1);
			નહીંતર
				પાછા "word_" + name.substr(1);
		નહીંતર
			પાછા name;
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

સ્થિર તાર્કિક is_code_getter(string name) {
	પાછા *(--(name.end())) == '+';
}

સ્થિર string get_cell_name(string name) {
	પાછા is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

સ્થિર શૂન્ય log_warning_flags(Pass *pass) {
	તાર્કિક has_warnings = ખોટું;
	અચલ string name = pass->pass_name;
	જો (pass->experimental_flag) {
		જો (!has_warnings) log("\n");
		has_warnings = સાચું;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	જો (pass->internal_flag) {
		જો (!has_warnings) log("\n");
		has_warnings = સાચું;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	જો (has_warnings)
		log("\n");
}

સ્થિર સંરચના CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#અન્તર્ભાવ "kernel/simlib_help.inc"
#અન્તર્ભાવ "kernel/simcells_help.inc"
		cell_help.sort();
	}
	તાર્કિક contains(string name) { પાછા cell_help.ગણતરી(get_cell_name(name)) > 0; }
	SimHelper get(string name) { પાછા cell_help[get_cell_name(name)]; }
} cell_help_messages;

સંરચના HelpPass : જાહેર Pass {
	HelpPass() : Pass("help", "display help messages") { }
	શૂન્ય help() override
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
	તાર્કિક dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		તાર્કિક raise_error = ખોટું;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_મૂળ = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		માટે (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			જો (!has_pretty_help) {
				ગણના PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto મૂળ_listing_2 = cmd_help.get_મૂળ();
				auto current_listing = મૂળ_listing_2;

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
				size_t def_strip_ગણતરી = 0;
				auto current_state = PUState_none;
				auto catch_verific = ખોટું;
				auto blank_લીટીઓ = 2;
				માટે (string લીટી; std::getline(ss, લીટી, '\n');) {
					// find position of first non space character
					std::size_t first_pos = લીટી.find_first_not_of(" \t");
					std::size_t last_pos = લીટી.find_last_not_of(" \t");
					જો (first_pos == std::string::npos) {
						પસંદગી (current_state)
						{
						સ્થિતિ PUState_signature:
							મૂળ_listing_2->usage(current_buffer, null_source);
							current_listing = મૂળ_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							તોડો;
						સ્થિતિ PUState_none:
						સ્થિતિ PUState_optionbody:
							blank_લીટીઓ += 1;
							તોડો;
						મૂળભૂત:
							તોડો;
						}
						// skip empty lines
						ચાલુ;
					}

					// strip leading and trailing whitespace
					std::string stripped_લીટી = લીટી.substr(first_pos, last_pos - first_pos +1);
					તાર્કિક IsDefinition = stripped_લીટી[0] == '-';
					IsDefinition &= stripped_લીટી[1] != ' ' && stripped_લીટી[1] != '>';
					તાર્કિક IsDedent = def_strip_ગણતરી && first_pos < def_strip_ગણતરી;
					તાર્કિક IsIndent = def_strip_ગણતરી < first_pos;

					// line looks like a signature
					તાર્કિક IsSignature = stripped_લીટી.find(name) == 0 && (stripped_લીટી.length() == name.length() || stripped_લીટી.at(name.size()) == ' ');

					જો (IsSignature && first_pos <= 4 && (blank_લીટીઓ >= 2 || current_state == PUState_signature)) {
						જો (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} નહીંતર જો (current_state == PUState_signature) {
							મૂળ_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} નહીંતર જો (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = મૂળ_listing_2;
						current_state = PUState_signature;
						def_strip_ગણતરી = first_pos;
						catch_verific = ખોટું;
					} નહીંતર જો (IsDedent) {
						def_strip_ગણતરી = first_pos;
						જો (current_state == PUState_optionbody) {
							જો (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							જો (IsIndent) {
								current_state = PUState_options;
								current_listing = મૂળ_listing_2->back();
							} નહીંતર {
								current_state = PUState_none;
								current_listing = મૂળ_listing_2;
							}
						} નહીંતર {
							current_state = PUState_none;
						}
					}

					જો (IsDefinition && !catch_verific && current_state != PUState_signature) {
						જો (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = મૂળ_listing_2->open_option(stripped_લીટી, null_source);
						def_strip_ગણતરી = first_pos;
					} નહીંતર {
						જો (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						જો (current_buffer.empty())
							current_buffer = stripped_લીટી;
						નહીંતર જો (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_લીટી;
						નહીંતર જો (current_state == PUState_none) {
							current_buffer += (blank_લીટીઓ > 0 ? "\n\n" : "\n") + લીટી;
						} નહીંતર
							current_buffer += (blank_લીટીઓ > 0 ? "\n\n" : "\n") + stripped_લીટી;
						જો (stripped_લીટી.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = સાચું;
					}
					blank_લીટીઓ = 0;
				}

				જો (!current_buffer.empty()) {
					જો (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} નહીંતર
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			તાર્કિક has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = ખોટું;

			જો (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_મૂળ, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			જો (!cmd_help.has_group()) {
				જો (pass->internal_flag)
					cmd_help.group = "internal";
				નહીંતર જો (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				નહીંતર જો (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				નહીંતર જો (has_source) {
					જો (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				નહીંતર જો (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				નહીંતર જો (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				નહીંતર જો (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				નહીંતર જો (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				નહીંતર જો (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			જો (groups.ગણતરી(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].ધકેલો_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			માટે (auto &content : cmd_help)
				json.કિંમત(content.to_json(source_મૂળ));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.લીટી());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		પાછા raise_error;
	}
	તાર્કિક dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		તાર્કિક raise_error = ખોટું;
		માટે (auto it : StaticCellTypes::builder.cells) {
			જો (!StaticCellTypes::categories.is_known(it.type))
				ચાલુ;
			auto name = ID::str(it.type);
			જો (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} નહીંતર {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= સાચું;
			}
		}
		માટે (auto &it : cell_help_messages.cell_help) {
			જો (cells.ગણતરી(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		માટે (auto &it : groups) {
			json.name(it.first.c_str()); json.કિંમત(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		માટે (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.કિંમત(ch.title);
			json.name("ports"); json.કિંમત(ch.ports);
			json.name("source"); json.કિંમત(ch.source);
			json.name("desc"); json.કિંમત(ch.desc);
			json.name("code"); json.કિંમત(ch.code);
			vector<string> inputs, outputs;
			માટે (auto &આગત : ct.inputs)
				inputs.ધકેલો_back(ID::str(આગત));
			json.name("inputs"); json.કિંમત(inputs);
			માટે (auto &output : ct.outputs)
				outputs.ધકેલો_back(ID::str(output));
			json.name("outputs"); json.કિંમત(outputs);
			vector<string> properties;
			// CellType properties
			જો (ct.features.is_evaluable) properties.ધકેલો_back("is_evaluable");
			જો (ct.features.is_combinatorial) properties.ધકેલો_back("is_combinatorial");
			જો (ct.features.is_synthesizable) properties.ધકેલો_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			જ્યારે ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.ધકેલો_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			જો (final_tag.size()) properties.ધકેલો_back(final_tag);
			json.name("properties"); json.કિંમત(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		પાછા raise_error;
	}
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		જો (args.size() == 1) {
			log("\n");
			માટે (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			પાછા;
		}

		જો (args.size() == 2) {
			જો (args[1] == "-all") {
				માટે (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string સંદેશ;
					માટે (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						સંદેશ += '=';
					log("%s\n", સંદેશ);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			નહીંતર જો (args[1] == "-cells") {
				log("\n");
				માટે (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				પાછા;
			}
			નહીંતર જો (pass_register.ગણતરી(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			નહીંતર જો (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				જો (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} નહીંતર {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					જો (help_cell.ver == "2" || help_cell.ver == "2a") {
						જો (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						માટે (std::string લીટી; std::getline(ss, લીટી, '\n');) {
							જો (લીટી != "::") log("%s\n", લીટી);
						}
					} નહીંતર જો (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} નહીંતર {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			નહીંતર
				log("No such command or cell type: %s\n", args[1]);
			પાછા;
		} નહીંતર જો (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			જો (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				જો (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				જો (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			નહીંતર જો (args[1] == "-dump-cells-json") {
				PrettyJson json;
				જો (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				જો (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			નહીંતર
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			પાછા;
		}

		help();
	}
} HelpPass;

સંરચના EchoPass : જાહેર Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	શૂન્ય help() override
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
	શૂન્ય execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		જો (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		જો (args.size() == 2) {
			જો (args[1] == "on")
				echo_mode = સાચું;
			નહીંતર જો (args[1] == "off")
				echo_mode = ખોટું;
			નહીંતર
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

સંરચના MinisatSatSolver : જાહેર SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = આ;
	}
	ezSAT *create() override {
		પાછા નવું ezMiniSAT();
	}
} MinisatSatSolver;

સંરચના LicensePass : જાહેર Pass {
	LicensePass() : Pass("license", "print license terms") { }
	શૂન્ય help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	શૂન્ય execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	શૂન્ય notice()
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

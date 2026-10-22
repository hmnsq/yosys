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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/satgen.h"
#अन्तर्भाव "kernel/json.h"
#अन्तर्भाव "kernel/gzip.h"
#अन्तर्भाव "kernel/log_help.h"
#अन्तर्भाव "kernel/newcelltypes.h"

#अन्तर्भाव <string.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <filesystem>

YOSYS_NAMESPACE_BEGIN

#define MAX_REG_गणनम् 1000

तार्किक echo_mode = मिच्छा;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

तार्किक GarbageCollectionGuard::is_enabled_ = सच्च;

स्थिर तार्किक garbage_collection_requested = मिच्छा;

शून्य request_garbage_collection()
{
	garbage_collection_requested = सच्च;
}

शून्य try_collect_garbage()
{
	सचे (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		पटिनिवत्त;
	garbage_collection_requested = मिच्छा;
	सचे (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = एतत्;
	call_counter = 0;
	runtime_ns = 0;
}

शून्य Pass::run_register()
{
	सचे (pass_register.गणनम्(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = एतत्;
}

शून्य Pass::init_register()
{
	vector<Pass*> added_passes;
	याव (first_queued_pass) {
		added_passes.प्रक्षिप_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	अत्थाय (auto added_pass : added_passes)
		added_pass->on_register();
}

शून्य Pass::done_register()
{
	अत्थाय (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == रिक्त);
}

शून्य Pass::on_register()
{
}

शून्य Pass::on_shutdown()
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
	current_pass = एतत्;
	clear_flags();
	पटिनिवत्त state;
}

शून्य Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

शून्य Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	सचे (current_pass)
		current_pass->runtime_ns -= time_ns;
}

शून्य Pass::help()
{
	auto prettyHelp = PrettyHelp();
	सचे (formatted_help()) {
		prettyHelp.log_help();
	} अञ्ञथा {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

तार्किक Pass::formatted_help()
{
	पटिनिवत्त मिच्छा;
}

शून्य Pass::clear_flags()
{
}

शून्य Pass::cmd_log_args(ध्रुव std::vector<std::string> &args)
{
	सचे (args.size() <= 1)
		पटिनिवत्त;
	std::string message = "Full command line:";
	अत्थाय (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

शून्य Pass::cmd_error(ध्रुव std::vector<std::string> &args, size_t argidx, std::string सन्देशः)
{
	std::string command_text;
	पूर्णाङ्क error_pos = 0;

	अत्थाय (size_t i = 0; i < args.size(); i++) {
		सचे (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			सन्देशः.c_str(), command_text.c_str(), error_pos, "");
}

शून्य Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, तार्किक select)
{
	अत्थाय (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		सचे (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		सचे (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(एतत्, args, argidx, args.size(), design);
		भङ्ग;
	}
	// cmd_log_args(args);
}

शून्य Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", सच्च);

	सचे (tok.empty())
		पटिनिवत्त;

	सचे (tok[0] == '!') {
#सचे defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		याव (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		पूर्णाङ्क retCode = run_command(cmd_buf);
		सचे (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		पटिनिवत्त;
#अञ्ञथा
		log_cmd_error("Shell is not available.\n");
#endif
	}

	याव (!tok.empty()) {
		सचे (tok[0] == '#') {
			पूर्णाङ्क stop;
			अत्थाय (stop = 0; stop < GetSize(cmd_buf); stop++)
				सचे (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					भङ्ग;
			cmd_buf = cmd_buf.substr(stop);
		} अञ्ञथा
		सचे (tok.back() == ';') {
			पूर्णाङ्क अङ्कः_semikolon = 0;
			याव (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), अङ्कः_semikolon++;
			सचे (!tok.empty())
				args.प्रक्षिप_back(tok);
			call(design, args);
			args.clear();
			सचे (अङ्कः_semikolon == 2)
				call(design, "clean");
			सचे (अङ्कः_semikolon == 3)
				call(design, "clean -purge");
		} अञ्ञथा
			args.प्रक्षिप_back(tok);
		तार्किक found_nl = मिच्छा;
		अत्थाय (auto c : cmd_buf) {
			सचे (c == ' ' || c == '\t')
				अनुवर्त;
			सचे (c == '\r' || c == '\n')
				found_nl = सच्च;
			भङ्ग;
		}
		सचे (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", सच्च);
	}

	call(design, args);
}

शून्य Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	सचे (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		पटिनिवत्त;

	सचे (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, मिच्छा));
		अत्थाय (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	सचे (pass_register.गणनम्(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	सचे (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_राशिः_pos = design->selection_राशिः.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	याव (design->selection_राशिः.size() > orig_sel_राशिः_pos)
		design->निष्कासय_selection();
}

शून्य Pass::call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->प्रक्षिप_selection(selection);

	Pass::call(design, command);

	design->निष्कासय_selection();
	design->selected_active_module = backup_selected_active_module;
}

शून्य Pass::call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->प्रक्षिप_selection(selection);

	Pass::call(design, args);

	design->निष्कासय_selection();
	design->selected_active_module = backup_selected_active_module;
}

शून्य Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->प्रक्षिप_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->निष्कासय_selection();
	design->selected_active_module = backup_selected_active_module;
}

शून्य Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->प्रक्षिप_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->निष्कासय_selection();
	design->selected_active_module = backup_selected_active_module;
}

तार्किक ScriptPass::check_label(std::string label, std::string info)
{
	सचे (active_design == nullptr) {
		log("\n");
		सचे (info.empty())
			log("    %s:\n", label);
		अञ्ञथा
			log("    %s:    %s\n", label, info);
		पटिनिवत्त सच्च;
	} अञ्ञथा {
		सचे (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} अञ्ञथा {
			सचे (label == active_run_from)
				block_active = सच्च;
			सचे (label == active_run_to)
				block_active = मिच्छा;
		}
		पटिनिवत्त block_active;
	}
}

शून्य ScriptPass::run(std::string command, std::string info)
{
	सचे (active_design == nullptr) {
		सचे (info.empty())
			log("        %s\n", command);
		अञ्ञथा
			log("        %s    %s\n", command, info);
	} अञ्ञथा {
		Pass::call(active_design, command);
		active_design->check();
	}
}

शून्य ScriptPass::run_nocheck(std::string command, std::string info)
{
	सचे (active_design == nullptr) {
		सचे (info.empty())
			log("        %s\n", command);
		अञ्ञथा
			log("        %s    %s\n", command, info);
	} अञ्ञथा {
		Pass::call(active_design, command);
	}
}

शून्य ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = मिच्छा;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

शून्य ScriptPass::help_script()
{
	clear_flags();
	help_mode = सच्च;
	active_design = nullptr;
	block_active = सच्च;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

शून्य Frontend::run_register()
{
	सचे (pass_register.गणनम्(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = एतत्;

	सचे (frontend_register.गणनम्(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = एतत्;
}

Frontend::~Frontend()
{
}

शून्य Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	करोहि {
		std::istream *f = रिक्त;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} याव (!args.empty());
}

FILE *Frontend::current_script_file = रिक्त;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
पूर्णाङ्क Frontend::current_script_lineno = 0;

शून्य Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, तार्किक bin_निवेशः)
{
	तार्किक called_with_fp = f != रिक्त;

	next_args.clear();

	सचे (argidx < args.size())
	{
		std::string arg = args[argidx];

		सचे (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		सचे (f != रिक्त)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		सचे (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		सचे (filename.compare(0, 2, "<<") == 0) {
			सचे (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			याव (1) {
				std::string buffer;
				वर्ण block[4096];
				याव (1) {
					सचे (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					सचे (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						भङ्ग;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				सचे (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					भङ्ग;
				last_here_document += buffer;
			}
			f = नूतन std::istringstream(last_here_document);
		} अञ्ञथा {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			सचे (GetSize(filenames) > 1) {
				next_args.निवेशय(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.निवेशय(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_निवेशः_files.निवेशय(filename);
			f = uncompressed(filename, bin_निवेशः ? std::ifstream::द्विमान : std::ifstream::in);
		}

		अत्थाय (size_t i = argidx+1; i < args.size(); i++)
			सचे (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		सचे (argidx+1 < args.size()) {
			सचे (next_args.empty())
				next_args.निवेशय(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.निवेशय(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	सचे (f == रिक्त)
		cmd_error(args, argidx, "No filename given.");

	सचे (called_with_fp)
		args.प्रक्षिप_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

शून्य Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	वर्ण *s = strdup(command.c_str());
	अत्थाय (वर्ण *p = strtok(s, " \t\r\n"); p; p = strtok(रिक्त, " \t\r\n"))
		args.प्रक्षिप_back(p);
	मुक्त(s);
	frontend_call(design, f, filename, args);
}

शून्य Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	सचे (args.size() == 0)
		पटिनिवत्त;
	सचे (frontend_register.गणनम्(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	सचे (f != रिक्त) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} अञ्ञथा सचे (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} अञ्ञथा {
		सचे (!filename.empty())
			args.प्रक्षिप_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

शून्य Backend::run_register()
{
	सचे (pass_register.गणनम्(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = एतत्;

	सचे (backend_register.गणनम्(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = एतत्;
}

Backend::~Backend()
{
}

शून्य Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = रिक्त;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	सचे (f != &std::cout)
		delete f;
}

शून्य Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, तार्किक bin_output)
{
	तार्किक called_with_fp = f != रिक्त;

	अत्थाय (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		सचे (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		सचे (f != रिक्त)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		सचे (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			अनुवर्त;
		}

		filename = arg;
		rewrite_filename(filename);
		सचे (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = नूतन gzip_ostream;
			सचे (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.निवेशय(filename);
			f = gf;
#अञ्ञथा
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} अञ्ञथा {
			std::ofstream *ff = नूतन std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::द्विमान) : std::ofstream::trunc);
			yosys_output_files.निवेशय(filename);
			सचे (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	सचे (called_with_fp)
		args.प्रक्षिप_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	सचे (f == रिक्त) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

शून्य Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	वर्ण *s = strdup(command.c_str());
	अत्थाय (वर्ण *p = strtok(s, " \t\r\n"); p; p = strtok(रिक्त, " \t\r\n"))
		args.प्रक्षिप_back(p);
	मुक्त(s);
	backend_call(design, f, filename, args);
}

शून्य Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	सचे (args.size() == 0)
		पटिनिवत्त;
	सचे (backend_register.गणनम्(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_राशिः_pos = design->selection_राशिः.size();

	सचे (f != रिक्त) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} अञ्ञथा सचे (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} अञ्ञथा {
		सचे (!filename.empty())
			args.प्रक्षिप_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	याव (design->selection_राशिः.size() > orig_sel_राशिः_pos)
		design->निष्कासय_selection();
}

संरचना SimHelper {
	string name;
	अन्तःस्थ string filesafe_name() {
		सचे (name.at(0) == '$')
			सचे (name.at(1) == '_')
				पटिनिवत्त "gate" + name.substr(1);
			अञ्ञथा
				पटिनिवत्त "word_" + name.substr(1);
		अञ्ञथा
			पटिनिवत्त name;
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

स्थिर तार्किक is_code_getter(string name) {
	पटिनिवत्त *(--(name.end())) == '+';
}

स्थिर string get_cell_name(string name) {
	पटिनिवत्त is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

स्थिर शून्य log_warning_flags(Pass *pass) {
	तार्किक has_warnings = मिच्छा;
	ध्रुव string name = pass->pass_name;
	सचे (pass->experimental_flag) {
		सचे (!has_warnings) log("\n");
		has_warnings = सच्च;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	सचे (pass->internal_flag) {
		सचे (!has_warnings) log("\n");
		has_warnings = सच्च;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	सचे (has_warnings)
		log("\n");
}

स्थिर संरचना CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#अन्तर्भाव "kernel/simlib_help.inc"
#अन्तर्भाव "kernel/simcells_help.inc"
		cell_help.sort();
	}
	तार्किक contains(string name) { पटिनिवत्त cell_help.गणनम्(get_cell_name(name)) > 0; }
	SimHelper get(string name) { पटिनिवत्त cell_help[get_cell_name(name)]; }
} cell_help_messages;

संरचना HelpPass : सार्वजनिक Pass {
	HelpPass() : Pass("help", "display help messages") { }
	शून्य help() override
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
	तार्किक dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		तार्किक raise_error = मिच्छा;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_मूलम् = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		अत्थाय (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			सचे (!has_pretty_help) {
				गणना PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto मूलम्_listing_2 = cmd_help.get_मूलम्();
				auto current_listing = मूलम्_listing_2;

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
				size_t def_strip_गणनम् = 0;
				auto current_state = PUState_none;
				auto catch_verific = मिच्छा;
				auto blank_पङ्क्तयः = 2;
				अत्थाय (string पङ्क्तिः; std::getline(ss, पङ्क्तिः, '\n');) {
					// find position of first non space character
					std::size_t first_pos = पङ्क्तिः.find_first_not_of(" \t");
					std::size_t last_pos = पङ्क्तिः.find_last_not_of(" \t");
					सचे (first_pos == std::string::npos) {
						विकल्प (current_state)
						{
						पक्ष PUState_signature:
							मूलम्_listing_2->usage(current_buffer, null_source);
							current_listing = मूलम्_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							भङ्ग;
						पक्ष PUState_none:
						पक्ष PUState_optionbody:
							blank_पङ्क्तयः += 1;
							भङ्ग;
						स्वभाव:
							भङ्ग;
						}
						// skip empty lines
						अनुवर्त;
					}

					// strip leading and trailing whitespace
					std::string stripped_पङ्क्तिः = पङ्क्तिः.substr(first_pos, last_pos - first_pos +1);
					तार्किक IsDefinition = stripped_पङ्क्तिः[0] == '-';
					IsDefinition &= stripped_पङ्क्तिः[1] != ' ' && stripped_पङ्क्तिः[1] != '>';
					तार्किक IsDedent = def_strip_गणनम् && first_pos < def_strip_गणनम्;
					तार्किक IsIndent = def_strip_गणनम् < first_pos;

					// line looks like a signature
					तार्किक IsSignature = stripped_पङ्क्तिः.find(name) == 0 && (stripped_पङ्क्तिः.length() == name.length() || stripped_पङ्क्तिः.at(name.size()) == ' ');

					सचे (IsSignature && first_pos <= 4 && (blank_पङ्क्तयः >= 2 || current_state == PUState_signature)) {
						सचे (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} अञ्ञथा सचे (current_state == PUState_signature) {
							मूलम्_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} अञ्ञथा सचे (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = मूलम्_listing_2;
						current_state = PUState_signature;
						def_strip_गणनम् = first_pos;
						catch_verific = मिच्छा;
					} अञ्ञथा सचे (IsDedent) {
						def_strip_गणनम् = first_pos;
						सचे (current_state == PUState_optionbody) {
							सचे (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							सचे (IsIndent) {
								current_state = PUState_options;
								current_listing = मूलम्_listing_2->back();
							} अञ्ञथा {
								current_state = PUState_none;
								current_listing = मूलम्_listing_2;
							}
						} अञ्ञथा {
							current_state = PUState_none;
						}
					}

					सचे (IsDefinition && !catch_verific && current_state != PUState_signature) {
						सचे (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = मूलम्_listing_2->open_option(stripped_पङ्क्तिः, null_source);
						def_strip_गणनम् = first_pos;
					} अञ्ञथा {
						सचे (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						सचे (current_buffer.empty())
							current_buffer = stripped_पङ्क्तिः;
						अञ्ञथा सचे (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_पङ्क्तिः;
						अञ्ञथा सचे (current_state == PUState_none) {
							current_buffer += (blank_पङ्क्तयः > 0 ? "\n\n" : "\n") + पङ्क्तिः;
						} अञ्ञथा
							current_buffer += (blank_पङ्क्तयः > 0 ? "\n\n" : "\n") + stripped_पङ्क्तिः;
						सचे (stripped_पङ्क्तिः.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = सच्च;
					}
					blank_पङ्क्तयः = 0;
				}

				सचे (!current_buffer.empty()) {
					सचे (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} अञ्ञथा
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			तार्किक has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = मिच्छा;

			सचे (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_मूलम्, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			सचे (!cmd_help.has_group()) {
				सचे (pass->internal_flag)
					cmd_help.group = "internal";
				अञ्ञथा सचे (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				अञ्ञथा सचे (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				अञ्ञथा सचे (has_source) {
					सचे (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				अञ्ञथा सचे (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				अञ्ञथा सचे (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				अञ्ञथा सचे (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				अञ्ञथा सचे (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				अञ्ञथा सचे (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			सचे (groups.गणनम्(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].प्रक्षिप_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			अत्थाय (auto &content : cmd_help)
				json.मानम्(content.to_json(source_मूलम्));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.पङ्क्तिः());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		पटिनिवत्त raise_error;
	}
	तार्किक dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		तार्किक raise_error = मिच्छा;
		अत्थाय (auto it : StaticCellTypes::builder.cells) {
			सचे (!StaticCellTypes::categories.is_known(it.type))
				अनुवर्त;
			auto name = ID::str(it.type);
			सचे (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} अञ्ञथा {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= सच्च;
			}
		}
		अत्थाय (auto &it : cell_help_messages.cell_help) {
			सचे (cells.गणनम्(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		अत्थाय (auto &it : groups) {
			json.name(it.first.c_str()); json.मानम्(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		अत्थाय (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.मानम्(ch.title);
			json.name("ports"); json.मानम्(ch.ports);
			json.name("source"); json.मानम्(ch.source);
			json.name("desc"); json.मानम्(ch.desc);
			json.name("code"); json.मानम्(ch.code);
			vector<string> inputs, outputs;
			अत्थाय (auto &निवेशः : ct.inputs)
				inputs.प्रक्षिप_back(ID::str(निवेशः));
			json.name("inputs"); json.मानम्(inputs);
			अत्थाय (auto &output : ct.outputs)
				outputs.प्रक्षिप_back(ID::str(output));
			json.name("outputs"); json.मानम्(outputs);
			vector<string> properties;
			// CellType properties
			सचे (ct.features.is_evaluable) properties.प्रक्षिप_back("is_evaluable");
			सचे (ct.features.is_combinatorial) properties.प्रक्षिप_back("is_combinatorial");
			सचे (ct.features.is_synthesizable) properties.प्रक्षिप_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			याव ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.प्रक्षिप_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			सचे (final_tag.size()) properties.प्रक्षिप_back(final_tag);
			json.name("properties"); json.मानम्(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		पटिनिवत्त raise_error;
	}
	शून्य execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		सचे (args.size() == 1) {
			log("\n");
			अत्थाय (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			पटिनिवत्त;
		}

		सचे (args.size() == 2) {
			सचे (args[1] == "-all") {
				अत्थाय (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string सन्देशः;
					अत्थाय (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						सन्देशः += '=';
					log("%s\n", सन्देशः);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			अञ्ञथा सचे (args[1] == "-cells") {
				log("\n");
				अत्थाय (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				पटिनिवत्त;
			}
			अञ्ञथा सचे (pass_register.गणनम्(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			अञ्ञथा सचे (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				सचे (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} अञ्ञथा {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					सचे (help_cell.ver == "2" || help_cell.ver == "2a") {
						सचे (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						अत्थाय (std::string पङ्क्तिः; std::getline(ss, पङ्क्तिः, '\n');) {
							सचे (पङ्क्तिः != "::") log("%s\n", पङ्क्तिः);
						}
					} अञ्ञथा सचे (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} अञ्ञथा {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			अञ्ञथा
				log("No such command or cell type: %s\n", args[1]);
			पटिनिवत्त;
		} अञ्ञथा सचे (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			सचे (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				सचे (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				सचे (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			अञ्ञथा सचे (args[1] == "-dump-cells-json") {
				PrettyJson json;
				सचे (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				सचे (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			अञ्ञथा
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			पटिनिवत्त;
		}

		help();
	}
} HelpPass;

संरचना EchoPass : सार्वजनिक Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	शून्य help() override
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
	शून्य execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		सचे (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		सचे (args.size() == 2) {
			सचे (args[1] == "on")
				echo_mode = सच्च;
			अञ्ञथा सचे (args[1] == "off")
				echo_mode = मिच्छा;
			अञ्ञथा
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

संरचना MinisatSatSolver : सार्वजनिक SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = एतत्;
	}
	ezSAT *create() override {
		पटिनिवत्त नूतन ezMiniSAT();
	}
} MinisatSatSolver;

संरचना LicensePass : सार्वजनिक Pass {
	LicensePass() : Pass("license", "print license terms") { }
	शून्य help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	शून्य execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	शून्य notice()
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

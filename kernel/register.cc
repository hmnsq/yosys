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

#define MAX_REG_गन्ती 1000

तार्किक echo_mode = असत्य;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

तार्किक GarbageCollectionGuard::is_enabled_ = सत्य;

स्थिर तार्किक garbage_collection_requested = असत्य;

शून्य request_garbage_collection()
{
	garbage_collection_requested = सत्य;
}

शून्य try_collect_garbage()
{
	यदि (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		फर्काऊ;
	garbage_collection_requested = असत्य;
	यदि (yosys_design)
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
	यदि (pass_register.गन्ती(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = एतत्;
}

शून्य Pass::init_register()
{
	vector<Pass*> added_passes;
	जबसम्म (first_queued_pass) {
		added_passes.थप_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	लागि (स्वतः added_pass : added_passes)
		added_pass->on_register();
}

शून्य Pass::done_register()
{
	लागि (स्वतः &it : pass_register)
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
	फर्काऊ state;
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
	यदि (current_pass)
		current_pass->runtime_ns -= time_ns;
}

शून्य Pass::help()
{
	स्वतः prettyHelp = PrettyHelp();
	यदि (formatted_help()) {
		prettyHelp.log_help();
	} अन्यथा {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

तार्किक Pass::formatted_help()
{
	फर्काऊ असत्य;
}

शून्य Pass::clear_flags()
{
}

शून्य Pass::cmd_log_args(अचर std::vector<std::string> &args)
{
	यदि (args.size() <= 1)
		फर्काऊ;
	std::string message = "Full command line:";
	लागि (माप_प्रकार i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

शून्य Pass::cmd_error(अचर std::vector<std::string> &args, माप_प्रकार argidx, std::string सन्देश)
{
	std::string command_text;
	पूर्णाङ्क error_pos = 0;

	लागि (माप_प्रकार i = 0; i < args.size(); i++) {
		यदि (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			सन्देश.c_str(), command_text.c_str(), error_pos, "");
}

शून्य Pass::extra_args(std::vector<std::string> args, माप_प्रकार argidx, RTLIL::Design *design, तार्किक select)
{
	लागि (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		यदि (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		यदि (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(एतत्, args, argidx, args.size(), design);
		रोक;
	}
	// cmd_log_args(args);
}

शून्य Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", सत्य);

	यदि (tok.empty())
		फर्काऊ;

	यदि (tok[0] == '!') {
#यदि defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		जबसम्म (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		पूर्णाङ्क retCode = run_command(cmd_buf);
		यदि (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		फर्काऊ;
#अन्यथा
		log_cmd_error("Shell is not available.\n");
#endif
	}

	जबसम्म (!tok.empty()) {
		यदि (tok[0] == '#') {
			पूर्णाङ्क stop;
			लागि (stop = 0; stop < GetSize(cmd_buf); stop++)
				यदि (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					रोक;
			cmd_buf = cmd_buf.substr(stop);
		} अन्यथा
		यदि (tok.back() == ';') {
			पूर्णाङ्क अङ्क_semikolon = 0;
			जबसम्म (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), अङ्क_semikolon++;
			यदि (!tok.empty())
				args.थप_back(tok);
			call(design, args);
			args.clear();
			यदि (अङ्क_semikolon == 2)
				call(design, "clean");
			यदि (अङ्क_semikolon == 3)
				call(design, "clean -purge");
		} अन्यथा
			args.थप_back(tok);
		तार्किक found_nl = असत्य;
		लागि (स्वतः c : cmd_buf) {
			यदि (c == ' ' || c == '\t')
				जारी;
			यदि (c == '\r' || c == '\n')
				found_nl = सत्य;
			रोक;
		}
		यदि (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", सत्य);
	}

	call(design, args);
}

शून्य Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	यदि (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		फर्काऊ;

	यदि (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, असत्य));
		लागि (माप_प्रकार i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	यदि (pass_register.गन्ती(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	यदि (pass->experimental_flag)
		log_experimental(args[0]);

	माप_प्रकार orig_sel_थाक_pos = design->selection_थाक.size();
	स्वतः state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	जबसम्म (design->selection_थाक.size() > orig_sel_थाक_pos)
		design->झिक_selection();
}

शून्य Pass::call_on_selection(RTLIL::Design *design, अचर RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->थप_selection(selection);

	Pass::call(design, command);

	design->झिक_selection();
	design->selected_active_module = backup_selected_active_module;
}

शून्य Pass::call_on_selection(RTLIL::Design *design, अचर RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->थप_selection(selection);

	Pass::call(design, args);

	design->झिक_selection();
	design->selected_active_module = backup_selected_active_module;
}

शून्य Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->थप_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->झिक_selection();
	design->selected_active_module = backup_selected_active_module;
}

शून्य Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->थप_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->झिक_selection();
	design->selected_active_module = backup_selected_active_module;
}

तार्किक ScriptPass::check_label(std::string label, std::string info)
{
	यदि (active_design == nullptr) {
		log("\n");
		यदि (info.empty())
			log("    %s:\n", label);
		अन्यथा
			log("    %s:    %s\n", label, info);
		फर्काऊ सत्य;
	} अन्यथा {
		यदि (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} अन्यथा {
			यदि (label == active_run_from)
				block_active = सत्य;
			यदि (label == active_run_to)
				block_active = असत्य;
		}
		फर्काऊ block_active;
	}
}

शून्य ScriptPass::run(std::string command, std::string info)
{
	यदि (active_design == nullptr) {
		यदि (info.empty())
			log("        %s\n", command);
		अन्यथा
			log("        %s    %s\n", command, info);
	} अन्यथा {
		Pass::call(active_design, command);
		active_design->check();
	}
}

शून्य ScriptPass::run_nocheck(std::string command, std::string info)
{
	यदि (active_design == nullptr) {
		यदि (info.empty())
			log("        %s\n", command);
		अन्यथा
			log("        %s    %s\n", command, info);
	} अन्यथा {
		Pass::call(active_design, command);
	}
}

शून्य ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = असत्य;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

शून्य ScriptPass::help_script()
{
	clear_flags();
	help_mode = सत्य;
	active_design = nullptr;
	block_active = सत्य;
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
	यदि (pass_register.गन्ती(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = एतत्;

	यदि (frontend_register.गन्ती(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = एतत्;
}

Frontend::~Frontend()
{
}

शून्य Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	गर {
		std::istream *f = रिक्त;
		next_args.clear();
		स्वतः state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} जबसम्म (!args.empty());
}

खाता *Frontend::current_script_file = रिक्त;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
पूर्णाङ्क Frontend::current_script_lineno = 0;

शून्य Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, माप_प्रकार argidx, तार्किक bin_आगत)
{
	तार्किक called_with_fp = f != रिक्त;

	next_args.clear();

	यदि (argidx < args.size())
	{
		std::string arg = args[argidx];

		यदि (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		यदि (f != रिक्त)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		यदि (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		यदि (filename.compare(0, 2, "<<") == 0) {
			यदि (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			जबसम्म (1) {
				std::string buffer;
				वर्ण block[4096];
				जबसम्म (1) {
					यदि (ख_माला_लो(block, 4096, Frontend::current_script_file == nullptr? मानक_निवेश : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					यदि (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						रोक;
				}
				Frontend::current_script_lineno++;
				माप_प्रकार indent = buffer.find_first_not_of(" \t\r\n");
				यदि (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					रोक;
				last_here_document += buffer;
			}
			f = नूतन std::istringstream(last_here_document);
		} अन्यथा {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			यदि (GetSize(filenames) > 1) {
				next_args.घुसाउ(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.घुसाउ(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_आगत_files.घुसाउ(filename);
			f = uncompressed(filename, bin_आगत ? std::ifstream::द्विआधारी : std::ifstream::in);
		}

		लागि (माप_प्रकार i = argidx+1; i < args.size(); i++)
			यदि (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		यदि (argidx+1 < args.size()) {
			यदि (next_args.empty())
				next_args.घुसाउ(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.घुसाउ(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	यदि (f == रिक्त)
		cmd_error(args, argidx, "No filename given.");

	यदि (called_with_fp)
		args.थप_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

शून्य Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	वर्ण *s = strdup(command.c_str());
	लागि (वर्ण *p = म_मोहर(s, " \t\r\n"); p; p = म_मोहर(रिक्त, " \t\r\n"))
		args.थप_back(p);
	मुक्त(s);
	frontend_call(design, f, filename, args);
}

शून्य Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	यदि (args.size() == 0)
		फर्काऊ;
	यदि (frontend_register.गन्ती(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	यदि (f != रिक्त) {
		स्वतः state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} अन्यथा यदि (filename == "-") {
		std::istream *f_cin = &std::cin;
		स्वतः state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} अन्यथा {
		यदि (!filename.empty())
			args.थप_back(filename);
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
	यदि (pass_register.गन्ती(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = एतत्;

	यदि (backend_register.गन्ती(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = एतत्;
}

Backend::~Backend()
{
}

शून्य Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = रिक्त;
	स्वतः state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	यदि (f != &std::cout)
		delete f;
}

शून्य Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, माप_प्रकार argidx, तार्किक bin_output)
{
	तार्किक called_with_fp = f != रिक्त;

	लागि (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		यदि (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		यदि (f != रिक्त)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		यदि (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			जारी;
		}

		filename = arg;
		rewrite_filename(filename);
		यदि (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = नूतन gzip_ostream;
			यदि (!gf->खोलो(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, म_त्रुटि(त्रुटि_सं));
			}
			yosys_output_files.घुसाउ(filename);
			f = gf;
#अन्यथा
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} अन्यथा {
			std::ofstream *ff = नूतन std::ofstream;
			ff->खोलो(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::द्विआधारी) : std::ofstream::trunc);
			yosys_output_files.घुसाउ(filename);
			यदि (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, म_त्रुटि(त्रुटि_सं));
			}
			f = ff;
		}
	}

	यदि (called_with_fp)
		args.थप_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	यदि (f == रिक्त) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

शून्य Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	वर्ण *s = strdup(command.c_str());
	लागि (वर्ण *p = म_मोहर(s, " \t\r\n"); p; p = म_मोहर(रिक्त, " \t\r\n"))
		args.थप_back(p);
	मुक्त(s);
	backend_call(design, f, filename, args);
}

शून्य Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	यदि (args.size() == 0)
		फर्काऊ;
	यदि (backend_register.गन्ती(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	माप_प्रकार orig_sel_थाक_pos = design->selection_थाक.size();

	यदि (f != रिक्त) {
		स्वतः state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} अन्यथा यदि (filename == "-") {
		std::ostream *f_cout = &std::cout;
		स्वतः state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} अन्यथा {
		यदि (!filename.empty())
			args.थप_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	जबसम्म (design->selection_थाक.size() > orig_sel_थाक_pos)
		design->झिक_selection();
}

संरचना SimHelper {
	string name;
	अन्तःस्थ string filesafe_name() {
		यदि (name.at(0) == '$')
			यदि (name.at(1) == '_')
				फर्काऊ "gate" + name.substr(1);
			अन्यथा
				फर्काऊ "word_" + name.substr(1);
		अन्यथा
			फर्काऊ name;
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
	फर्काऊ *(--(name.end())) == '+';
}

स्थिर string get_cell_name(string name) {
	फर्काऊ is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

स्थिर शून्य log_warning_flags(Pass *pass) {
	तार्किक has_warnings = असत्य;
	अचर string name = pass->pass_name;
	यदि (pass->experimental_flag) {
		यदि (!has_warnings) log("\n");
		has_warnings = सत्य;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	यदि (pass->internal_flag) {
		यदि (!has_warnings) log("\n");
		has_warnings = सत्य;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	यदि (has_warnings)
		log("\n");
}

स्थिर संरचना CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#अन्तर्भाव "kernel/simlib_help.inc"
#अन्तर्भाव "kernel/simcells_help.inc"
		cell_help.sort();
	}
	तार्किक contains(string name) { फर्काऊ cell_help.गन्ती(get_cell_name(name)) > 0; }
	SimHelper get(string name) { फर्काऊ cell_help[get_cell_name(name)]; }
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

		तार्किक raise_error = असत्य;
		std::map<string, vector<string>> groups;

		// get root path
		स्वतः this_path = std::filesystem::path(source_location::current().file_name());
		स्वतः source_जरा = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		लागि (स्वतः &it : pass_register) {
			स्वतः name = it.first;
			स्वतः pass = it.second;
			स्वतः title = pass->short_help;

			स्वतः cmd_help = PrettyHelp();
			स्वतः has_pretty_help = pass->formatted_help();

			यदि (!has_pretty_help) {
				गणना PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				स्वतः जरा_listing_2 = cmd_help.get_जरा();
				स्वतः current_listing = जरा_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					स्वतः log_scope = logger().sink_scope();
					logger().add_sink<StreamLogSink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				माप_प्रकार def_strip_गन्ती = 0;
				स्वतः current_state = PUState_none;
				स्वतः catch_verific = असत्य;
				स्वतः blank_पङ्क्तिहरू = 2;
				लागि (string पङ्क्ति; std::getline(ss, पङ्क्ति, '\n');) {
					// find position of first non space character
					std::माप_प्रकार first_pos = पङ्क्ति.find_first_not_of(" \t");
					std::माप_प्रकार last_pos = पङ्क्ति.find_last_not_of(" \t");
					यदि (first_pos == std::string::npos) {
						विकल्प (current_state)
						{
						पक्ष PUState_signature:
							जरा_listing_2->usage(current_buffer, null_source);
							current_listing = जरा_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							रोक;
						पक्ष PUState_none:
						पक्ष PUState_optionbody:
							blank_पङ्क्तिहरू += 1;
							रोक;
						स्वभाव:
							रोक;
						}
						// skip empty lines
						जारी;
					}

					// strip leading and trailing whitespace
					std::string stripped_पङ्क्ति = पङ्क्ति.substr(first_pos, last_pos - first_pos +1);
					तार्किक IsDefinition = stripped_पङ्क्ति[0] == '-';
					IsDefinition &= stripped_पङ्क्ति[1] != ' ' && stripped_पङ्क्ति[1] != '>';
					तार्किक IsDedent = def_strip_गन्ती && first_pos < def_strip_गन्ती;
					तार्किक IsIndent = def_strip_गन्ती < first_pos;

					// line looks like a signature
					तार्किक IsSignature = stripped_पङ्क्ति.find(name) == 0 && (stripped_पङ्क्ति.length() == name.length() || stripped_पङ्क्ति.at(name.size()) == ' ');

					यदि (IsSignature && first_pos <= 4 && (blank_पङ्क्तिहरू >= 2 || current_state == PUState_signature)) {
						यदि (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} अन्यथा यदि (current_state == PUState_signature) {
							जरा_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} अन्यथा यदि (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = जरा_listing_2;
						current_state = PUState_signature;
						def_strip_गन्ती = first_pos;
						catch_verific = असत्य;
					} अन्यथा यदि (IsDedent) {
						def_strip_गन्ती = first_pos;
						यदि (current_state == PUState_optionbody) {
							यदि (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							यदि (IsIndent) {
								current_state = PUState_options;
								current_listing = जरा_listing_2->back();
							} अन्यथा {
								current_state = PUState_none;
								current_listing = जरा_listing_2;
							}
						} अन्यथा {
							current_state = PUState_none;
						}
					}

					यदि (IsDefinition && !catch_verific && current_state != PUState_signature) {
						यदि (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = जरा_listing_2->open_option(stripped_पङ्क्ति, null_source);
						def_strip_गन्ती = first_pos;
					} अन्यथा {
						यदि (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						यदि (current_buffer.empty())
							current_buffer = stripped_पङ्क्ति;
						अन्यथा यदि (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_पङ्क्ति;
						अन्यथा यदि (current_state == PUState_none) {
							current_buffer += (blank_पङ्क्तिहरू > 0 ? "\n\n" : "\n") + पङ्क्ति;
						} अन्यथा
							current_buffer += (blank_पङ्क्तिहरू > 0 ? "\n\n" : "\n") + stripped_पङ्क्ति;
						यदि (stripped_पङ्क्ति.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = सत्य;
					}
					blank_पङ्क्तिहरू = 0;
				}

				यदि (!current_buffer.empty()) {
					यदि (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} अन्यथा
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			तार्किक has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			स्वतः skip_source_group = असत्य;

			यदि (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_जरा, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			यदि (!cmd_help.has_group()) {
				यदि (pass->internal_flag)
					cmd_help.group = "internal";
				अन्यथा यदि (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				अन्यथा यदि (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				अन्यथा यदि (has_source) {
					यदि (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				अन्यथा यदि (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				अन्यथा यदि (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				अन्यथा यदि (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				अन्यथा यदि (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				अन्यथा यदि (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			यदि (groups.गन्ती(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].थप_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			लागि (स्वतः &content : cmd_help)
				json.मान(content.to_json(source_जरा));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.पङ्क्ति());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		फर्काऊ raise_error;
	}
	तार्किक dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		तार्किक raise_error = असत्य;
		लागि (स्वतः it : StaticCellTypes::builder.cells) {
			यदि (!StaticCellTypes::categories.is_known(it.type))
				जारी;
			स्वतः name = ID::str(it.type);
			यदि (cell_help_messages.contains(name)) {
				स्वतः cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				स्वतः cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} अन्यथा {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= सत्य;
			}
		}
		लागि (स्वतः &it : cell_help_messages.cell_help) {
			यदि (cells.गन्ती(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		लागि (स्वतः &it : groups) {
			json.name(it.first.c_str()); json.मान(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		लागि (स्वतः &it : cells) {
			स्वतः ch = it.second.first;
			स्वतः ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.मान(ch.title);
			json.name("ports"); json.मान(ch.ports);
			json.name("source"); json.मान(ch.source);
			json.name("desc"); json.मान(ch.desc);
			json.name("code"); json.मान(ch.code);
			vector<string> inputs, outputs;
			लागि (स्वतः &आगत : ct.inputs)
				inputs.थप_back(ID::str(आगत));
			json.name("inputs"); json.मान(inputs);
			लागि (स्वतः &output : ct.outputs)
				outputs.थप_back(ID::str(output));
			json.name("outputs"); json.मान(outputs);
			vector<string> properties;
			// CellType properties
			यदि (ct.features.is_evaluable) properties.थप_back("is_evaluable");
			यदि (ct.features.is_combinatorial) properties.थप_back("is_combinatorial");
			यदि (ct.features.is_synthesizable) properties.थप_back("is_synthesizable");
			// SimHelper properties
			माप_प्रकार last = 0; माप_प्रकार next = 0;
			जबसम्म ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.थप_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			स्वतः final_tag = ch.tags.substr(last);
			यदि (final_tag.size()) properties.थप_back(final_tag);
			json.name("properties"); json.मान(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		फर्काऊ raise_error;
	}
	शून्य execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		यदि (args.size() == 1) {
			log("\n");
			लागि (स्वतः &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			फर्काऊ;
		}

		यदि (args.size() == 2) {
			यदि (args[1] == "-all") {
				लागि (स्वतः &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string सन्देश;
					लागि (माप_प्रकार i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						सन्देश += '=';
					log("%s\n", सन्देश);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			अन्यथा यदि (args[1] == "-cells") {
				log("\n");
				लागि (स्वतः &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				फर्काऊ;
			}
			अन्यथा यदि (pass_register.गन्ती(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			अन्यथा यदि (cell_help_messages.contains(args[1])) {
				स्वतः help_cell = cell_help_messages.get(args[1]);
				यदि (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} अन्यथा {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					यदि (help_cell.ver == "2" || help_cell.ver == "2a") {
						यदि (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						लागि (std::string पङ्क्ति; std::getline(ss, पङ्क्ति, '\n');) {
							यदि (पङ्क्ति != "::") log("%s\n", पङ्क्ति);
						}
					} अन्यथा यदि (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} अन्यथा {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			अन्यथा
				log("No such command or cell type: %s\n", args[1]);
			फर्काऊ;
		} अन्यथा यदि (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			यदि (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				यदि (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], म_त्रुटि(त्रुटि_सं));
				यदि (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			अन्यथा यदि (args[1] == "-dump-cells-json") {
				PrettyJson json;
				यदि (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], म_त्रुटि(त्रुटि_सं));
				यदि (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			अन्यथा
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			फर्काऊ;
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
		यदि (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		यदि (args.size() == 2) {
			यदि (args[1] == "on")
				echo_mode = सत्य;
			अन्यथा यदि (args[1] == "off")
				echo_mode = असत्य;
			अन्यथा
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
		फर्काऊ नूतन ezMiniSAT();
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

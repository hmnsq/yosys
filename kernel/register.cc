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

#समावेश "kernel/yosys.h"
#समावेश "kernel/satgen.h"
#समावेश "kernel/json.h"
#समावेश "kernel/gzip.h"
#समावेश "kernel/log_help.h"
#समावेश "kernel/newcelltypes.h"

#समावेश <string.h>
#समावेश <stdlib.h>
#समावेश <stdio.h>
#समावेश <errno.h>
#समावेश <filesystem>

YOSYS_NAMESPACE_BEGIN

#घोषणा MAX_REG_गिनती 1000

bool echo_mode = false;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::अग्र_args;

bool GarbageCollectionGuard::is_enabled_ = true;

अटल bool garbage_collection_requested = false;

व्योम request_garbage_collection()
{
	garbage_collection_requested = true;
}

व्योम try_collect_garbage()
{
	अगर (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		वापस;
	garbage_collection_requested = false;
	अगर (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	अग्र_queued_pass = first_queued_pass;
	first_queued_pass = यह;
	call_गणक = 0;
	runtime_ns = 0;
}

व्योम Pass::run_register()
{
	अगर (pass_register.गिनती(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = यह;
}

व्योम Pass::init_register()
{
	vector<Pass*> added_passes;
	जबतक (first_queued_pass) {
		added_passes.धकेलो_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->अग्र_queued_pass;
	}
	क्रम (स्वतः added_pass : added_passes)
		added_pass->on_register();
}

व्योम Pass::done_register()
{
	क्रम (स्वतः &it : pass_register)
		it.second->on_shutdown();

	frontend_register.सफा();
	pass_register.सफा();
	backend_register.सफा();
	log_assert(first_queued_pass == शून्य);
}

व्योम Pass::on_register()
{
}

व्योम Pass::on_shutdown()
{
}

Pass::~Pass()
{
}

Pass::pre_post_exec_state_t Pass::pre_execute()
{
	pre_post_exec_state_t state;
	call_गणक++;
	state.begin_ns = PerformanceTimer::query();
	state.parent_pass = current_pass;
	current_pass = यह;
	clear_flags();
	वापस state;
}

व्योम Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

व्योम Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	अगर (current_pass)
		current_pass->runtime_ns -= time_ns;
}

व्योम Pass::help()
{
	स्वतः prettyHelp = PrettyHelp();
	अगर (formatted_help()) {
		prettyHelp.log_help();
	} अन्यथा {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

bool Pass::formatted_help()
{
	वापस false;
}

व्योम Pass::clear_flags()
{
}

व्योम Pass::cmd_log_args(स्थिर std::vector<std::string> &args)
{
	अगर (args.आकार() <= 1)
		वापस;
	std::string message = "Full command line:";
	क्रम (माप_प्रकार i = 0; i < args.आकार(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

व्योम Pass::cmd_error(स्थिर std::vector<std::string> &args, माप_प्रकार argidx, std::string संदेश)
{
	std::string command_text;
	पूर्णांक error_pos = 0;

	क्रम (माप_प्रकार i = 0; i < args.आकार(); i++) {
		अगर (i < argidx)
			error_pos += args[i].आकार() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			संदेश.c_str(), command_text.c_str(), error_pos, "");
}

व्योम Pass::extra_args(std::vector<std::string> args, माप_प्रकार argidx, RTLIL::Design *design, bool select)
{
	क्रम (; argidx < args.आकार(); argidx++)
	{
		std::string arg = args[argidx];

		अगर (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		अगर (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(यह, args, argidx, args.आकार(), design);
		अवरोध;
	}
	// cmd_log_args(args);
}

व्योम Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = अग्र_token(cmd_buf, " \t\r\n", true);

	अगर (tok.empty())
		वापस;

	अगर (tok[0] == '!') {
#अगर defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		जबतक (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.आकार()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		पूर्णांक retCode = run_command(cmd_buf);
		अगर (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		वापस;
#अन्यथा
		log_cmd_error("Shell is not available.\n");
#पूर्ण_अगर
	}

	जबतक (!tok.empty()) {
		अगर (tok[0] == '#') {
			पूर्णांक stop;
			क्रम (stop = 0; stop < Get_आकार(cmd_buf); stop++)
				अगर (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					अवरोध;
			cmd_buf = cmd_buf.substr(stop);
		} अन्यथा
		अगर (tok.back() == ';') {
			पूर्णांक अंक_semikolon = 0;
			जबतक (!tok.empty() && tok.back() == ';')
				tok.resize(tok.आकार()-1), अंक_semikolon++;
			अगर (!tok.empty())
				args.धकेलो_back(tok);
			call(design, args);
			args.सफा();
			अगर (अंक_semikolon == 2)
				call(design, "clean");
			अगर (अंक_semikolon == 3)
				call(design, "clean -purge");
		} अन्यथा
			args.धकेलो_back(tok);
		bool found_nl = false;
		क्रम (स्वतः c : cmd_buf) {
			अगर (c == ' ' || c == '\t')
				जारी;
			अगर (c == '\r' || c == '\n')
				found_nl = true;
			अवरोध;
		}
		अगर (found_nl) {
			call(design, args);
			args.सफा();
		}
		tok = अग्र_token(cmd_buf, " \t\r\n", true);
	}

	call(design, args);
}

व्योम Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	अगर (args.आकार() == 0 || args[0][0] == '#' || args[0][0] == ':')
		वापस;

	अगर (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, false));
		क्रम (माप_प्रकार i = 0; i < args.आकार(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	अगर (pass_register.गिनती(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	अगर (pass->experimental_flag)
		log_experimental(args[0]);

	माप_प्रकार orig_sel_ढेर_pos = design->selection_ढेर.आकार();
	स्वतः state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	जबतक (design->selection_ढेर.आकार() > orig_sel_ढेर_pos)
		design->निकालो_selection();
}

व्योम Pass::call_on_selection(RTLIL::Design *design, स्थिर RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.सफा();
	design->धकेलो_selection(selection);

	Pass::call(design, command);

	design->निकालो_selection();
	design->selected_active_module = backup_selected_active_module;
}

व्योम Pass::call_on_selection(RTLIL::Design *design, स्थिर RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.सफा();
	design->धकेलो_selection(selection);

	Pass::call(design, args);

	design->निकालो_selection();
	design->selected_active_module = backup_selected_active_module;
}

व्योम Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->धकेलो_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->निकालो_selection();
	design->selected_active_module = backup_selected_active_module;
}

व्योम Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->धकेलो_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->निकालो_selection();
	design->selected_active_module = backup_selected_active_module;
}

bool ScriptPass::check_label(std::string label, std::string info)
{
	अगर (active_design == nullptr) {
		log("\n");
		अगर (info.empty())
			log("    %s:\n", label);
		अन्यथा
			log("    %s:    %s\n", label, info);
		वापस true;
	} अन्यथा {
		अगर (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} अन्यथा {
			अगर (label == active_run_from)
				block_active = true;
			अगर (label == active_run_to)
				block_active = false;
		}
		वापस block_active;
	}
}

व्योम ScriptPass::run(std::string command, std::string info)
{
	अगर (active_design == nullptr) {
		अगर (info.empty())
			log("        %s\n", command);
		अन्यथा
			log("        %s    %s\n", command, info);
	} अन्यथा {
		Pass::call(active_design, command);
		active_design->check();
	}
}

व्योम ScriptPass::run_nocheck(std::string command, std::string info)
{
	अगर (active_design == nullptr) {
		अगर (info.empty())
			log("        %s\n", command);
		अन्यथा
			log("        %s    %s\n", command, info);
	} अन्यथा {
		Pass::call(active_design, command);
	}
}

व्योम ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = false;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

व्योम ScriptPass::help_script()
{
	clear_flags();
	help_mode = true;
	active_design = nullptr;
	block_active = true;
	active_run_from.सफा();
	active_run_to.सफा();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

व्योम Frontend::run_register()
{
	अगर (pass_register.गिनती(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = यह;

	अगर (frontend_register.गिनती(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = यह;
}

Frontend::~Frontend()
{
}

व्योम Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(अग्र_args.empty());
	करो {
		std::अ_प्रवाह *f = शून्य;
		अग्र_args.सफा();
		स्वतः state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = अग्र_args;
		मिटाओ f;
	} जबतक (!args.empty());
}

खाता *Frontend::current_script_file = शून्य;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
पूर्णांक Frontend::current_script_lineno = 0;

व्योम Frontend::extra_args(std::अ_प्रवाह *&f, std::string &filename, std::vector<std::string> args, माप_प्रकार argidx, bool bin_निवेश)
{
	bool called_with_fp = f != शून्य;

	अग्र_args.सफा();

	अगर (argidx < args.आकार())
	{
		std::string arg = args[argidx];

		अगर (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		अगर (f != शून्य)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		अगर (filename == "<<" && argidx+1 < args.आकार())
			filename += args[++argidx];
		अगर (filename.compare(0, 2, "<<") == 0) {
			अगर (filename.आकार() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.सफा();
			जबतक (1) {
				std::string बफर;
				अक्षर block[4096];
				जबतक (1) {
					अगर (ख_माला_लो(block, 4096, Frontend::current_script_file == nullptr? मानक_निवेश : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					बफर += block;
					अगर (बफर.आकार() > 0 && (बफर[बफर.आकार() - 1] == '\n' || बफर[बफर.आकार() - 1] == '\r'))
						अवरोध;
				}
				Frontend::current_script_lineno++;
				माप_प्रकार indent = बफर.find_first_not_of(" \t\r\n");
				अगर (indent != std::string::npos && बफर.compare(indent, eot_marker.आकार(), eot_marker) == 0)
					अवरोध;
				last_here_document += बफर;
			}
			f = नया std::istringstream(last_here_document);
		} अन्यथा {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			अगर (Get_आकार(filenames) > 1) {
				अग्र_args.जोड़ो(अग्र_args.end(), args.begin(), args.begin()+argidx);
				अग्र_args.जोड़ो(अग्र_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_निवेश_files.जोड़ो(filename);
			f = uncompressed(filename, bin_निवेश ? std::अख_प्रवाह::द्विग : std::अख_प्रवाह::in);
		}

		क्रम (माप_प्रकार i = argidx+1; i < args.आकार(); i++)
			अगर (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		अगर (argidx+1 < args.आकार()) {
			अगर (अग्र_args.empty())
				अग्र_args.जोड़ो(अग्र_args.end(), args.begin(), args.begin()+argidx);
			अग्र_args.जोड़ो(अग्र_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	अगर (f == शून्य)
		cmd_error(args, argidx, "No filename given.");

	अगर (called_with_fp)
		args.धकेलो_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

व्योम Frontend::frontend_call(RTLIL::Design *design, std::अ_प्रवाह *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	अक्षर *s = strdup(command.c_str());
	क्रम (अक्षर *p = म_मोहर(s, " \t\r\n"); p; p = म_मोहर(शून्य, " \t\r\n"))
		args.धकेलो_back(p);
	मुक्त(s);
	frontend_call(design, f, filename, args);
}

व्योम Frontend::frontend_call(RTLIL::Design *design, std::अ_प्रवाह *f, std::string filename, std::vector<std::string> args)
{
	अगर (args.आकार() == 0)
		वापस;
	अगर (frontend_register.गिनती(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	अगर (f != शून्य) {
		स्वतः state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} अन्यथा अगर (filename == "-") {
		std::अ_प्रवाह *f_cin = &std::अ_अंदर;
		स्वतः state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} अन्यथा {
		अगर (!filename.empty())
			args.धकेलो_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

व्योम Backend::run_register()
{
	अगर (pass_register.गिनती(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = यह;

	अगर (backend_register.गिनती(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = यह;
}

Backend::~Backend()
{
}

व्योम Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ब_प्रवाह *f = शून्य;
	स्वतः state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	अगर (f != &std::अ_बाहर)
		मिटाओ f;
}

व्योम Backend::extra_args(std::ब_प्रवाह *&f, std::string &filename, std::vector<std::string> args, माप_प्रकार argidx, bool bin_output)
{
	bool called_with_fp = f != शून्य;

	क्रम (; argidx < args.आकार(); argidx++)
	{
		std::string arg = args[argidx];

		अगर (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		अगर (f != शून्य)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		अगर (arg == "-") {
			filename = "<stdout>";
			f = &std::अ_बाहर;
			जारी;
		}

		filename = arg;
		rewrite_filename(filename);
		अगर (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".gz") == 0) {
#अगर_घोषित YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = नया gzip_ostream;
			अगर (!gf->खोलो(filename)) {
				मिटाओ gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, म_त्रुटि(त्रुटि_सं));
			}
			yosys_output_files.जोड़ो(filename);
			f = gf;
#अन्यथा
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#पूर्ण_अगर
		} अन्यथा {
			std::बख_प्रवाह *ff = नया std::बख_प्रवाह;
			ff->खोलो(filename.c_str(), bin_output ? (std::बख_प्रवाह::छाँटो | std::बख_प्रवाह::द्विग) : std::बख_प्रवाह::छाँटो);
			yosys_output_files.जोड़ो(filename);
			अगर (ff->हार()) {
				मिटाओ ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, म_त्रुटि(त्रुटि_सं));
			}
			f = ff;
		}
	}

	अगर (called_with_fp)
		args.धकेलो_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	अगर (f == शून्य) {
		filename = "<stdout>";
		f = &std::अ_बाहर;
	}
}

व्योम Backend::backend_call(RTLIL::Design *design, std::ब_प्रवाह *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	अक्षर *s = strdup(command.c_str());
	क्रम (अक्षर *p = म_मोहर(s, " \t\r\n"); p; p = म_मोहर(शून्य, " \t\r\n"))
		args.धकेलो_back(p);
	मुक्त(s);
	backend_call(design, f, filename, args);
}

व्योम Backend::backend_call(RTLIL::Design *design, std::ब_प्रवाह *f, std::string filename, std::vector<std::string> args)
{
	अगर (args.आकार() == 0)
		वापस;
	अगर (backend_register.गिनती(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	माप_प्रकार orig_sel_ढेर_pos = design->selection_ढेर.आकार();

	अगर (f != शून्य) {
		स्वतः state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} अन्यथा अगर (filename == "-") {
		std::ब_प्रवाह *f_cout = &std::अ_बाहर;
		स्वतः state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} अन्यथा {
		अगर (!filename.empty())
			args.धकेलो_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	जबतक (design->selection_ढेर.आकार() > orig_sel_ढेर_pos)
		design->निकालो_selection();
}

काष्ठा SimHelper {
	string name;
	अंतरभूत string filesafe_name() {
		अगर (name.at(0) == '$')
			अगर (name.at(1) == '_')
				वापस "gate" + name.substr(1);
			अन्यथा
				वापस "word_" + name.substr(1);
		अन्यथा
			वापस name;
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

अटल bool is_code_getter(string name) {
	वापस *(--(name.end())) == '+';
}

अटल string get_cell_name(string name) {
	वापस is_code_getter(name) ? name.substr(0, name.लंबाई()-1) : name;
}

अटल व्योम log_warning_flags(Pass *pass) {
	bool has_warnings = false;
	स्थिर string name = pass->pass_name;
	अगर (pass->experimental_flag) {
		अगर (!has_warnings) log("\n");
		has_warnings = true;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	अगर (pass->internal_flag) {
		अगर (!has_warnings) log("\n");
		has_warnings = true;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	अगर (has_warnings)
		log("\n");
}

अटल काष्ठा CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#समावेश "kernel/simlib_help.inc"
#समावेश "kernel/simcells_help.inc"
		cell_help.sort();
	}
	bool contains(string name) { वापस cell_help.गिनती(get_cell_name(name)) > 0; }
	SimHelper लो(string name) { वापस cell_help[get_cell_name(name)]; }
} cell_help_messages;

काष्ठा HelpPass : खुला Pass {
	HelpPass() : Pass("help", "display help messages") { }
	व्योम help() override
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
	bool dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		bool raise_error = false;
		std::map<string, vector<string>> groups;

		// get root path
		स्वतः this_path = std::filesystem::path(source_location::current().file_name());
		स्वतः source_मूल = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		क्रम (स्वतः &it : pass_register) {
			स्वतः name = it.first;
			स्वतः pass = it.second;
			स्वतः title = pass->short_help;

			स्वतः cmd_help = PrettyHelp();
			स्वतः has_pretty_help = pass->formatted_help();

			अगर (!has_pretty_help) {
				क्रमागत PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_बफर = "";
				स्वतः मूल_listing_2 = cmd_help.get_मूल();
				स्वतः current_listing = मूल_listing_2;

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
				माप_प्रकार def_strip_गिनती = 0;
				स्वतः current_state = PUState_none;
				स्वतः catch_verific = false;
				स्वतः blank_पंक्तियाँ = 2;
				क्रम (string पंक्ति; std::लो_पंक्ति(ss, पंक्ति, '\n');) {
					// find position of first non space character
					std::माप_प्रकार first_pos = पंक्ति.find_first_not_of(" \t");
					std::माप_प्रकार last_pos = पंक्ति.find_last_not_of(" \t");
					अगर (first_pos == std::string::npos) {
						चयन (current_state)
						{
						हाल PUState_signature:
							मूल_listing_2->usage(current_बफर, null_source);
							current_listing = मूल_listing_2;
							current_state = PUState_none;
							current_बफर = "";
							अवरोध;
						हाल PUState_none:
						हाल PUState_optionbody:
							blank_पंक्तियाँ += 1;
							अवरोध;
						शेष:
							अवरोध;
						}
						// skip empty lines
						जारी;
					}

					// strip leading and trailing whitespace
					std::string stripped_पंक्ति = पंक्ति.substr(first_pos, last_pos - first_pos +1);
					bool IsDefinition = stripped_पंक्ति[0] == '-';
					IsDefinition &= stripped_पंक्ति[1] != ' ' && stripped_पंक्ति[1] != '>';
					bool IsDedent = def_strip_गिनती && first_pos < def_strip_गिनती;
					bool IsIndent = def_strip_गिनती < first_pos;

					// line looks like a signature
					bool IsSignature = stripped_पंक्ति.find(name) == 0 && (stripped_पंक्ति.लंबाई() == name.लंबाई() || stripped_पंक्ति.at(name.आकार()) == ' ');

					अगर (IsSignature && first_pos <= 4 && (blank_पंक्तियाँ >= 2 || current_state == PUState_signature)) {
						अगर (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_बफर, "none", null_source);
							current_बफर = "";
						} अन्यथा अगर (current_state == PUState_signature) {
							मूल_listing_2->usage(current_बफर, null_source);
							current_बफर = "";
						} अन्यथा अगर (current_state == PUState_none && !current_बफर.empty()) {
							current_listing->codeblock(current_बफर, "none", null_source);
							current_बफर = "";
						}
						current_listing = मूल_listing_2;
						current_state = PUState_signature;
						def_strip_गिनती = first_pos;
						catch_verific = false;
					} अन्यथा अगर (IsDedent) {
						def_strip_गिनती = first_pos;
						अगर (current_state == PUState_optionbody) {
							अगर (!current_बफर.empty()) {
								current_listing->codeblock(current_बफर, "none", null_source);
								current_बफर = "";
							}
							अगर (IsIndent) {
								current_state = PUState_options;
								current_listing = मूल_listing_2->back();
							} अन्यथा {
								current_state = PUState_none;
								current_listing = मूल_listing_2;
							}
						} अन्यथा {
							current_state = PUState_none;
						}
					}

					अगर (IsDefinition && !catch_verific && current_state != PUState_signature) {
						अगर (!current_बफर.empty()) {
							current_listing->codeblock(current_बफर, "none", null_source);
							current_बफर = "";
						}
						current_state = PUState_options;
						current_listing = मूल_listing_2->open_option(stripped_पंक्ति, null_source);
						def_strip_गिनती = first_pos;
					} अन्यथा {
						अगर (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						अगर (current_बफर.empty())
							current_बफर = stripped_पंक्ति;
						अन्यथा अगर (current_state == PUState_signature && IsIndent)
							current_बफर += stripped_पंक्ति;
						अन्यथा अगर (current_state == PUState_none) {
							current_बफर += (blank_पंक्तियाँ > 0 ? "\n\n" : "\n") + पंक्ति;
						} अन्यथा
							current_बफर += (blank_पंक्तियाँ > 0 ? "\n\n" : "\n") + stripped_पंक्ति;
						अगर (stripped_पंक्ति.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = true;
					}
					blank_पंक्तियाँ = 0;
				}

				अगर (!current_बफर.empty()) {
					अगर (current_बफर.आकार() > 64 && current_बफर.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_बफर.substr(0, 62), null_source);
						current_listing->codeblock(current_बफर.substr(64), "yoscrypt", null_source);
					} अन्यथा
						current_listing->codeblock(current_बफर, "none", null_source);
					current_बफर = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			bool has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			स्वतः skip_source_group = false;

			अगर (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_मूल, &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			अगर (!cmd_help.has_group()) {
				अगर (pass->internal_flag)
					cmd_help.group = "internal";
				अन्यथा अगर (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				अन्यथा अगर (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				अन्यथा अगर (has_source) {
					अगर (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				अन्यथा अगर (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				अन्यथा अगर (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				अन्यथा अगर (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				अन्यथा अगर (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				अन्यथा अगर (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			अगर (groups.गिनती(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].धकेलो_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			क्रम (स्वतः &content : cmd_help)
				json.मूल्य(content.to_json(source_मूल));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.पंक्ति());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		वापस raise_error;
	}
	bool dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		bool raise_error = false;
		क्रम (स्वतः it : StaticCellTypes::builder.cells) {
			अगर (!StaticCellTypes::categories.is_known(it.type))
				जारी;
			स्वतः name = ID::str(it.type);
			अगर (cell_help_messages.contains(name)) {
				स्वतः cell_help = cell_help_messages.लो(name);
				groups[cell_help.group].emplace_back(name);
				स्वतः cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} अन्यथा {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= true;
			}
		}
		क्रम (स्वतः &it : cell_help_messages.cell_help) {
			अगर (cells.गिनती(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		क्रम (स्वतः &it : groups) {
			json.name(it.first.c_str()); json.मूल्य(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		क्रम (स्वतः &it : cells) {
			स्वतः ch = it.second.first;
			स्वतः ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.मूल्य(ch.title);
			json.name("ports"); json.मूल्य(ch.ports);
			json.name("source"); json.मूल्य(ch.source);
			json.name("desc"); json.मूल्य(ch.desc);
			json.name("code"); json.मूल्य(ch.code);
			vector<string> inputs, outputs;
			क्रम (स्वतः &निवेश : ct.inputs)
				inputs.धकेलो_back(ID::str(निवेश));
			json.name("inputs"); json.मूल्य(inputs);
			क्रम (स्वतः &output : ct.outputs)
				outputs.धकेलो_back(ID::str(output));
			json.name("outputs"); json.मूल्य(outputs);
			vector<string> properties;
			// CellType properties
			अगर (ct.features.is_evaluable) properties.धकेलो_back("is_evaluable");
			अगर (ct.features.is_combinatorial) properties.धकेलो_back("is_combinatorial");
			अगर (ct.features.is_synthesizable) properties.धकेलो_back("is_synthesizable");
			// SimHelper properties
			माप_प्रकार last = 0; माप_प्रकार अग्र = 0;
			जबतक ((अग्र = ch.tags.find(", ", last)) != string::npos) {
				properties.धकेलो_back(ch.tags.substr(last, अग्र-last));
				last = अग्र + 2;
			}
			स्वतः final_tag = ch.tags.substr(last);
			अगर (final_tag.आकार()) properties.धकेलो_back(final_tag);
			json.name("properties"); json.मूल्य(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		वापस raise_error;
	}
	व्योम execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		अगर (args.आकार() == 1) {
			log("\n");
			क्रम (स्वतः &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			वापस;
		}

		अगर (args.आकार() == 2) {
			अगर (args[1] == "-all") {
				क्रम (स्वतः &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string संदेश;
					क्रम (माप_प्रकार i = 0; i < it.first.आकार() + it.second->short_help.आकार() + 6; i++)
						संदेश += '=';
					log("%s\n", संदेश);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			अन्यथा अगर (args[1] == "-cells") {
				log("\n");
				क्रम (स्वतः &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				वापस;
			}
			अन्यथा अगर (pass_register.गिनती(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			अन्यथा अगर (cell_help_messages.contains(args[1])) {
				स्वतः help_cell = cell_help_messages.लो(args[1]);
				अगर (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} अन्यथा {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					अगर (help_cell.ver == "2" || help_cell.ver == "2a") {
						अगर (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						क्रम (std::string पंक्ति; std::लो_पंक्ति(ss, पंक्ति, '\n');) {
							अगर (पंक्ति != "::") log("%s\n", पंक्ति);
						}
					} अन्यथा अगर (help_cell.desc.लंबाई()) {
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
			वापस;
		} अन्यथा अगर (args.आकार() == 3) {
			// this option is undocumented as it is for internal use only
			अगर (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				अगर (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], म_त्रुटि(त्रुटि_सं));
				अगर (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			अन्यथा अगर (args[1] == "-dump-cells-json") {
				PrettyJson json;
				अगर (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], म_त्रुटि(त्रुटि_सं));
				अगर (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			अन्यथा
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			वापस;
		}

		help();
	}
} HelpPass;

काष्ठा EchoPass : खुला Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	व्योम help() override
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
	व्योम execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		अगर (args.आकार() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		अगर (args.आकार() == 2) {
			अगर (args[1] == "on")
				echo_mode = true;
			अन्यथा अगर (args[1] == "off")
				echo_mode = false;
			अन्यथा
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

काष्ठा MinisatSatSolver : खुला SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = यह;
	}
	ezSAT *create() override {
		वापस नया ezMiniSAT();
	}
} MinisatSatSolver;

काष्ठा LicensePass : खुला Pass {
	LicensePass() : Pass("license", "print license terms") { }
	व्योम help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	व्योम execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	व्योम notice()
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

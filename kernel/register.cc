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

#परिभाषय MAX_REG_गणनम् 1000

तार्किक echo_बहुलकम् = असत्य;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_पञ्जिकाकोशः;
std::map<std::string, Pass*> pass_पञ्जिकाकोशः;
std::map<std::string, Backend*> backend_पञ्जिकाकोशः;
std::map<std::string, Target> target_पञ्जिकाकोशः;

std::सदिशम्<std::string> Frontend::अग्र_तर्काः;

तार्किक GarbageCollectionGuard::is_enabled_ = सत्य;

स्थिर तार्किक garbage_collection_requested = असत्य;

शून्य request_garbage_collection()
{
	garbage_collection_requested = सत्य;
}

शून्य try_collect_garbage()
{
	यदि (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		प्रत्यागम;
	garbage_collection_requested = असत्य;
	यदि (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	अग्र_queued_pass = first_queued_pass;
	first_queued_pass = एतत्;
	call_गणक = 0;
	runtime_ns = 0;
}

शून्य Pass::सततम्_पञ्जिकाकोशः()
{
	यदि (pass_पञ्जिकाकोशः.गणनम्(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_पञ्जिकाकोशः[pass_name] = एतत्;
}

शून्य Pass::init_पञ्जिकाकोशः()
{
	सदिशम्<Pass*> added_passes;
	यावत् (first_queued_pass) {
		added_passes.प्रक्षिप_पृष्ठभागः(first_queued_pass);
		first_queued_pass->सततम्_पञ्जिकाकोशः();
		first_queued_pass = first_queued_pass->अग्र_queued_pass;
	}
	प्रति (स्वचालित added_pass : added_passes)
		added_pass->on_पञ्जिकाकोशः();
}

शून्य Pass::समाप्तम्_पञ्जिकाकोशः()
{
	प्रति (स्वचालित &it : pass_पञ्जिकाकोशः)
		it.second->on_shutdown();

	frontend_पञ्जिकाकोशः.clear();
	pass_पञ्जिकाकोशः.clear();
	backend_पञ्जिकाकोशः.clear();
	log_assert(first_queued_pass == रिक्त);
}

शून्य Pass::on_पञ्जिकाकोशः()
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
	call_गणक++;
	state.begin_ns = PerformanceTimer::query();
	state.parent_pass = current_pass;
	current_pass = एतत्;
	clear_flags();
	प्रत्यागम state;
}

शून्य Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	व्यवकलय_from_current_runtime_ns(time_ns);
}

शून्य Pass::व्यवकलय_from_current_runtime_ns(int64_t time_ns)
{
	यदि (current_pass)
		current_pass->runtime_ns -= time_ns;
}

शून्य Pass::help()
{
	स्वचालित prettyHelp = PrettyHelp();
	यदि (formatted_help()) {
		prettyHelp.log_help();
	} अन्यथा {
		लघुगणकः("\n");
		लघुगणकः("No help message for command `%s'.\n", pass_name);
		लघुगणकः("\n");
	}
}

तार्किक Pass::formatted_help()
{
	प्रत्यागम असत्य;
}

शून्य Pass::clear_flags()
{
}

शून्य Pass::cmd_log_तर्काः(ध्रुव std::सदिशम्<std::string> &तर्काः_2)
{
	यदि (तर्काः_2.आकार() <= 1)
		प्रत्यागम;
	std::string सन्देशः_2 = "Full command line:";
	प्रति (आकार_t i = 0; i < तर्काः_2.आकार(); i++)
		सन्देशः_2 += stringf(" %s", तर्काः_2[i]);
	लघुगणकः("%s\n", सन्देशः_2);
}

शून्य Pass::cmd_error(ध्रुव std::सदिशम्<std::string> &तर्काः_2, आकार_t argidx, std::string सन्देशः)
{
	std::string command_text;
	पूर्णाङ्क error_pos = 0;

	प्रति (आकार_t i = 0; i < तर्काः_2.आकार(); i++) {
		यदि (i < argidx)
			error_pos += तर्काः_2[i].आकार() + 1;
		command_text = command_text + (command_text.रिक्तानि() ? "" : " ") + तर्काः_2[i];
	}

	लघुगणकः("\n");
	लघुगणकः("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			सन्देशः.c_str(), command_text.c_str(), error_pos, "");
}

शून्य Pass::extra_तर्काः(std::सदिशम्<std::string> तर्काः_2, आकार_t argidx, RTLIL::Design *design, तार्किक select)
{
	प्रति (; argidx < तर्काः_2.आकार(); argidx++)
	{
		std::string arg = तर्काः_2[argidx];

		यदि (arg.compare(0, 1, "-") == 0)
			cmd_error(तर्काः_2, argidx, "Unknown option or option in arguments.");

		यदि (!select)
			cmd_error(तर्काः_2, argidx, "Extra argument.");

		handle_extra_select_तर्काः(एतत्, तर्काः_2, argidx, तर्काः_2.आकार(), design);
		भङ्ग;
	}
	// cmd_log_args(args);
}

शून्य Pass::call(RTLIL::Design *design, std::string command)
{
	std::सदिशम्<std::string> तर्काः_2;

	std::string cmd_buf = command;
	std::string tok = अग्र_token(cmd_buf, " \t\r\n", सत्य);

	यदि (tok.रिक्तानि())
		प्रत्यागम;

	यदि (tok[0] == '!') {
#यदि defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		यावत् (!cmd_buf.रिक्तानि() && (cmd_buf.पृष्ठभागः() == ' ' || cmd_buf.पृष्ठभागः() == '\t' ||
				cmd_buf.पृष्ठभागः() == '\r' || cmd_buf.पृष्ठभागः() == '\n'))
			cmd_buf.resize(cmd_buf.आकार()-1);
		log_शीर्षकम्(design, "Shell command: %s\n", cmd_buf);
		पूर्णाङ्क ret_संहिता = सततम्_command(cmd_buf);
		यदि (ret_संहिता != 0)
			log_cmd_error("Shell command returned error code %d.\n", ret_संहिता);
		प्रत्यागम;
#अन्यथा
		log_cmd_error("Shell is not available.\n");
#यदि_अन्तः
	}

	यावत् (!tok.रिक्तानि()) {
		यदि (tok[0] == '#') {
			पूर्णाङ्क stop;
			प्रति (stop = 0; stop < Get_आकार(cmd_buf); stop++)
				यदि (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					भङ्ग;
			cmd_buf = cmd_buf.substr(stop);
		} अन्यथा
		यदि (tok.पृष्ठभागः() == ';') {
			पूर्णाङ्क अङ्कः_semikolon = 0;
			यावत् (!tok.रिक्तानि() && tok.पृष्ठभागः() == ';')
				tok.resize(tok.आकार()-1), अङ्कः_semikolon++;
			यदि (!tok.रिक्तानि())
				तर्काः_2.प्रक्षिप_पृष्ठभागः(tok);
			call(design, तर्काः_2);
			तर्काः_2.clear();
			यदि (अङ्कः_semikolon == 2)
				call(design, "clean");
			यदि (अङ्कः_semikolon == 3)
				call(design, "clean -purge");
		} अन्यथा
			तर्काः_2.प्रक्षिप_पृष्ठभागः(tok);
		तार्किक प्राप्तम्_nl = असत्य;
		प्रति (स्वचालित c : cmd_buf) {
			यदि (c == ' ' || c == '\t')
				अनुवर्त;
			यदि (c == '\r' || c == '\n')
				प्राप्तम्_nl = सत्य;
			भङ्ग;
		}
		यदि (प्राप्तम्_nl) {
			call(design, तर्काः_2);
			तर्काः_2.clear();
		}
		tok = अग्र_token(cmd_buf, " \t\r\n", सत्य);
	}

	call(design, तर्काः_2);
}

शून्य Pass::call(RTLIL::Design *design, std::सदिशम्<std::string> तर्काः_2)
{
	यदि (तर्काः_2.आकार() == 0 || तर्काः_2[0][0] == '#' || तर्काः_2[0][0] == ':')
		प्रत्यागम;

	यदि (echo_बहुलकम्) {
		std::string सन्देशः_2 = stringf("%s", create_prompt(design, 0, असत्य));
		प्रति (आकार_t i = 0; i < तर्काः_2.आकार(); i++)
			सन्देशः_2 += stringf("%s%s", i ? " " : "", तर्काः_2[i]);
		लघुगणकः("\n");
		लघुगणकः("%s\n", सन्देशः_2);
	}

	यदि (pass_पञ्जिकाकोशः.गणनम्(तर्काः_2[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", तर्काः_2[0]);
	Pass *pass = pass_पञ्जिकाकोशः[तर्काः_2[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	यदि (pass->experimental_flag)
		log_experimental(तर्काः_2[0]);

	आकार_t orig_sel_राशिः_pos = design->selection_राशिः.आकार();
	स्वचालित state = pass->pre_execute();
	pass->execute(तर्काः_2, design);
	pass->post_execute(state);
	यावत् (design->selection_राशिः.आकार() > orig_sel_राशिः_pos)
		design->निष्कासय_selection();
}

शून्य Pass::call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_सक्रिय_module = design->selected_सक्रिय_module;
	design->selected_सक्रिय_module.clear();
	design->प्रक्षिप_selection(selection);

	Pass::call(design, command);

	design->निष्कासय_selection();
	design->selected_सक्रिय_module = backup_selected_सक्रिय_module;
}

शून्य Pass::call_on_selection(RTLIL::Design *design, ध्रुव RTLIL::Selection &selection, std::सदिशम्<std::string> तर्काः_2)
{
	IdString backup_selected_सक्रिय_module = design->selected_सक्रिय_module;
	design->selected_सक्रिय_module.clear();
	design->प्रक्षिप_selection(selection);

	Pass::call(design, तर्काः_2);

	design->निष्कासय_selection();
	design->selected_सक्रिय_module = backup_selected_सक्रिय_module;
}

शून्य Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_सक्रिय_module = design->selected_सक्रिय_module;
	design->selected_सक्रिय_module = module->name;
	design->प्रक्षिप_रिक्तानि_selection();
	design->select(module);

	Pass::call(design, command);

	design->निष्कासय_selection();
	design->selected_सक्रिय_module = backup_selected_सक्रिय_module;
}

शून्य Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::सदिशम्<std::string> तर्काः_2)
{
	IdString backup_selected_सक्रिय_module = design->selected_सक्रिय_module;
	design->selected_सक्रिय_module = module->name;
	design->प्रक्षिप_रिक्तानि_selection();
	design->select(module);

	Pass::call(design, तर्काः_2);

	design->निष्कासय_selection();
	design->selected_सक्रिय_module = backup_selected_सक्रिय_module;
}

तार्किक ScriptPass::check_label(std::string label, std::string info)
{
	यदि (सक्रिय_design == शून्यसूचकः) {
		लघुगणकः("\n");
		यदि (info.रिक्तानि())
			लघुगणकः("    %s:\n", label);
		अन्यथा
			लघुगणकः("    %s:    %s\n", label, info);
		प्रत्यागम सत्य;
	} अन्यथा {
		यदि (!सक्रिय_सततम्_from.रिक्तानि() && सक्रिय_सततम्_from == सक्रिय_सततम्_गन्तव्यम्) {
			खण्डम्_सक्रिय = (label == सक्रिय_सततम्_from);
		} अन्यथा {
			यदि (label == सक्रिय_सततम्_from)
				खण्डम्_सक्रिय = सत्य;
			यदि (label == सक्रिय_सततम्_गन्तव्यम्)
				खण्डम्_सक्रिय = असत्य;
		}
		प्रत्यागम खण्डम्_सक्रिय;
	}
}

शून्य ScriptPass::सततम्(std::string command, std::string info)
{
	यदि (सक्रिय_design == शून्यसूचकः) {
		यदि (info.रिक्तानि())
			लघुगणकः("        %s\n", command);
		अन्यथा
			लघुगणकः("        %s    %s\n", command, info);
	} अन्यथा {
		Pass::call(सक्रिय_design, command);
		सक्रिय_design->check();
	}
}

शून्य ScriptPass::सततम्_nocheck(std::string command, std::string info)
{
	यदि (सक्रिय_design == शून्यसूचकः) {
		यदि (info.रिक्तानि())
			लघुगणकः("        %s\n", command);
		अन्यथा
			लघुगणकः("        %s    %s\n", command, info);
	} अन्यथा {
		Pass::call(सक्रिय_design, command);
	}
}

शून्य ScriptPass::सततम्_script(RTLIL::Design *design, std::string सततम्_from, std::string सततम्_गन्तव्यम्)
{
	help_बहुलकम् = असत्य;
	सक्रिय_design = design;
	खण्डम्_सक्रिय = सततम्_from.रिक्तानि();
	सक्रिय_सततम्_from = सततम्_from;
	सक्रिय_सततम्_गन्तव्यम् = सततम्_गन्तव्यम्;
	script();
}

शून्य ScriptPass::help_script()
{
	clear_flags();
	help_बहुलकम् = सत्य;
	सक्रिय_design = शून्यसूचकः;
	खण्डम्_सक्रिय = सत्य;
	सक्रिय_सततम्_from.clear();
	सक्रिय_सततम्_गन्तव्यम्.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

शून्य Frontend::सततम्_पञ्जिकाकोशः()
{
	यदि (pass_पञ्जिकाकोशः.गणनम्(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_पञ्जिकाकोशः[pass_name] = एतत्;

	यदि (frontend_पञ्जिकाकोशः.गणनम्(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_पञ्जिकाकोशः[frontend_name] = एतत्;
}

Frontend::~Frontend()
{
}

शून्य Frontend::execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design)
{
	log_assert(अग्र_तर्काः.रिक्तानि());
	कुरु {
		std::istream *f = रिक्त;
		अग्र_तर्काः.clear();
		स्वचालित state = pre_execute();
		execute(f, std::string(), तर्काः_2, design);
		post_execute(state);
		तर्काः_2 = अग्र_तर्काः;
		विलोपय f;
	} यावत् (!तर्काः_2.रिक्तानि());
}

FILE *Frontend::current_script_file = रिक्त;
std::string Frontend::चरमः_अत्र_document;
std::string Frontend::current_script_filename;
पूर्णाङ्क Frontend::current_script_lineno = 0;

शून्य Frontend::extra_तर्काः(std::istream *&f, std::string &filename, std::सदिशम्<std::string> तर्काः_2, आकार_t argidx, तार्किक bin_निवेशः)
{
	तार्किक called_with_fp = f != रिक्त;

	अग्र_तर्काः.clear();

	यदि (argidx < तर्काः_2.आकार())
	{
		std::string arg = तर्काः_2[argidx];

		यदि (arg.compare(0, 1, "-") == 0)
			cmd_error(तर्काः_2, argidx, "Unknown option or option in arguments.");
		यदि (f != रिक्त)
			cmd_error(तर्काः_2, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		यदि (filename == "<<" && argidx+1 < तर्काः_2.आकार())
			filename += तर्काः_2[++argidx];
		यदि (filename.compare(0, 2, "<<") == 0) {
			यदि (filename.आकार() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == शून्यसूचकः) ?  "<stdin>" : "<inlined>";
			चरमः_अत्र_document.clear();
			यावत् (1) {
				std::string बफर;
				वर्ण खण्डम्[4096];
				यावत् (1) {
					यदि (fgets(खण्डम्, 4096, Frontend::current_script_file == शून्यसूचकः? stdin : Frontend::current_script_file) == शून्यसूचकः)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					बफर += खण्डम्;
					यदि (बफर.आकार() > 0 && (बफर[बफर.आकार() - 1] == '\n' || बफर[बफर.आकार() - 1] == '\r'))
						भङ्ग;
				}
				Frontend::current_script_lineno++;
				आकार_t indent = बफर.find_first_not_सम्बन्धि(" \t\r\n");
				यदि (indent != std::string::npos && बफर.compare(indent, eot_marker.आकार(), eot_marker) == 0)
					भङ्ग;
				चरमः_अत्र_document += बफर;
			}
			f = नूतन std::istringstream(चरमः_अत्र_document);
		} अन्यथा {
			rewrite_filename(filename);
			सदिशम्<string> filenames = glob_filename(filename);
			filename = filenames.front();
			यदि (Get_आकार(filenames) > 1) {
				अग्र_तर्काः.निवेशय(अग्र_तर्काः.अन्तः(), तर्काः_2.begin(), तर्काः_2.begin()+argidx);
				अग्र_तर्काः.निवेशय(अग्र_तर्काः.अन्तः(), filenames.begin()+1, filenames.अन्तः());
			}
			yosys_निवेशः_files.निवेशय(filename);
			f = uncompressed(filename, bin_निवेशः ? std::ifstream::द्विमान : std::ifstream::अन्तर्);
		}

		प्रति (आकार_t i = argidx+1; i < तर्काः_2.आकार(); i++)
			यदि (तर्काः_2[i].compare(0, 1, "-") == 0)
				cmd_error(तर्काः_2, i, "Found option, expected arguments.");

		यदि (argidx+1 < तर्काः_2.आकार()) {
			यदि (अग्र_तर्काः.रिक्तानि())
				अग्र_तर्काः.निवेशय(अग्र_तर्काः.अन्तः(), तर्काः_2.begin(), तर्काः_2.begin()+argidx);
			अग्र_तर्काः.निवेशय(अग्र_तर्काः.अन्तः(), तर्काः_2.begin()+argidx+1, तर्काः_2.अन्तः());
			तर्काः_2.erase(तर्काः_2.begin()+argidx+1, तर्काः_2.अन्तः());
		}
	}

	यदि (f == रिक्त)
		cmd_error(तर्काः_2, argidx, "No filename given.");

	यदि (called_with_fp)
		तर्काः_2.प्रक्षिप_पृष्ठभागः(filename);
	तर्काः_2[0] = pass_name;
	// cmd_log_args(args);
}

शून्य Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::सदिशम्<std::string> तर्काः_2;
	वर्ण *s = strdup(command.c_str());
	प्रति (वर्ण *p = strtok(s, " \t\r\n"); p; p = strtok(रिक्त, " \t\r\n"))
		तर्काः_2.प्रक्षिप_पृष्ठभागः(p);
	मुक्त(s);
	frontend_call(design, f, filename, तर्काः_2);
}

शून्य Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::सदिशम्<std::string> तर्काः_2)
{
	यदि (तर्काः_2.आकार() == 0)
		प्रत्यागम;
	यदि (frontend_पञ्जिकाकोशः.गणनम्(तर्काः_2[0]) == 0)
		log_cmd_error("No such frontend: %s\n", तर्काः_2[0]);

	यदि (f != रिक्त) {
		स्वचालित state = frontend_पञ्जिकाकोशः[तर्काः_2[0]]->pre_execute();
		frontend_पञ्जिकाकोशः[तर्काः_2[0]]->execute(f, filename, तर्काः_2, design);
		frontend_पञ्जिकाकोशः[तर्काः_2[0]]->post_execute(state);
	} अन्यथा यदि (filename == "-") {
		std::istream *f_cin = &std::आगमः;
		स्वचालित state = frontend_पञ्जिकाकोशः[तर्काः_2[0]]->pre_execute();
		frontend_पञ्जिकाकोशः[तर्काः_2[0]]->execute(f_cin, "<stdin>", तर्काः_2, design);
		frontend_पञ्जिकाकोशः[तर्काः_2[0]]->post_execute(state);
	} अन्यथा {
		यदि (!filename.रिक्तानि())
			तर्काः_2.प्रक्षिप_पृष्ठभागः(filename);
		frontend_पञ्जिकाकोशः[तर्काः_2[0]]->execute(तर्काः_2, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

शून्य Backend::सततम्_पञ्जिकाकोशः()
{
	यदि (pass_पञ्जिकाकोशः.गणनम्(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_पञ्जिकाकोशः[pass_name] = एतत्;

	यदि (backend_पञ्जिकाकोशः.गणनम्(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_पञ्जिकाकोशः[backend_name] = एतत्;
}

Backend::~Backend()
{
}

शून्य Backend::execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design *design)
{
	std::ostream *f = रिक्त;
	स्वचालित state = pre_execute();
	execute(f, std::string(), तर्काः_2, design);
	post_execute(state);
	यदि (f != &std::निर्गमः)
		विलोपय f;
}

शून्य Backend::extra_तर्काः(std::ostream *&f, std::string &filename, std::सदिशम्<std::string> तर्काः_2, आकार_t argidx, तार्किक bin_output)
{
	तार्किक called_with_fp = f != रिक्त;

	प्रति (; argidx < तर्काः_2.आकार(); argidx++)
	{
		std::string arg = तर्काः_2[argidx];

		यदि (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(तर्काः_2, argidx, "Unknown option or option in arguments.");
		यदि (f != रिक्त)
			cmd_error(तर्काः_2, argidx, "Extra filename argument in direct file mode.");

		यदि (arg == "-") {
			filename = "<stdout>";
			f = &std::निर्गमः;
			अनुवर्त;
		}

		filename = arg;
		rewrite_filename(filename);
		यदि (filename.आकार() > 3 && filename.compare(filename.आकार()-3, std::string::npos, ".gz") == 0) {
#यदि_परिभाषितम् YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = नूतन gzip_ostream;
			यदि (!gf->उद्घाटय(filename)) {
				विलोपय gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.निवेशय(filename);
			f = gf;
#अन्यथा
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#यदि_अन्तः
		} अन्यथा {
			std::ofstream *ff = नूतन std::ofstream;
			ff->उद्घाटय(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::द्विमान) : std::ofstream::trunc);
			yosys_output_files.निवेशय(filename);
			यदि (ff->fail()) {
				विलोपय ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	यदि (called_with_fp)
		तर्काः_2.प्रक्षिप_पृष्ठभागः(filename);
	तर्काः_2[0] = pass_name;
	// cmd_log_args(args);

	यदि (f == रिक्त) {
		filename = "<stdout>";
		f = &std::निर्गमः;
	}
}

शून्य Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::सदिशम्<std::string> तर्काः_2;
	वर्ण *s = strdup(command.c_str());
	प्रति (वर्ण *p = strtok(s, " \t\r\n"); p; p = strtok(रिक्त, " \t\r\n"))
		तर्काः_2.प्रक्षिप_पृष्ठभागः(p);
	मुक्त(s);
	backend_call(design, f, filename, तर्काः_2);
}

शून्य Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::सदिशम्<std::string> तर्काः_2)
{
	यदि (तर्काः_2.आकार() == 0)
		प्रत्यागम;
	यदि (backend_पञ्जिकाकोशः.गणनम्(तर्काः_2[0]) == 0)
		log_cmd_error("No such backend: %s\n", तर्काः_2[0]);

	आकार_t orig_sel_राशिः_pos = design->selection_राशिः.आकार();

	यदि (f != रिक्त) {
		स्वचालित state = backend_पञ्जिकाकोशः[तर्काः_2[0]]->pre_execute();
		backend_पञ्जिकाकोशः[तर्काः_2[0]]->execute(f, filename, तर्काः_2, design);
		backend_पञ्जिकाकोशः[तर्काः_2[0]]->post_execute(state);
	} अन्यथा यदि (filename == "-") {
		std::ostream *f_cout = &std::निर्गमः;
		स्वचालित state = backend_पञ्जिकाकोशः[तर्काः_2[0]]->pre_execute();
		backend_पञ्जिकाकोशः[तर्काः_2[0]]->execute(f_cout, "<stdout>", तर्काः_2, design);
		backend_पञ्जिकाकोशः[तर्काः_2[0]]->post_execute(state);
	} अन्यथा {
		यदि (!filename.रिक्तानि())
			तर्काः_2.प्रक्षिप_पृष्ठभागः(filename);
		backend_पञ्जिकाकोशः[तर्काः_2[0]]->execute(तर्काः_2, design);
	}

	यावत् (design->selection_राशिः.आकार() > orig_sel_राशिः_pos)
		design->निष्कासय_selection();
}

संरचना SimHelper {
	string name;
	अन्तःस्थ string filesafe_name() {
		यदि (name.at(0) == '$')
			यदि (name.at(1) == '_')
				प्रत्यागम "gate" + name.substr(1);
			अन्यथा
				प्रत्यागम "word_" + name.substr(1);
		अन्यथा
			प्रत्यागम name;
	}
	string title;
	string ports;
	string source;
	string desc;
	string संहिता;
	string गणः;
	string ver;
	string चिह्नकानि;
};

स्थिर तार्किक is_संहिता_getter(string name) {
	प्रत्यागम *(--(name.अन्तः())) == '+';
}

स्थिर string get_cell_name(string name) {
	प्रत्यागम is_संहिता_getter(name) ? name.substr(0, name.दीर्घता_2()-1) : name;
}

स्थिर शून्य log_warning_flags(Pass *pass) {
	तार्किक has_warnings = असत्य;
	ध्रुव string name = pass->pass_name;
	यदि (pass->experimental_flag) {
		यदि (!has_warnings) लघुगणकः("\n");
		has_warnings = सत्य;
		लघुगणकः("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	यदि (pass->आन्तरिकम्_flag) {
		यदि (!has_warnings) लघुगणकः("\n");
		has_warnings = सत्य;
		लघुगणकः("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	यदि (has_warnings)
		लघुगणकः("\n");
}

स्थिर संरचना CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#अन्तर्भाव "kernel/simlib_help.inc"
#अन्तर्भाव "kernel/simcells_help.inc"
		cell_help.क्रमय();
	}
	तार्किक अन्तर्भवति(string name) { प्रत्यागम cell_help.गणनम्(get_cell_name(name)) > 0; }
	SimHelper get(string name) { प्रत्यागम cell_help[get_cell_name(name)]; }
} cell_help_messages;

संरचना HelpPass : सार्वजनिक Pass {
	HelpPass() : Pass("help", "display help messages") { }
	शून्य help() override
	{
		लघुगणकः("\n");
		लघुगणकः("    help  ................  list all commands\n");
		लघुगणकः("    help <command>  ......  print help message for given command\n");
		लघुगणकः("    help -all  ...........  print complete command reference\n");
		लघुगणकः("\n");
		लघुगणकः("    help -cells ..........  list all cell types\n");
		लघुगणकः("    help <celltype>  .....  print help message for given cell type\n");
		लघुगणकः("    help <celltype>+  ....  print verilog code for given cell type\n");
		लघुगणकः("\n");
	}
	तार्किक dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.प्रविष्टिः("version", "Yosys command reference");
		json.प्रविष्टिः("generator", yosys_संस्करणम्_str);

		तार्किक raise_error = असत्य;
		std::map<string, सदिशम्<string>> groups;

		// get root path
		स्वचालित this_मार्गः = std::filesystem::मार्गः_2(source_location::current().file_name());
		स्वचालित source_मूलम् = this_मार्गः.parent_मार्गः().parent_मार्गः();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		प्रति (स्वचालित &it : pass_पञ्जिकाकोशः) {
			स्वचालित name = it.first;
			स्वचालित pass = it.second;
			स्वचालित title = pass->short_help;

			स्वचालित cmd_help = PrettyHelp();
			स्वचालित has_pretty_help = pass->formatted_help();

			यदि (!has_pretty_help) {
				गणना PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_बफर = "";
				स्वचालित मूलम्_listing_2 = cmd_help.get_मूलम्();
				स्वचालित current_listing = मूलम्_listing_2;

				// dump command help
				std::ostringstream buf;
				{
					स्वचालित log_scope = logger().sink_scope();
					logger().सङ्कलय_sink<प्रवाहः_Log_Sink>(buf);
					pass->help();
				}
				std::stringstream ss;
				ss << buf.str();

				// parse command help
				आकार_t def_strip_गणनम् = 0;
				स्वचालित current_state = PUState_none;
				स्वचालित catch_verific = असत्य;
				स्वचालित blank_पङ्क्तयः = 2;
				प्रति (string पङ्क्तिः; std::getline(ss, पङ्क्तिः, '\n');) {
					// find position of first non space character
					std::आकार_t first_pos = पङ्क्तिः.find_first_not_सम्बन्धि(" \t");
					std::आकार_t चरमः_pos = पङ्क्तिः.find_चरमः_not_सम्बन्धि(" \t");
					यदि (first_pos == std::string::npos) {
						विकल्प (current_state)
						{
						पक्ष PUState_signature:
							मूलम्_listing_2->usage(current_बफर, null_source);
							current_listing = मूलम्_listing_2;
							current_state = PUState_none;
							current_बफर = "";
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
					std::string stripped_पङ्क्तिः = पङ्क्तिः.substr(first_pos, चरमः_pos - first_pos +1);
					तार्किक IsDefinition = stripped_पङ्क्तिः[0] == '-';
					IsDefinition &= stripped_पङ्क्तिः[1] != ' ' && stripped_पङ्क्तिः[1] != '>';
					तार्किक IsDedent = def_strip_गणनम् && first_pos < def_strip_गणनम्;
					तार्किक IsIndent = def_strip_गणनम् < first_pos;

					// line looks like a signature
					तार्किक IsSignature = stripped_पङ्क्तिः.find(name) == 0 && (stripped_पङ्क्तिः.दीर्घता_2() == name.दीर्घता_2() || stripped_पङ्क्तिः.at(name.आकार()) == ' ');

					यदि (IsSignature && first_pos <= 4 && (blank_पङ्क्तयः >= 2 || current_state == PUState_signature)) {
						यदि (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_बफर, "none", null_source);
							current_बफर = "";
						} अन्यथा यदि (current_state == PUState_signature) {
							मूलम्_listing_2->usage(current_बफर, null_source);
							current_बफर = "";
						} अन्यथा यदि (current_state == PUState_none && !current_बफर.रिक्तानि()) {
							current_listing->codeblock(current_बफर, "none", null_source);
							current_बफर = "";
						}
						current_listing = मूलम्_listing_2;
						current_state = PUState_signature;
						def_strip_गणनम् = first_pos;
						catch_verific = असत्य;
					} अन्यथा यदि (IsDedent) {
						def_strip_गणनम् = first_pos;
						यदि (current_state == PUState_optionbody) {
							यदि (!current_बफर.रिक्तानि()) {
								current_listing->codeblock(current_बफर, "none", null_source);
								current_बफर = "";
							}
							यदि (IsIndent) {
								current_state = PUState_options;
								current_listing = मूलम्_listing_2->पृष्ठभागः();
							} अन्यथा {
								current_state = PUState_none;
								current_listing = मूलम्_listing_2;
							}
						} अन्यथा {
							current_state = PUState_none;
						}
					}

					यदि (IsDefinition && !catch_verific && current_state != PUState_signature) {
						यदि (!current_बफर.रिक्तानि()) {
							current_listing->codeblock(current_बफर, "none", null_source);
							current_बफर = "";
						}
						current_state = PUState_options;
						current_listing = मूलम्_listing_2->open_option(stripped_पङ्क्तिः, null_source);
						def_strip_गणनम् = first_pos;
					} अन्यथा {
						यदि (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						यदि (current_बफर.रिक्तानि())
							current_बफर = stripped_पङ्क्तिः;
						अन्यथा यदि (current_state == PUState_signature && IsIndent)
							current_बफर += stripped_पङ्क्तिः;
						अन्यथा यदि (current_state == PUState_none) {
							current_बफर += (blank_पङ्क्तयः > 0 ? "\n\n" : "\n") + पङ्क्तिः;
						} अन्यथा
							current_बफर += (blank_पङ्क्तयः > 0 ? "\n\n" : "\n") + stripped_पङ्क्तिः;
						यदि (stripped_पङ्क्तिः.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = सत्य;
					}
					blank_पङ्क्तयः = 0;
				}

				यदि (!current_बफर.रिक्तानि()) {
					यदि (current_बफर.आकार() > 64 && current_बफर.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_बफर.substr(0, 62), null_source);
						current_listing->codeblock(current_बफर.substr(64), "yoscrypt", null_source);
					} अन्यथा
						current_listing->codeblock(current_बफर, "none", null_source);
					current_बफर = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			तार्किक has_source = source_file.compare("unknown") != 0;
			std::filesystem::मार्गः_2 source_मार्गः;
			स्वचालित skip_source_गणः = असत्य;

			यदि (has_source) {
				source_मार्गः = fixup_source_मार्गः(pass->location.file_name(), source_मूलम्, &skip_source_गणः);
				source_file = source_मार्गः.string();
			}

			// attempt auto group
			यदि (!cmd_help.has_गणः()) {
				यदि (pass->आन्तरिकम्_flag)
					cmd_help.गणः = "internal";
				अन्यथा यदि (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.गणः = "backends";
				अन्यथा यदि (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.गणः = "frontends";
				अन्यथा यदि (has_source) {
					यदि (source_मार्गः.has_parent_मार्गः() && !skip_source_गणः)
						cmd_help.गणः = source_मार्गः.parent_मार्गः().string();
				}
				// implicit !has_source
				अन्यथा यदि (name.find("equiv") == 0)
					cmd_help.गणः = "passes/equiv";
				अन्यथा यदि (name.find("fsm") == 0)
					cmd_help.गणः = "passes/fsm";
				अन्यथा यदि (name.find("memory") == 0)
					cmd_help.गणः = "passes/memory";
				अन्यथा यदि (name.find("opt") == 0)
					cmd_help.गणः = "passes/opt";
				अन्यथा यदि (name.find("proc") == 0)
					cmd_help.गणः = "passes/proc";
			}

			यदि (groups.गणनम्(cmd_help.गणः) == 0) {
				groups[cmd_help.गणः] = सदिशम्<string>();
			}
			groups[cmd_help.गणः].प्रक्षिप_पृष्ठभागः(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.प्रविष्टिः("title", title);
			json.name("content"); json.begin_array();
			प्रति (स्वचालित &content : cmd_help)
				json.मानम्(content.गन्तव्यम्_json(source_मूलम्));
			json.अन्तः_array();
			json.प्रविष्टिः("group", cmd_help.गणः);
			json.प्रविष्टिः("source_file", source_file);
			json.प्रविष्टिः("source_line", pass->location.पङ्क्तिः());
			json.प्रविष्टिः("source_func", pass->location.function_name());
			json.प्रविष्टिः("experimental_flag", pass->experimental_flag);
			json.प्रविष्टिः("internal_flag", pass->आन्तरिकम्_flag);
			json.अन्तः_object();
		}
		json.अन्तः_object();

		json.प्रविष्टिः("groups", groups);

		json.अन्तः_object();
		प्रत्यागम raise_error;
	}
	तार्किक dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.प्रविष्टिः("version", "Yosys internal cells");
		json.प्रविष्टिः("generator", yosys_maybe_संस्करणम्());

		dict<string, सदिशम्<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::Cell_सारणी_Builder::CellInfo>> cells;

		// iterate over cells
		तार्किक raise_error = असत्य;
		प्रति (स्वचालित it : StaticCellTypes::builder.cells) {
			यदि (!StaticCellTypes::categories.is_ज्ञातम्(it.type))
				अनुवर्त;
			स्वचालित name = ID::str(it.type);
			यदि (cell_help_messages.अन्तर्भवति(name)) {
				स्वचालित cell_help = cell_help_messages.get(name);
				groups[cell_help.गणः].emplace_पृष्ठभागः(name);
				स्वचालित cell_pair = pair<SimHelper, StaticCellTypes::Cell_सारणी_Builder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} अन्यथा {
				लघुगणकः("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= सत्य;
			}
		}
		प्रति (स्वचालित &it : cell_help_messages.cell_help) {
			यदि (cells.गणनम्(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.क्रमय();
		प्रति (स्वचालित &it : groups) {
			json.name(it.first.c_str()); json.मानम्(it.second);
		}
		json.अन्तः_object();

		json.name("cells"); json.begin_object();
		cells.क्रमय();
		प्रति (स्वचालित &it : cells) {
			स्वचालित ch = it.second.first;
			स्वचालित ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.मानम्(ch.title);
			json.name("ports"); json.मानम्(ch.ports);
			json.name("source"); json.मानम्(ch.source);
			json.name("desc"); json.मानम्(ch.desc);
			json.name("code"); json.मानम्(ch.संहिता);
			सदिशम्<string> inputs, outputs;
			प्रति (स्वचालित &निवेशः : ct.inputs)
				inputs.प्रक्षिप_पृष्ठभागः(ID::str(निवेशः));
			json.name("inputs"); json.मानम्(inputs);
			प्रति (स्वचालित &output : ct.outputs)
				outputs.प्रक्षिप_पृष्ठभागः(ID::str(output));
			json.name("outputs"); json.मानम्(outputs);
			सदिशम्<string> properties;
			// CellType properties
			यदि (ct.features.is_evaluable) properties.प्रक्षिप_पृष्ठभागः("is_evaluable");
			यदि (ct.features.is_combinatorial) properties.प्रक्षिप_पृष्ठभागः("is_combinatorial");
			यदि (ct.features.is_synthesizable) properties.प्रक्षिप_पृष्ठभागः("is_synthesizable");
			// SimHelper properties
			आकार_t चरमः = 0; आकार_t अग्र = 0;
			यावत् ((अग्र = ch.चिह्नकानि.find(", ", चरमः)) != string::npos) {
				properties.प्रक्षिप_पृष्ठभागः(ch.चिह्नकानि.substr(चरमः, अग्र-चरमः));
				चरमः = अग्र + 2;
			}
			स्वचालित final_चिह्नकम् = ch.चिह्नकानि.substr(चरमः);
			यदि (final_चिह्नकम्.आकार()) properties.प्रक्षिप_पृष्ठभागः(final_चिह्नकम्);
			json.name("properties"); json.मानम्(properties);
			json.अन्तः_object();
		}
		json.अन्तः_object();

		json.अन्तः_object();
		प्रत्यागम raise_error;
	}
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design*) override
	{
		यदि (तर्काः_2.आकार() == 1) {
			लघुगणकः("\n");
			प्रति (स्वचालित &it : pass_पञ्जिकाकोशः)
				लघुगणकः("    %-20s %s\n", it.first, it.second->short_help);
			लघुगणकः("\n");
			लघुगणकः("Type 'help <command>' for more information on a command.\n");
			लघुगणकः("Type 'help -cells' for a list of all cell types.\n");
			लघुगणकः("\n");
			प्रत्यागम;
		}

		यदि (तर्काः_2.आकार() == 2) {
			यदि (तर्काः_2[1] == "-all") {
				प्रति (स्वचालित &it : pass_पञ्जिकाकोशः) {
					लघुगणकः("\n");
					लघुगणकः("\n");
					लघुगणकः("%s  --  %s\n", it.first, it.second->short_help);
					std::string सन्देशः;
					प्रति (आकार_t i = 0; i < it.first.आकार() + it.second->short_help.आकार() + 6; i++)
						सन्देशः += '=';
					लघुगणकः("%s\n", सन्देशः);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			अन्यथा यदि (तर्काः_2[1] == "-cells") {
				लघुगणकः("\n");
				प्रति (स्वचालित &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					लघुगणकः("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				लघुगणकः("\n");
				लघुगणकः("Type 'help <cell_type>' for more information on a cell type.\n");
				लघुगणकः("\n");
				प्रत्यागम;
			}
			अन्यथा यदि (pass_पञ्जिकाकोशः.गणनम्(तर्काः_2[1])) {
				pass_पञ्जिकाकोशः.at(तर्काः_2[1])->help();
				log_warning_flags(pass_पञ्जिकाकोशः.at(तर्काः_2[1]));
			}
			अन्यथा यदि (cell_help_messages.अन्तर्भवति(तर्काः_2[1])) {
				स्वचालित help_cell = cell_help_messages.get(तर्काः_2[1]);
				यदि (is_संहिता_getter(तर्काः_2[1])) {
						लघुगणकः("\n");
						लघुगणकः("%s\n", help_cell.संहिता);
				} अन्यथा {
					लघुगणकः("\n");
					लघुगणकः("    %s %s\n", help_cell.name, help_cell.ports);
					लघुगणकः("\n");
					यदि (help_cell.ver == "2" || help_cell.ver == "2a") {
						यदि (help_cell.title != "") लघुगणकः("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						प्रति (std::string पङ्क्तिः; std::getline(ss, पङ्क्तिः, '\n');) {
							यदि (पङ्क्तिः != "::") लघुगणकः("%s\n", पङ्क्तिः);
						}
					} अन्यथा यदि (help_cell.desc.दीर्घता_2()) {
						लघुगणकः("%s\n", help_cell.desc);
					} अन्यथा {
						लघुगणकः("No help message for this cell type found.\n");
					}
					लघुगणकः("\n");
					लघुगणकः("Run 'help %s+' to display the Verilog model for this cell type.\n", तर्काः_2[1]);
					लघुगणकः("\n");
				}
			}
			अन्यथा
				लघुगणकः("No such command or cell type: %s\n", तर्काः_2[1]);
			प्रत्यागम;
		} अन्यथा यदि (तर्काः_2.आकार() == 3) {
			// this option is undocumented as it is for internal use only
			यदि (तर्काः_2[1] == "-dump-cmds-json") {
				PrettyJson json;
				यदि (!json.write_गन्तव्यम्_file(तर्काः_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", तर्काः_2[2], strerror(errno));
				यदि (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			अन्यथा यदि (तर्काः_2[1] == "-dump-cells-json") {
				PrettyJson json;
				यदि (!json.write_गन्तव्यम्_file(तर्काः_2[2]))
					log_error("Can't open file `%s' for writing: %s\n", तर्काः_2[2], strerror(errno));
				यदि (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			अन्यथा
				लघुगणकः("Unknown help command: `%s %s'\n", तर्काः_2[1], तर्काः_2[2]);
			प्रत्यागम;
		}

		help();
	}
} HelpPass;

संरचना EchoPass : सार्वजनिक Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	शून्य help() override
	{
		लघुगणकः("\n");
		लघुगणकः("    echo on\n");
		लघुगणकः("\n");
		लघुगणकः("Print all commands to log before executing them.\n");
		लघुगणकः("\n");
		लघुगणकः("\n");
		लघुगणकः("    echo off\n");
		लघुगणकः("\n");
		लघुगणकः("Do not print all commands to log before executing them. (default)\n");
		लघुगणकः("\n");
	}
	शून्य execute(std::सदिशम्<std::string> तर्काः_2, RTLIL::Design*) override
	{
		यदि (तर्काः_2.आकार() > 2)
			cmd_error(तर्काः_2, 2, "Unexpected argument.");

		यदि (तर्काः_2.आकार() == 2) {
			यदि (तर्काः_2[1] == "on")
				echo_बहुलकम् = सत्य;
			अन्यथा यदि (तर्काः_2[1] == "off")
				echo_बहुलकम् = असत्य;
			अन्यथा
				cmd_error(तर्काः_2, 1, "Unexpected argument.");
		}

		लघुगणकः("echo %s\n", echo_बहुलकम् ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

संरचना MinisatSatSolver : सार्वजनिक SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = एतत्;
	}
	ezSAT *create() override {
		प्रत्यागम नूतन ezMiniSAT();
	}
} MinisatSatSolver;

संरचना LicensePass : सार्वजनिक Pass {
	LicensePass() : Pass("license", "print license terms") { }
	शून्य help() override
	{
		लघुगणकः("\n");
		लघुगणकः("    license\n");
		लघुगणकः("\n");
		लघुगणकः("This command produces the following notice.\n");
		notice();
	}
	शून्य execute(std::सदिशम्<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	शून्य notice()
	{
		लघुगणकः("\n");
		लघुगणकः(" /----------------------------------------------------------------------------\\\n");
		लघुगणकः(" |                                                                            |\n");
		लघुगणकः(" |  yosys -- Yosys Open SYnthesis Suite                                       |\n");
		लघुगणकः(" |                                                                            |\n");
		लघुगणकः(" |  Copyright (C) 2012 - 2026  Claire Xenia Wolf <claire@yosyshq.com>         |\n");
		लघुगणकः(" |                                                                            |\n");
		लघुगणकः(" |  Permission to use, copy, modify, and/or distribute this software for any  |\n");
		लघुगणकः(" |  purpose with or without fee is hereby granted, provided that the above    |\n");
		लघुगणकः(" |  copyright notice and this permission notice appear in all copies.         |\n");
		लघुगणकः(" |                                                                            |\n");
		लघुगणकः(" |  THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  |\n");
		लघुगणकः(" |  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          |\n");
		लघुगणकः(" |  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   |\n");
		लघुगणकः(" |  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    |\n");
		लघुगणकः(" |  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     |\n");
		लघुगणकः(" |  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   |\n");
		लघुगणकः(" |  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            |\n");
		लघुगणकः(" |                                                                            |\n");
		लघुगणकः(" \\----------------------------------------------------------------------------/\n");
		लघुगणकः("\n");
	}
} LicensePass;

YOSYS_NAMESPACE_अन्तः

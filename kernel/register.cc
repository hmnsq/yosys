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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/satgen.h"
#அந்தர்பாவ "kernel/json.h"
#அந்தர்பாவ "kernel/gzip.h"
#அந்தர்பாவ "kernel/log_help.h"
#அந்தர்பாவ "kernel/newcelltypes.h"

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <filesystem>

YOSYS_NAMESPACE_BEGIN

#define MAX_REG_மொத்தம் 1000

தருக்க echo_mode = பொய்;
Pass *first_queued_pass;
Pass *current_pass;

std::map<std::string, Frontend*> frontend_register;
std::map<std::string, Pass*> pass_register;
std::map<std::string, Backend*> backend_register;
std::map<std::string, Target> target_register;

std::vector<std::string> Frontend::next_args;

தருக்க GarbageCollectionGuard::is_enabled_ = உண்மை;

நிலையான தருக்க garbage_collection_requested = பொய்;

வெறுமை request_garbage_collection()
{
	garbage_collection_requested = உண்மை;
}

வெறுமை try_collect_garbage()
{
	எனில் (!GarbageCollectionGuard::is_enabled() || !garbage_collection_requested)
		திரும்பு;
	garbage_collection_requested = பொய்;
	எனில் (yosys_design)
		yosys_design->gc_twines();
}

Pass::Pass(std::string name, std::string short_help, source_location location) :
	pass_name(name), short_help(short_help), location(location)
{
	next_queued_pass = first_queued_pass;
	first_queued_pass = இது;
	call_counter = 0;
	runtime_ns = 0;
}

வெறுமை Pass::run_register()
{
	எனில் (pass_register.மொத்தம்(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = இது;
}

வெறுமை Pass::init_register()
{
	vector<Pass*> added_passes;
	போது (first_queued_pass) {
		added_passes.தள்ளு_back(first_queued_pass);
		first_queued_pass->run_register();
		first_queued_pass = first_queued_pass->next_queued_pass;
	}
	க்கு (auto added_pass : added_passes)
		added_pass->on_register();
}

வெறுமை Pass::done_register()
{
	க்கு (auto &it : pass_register)
		it.second->on_shutdown();

	frontend_register.clear();
	pass_register.clear();
	backend_register.clear();
	log_assert(first_queued_pass == வெற்று);
}

வெறுமை Pass::on_register()
{
}

வெறுமை Pass::on_shutdown()
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
	current_pass = இது;
	clear_flags();
	திரும்பு state;
}

வெறுமை Pass::post_execute(Pass::pre_post_exec_state_t state)
{
	log_suppressed();

	int64_t time_ns = PerformanceTimer::query() - state.begin_ns;
	runtime_ns += time_ns;
	current_pass = state.parent_pass;
	subtract_from_current_runtime_ns(time_ns);
}

வெறுமை Pass::subtract_from_current_runtime_ns(int64_t time_ns)
{
	எனில் (current_pass)
		current_pass->runtime_ns -= time_ns;
}

வெறுமை Pass::help()
{
	auto prettyHelp = PrettyHelp();
	எனில் (formatted_help()) {
		prettyHelp.log_help();
	} இல்லையெனில் {
		log("\n");
		log("No help message for command `%s'.\n", pass_name);
		log("\n");
	}
}

தருக்க Pass::formatted_help()
{
	திரும்பு பொய்;
}

வெறுமை Pass::clear_flags()
{
}

வெறுமை Pass::cmd_log_args(மாறிலி std::vector<std::string> &args)
{
	எனில் (args.size() <= 1)
		திரும்பு;
	std::string message = "Full command line:";
	க்கு (size_t i = 0; i < args.size(); i++)
		message += stringf(" %s", args[i]);
	log("%s\n", message);
}

வெறுமை Pass::cmd_error(மாறிலி std::vector<std::string> &args, size_t argidx, std::string செய்தி)
{
	std::string command_text;
	முழு error_pos = 0;

	க்கு (size_t i = 0; i < args.size(); i++) {
		எனில் (i < argidx)
			error_pos += args[i].size() + 1;
		command_text = command_text + (command_text.empty() ? "" : " ") + args[i];
	}

	log("\n");
	log("Syntax error in command `%s':\n", command_text);
	help();

	log_cmd_error("Command syntax error: %s\n> %s\n> %*s^\n",
			செய்தி.c_str(), command_text.c_str(), error_pos, "");
}

வெறுமை Pass::extra_args(std::vector<std::string> args, size_t argidx, RTLIL::Design *design, தருக்க select)
{
	க்கு (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		எனில் (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");

		எனில் (!select)
			cmd_error(args, argidx, "Extra argument.");

		handle_extra_select_args(இது, args, argidx, args.size(), design);
		நிறுத்து;
	}
	// cmd_log_args(args);
}

வெறுமை Pass::call(RTLIL::Design *design, std::string command)
{
	std::vector<std::string> args;

	std::string cmd_buf = command;
	std::string tok = next_token(cmd_buf, " \t\r\n", உண்மை);

	எனில் (tok.empty())
		திரும்பு;

	எனில் (tok[0] == '!') {
#எனில் defined(YOSYS_ENABLE_SPAWN)
		cmd_buf = command.substr(command.find('!') + 1);
		போது (!cmd_buf.empty() && (cmd_buf.back() == ' ' || cmd_buf.back() == '\t' ||
				cmd_buf.back() == '\r' || cmd_buf.back() == '\n'))
			cmd_buf.resize(cmd_buf.size()-1);
		log_header(design, "Shell command: %s\n", cmd_buf);
		முழு retCode = run_command(cmd_buf);
		எனில் (retCode != 0)
			log_cmd_error("Shell command returned error code %d.\n", retCode);
		திரும்பு;
#இல்லையெனில்
		log_cmd_error("Shell is not available.\n");
#endif
	}

	போது (!tok.empty()) {
		எனில் (tok[0] == '#') {
			முழு stop;
			க்கு (stop = 0; stop < GetSize(cmd_buf); stop++)
				எனில் (cmd_buf[stop] == '\r' || cmd_buf[stop] == '\n')
					நிறுத்து;
			cmd_buf = cmd_buf.substr(stop);
		} இல்லையெனில்
		எனில் (tok.back() == ';') {
			முழு இலக்கம்_semikolon = 0;
			போது (!tok.empty() && tok.back() == ';')
				tok.resize(tok.size()-1), இலக்கம்_semikolon++;
			எனில் (!tok.empty())
				args.தள்ளு_back(tok);
			call(design, args);
			args.clear();
			எனில் (இலக்கம்_semikolon == 2)
				call(design, "clean");
			எனில் (இலக்கம்_semikolon == 3)
				call(design, "clean -purge");
		} இல்லையெனில்
			args.தள்ளு_back(tok);
		தருக்க found_nl = பொய்;
		க்கு (auto c : cmd_buf) {
			எனில் (c == ' ' || c == '\t')
				தொடர்;
			எனில் (c == '\r' || c == '\n')
				found_nl = உண்மை;
			நிறுத்து;
		}
		எனில் (found_nl) {
			call(design, args);
			args.clear();
		}
		tok = next_token(cmd_buf, " \t\r\n", உண்மை);
	}

	call(design, args);
}

வெறுமை Pass::call(RTLIL::Design *design, std::vector<std::string> args)
{
	எனில் (args.size() == 0 || args[0][0] == '#' || args[0][0] == ':')
		திரும்பு;

	எனில் (echo_mode) {
		std::string message = stringf("%s", create_prompt(design, 0, பொய்));
		க்கு (size_t i = 0; i < args.size(); i++)
			message += stringf("%s%s", i ? " " : "", args[i]);
		log("\n");
		log("%s\n", message);
	}

	எனில் (pass_register.மொத்தம்(args[0]) == 0)
		log_cmd_error("No such command: %s (type 'help' for a command overview)\n", args[0]);
	Pass *pass = pass_register[args[0]];

	// Collect garbage before the next pass if requested. No need to collect garbage after the last pass.
	try_collect_garbage();
	GarbageCollectionGuard gc_guard(pass->allow_garbage_collection_during_pass());

	எனில் (pass->experimental_flag)
		log_experimental(args[0]);

	size_t orig_sel_அடுக்கு_pos = design->selection_அடுக்கு.size();
	auto state = pass->pre_execute();
	pass->execute(args, design);
	pass->post_execute(state);
	போது (design->selection_அடுக்கு.size() > orig_sel_அடுக்கு_pos)
		design->எடு_selection();
}

வெறுமை Pass::call_on_selection(RTLIL::Design *design, மாறிலி RTLIL::Selection &selection, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->தள்ளு_selection(selection);

	Pass::call(design, command);

	design->எடு_selection();
	design->selected_active_module = backup_selected_active_module;
}

வெறுமை Pass::call_on_selection(RTLIL::Design *design, மாறிலி RTLIL::Selection &selection, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module.clear();
	design->தள்ளு_selection(selection);

	Pass::call(design, args);

	design->எடு_selection();
	design->selected_active_module = backup_selected_active_module;
}

வெறுமை Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::string command)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->தள்ளு_empty_selection();
	design->select(module);

	Pass::call(design, command);

	design->எடு_selection();
	design->selected_active_module = backup_selected_active_module;
}

வெறுமை Pass::call_on_module(RTLIL::Design *design, RTLIL::Module *module, std::vector<std::string> args)
{
	IdString backup_selected_active_module = design->selected_active_module;
	design->selected_active_module = module->name;
	design->தள்ளு_empty_selection();
	design->select(module);

	Pass::call(design, args);

	design->எடு_selection();
	design->selected_active_module = backup_selected_active_module;
}

தருக்க ScriptPass::check_label(std::string label, std::string info)
{
	எனில் (active_design == nullptr) {
		log("\n");
		எனில் (info.empty())
			log("    %s:\n", label);
		இல்லையெனில்
			log("    %s:    %s\n", label, info);
		திரும்பு உண்மை;
	} இல்லையெனில் {
		எனில் (!active_run_from.empty() && active_run_from == active_run_to) {
			block_active = (label == active_run_from);
		} இல்லையெனில் {
			எனில் (label == active_run_from)
				block_active = உண்மை;
			எனில் (label == active_run_to)
				block_active = பொய்;
		}
		திரும்பு block_active;
	}
}

வெறுமை ScriptPass::run(std::string command, std::string info)
{
	எனில் (active_design == nullptr) {
		எனில் (info.empty())
			log("        %s\n", command);
		இல்லையெனில்
			log("        %s    %s\n", command, info);
	} இல்லையெனில் {
		Pass::call(active_design, command);
		active_design->check();
	}
}

வெறுமை ScriptPass::run_nocheck(std::string command, std::string info)
{
	எனில் (active_design == nullptr) {
		எனில் (info.empty())
			log("        %s\n", command);
		இல்லையெனில்
			log("        %s    %s\n", command, info);
	} இல்லையெனில் {
		Pass::call(active_design, command);
	}
}

வெறுமை ScriptPass::run_script(RTLIL::Design *design, std::string run_from, std::string run_to)
{
	help_mode = பொய்;
	active_design = design;
	block_active = run_from.empty();
	active_run_from = run_from;
	active_run_to = run_to;
	script();
}

வெறுமை ScriptPass::help_script()
{
	clear_flags();
	help_mode = உண்மை;
	active_design = nullptr;
	block_active = உண்மை;
	active_run_from.clear();
	active_run_to.clear();
	script();
}

Frontend::Frontend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "read_" + name, short_help, location),
		frontend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

வெறுமை Frontend::run_register()
{
	எனில் (pass_register.மொத்தம்(pass_name) && !replace_existing_pass())
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = இது;

	எனில் (frontend_register.மொத்தம்(frontend_name) && !replace_existing_pass())
		log_error("Unable to register frontend '%s', frontend already exists!\n", frontend_name);
	frontend_register[frontend_name] = இது;
}

Frontend::~Frontend()
{
}

வெறுமை Frontend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	log_assert(next_args.empty());
	செய் {
		std::istream *f = வெற்று;
		next_args.clear();
		auto state = pre_execute();
		execute(f, std::string(), args, design);
		post_execute(state);
		args = next_args;
		delete f;
	} போது (!args.empty());
}

FILE *Frontend::current_script_file = வெற்று;
std::string Frontend::last_here_document;
std::string Frontend::current_script_filename;
முழு Frontend::current_script_lineno = 0;

வெறுமை Frontend::extra_args(std::istream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, தருக்க bin_உள்ளீடு)
{
	தருக்க called_with_fp = f != வெற்று;

	next_args.clear();

	எனில் (argidx < args.size())
	{
		std::string arg = args[argidx];

		எனில் (arg.compare(0, 1, "-") == 0)
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		எனில் (f != வெற்று)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		filename = arg;
		//Accommodate heredocs with EOT marker spaced out from "<<", e.g. "<< EOT" vs. "<<EOT"
		எனில் (filename == "<<" && argidx+1 < args.size())
			filename += args[++argidx];
		எனில் (filename.compare(0, 2, "<<") == 0) {
			எனில் (filename.size() <= 2)
				log_error("Missing EOT marker in here document!\n");
			std::string eot_marker = filename.substr(2);
			filename = (Frontend::current_script_file == nullptr) ?  "<stdin>" : "<inlined>";
			last_here_document.clear();
			போது (1) {
				std::string buffer;
				எழுத்து block[4096];
				போது (1) {
					எனில் (fgets(block, 4096, Frontend::current_script_file == nullptr? stdin : Frontend::current_script_file) == nullptr)
						log_error("Unexpected end of file in here document '%s'!\n", filename);
					buffer += block;
					எனில் (buffer.size() > 0 && (buffer[buffer.size() - 1] == '\n' || buffer[buffer.size() - 1] == '\r'))
						நிறுத்து;
				}
				Frontend::current_script_lineno++;
				size_t indent = buffer.find_first_not_of(" \t\r\n");
				எனில் (indent != std::string::npos && buffer.compare(indent, eot_marker.size(), eot_marker) == 0)
					நிறுத்து;
				last_here_document += buffer;
			}
			f = புதிய std::istringstream(last_here_document);
		} இல்லையெனில் {
			rewrite_filename(filename);
			vector<string> filenames = glob_filename(filename);
			filename = filenames.front();
			எனில் (GetSize(filenames) > 1) {
				next_args.செருகு(next_args.end(), args.begin(), args.begin()+argidx);
				next_args.செருகு(next_args.end(), filenames.begin()+1, filenames.end());
			}
			yosys_உள்ளீடு_files.செருகு(filename);
			f = uncompressed(filename, bin_உள்ளீடு ? std::ifstream::இருமம் : std::ifstream::in);
		}

		க்கு (size_t i = argidx+1; i < args.size(); i++)
			எனில் (args[i].compare(0, 1, "-") == 0)
				cmd_error(args, i, "Found option, expected arguments.");

		எனில் (argidx+1 < args.size()) {
			எனில் (next_args.empty())
				next_args.செருகு(next_args.end(), args.begin(), args.begin()+argidx);
			next_args.செருகு(next_args.end(), args.begin()+argidx+1, args.end());
			args.erase(args.begin()+argidx+1, args.end());
		}
	}

	எனில் (f == வெற்று)
		cmd_error(args, argidx, "No filename given.");

	எனில் (called_with_fp)
		args.தள்ளு_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);
}

வெறுமை Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	எழுத்து *s = strdup(command.c_str());
	க்கு (எழுத்து *p = strtok(s, " \t\r\n"); p; p = strtok(வெற்று, " \t\r\n"))
		args.தள்ளு_back(p);
	விடுவி(s);
	frontend_call(design, f, filename, args);
}

வெறுமை Frontend::frontend_call(RTLIL::Design *design, std::istream *f, std::string filename, std::vector<std::string> args)
{
	எனில் (args.size() == 0)
		திரும்பு;
	எனில் (frontend_register.மொத்தம்(args[0]) == 0)
		log_cmd_error("No such frontend: %s\n", args[0]);

	எனில் (f != வெற்று) {
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f, filename, args, design);
		frontend_register[args[0]]->post_execute(state);
	} இல்லையெனில் எனில் (filename == "-") {
		std::istream *f_cin = &std::cin;
		auto state = frontend_register[args[0]]->pre_execute();
		frontend_register[args[0]]->execute(f_cin, "<stdin>", args, design);
		frontend_register[args[0]]->post_execute(state);
	} இல்லையெனில் {
		எனில் (!filename.empty())
			args.தள்ளு_back(filename);
		frontend_register[args[0]]->execute(args, design);
	}
}

Backend::Backend(std::string name, std::string short_help, source_location location) :
		Pass(name.rfind("=", 0) == 0 ? name.substr(1) : "write_" + name, short_help, location),
		backend_name(name.rfind("=", 0) == 0 ? name.substr(1) : name)
{
}

வெறுமை Backend::run_register()
{
	எனில் (pass_register.மொத்தம்(pass_name))
		log_error("Unable to register pass '%s', pass already exists!\n", pass_name);
	pass_register[pass_name] = இது;

	எனில் (backend_register.மொத்தம்(backend_name))
		log_error("Unable to register backend '%s', backend already exists!\n", backend_name);
	backend_register[backend_name] = இது;
}

Backend::~Backend()
{
}

வெறுமை Backend::execute(std::vector<std::string> args, RTLIL::Design *design)
{
	std::ostream *f = வெற்று;
	auto state = pre_execute();
	execute(f, std::string(), args, design);
	post_execute(state);
	எனில் (f != &std::cout)
		delete f;
}

வெறுமை Backend::extra_args(std::ostream *&f, std::string &filename, std::vector<std::string> args, size_t argidx, தருக்க bin_output)
{
	தருக்க called_with_fp = f != வெற்று;

	க்கு (; argidx < args.size(); argidx++)
	{
		std::string arg = args[argidx];

		எனில் (arg.compare(0, 1, "-") == 0 && arg != "-")
			cmd_error(args, argidx, "Unknown option or option in arguments.");
		எனில் (f != வெற்று)
			cmd_error(args, argidx, "Extra filename argument in direct file mode.");

		எனில் (arg == "-") {
			filename = "<stdout>";
			f = &std::cout;
			தொடர்;
		}

		filename = arg;
		rewrite_filename(filename);
		எனில் (filename.size() > 3 && filename.compare(filename.size()-3, std::string::npos, ".gz") == 0) {
#ifdef YOSYS_ENABLE_ZLIB
			gzip_ostream *gf = புதிய gzip_ostream;
			எனில் (!gf->open(filename)) {
				delete gf;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			yosys_output_files.செருகு(filename);
			f = gf;
#இல்லையெனில்
			log_cmd_error("Yosys is compiled without zlib support, unable to write gzip output.\n");
#endif
		} இல்லையெனில் {
			std::ofstream *ff = புதிய std::ofstream;
			ff->open(filename.c_str(), bin_output ? (std::ofstream::trunc | std::ofstream::இருமம்) : std::ofstream::trunc);
			yosys_output_files.செருகு(filename);
			எனில் (ff->fail()) {
				delete ff;
				log_cmd_error("Can't open output file `%s' for writing: %s\n", filename, strerror(errno));
			}
			f = ff;
		}
	}

	எனில் (called_with_fp)
		args.தள்ளு_back(filename);
	args[0] = pass_name;
	// cmd_log_args(args);

	எனில் (f == வெற்று) {
		filename = "<stdout>";
		f = &std::cout;
	}
}

வெறுமை Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::string command)
{
	std::vector<std::string> args;
	எழுத்து *s = strdup(command.c_str());
	க்கு (எழுத்து *p = strtok(s, " \t\r\n"); p; p = strtok(வெற்று, " \t\r\n"))
		args.தள்ளு_back(p);
	விடுவி(s);
	backend_call(design, f, filename, args);
}

வெறுமை Backend::backend_call(RTLIL::Design *design, std::ostream *f, std::string filename, std::vector<std::string> args)
{
	எனில் (args.size() == 0)
		திரும்பு;
	எனில் (backend_register.மொத்தம்(args[0]) == 0)
		log_cmd_error("No such backend: %s\n", args[0]);

	size_t orig_sel_அடுக்கு_pos = design->selection_அடுக்கு.size();

	எனில் (f != வெற்று) {
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f, filename, args, design);
		backend_register[args[0]]->post_execute(state);
	} இல்லையெனில் எனில் (filename == "-") {
		std::ostream *f_cout = &std::cout;
		auto state = backend_register[args[0]]->pre_execute();
		backend_register[args[0]]->execute(f_cout, "<stdout>", args, design);
		backend_register[args[0]]->post_execute(state);
	} இல்லையெனில் {
		எனில் (!filename.empty())
			args.தள்ளு_back(filename);
		backend_register[args[0]]->execute(args, design);
	}

	போது (design->selection_அடுக்கு.size() > orig_sel_அடுக்கு_pos)
		design->எடு_selection();
}

கட்டமைப்பு SimHelper {
	string name;
	உள்வரி string filesafe_name() {
		எனில் (name.at(0) == '$')
			எனில் (name.at(1) == '_')
				திரும்பு "gate" + name.substr(1);
			இல்லையெனில்
				திரும்பு "word_" + name.substr(1);
		இல்லையெனில்
			திரும்பு name;
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

நிலையான தருக்க is_code_getter(string name) {
	திரும்பு *(--(name.end())) == '+';
}

நிலையான string get_cell_name(string name) {
	திரும்பு is_code_getter(name) ? name.substr(0, name.length()-1) : name;
}

நிலையான வெறுமை log_warning_flags(Pass *pass) {
	தருக்க has_warnings = பொய்;
	மாறிலி string name = pass->pass_name;
	எனில் (pass->experimental_flag) {
		எனில் (!has_warnings) log("\n");
		has_warnings = உண்மை;
		log("WARNING: THE '%s' COMMAND IS EXPERIMENTAL.\n", name);
	}
	எனில் (pass->internal_flag) {
		எனில் (!has_warnings) log("\n");
		has_warnings = உண்மை;
		log("WARNING: THE '%s' COMMAND IS INTENDED FOR INTERNAL DEVELOPER USE ONLY.\n", name);
	}
	எனில் (has_warnings)
		log("\n");
}

நிலையான கட்டமைப்பு CellHelpMessages {
	dict<string, SimHelper> cell_help;
	CellHelpMessages() {
#அந்தர்பாவ "kernel/simlib_help.inc"
#அந்தர்பாவ "kernel/simcells_help.inc"
		cell_help.sort();
	}
	தருக்க contains(string name) { திரும்பு cell_help.மொத்தம்(get_cell_name(name)) > 0; }
	SimHelper get(string name) { திரும்பு cell_help[get_cell_name(name)]; }
} cell_help_messages;

கட்டமைப்பு HelpPass : பொது Pass {
	HelpPass() : Pass("help", "display help messages") { }
	வெறுமை help() override
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
	தருக்க dump_cmds_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys command reference");
		json.entry("generator", yosys_version_str);

		தருக்க raise_error = பொய்;
		std::map<string, vector<string>> groups;

		// get root path
		auto this_path = std::filesystem::path(source_location::current().file_name());
		auto source_வேர் = this_path.parent_path().parent_path();

		json.name("cmds"); json.begin_object();
		// iterate over commands
		க்கு (auto &it : pass_register) {
			auto name = it.first;
			auto pass = it.second;
			auto title = pass->short_help;

			auto cmd_help = PrettyHelp();
			auto has_pretty_help = pass->formatted_help();

			எனில் (!has_pretty_help) {
				எண்ணிக்கை PassUsageState {
					PUState_none,
					PUState_signature,
					PUState_options,
					PUState_optionbody,
				};

				source_location null_source;
				string current_buffer = "";
				auto வேர்_listing_2 = cmd_help.get_வேர்();
				auto current_listing = வேர்_listing_2;

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
				size_t def_strip_மொத்தம் = 0;
				auto current_state = PUState_none;
				auto catch_verific = பொய்;
				auto blank_வரிகள் = 2;
				க்கு (string வரி; std::getline(ss, வரி, '\n');) {
					// find position of first non space character
					std::size_t first_pos = வரி.find_first_not_of(" \t");
					std::size_t last_pos = வரி.find_last_not_of(" \t");
					எனில் (first_pos == std::string::npos) {
						தேர்வு (current_state)
						{
						நிலை PUState_signature:
							வேர்_listing_2->usage(current_buffer, null_source);
							current_listing = வேர்_listing_2;
							current_state = PUState_none;
							current_buffer = "";
							நிறுத்து;
						நிலை PUState_none:
						நிலை PUState_optionbody:
							blank_வரிகள் += 1;
							நிறுத்து;
						இயல்பு:
							நிறுத்து;
						}
						// skip empty lines
						தொடர்;
					}

					// strip leading and trailing whitespace
					std::string stripped_வரி = வரி.substr(first_pos, last_pos - first_pos +1);
					தருக்க IsDefinition = stripped_வரி[0] == '-';
					IsDefinition &= stripped_வரி[1] != ' ' && stripped_வரி[1] != '>';
					தருக்க IsDedent = def_strip_மொத்தம் && first_pos < def_strip_மொத்தம்;
					தருக்க IsIndent = def_strip_மொத்தம் < first_pos;

					// line looks like a signature
					தருக்க IsSignature = stripped_வரி.find(name) == 0 && (stripped_வரி.length() == name.length() || stripped_வரி.at(name.size()) == ' ');

					எனில் (IsSignature && first_pos <= 4 && (blank_வரிகள் >= 2 || current_state == PUState_signature)) {
						எனில் (current_state == PUState_options || current_state == PUState_optionbody) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						} இல்லையெனில் எனில் (current_state == PUState_signature) {
							வேர்_listing_2->usage(current_buffer, null_source);
							current_buffer = "";
						} இல்லையெனில் எனில் (current_state == PUState_none && !current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_listing = வேர்_listing_2;
						current_state = PUState_signature;
						def_strip_மொத்தம் = first_pos;
						catch_verific = பொய்;
					} இல்லையெனில் எனில் (IsDedent) {
						def_strip_மொத்தம் = first_pos;
						எனில் (current_state == PUState_optionbody) {
							எனில் (!current_buffer.empty()) {
								current_listing->codeblock(current_buffer, "none", null_source);
								current_buffer = "";
							}
							எனில் (IsIndent) {
								current_state = PUState_options;
								current_listing = வேர்_listing_2->back();
							} இல்லையெனில் {
								current_state = PUState_none;
								current_listing = வேர்_listing_2;
							}
						} இல்லையெனில் {
							current_state = PUState_none;
						}
					}

					எனில் (IsDefinition && !catch_verific && current_state != PUState_signature) {
						எனில் (!current_buffer.empty()) {
							current_listing->codeblock(current_buffer, "none", null_source);
							current_buffer = "";
						}
						current_state = PUState_options;
						current_listing = வேர்_listing_2->open_option(stripped_வரி, null_source);
						def_strip_மொத்தம் = first_pos;
					} இல்லையெனில் {
						எனில் (current_state == PUState_options) {
							current_state = PUState_optionbody;
						}
						எனில் (current_buffer.empty())
							current_buffer = stripped_வரி;
						இல்லையெனில் எனில் (current_state == PUState_signature && IsIndent)
							current_buffer += stripped_வரி;
						இல்லையெனில் எனில் (current_state == PUState_none) {
							current_buffer += (blank_வரிகள் > 0 ? "\n\n" : "\n") + வரி;
						} இல்லையெனில்
							current_buffer += (blank_வரிகள் > 0 ? "\n\n" : "\n") + stripped_வரி;
						எனில் (stripped_வரி.compare("Command file parser supports following commands in file:") == 0)
							catch_verific = உண்மை;
					}
					blank_வரிகள் = 0;
				}

				எனில் (!current_buffer.empty()) {
					எனில் (current_buffer.size() > 64 && current_buffer.substr(0, 64).compare("The following commands are executed by this synthesis command:\n\n") == 0) {
						current_listing->paragraph(current_buffer.substr(0, 62), null_source);
						current_listing->codeblock(current_buffer.substr(64), "yoscrypt", null_source);
					} இல்லையெனில்
						current_listing->codeblock(current_buffer, "none", null_source);
					current_buffer = "";
				}
			}

			// fix path
			string source_file = pass->location.file_name();
			தருக்க has_source = source_file.compare("unknown") != 0;
			std::filesystem::path source_path;
			auto skip_source_group = பொய்;

			எனில் (has_source) {
				source_path = fixup_source_path(pass->location.file_name(), source_வேர், &skip_source_group);
				source_file = source_path.string();
			}

			// attempt auto group
			எனில் (!cmd_help.has_group()) {
				எனில் (pass->internal_flag)
					cmd_help.group = "internal";
				இல்லையெனில் எனில் (source_file.find("backends/") == 0 || (!has_source && name.find("read_") == 0))
					cmd_help.group = "backends";
				இல்லையெனில் எனில் (source_file.find("frontends/") == 0 || (!has_source && name.find("write_") == 0))
					cmd_help.group = "frontends";
				இல்லையெனில் எனில் (has_source) {
					எனில் (source_path.has_parent_path() && !skip_source_group)
						cmd_help.group = source_path.parent_path().string();
				}
				// implicit !has_source
				இல்லையெனில் எனில் (name.find("equiv") == 0)
					cmd_help.group = "passes/equiv";
				இல்லையெனில் எனில் (name.find("fsm") == 0)
					cmd_help.group = "passes/fsm";
				இல்லையெனில் எனில் (name.find("memory") == 0)
					cmd_help.group = "passes/memory";
				இல்லையெனில் எனில் (name.find("opt") == 0)
					cmd_help.group = "passes/opt";
				இல்லையெனில் எனில் (name.find("proc") == 0)
					cmd_help.group = "passes/proc";
			}

			எனில் (groups.மொத்தம்(cmd_help.group) == 0) {
				groups[cmd_help.group] = vector<string>();
			}
			groups[cmd_help.group].தள்ளு_back(name);

			// write to json
			json.name(name.c_str()); json.begin_object();
			json.entry("title", title);
			json.name("content"); json.begin_array();
			க்கு (auto &content : cmd_help)
				json.மதிப்பு(content.to_json(source_வேர்));
			json.end_array();
			json.entry("group", cmd_help.group);
			json.entry("source_file", source_file);
			json.entry("source_line", pass->location.வரி());
			json.entry("source_func", pass->location.function_name());
			json.entry("experimental_flag", pass->experimental_flag);
			json.entry("internal_flag", pass->internal_flag);
			json.end_object();
		}
		json.end_object();

		json.entry("groups", groups);

		json.end_object();
		திரும்பு raise_error;
	}
	தருக்க dump_cells_json(PrettyJson &json) {
		// init json
		json.begin_object();
		json.entry("version", "Yosys internal cells");
		json.entry("generator", yosys_maybe_version());

		dict<string, vector<string>> groups;
		dict<string, pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>> cells;

		// iterate over cells
		தருக்க raise_error = பொய்;
		க்கு (auto it : StaticCellTypes::builder.cells) {
			எனில் (!StaticCellTypes::categories.is_known(it.type))
				தொடர்;
			auto name = ID::str(it.type);
			எனில் (cell_help_messages.contains(name)) {
				auto cell_help = cell_help_messages.get(name);
				groups[cell_help.group].emplace_back(name);
				auto cell_pair = pair<SimHelper, StaticCellTypes::CellTableBuilder::CellInfo>(cell_help, it);
				cells.emplace(name, cell_pair);
			} இல்லையெனில் {
				log("ERROR: Missing cell help for cell '%s'.\n", name);
				raise_error |= உண்மை;
			}
		}
		க்கு (auto &it : cell_help_messages.cell_help) {
			எனில் (cells.மொத்தம்(it.first) == 0) {
				log_warning("Found cell model '%s' without matching cell type.\n", it.first);
			}
		}

		// write to json
		json.name("groups"); json.begin_object();
		groups.sort();
		க்கு (auto &it : groups) {
			json.name(it.first.c_str()); json.மதிப்பு(it.second);
		}
		json.end_object();

		json.name("cells"); json.begin_object();
		cells.sort();
		க்கு (auto &it : cells) {
			auto ch = it.second.first;
			auto ct = it.second.second;
			json.name(ch.name.c_str()); json.begin_object();
			json.name("title"); json.மதிப்பு(ch.title);
			json.name("ports"); json.மதிப்பு(ch.ports);
			json.name("source"); json.மதிப்பு(ch.source);
			json.name("desc"); json.மதிப்பு(ch.desc);
			json.name("code"); json.மதிப்பு(ch.code);
			vector<string> inputs, outputs;
			க்கு (auto &உள்ளீடு : ct.inputs)
				inputs.தள்ளு_back(ID::str(உள்ளீடு));
			json.name("inputs"); json.மதிப்பு(inputs);
			க்கு (auto &output : ct.outputs)
				outputs.தள்ளு_back(ID::str(output));
			json.name("outputs"); json.மதிப்பு(outputs);
			vector<string> properties;
			// CellType properties
			எனில் (ct.features.is_evaluable) properties.தள்ளு_back("is_evaluable");
			எனில் (ct.features.is_combinatorial) properties.தள்ளு_back("is_combinatorial");
			எனில் (ct.features.is_synthesizable) properties.தள்ளு_back("is_synthesizable");
			// SimHelper properties
			size_t last = 0; size_t next = 0;
			போது ((next = ch.tags.find(", ", last)) != string::npos) {
				properties.தள்ளு_back(ch.tags.substr(last, next-last));
				last = next + 2;
			}
			auto final_tag = ch.tags.substr(last);
			எனில் (final_tag.size()) properties.தள்ளு_back(final_tag);
			json.name("properties"); json.மதிப்பு(properties);
			json.end_object();
		}
		json.end_object();

		json.end_object();
		திரும்பு raise_error;
	}
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		எனில் (args.size() == 1) {
			log("\n");
			க்கு (auto &it : pass_register)
				log("    %-20s %s\n", it.first, it.second->short_help);
			log("\n");
			log("Type 'help <command>' for more information on a command.\n");
			log("Type 'help -cells' for a list of all cell types.\n");
			log("\n");
			திரும்பு;
		}

		எனில் (args.size() == 2) {
			எனில் (args[1] == "-all") {
				க்கு (auto &it : pass_register) {
					log("\n");
					log("\n");
					log("%s  --  %s\n", it.first, it.second->short_help);
					std::string செய்தி;
					க்கு (size_t i = 0; i < it.first.size() + it.second->short_help.size() + 6; i++)
						செய்தி += '=';
					log("%s\n", செய்தி);
					it.second->help();
					log_warning_flags(it.second);
				}
			}
			இல்லையெனில் எனில் (args[1] == "-cells") {
				log("\n");
				க்கு (auto &it : cell_help_messages.cell_help) {
					SimHelper help_cell = it.second;
					log("    %-15s %s\n", help_cell.name, help_cell.ports);
				}
				log("\n");
				log("Type 'help <cell_type>' for more information on a cell type.\n");
				log("\n");
				திரும்பு;
			}
			இல்லையெனில் எனில் (pass_register.மொத்தம்(args[1])) {
				pass_register.at(args[1])->help();
				log_warning_flags(pass_register.at(args[1]));
			}
			இல்லையெனில் எனில் (cell_help_messages.contains(args[1])) {
				auto help_cell = cell_help_messages.get(args[1]);
				எனில் (is_code_getter(args[1])) {
						log("\n");
						log("%s\n", help_cell.code);
				} இல்லையெனில் {
					log("\n");
					log("    %s %s\n", help_cell.name, help_cell.ports);
					log("\n");
					எனில் (help_cell.ver == "2" || help_cell.ver == "2a") {
						எனில் (help_cell.title != "") log("%s:\n", help_cell.title);
						std::stringstream ss;
						ss << help_cell.desc;
						க்கு (std::string வரி; std::getline(ss, வரி, '\n');) {
							எனில் (வரி != "::") log("%s\n", வரி);
						}
					} இல்லையெனில் எனில் (help_cell.desc.length()) {
						log("%s\n", help_cell.desc);
					} இல்லையெனில் {
						log("No help message for this cell type found.\n");
					}
					log("\n");
					log("Run 'help %s+' to display the Verilog model for this cell type.\n", args[1]);
					log("\n");
				}
			}
			இல்லையெனில்
				log("No such command or cell type: %s\n", args[1]);
			திரும்பு;
		} இல்லையெனில் எனில் (args.size() == 3) {
			// this option is undocumented as it is for internal use only
			எனில் (args[1] == "-dump-cmds-json") {
				PrettyJson json;
				எனில் (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				எனில் (dump_cmds_json(json)) {
					log_abort();
				}
			}
			// this option is undocumented as it is for internal use only
			இல்லையெனில் எனில் (args[1] == "-dump-cells-json") {
				PrettyJson json;
				எனில் (!json.write_to_file(args[2]))
					log_error("Can't open file `%s' for writing: %s\n", args[2], strerror(errno));
				எனில் (dump_cells_json(json)) {
					log_error("One or more cells defined in celltypes.h are missing help documentation.\n");
				}
			}
			இல்லையெனில்
				log("Unknown help command: `%s %s'\n", args[1], args[2]);
			திரும்பு;
		}

		help();
	}
} HelpPass;

கட்டமைப்பு EchoPass : பொது Pass {
	EchoPass() : Pass("echo", "turning echoing back of commands on and off") { }
	வெறுமை help() override
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
	வெறுமை execute(std::vector<std::string> args, RTLIL::Design*) override
	{
		எனில் (args.size() > 2)
			cmd_error(args, 2, "Unexpected argument.");

		எனில் (args.size() == 2) {
			எனில் (args[1] == "on")
				echo_mode = உண்மை;
			இல்லையெனில் எனில் (args[1] == "off")
				echo_mode = பொய்;
			இல்லையெனில்
				cmd_error(args, 1, "Unexpected argument.");
		}

		log("echo %s\n", echo_mode ? "on" : "off");
	}
} EchoPass;

SatSolver *yosys_satsolver_list;
SatSolver *yosys_satsolver;

கட்டமைப்பு MinisatSatSolver : பொது SatSolver {
	MinisatSatSolver() : SatSolver("minisat") {
		yosys_satsolver = இது;
	}
	ezSAT *create() override {
		திரும்பு புதிய ezMiniSAT();
	}
} MinisatSatSolver;

கட்டமைப்பு LicensePass : பொது Pass {
	LicensePass() : Pass("license", "print license terms") { }
	வெறுமை help() override
	{
		log("\n");
		log("    license\n");
		log("\n");
		log("This command produces the following notice.\n");
		notice();
	}
	வெறுமை execute(std::vector<std::string>, RTLIL::Design*) override
	{
		notice();
	}
	வெறுமை notice()
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

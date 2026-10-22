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
#अन्तर्भाव "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#अन्तर्भाव "libs/cxxopts/include/cxxopts.hpp"
#अन्तर्भाव <iostream>
#अन्तर्भाव <chrono>

#ifdef YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

#ifdef YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#endif

#ifdef YOSYS_ENABLE_TCL
#  include <tcl.h>
#endif

#ifdef YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
नामाकाश py = pybind11;
#endif

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#अन्तर्भाव <inttypes.h>

#यदि defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#यदि !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#अन्तर्भाव <fmt/color.h>
#अन्तर्भाव <fmt/format.h>

USING_YOSYS_NAMESPACE

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
पूर्णाङ्क yosys_history_offset = 0;
std::string yosys_history_file;
#endif

शून्य yosys_atexit()
{
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	यदि (!yosys_history_file.empty()) {
#यदि defined(YOSYS_ENABLE_READLINE)
		यदि (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} अन्यथा
			write_history(yosys_history_file.c_str());
#अन्यथा
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#यदि defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	यदि (hist_list != रिक्त)
		मुक्त(hist_list);
#endif
#endif
}

#यदि defined(__OpenBSD__)
नामाकाश Yosys {
बाह्य वर्ण *yosys_argv0;
बाह्य वर्ण yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
नामाकाश Yosys {
	बाह्य पूर्णाङ्क yosys_tcl_interp_init(Tcl_Interp *interp);
	बाह्य शून्य yosys_tcl_activate_repl();
};
#endif

नामाकाश Yosys {
	बाह्य तार्किक log_stderr_sink_forced;
};

वर्ग ColorConsoleLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit ColorConsoleLogSink(तार्किक err, तार्किक quiet) : error_output(err), quiet_warnings(quiet) {}
	शून्य log(अचर LogMessage &सन्देश) override;
	तार्किक should_log(अचर LogMessage &सन्देश) अचर override;
	शून्य flush() override;
	खाता *file_handle() override { फर्काऊ error_output ? nullptr : मानक_निकास; }
गुप्त:
	तार्किक error_output;
	तार्किक quiet_warnings;
};

तार्किक no_color = असत्य;

तार्किक is_colored(खाता *file)
{
	यदि (no_color)
		फर्काऊ असत्य;

#यदि defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	फर्काऊ handle != INVALID_HANDLE_मान &&
			GetConsoleMode(handle, &mode);
#अन्यथा
	फर्काऊ isatty(fileno(file));
#endif
}

शून्य ColorConsoleLogSink::flush()
{
	ख_साफ(मानक_निकास);
	ख_साफ(मानक_त्रुटि);
}

तार्किक next_print_log = सत्य;

शून्य ColorConsoleLogSink::log(अचर LogMessage &सन्देश)
{
	खाता *f = (error_output || सन्देश.severity == LogSeverity::Error) ? मानक_त्रुटि : मानक_निकास;

	यदि (logger().get_log_time() && next_print_log) {
		स्वतः elapsed = सन्देश.timestamp - logger().get_initial_time();
		स्वतः us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", पूर्णाङ्क(us.गन्ती() / 1'000'000),पूर्णाङ्क(us.गन्ती() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = असत्य;
	}

	यदि (!सन्देश.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", सन्देश.src.filename, सन्देश.src.start_पङ्क्ति);

	विकल्प (सन्देश.severity) {
		पक्ष LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", सन्देश.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", सन्देश.message);
			रोक;

		पक्ष LogSeverity::Error:
		पक्ष LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", सन्देश.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", सन्देश.message);
			रोक;

		पक्ष LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", सन्देश.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", सन्देश.message);
			रोक;

		पक्ष LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", सन्देश.prefix, सन्देश.message);
			रोक;

		पक्ष LogSeverity::Comment:
		पक्ष LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", सन्देश.prefix, सन्देश.message);
			रोक;

		पक्ष LogSeverity::Info:
		स्वभाव:
			यदि (!सन्देश.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", सन्देश.prefix);
			fmt::print(f, "{}", सन्देश.message);
			रोक;
	}
	यदि (!सन्देश.message.empty())
		next_print_log = सन्देश.message.back() == '\n';
}

तार्किक ColorConsoleLogSink::should_log(अचर LogMessage &सन्देश) अचर
{
	फर्काऊ (!error_output) || (सन्देश.severity == LogSeverity::Error || सन्देश.severity == LogSeverity::NonFatalError ||
			(सन्देश.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
पूर्णाङ्क wmain(पूर्णाङ्क argc, ब_अक्षर_प्रकार **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	रखो_क्षेत्र(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<वर्ण*> uargs;
	लागि (पूर्णाङ्क i = 0; i < argc; i++) {
		पूर्णाङ्क usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, रिक्त, 0, रिक्त, रिक्त);
		वर्ण *uarg = (वर्ण *)दो_स्मृति(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, रिक्त, रिक्त);
		uargs.थप_back(uarg);
	}
	uargs.थप_back(रिक्त);
	वर्ण **argv = uargs.data();
#अन्यथा
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
#endif
	स्वतः wall_clock_start = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::vector<std::string> vlog_defines;
	std::vector<std::string> passes_commands;
	std::vector<std::string> frontend_files;
	std::vector<std::string> plugin_filenames;
	std::vector<std::string> special_args;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	तार्किक scriptfile_tcl = असत्य;
	तार्किक scriptfile_python = असत्य;
	तार्किक print_banner = सत्य;
	तार्किक print_stats = सत्य;
	तार्किक call_abort = असत्य;
	तार्किक timing_details = असत्य;
	तार्किक run_shell = सत्य;
	तार्किक run_tcl_shell = असत्य;
	तार्किक mode_v = असत्य;
	तार्किक mode_q = असत्य;
	तार्किक quiet_warnings = असत्य;
	तार्किक log_stderr = असत्य;
	no_color = std::दो_पर्या("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::मान<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::मान<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::मान<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::मान<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::मान<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::मान<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::मान<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::मान<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::मान<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::मान<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::मान<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::मान<पूर्णाङ्क>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::मान<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::मान<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::मान<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::मान<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::मान<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::मान<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::मान<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::मान<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::मान<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::मान<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::मान<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	यदि (argc == 2 && (!म_भेद(argv[1], "-h") || !म_भेद(argv[1], "-help") || !म_भेद(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		निकास(0);
	}
	यत्न {
		// Check for "--" in arguments
		स्वतः it = std::find(argv, argv + argc, std::string("--"));
		यदि (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		स्वतः result = options.parse(argc, argv);

		यदि (result.गन्ती("M")) memhasher_on();
		यदि (result.गन्ती("X")) yosys_xtrace += result.गन्ती("X");
		यदि (result.गन्ती("A")) call_abort = सत्य;
		यदि (result.गन्ती("Q")) print_banner = असत्य;
		यदि (result.गन्ती("T")) print_stats = असत्य;
		यदि (result.गन्ती("no-version")) yosys_write_versions = असत्य;
		यदि (result.गन्ती("V")) {
			std::cout << yosys_version_str << std::endl;
			निकास(0);
		}
		यदि (result.गन्ती("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			निकास(0);
		}
		यदि (result.गन्ती("S")) {
			passes_commands.थप_back("synth");
			run_shell = असत्य;
		}
		यदि (result.गन्ती("C")) run_tcl_shell = सत्य;
		यदि (result.गन्ती("g")) logger().force_debug_on();
		यदि (result.गन्ती("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		यदि (result.गन्ती("f")) frontend_command = result["f"].as<std::string>();
		यदि (result.गन्ती("H")) {
			passes_commands.थप_back("help");
			run_shell = असत्य;
		}
		यदि (result.गन्ती("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.थप_back("help " + res);
			run_shell = असत्य;
		}
		यदि (result.गन्ती("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = असत्य;
		}
		यदि (result.गन्ती("p")) {
			स्वतः cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.घुसाउ(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = असत्य;
		}
		यदि (result.गन्ती("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = असत्य;
		}
		लागि (अचर स्वतः& साँचो : {"l", "L"}) {
			यदि (result.गन्ती(साँचो)) {
				लागि (अचर स्वतः& filename : result[साँचो].as<std::vector<std::string>>()) {
					यत्न {
						logger().add_sink<FileLogSink>(filename, साँचो[0] == 'L', असत्य);
					} गृहाण (अचर std::runtime_error &e) {
 					   std::cerr << e.what();
					   निकास(1);
					}
				}
			}
		}
		यदि (result.गन्ती("q")) {
			mode_q = सत्य;
			यदि (log_stderr) quiet_warnings = सत्य;
			log_stderr = सत्य;
		}
		यदि (result.गन्ती("v")) {
			mode_v = सत्य;
			log_stderr = सत्य;
			logger().set_verbose_level(result["v"].as<पूर्णाङ्क>());
		}
		यदि (result.गन्ती("t")) logger().set_log_time(सत्य);
		यदि (result.गन्ती("no-color")) no_color = सत्य;
		यदि (result.गन्ती("d")) timing_details = सत्य;
		लागि (अचर स्वतः& साँचो : {"s", "c"}) {
			यदि (result.गन्ती(साँचो)) {
				scriptfile = result[साँचो].as<std::string>();
				scriptfile_tcl = std::string(साँचो) == "c";
				run_shell = असत्य;
			}
		}
		यदि (result.गन्ती("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = सत्य;
			run_shell = असत्य;
		}
		लागि (अचर स्वतः& साँचो : {"W", "w", "e"}) {
			यदि (result.गन्ती(साँचो)) {
				स्वतः regexes = result[साँचो].as<std::vector<std::string>>();
				लागि (अचर स्वतः& regex : regexes) {
					यदि (std::string(साँचो) == "W")
						logger().add_warn(regex);
					यदि (std::string(साँचो) == "w")
						logger().add_nowarn(regex);
					यदि (std::string(साँचो) == "e")
						logger().add_werror(regex);
				}
			}
		}
		यदि (result.गन्ती("r")) topmodule = result["r"].as<std::string>();
		यदि (result.गन्ती("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		यदि (result.गन्ती("P")) {
			स्वतः dump_args = result["P"].as<std::vector<std::string>>();
			लागि (अचर स्वतः& arg : dump_args) {
				स्वतः tokens = split_tokens(arg, ":");
				यदि (!tokens.empty() && tokens[0] == "ALL") {
					यदि (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						निकास(1);
					}
					logger().set_hdump_all(सत्य);
				} अन्यथा {
					यदि (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].झिक_back();
					यदि (tokens.size() == 1)
						tokens.थप_back("yosys_dump_" + tokens[0] + ".il");
					यदि (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						निकास(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		यदि (result.गन्ती("E")) depsfile = result["E"].as<std::string>();
		यदि (result.गन्ती("x")) {
			लागि (अचर स्वतः &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		यदि (result.गन्ती("perffile")) perffile = result["perffile"].as<std::string>();
		यदि (result.गन्ती("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		यदि (result.गन्ती("autoidx")) {
			पूर्णाङ्क idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		यदि (!is_colored(मानक_निकास) || !is_colored(मानक_त्रुटि)) {
			यदि (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} अन्यथा {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} अन्यथा
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		यदि (print_banner)
			yosys_banner();

	}
	गृहाण (अचर cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		निकास(1);
	}

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#यदि defined(_WIN32)
		यदि (दो_पर्या("HOMEDRIVE") != रिक्त && दो_पर्या("HOMEPATH") != रिक्त) {
			state_dir = stringf("%s%s/.local/state", दो_पर्या("HOMEDRIVE"), दो_पर्या("HOMEPATH"));
		} अन्यथा {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#अन्यथा
		यदि (दो_पर्या("XDG_STATE_HOME") == रिक्त || दो_पर्या("XDG_STATE_HOME")[0] == '\0') {
			यदि (दो_पर्या("HOME") != रिक्त) {
				state_dir = stringf("%s/.local/state", दो_पर्या("HOME"));
			} अन्यथा {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} अन्यथा {
			state_dir = stringf("%s", दो_पर्या("XDG_STATE_HOME"));
		}
	#endif

	यदि (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	यदि (print_stats)
		logger().start_hasher();

#यदि defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#यदि defined(__linux__)
	// set stack size to >= 128 MB
	{
		संरचना rlimit rl;
		अचर rlim_t थाक_size = 128L * 1024L * 1024L;
		यदि (getrlimit(RLIMIT_थाक, &rl) == 0 && rl.rlim_cur < थाक_size) {
			rl.rlim_cur = थाक_size;
			setrlimit(RLIMIT_थाक, &rl);
		}
	}
#endif

	yosys_setup();
#ifdef YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#endif
	log_error_atexit = yosys_atexit;

	लागि (स्वतः &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	यदि (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		लागि (स्वतः vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	यदि (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		लागि (स्वतः special_arg : special_args)
			frontend_files.थप_back(special_arg);
	}

	लागि (स्वतः it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		यदि (run_frontend((*it).c_str(), frontend_command))
			run_shell = असत्य;
	}

	यदि (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	यदि (!scriptfile.empty()) {
		यदि (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			पूर्णाङ्क tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			लागि (स्वतः arg : special_args)
				script_args.थप_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), रिक्त, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), रिक्त, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), रिक्त, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			यदि (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#अन्यथा
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} अन्यथा यदि (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			पूर्णाङ्क py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			लागि (पूर्णाङ्क i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			खाता *scriptfp = ख_खोलो(scriptfile.c_str(), "r");
			यदि (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			यदि (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#अन्यथा
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} अन्यथा
			run_frontend(scriptfile, "script");
	}

	लागि (स्वतः it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	यदि (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#अन्यथा
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} अन्यथा {
		यदि (run_shell)
			shell(yosys_design);
		अन्यथा
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	लागि (स्वतः it : saved_designs)
		it.second->check();
	लागि (स्वतः it : pushed_designs)
		it->check();

	यदि (!depsfile.empty())
	{
		खाता *f = ख_खोलो(depsfile.c_str(), "wt");
		यदि (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", म_त्रुटि(त्रुटि_सं));
		तार्किक first = सत्य;
		लागि (स्वतः fn : yosys_output_files) {
			ख_लिखो(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = असत्य;
		}
		ख_लिखो(f, ":");
		लागि (स्वतः fn : yosys_आगत_files) {
			यदि (yosys_output_files.गन्ती(fn) == 0)
				ख_लिखो(f, " %s", escape_filename_spaces(fn).c_str());
		}
		ख_लिखो(f, "\n");
	}

	logger().report_unexpected_error();

	यदि (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(असत्य);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		यदि (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		यदि (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		यदि (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			लागि (स्वतः &[feature, earliest_removal] : logger().get_deprecated())
				यदि (earliest_removal.empty())
					log("    %s\n", feature);
				अन्यथा
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(शून्य)wall_clock_start;
#अन्यथा
		std::string meminfo;
		std::string stats_divider = ", ";

		संरचना rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		यदि (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			संरचना rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#यदि defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#यदि defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#अथवायदि defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		द्विगुण wall_seconds = std::chrono::duration<द्विगुण>(
				std::chrono::steady_clock::now() - wall_clock_start).गन्ती();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, पूर्णाङ्क, std::string>> timedat;

		लागि (स्वतः &it : pass_register)
			यदि (it.second->call_counter) {
				स्वतः pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.घुसाउ(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		यदि (twine_gc_गन्ती) {
			total_ns += twine_gc_ns;
			timedat.घुसाउ(make_tuple(twine_gc_ns, twine_gc_गन्ती, "twine_gc"));
		}

		यदि (timing_details)
		{
			log("Time spent:\n");
			लागि (स्वतः it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", पूर्णाङ्क(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		अन्यथा
		{
			पूर्णाङ्क out_गन्ती = 0;
			std::string message = "Time spent:";
			लागि (स्वतः it = timedat.rbegin(); it != timedat.rend() && out_गन्ती < 4; it++, out_गन्ती++) {
				यदि (out_गन्ती >= 2 && (std::get<0>(*it) < 1000000000 || पूर्णाङ्क(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					रोक;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_गन्ती ? "," : "", पूर्णाङ्क(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), पूर्णाङ्क(std::get<0>(*it) / 1000000000));
			}
			message += out_गन्ती ? "" : " no commands executed";
			log("%s\n", message);
		}
		यदि(!perffile.empty())
		{
			खाता *f = ख_खोलो(perffile.c_str(), "wt");
			यदि (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", म_त्रुटि(त्रुटि_सं));

			ख_लिखो(f, "{\n");
			ख_लिखो(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			ख_लिखो(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			ख_लिखो(f, "  \"passes\": {");

			तार्किक first = सत्य;
			लागि (स्वतः it = timedat.rbegin(); it != timedat.rend(); it++) {
				यदि (!first)
					ख_लिखो(f, ",");
				ख_लिखो(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				ख_लिखो(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				ख_लिखो(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				ख_लिखो(f, "	}");
				first = असत्य;
			}
			ख_लिखो(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	यदि (call_abort)
		पात();

	log_flush();

	yosys_shutdown();

#यदि defined(_MSC_VER)
	_exit(0);
#अथवायदि defined(_WIN32)
	_Exit(0);
#endif
	फर्काऊ 0;
}

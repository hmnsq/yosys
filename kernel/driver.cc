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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#അന്തര്ഭാവ "libs/cxxopts/include/cxxopts.hpp"
#അന്തര്ഭാവ <iostream>
#അന്തര്ഭാവ <chrono>

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
നാമാകാശ py = pybind11;
#endif

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#അന്തര്ഭാവ <inttypes.h>

#എങ്കിൽ defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#എങ്കിൽ !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#അന്തര്ഭാവ <fmt/color.h>
#അന്തര്ഭാവ <fmt/format.h>

USING_YOSYS_NAMESPACE

#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
പൂർണ്ണസംഖ്യ yosys_history_offset = 0;
std::string yosys_history_file;
#endif

ശൂന്യം yosys_atexit()
{
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	എങ്കിൽ (!yosys_history_file.empty()) {
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE)
		എങ്കിൽ (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} അല്ലെങ്കിൽ
			write_history(yosys_history_file.c_str());
#അല്ലെങ്കിൽ
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#എങ്കിൽ defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	എങ്കിൽ (hist_list != ശൂന്യ)
		സ്വതന്ത്രം(hist_list);
#endif
#endif
}

#എങ്കിൽ defined(__OpenBSD__)
നാമാകാശ Yosys {
ബാഹ്യ അക്ഷരം *yosys_argv0;
ബാഹ്യ അക്ഷരം yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
നാമാകാശ Yosys {
	ബാഹ്യ പൂർണ്ണസംഖ്യ yosys_tcl_interp_init(Tcl_Interp *interp);
	ബാഹ്യ ശൂന്യം yosys_tcl_activate_repl();
};
#endif

നാമാകാശ Yosys {
	ബാഹ്യ തർക്കം log_stderr_sink_forced;
};

വർഗ്ഗം ColorConsoleLogSink : പൊതു LogSink
{
പൊതു:
	explicit ColorConsoleLogSink(തർക്കം err, തർക്കം quiet) : error_output(err), quiet_warnings(quiet) {}
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override;
	തർക്കം should_log(സ്ഥിരം LogMessage &സന്ദേശം) സ്ഥിരം override;
	ശൂന്യം flush() override;
	FILE *file_handle() override { തിരികെ error_output ? nullptr : stdout; }
സ്വകാര്യ:
	തർക്കം error_output;
	തർക്കം quiet_warnings;
};

തർക്കം no_color = അസത്യം;

തർക്കം is_colored(FILE *file)
{
	എങ്കിൽ (no_color)
		തിരികെ അസത്യം;

#എങ്കിൽ defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	തിരികെ handle != INVALID_HANDLE_മൂല്യം &&
			GetConsoleMode(handle, &mode);
#അല്ലെങ്കിൽ
	തിരികെ isatty(fileno(file));
#endif
}

ശൂന്യം ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

തർക്കം next_print_log = സത്യം;

ശൂന്യം ColorConsoleLogSink::log(സ്ഥിരം LogMessage &സന്ദേശം)
{
	FILE *f = (error_output || സന്ദേശം.severity == LogSeverity::Error) ? stderr : stdout;

	എങ്കിൽ (logger().get_log_time() && next_print_log) {
		auto elapsed = സന്ദേശം.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", പൂർണ്ണസംഖ്യ(us.എണ്ണം() / 1'000'000),പൂർണ്ണസംഖ്യ(us.എണ്ണം() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = അസത്യം;
	}

	എങ്കിൽ (!സന്ദേശം.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", സന്ദേശം.src.filename, സന്ദേശം.src.start_വരി);

	തിരഞ്ഞെടുപ്പ് (സന്ദേശം.severity) {
		അവസ്ഥ LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", സന്ദേശം.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", സന്ദേശം.message);
			നിർത്തുക;

		അവസ്ഥ LogSeverity::Error:
		അവസ്ഥ LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", സന്ദേശം.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", സന്ദേശം.message);
			നിർത്തുക;

		അവസ്ഥ LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", സന്ദേശം.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", സന്ദേശം.message);
			നിർത്തുക;

		അവസ്ഥ LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", സന്ദേശം.prefix, സന്ദേശം.message);
			നിർത്തുക;

		അവസ്ഥ LogSeverity::Comment:
		അവസ്ഥ LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", സന്ദേശം.prefix, സന്ദേശം.message);
			നിർത്തുക;

		അവസ്ഥ LogSeverity::Info:
		സ്ഥിരസ്ഥിതി:
			എങ്കിൽ (!സന്ദേശം.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", സന്ദേശം.prefix);
			fmt::print(f, "{}", സന്ദേശം.message);
			നിർത്തുക;
	}
	എങ്കിൽ (!സന്ദേശം.message.empty())
		next_print_log = സന്ദേശം.message.back() == '\n';
}

തർക്കം ColorConsoleLogSink::should_log(സ്ഥിരം LogMessage &സന്ദേശം) സ്ഥിരം
{
	തിരികെ (!error_output) || (സന്ദേശം.severity == LogSeverity::Error || സന്ദേശം.severity == LogSeverity::NonFatalError ||
			(സന്ദേശം.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
പൂർണ്ണസംഖ്യ wmain(പൂർണ്ണസംഖ്യ argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<അക്ഷരം*> uargs;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < argc; i++) {
		പൂർണ്ണസംഖ്യ usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, ശൂന്യ, 0, ശൂന്യ, ശൂന്യ);
		അക്ഷരം *uarg = (അക്ഷരം *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ശൂന്യ, ശൂന്യ);
		uargs.തള്ളുക_back(uarg);
	}
	uargs.തള്ളുക_back(ശൂന്യ);
	അക്ഷരം **argv = uargs.data();
#അല്ലെങ്കിൽ
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
#endif
	auto wall_clock_start = std::chrono::steady_clock::now();
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
	തർക്കം scriptfile_tcl = അസത്യം;
	തർക്കം scriptfile_python = അസത്യം;
	തർക്കം print_banner = സത്യം;
	തർക്കം print_stats = സത്യം;
	തർക്കം call_abort = അസത്യം;
	തർക്കം timing_details = അസത്യം;
	തർക്കം run_shell = സത്യം;
	തർക്കം run_tcl_shell = അസത്യം;
	തർക്കം mode_v = അസത്യം;
	തർക്കം mode_q = അസത്യം;
	തർക്കം quiet_warnings = അസത്യം;
	തർക്കം log_stderr = അസത്യം;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::മൂല്യം<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::മൂല്യം<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::മൂല്യം<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::മൂല്യം<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::മൂല്യം<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::മൂല്യം<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::മൂല്യം<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::മൂല്യം<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::മൂല്യം<പൂർണ്ണസംഖ്യ>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::മൂല്യം<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::മൂല്യം<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::മൂല്യം<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::മൂല്യം<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::മൂല്യം<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::മൂല്യം<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	എങ്കിൽ (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	ശ്രമിക്കുക {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		എങ്കിൽ (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		എങ്കിൽ (result.എണ്ണം("M")) memhasher_on();
		എങ്കിൽ (result.എണ്ണം("X")) yosys_xtrace += result.എണ്ണം("X");
		എങ്കിൽ (result.എണ്ണം("A")) call_abort = സത്യം;
		എങ്കിൽ (result.എണ്ണം("Q")) print_banner = അസത്യം;
		എങ്കിൽ (result.എണ്ണം("T")) print_stats = അസത്യം;
		എങ്കിൽ (result.എണ്ണം("no-version")) yosys_write_versions = അസത്യം;
		എങ്കിൽ (result.എണ്ണം("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		എങ്കിൽ (result.എണ്ണം("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		എങ്കിൽ (result.എണ്ണം("S")) {
			passes_commands.തള്ളുക_back("synth");
			run_shell = അസത്യം;
		}
		എങ്കിൽ (result.എണ്ണം("C")) run_tcl_shell = സത്യം;
		എങ്കിൽ (result.എണ്ണം("g")) logger().force_debug_on();
		എങ്കിൽ (result.എണ്ണം("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		എങ്കിൽ (result.എണ്ണം("f")) frontend_command = result["f"].as<std::string>();
		എങ്കിൽ (result.എണ്ണം("H")) {
			passes_commands.തള്ളുക_back("help");
			run_shell = അസത്യം;
		}
		എങ്കിൽ (result.എണ്ണം("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.തള്ളുക_back("help " + res);
			run_shell = അസത്യം;
		}
		എങ്കിൽ (result.എണ്ണം("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = അസത്യം;
		}
		എങ്കിൽ (result.എണ്ണം("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.തിരുകുക(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = അസത്യം;
		}
		എങ്കിൽ (result.എണ്ണം("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = അസത്യം;
		}
		വേണ്ടി (സ്ഥിരം auto& താക്കോൽ : {"l", "L"}) {
			എങ്കിൽ (result.എണ്ണം(താക്കോൽ)) {
				വേണ്ടി (സ്ഥിരം auto& filename : result[താക്കോൽ].as<std::vector<std::string>>()) {
					ശ്രമിക്കുക {
						logger().add_sink<FileLogSink>(filename, താക്കോൽ[0] == 'L', അസത്യം);
					} പിടിക്കുക (സ്ഥിരം std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		എങ്കിൽ (result.എണ്ണം("q")) {
			mode_q = സത്യം;
			എങ്കിൽ (log_stderr) quiet_warnings = സത്യം;
			log_stderr = സത്യം;
		}
		എങ്കിൽ (result.എണ്ണം("v")) {
			mode_v = സത്യം;
			log_stderr = സത്യം;
			logger().set_verbose_level(result["v"].as<പൂർണ്ണസംഖ്യ>());
		}
		എങ്കിൽ (result.എണ്ണം("t")) logger().set_log_time(സത്യം);
		എങ്കിൽ (result.എണ്ണം("no-color")) no_color = സത്യം;
		എങ്കിൽ (result.എണ്ണം("d")) timing_details = സത്യം;
		വേണ്ടി (സ്ഥിരം auto& താക്കോൽ : {"s", "c"}) {
			എങ്കിൽ (result.എണ്ണം(താക്കോൽ)) {
				scriptfile = result[താക്കോൽ].as<std::string>();
				scriptfile_tcl = std::string(താക്കോൽ) == "c";
				run_shell = അസത്യം;
			}
		}
		എങ്കിൽ (result.എണ്ണം("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = സത്യം;
			run_shell = അസത്യം;
		}
		വേണ്ടി (സ്ഥിരം auto& താക്കോൽ : {"W", "w", "e"}) {
			എങ്കിൽ (result.എണ്ണം(താക്കോൽ)) {
				auto regexes = result[താക്കോൽ].as<std::vector<std::string>>();
				വേണ്ടി (സ്ഥിരം auto& regex : regexes) {
					എങ്കിൽ (std::string(താക്കോൽ) == "W")
						logger().add_warn(regex);
					എങ്കിൽ (std::string(താക്കോൽ) == "w")
						logger().add_nowarn(regex);
					എങ്കിൽ (std::string(താക്കോൽ) == "e")
						logger().add_werror(regex);
				}
			}
		}
		എങ്കിൽ (result.എണ്ണം("r")) topmodule = result["r"].as<std::string>();
		എങ്കിൽ (result.എണ്ണം("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		എങ്കിൽ (result.എണ്ണം("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			വേണ്ടി (സ്ഥിരം auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				എങ്കിൽ (!tokens.empty() && tokens[0] == "ALL") {
					എങ്കിൽ (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(സത്യം);
				} അല്ലെങ്കിൽ {
					എങ്കിൽ (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].എടുക്കുക_back();
					എങ്കിൽ (tokens.size() == 1)
						tokens.തള്ളുക_back("yosys_dump_" + tokens[0] + ".il");
					എങ്കിൽ (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		എങ്കിൽ (result.എണ്ണം("E")) depsfile = result["E"].as<std::string>();
		എങ്കിൽ (result.എണ്ണം("x")) {
			വേണ്ടി (സ്ഥിരം auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		എങ്കിൽ (result.എണ്ണം("perffile")) perffile = result["perffile"].as<std::string>();
		എങ്കിൽ (result.എണ്ണം("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		എങ്കിൽ (result.എണ്ണം("autoidx")) {
			പൂർണ്ണസംഖ്യ idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		എങ്കിൽ (!is_colored(stdout) || !is_colored(stderr)) {
			എങ്കിൽ (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} അല്ലെങ്കിൽ {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} അല്ലെങ്കിൽ
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		എങ്കിൽ (print_banner)
			yosys_banner();

	}
	പിടിക്കുക (സ്ഥിരം cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#എങ്കിൽ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#എങ്കിൽ defined(_WIN32)
		എങ്കിൽ (getenv("HOMEDRIVE") != ശൂന്യ && getenv("HOMEPATH") != ശൂന്യ) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} അല്ലെങ്കിൽ {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#അല്ലെങ്കിൽ
		എങ്കിൽ (getenv("XDG_STATE_HOME") == ശൂന്യ || getenv("XDG_STATE_HOME")[0] == '\0') {
			എങ്കിൽ (getenv("HOME") != ശൂന്യ) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} അല്ലെങ്കിൽ {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} അല്ലെങ്കിൽ {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	എങ്കിൽ (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	എങ്കിൽ (print_stats)
		logger().start_hasher();

#എങ്കിൽ defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#എങ്കിൽ defined(__linux__)
	// set stack size to >= 128 MB
	{
		ഘടന rlimit rl;
		സ്ഥിരം rlim_t അട്ടി_size = 128L * 1024L * 1024L;
		എങ്കിൽ (getrlimit(RLIMIT_അട്ടി, &rl) == 0 && rl.rlim_cur < അട്ടി_size) {
			rl.rlim_cur = അട്ടി_size;
			setrlimit(RLIMIT_അട്ടി, &rl);
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

	വേണ്ടി (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	എങ്കിൽ (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		വേണ്ടി (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	എങ്കിൽ (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		വേണ്ടി (auto special_arg : special_args)
			frontend_files.തള്ളുക_back(special_arg);
	}

	വേണ്ടി (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		എങ്കിൽ (run_frontend((*it).c_str(), frontend_command))
			run_shell = അസത്യം;
	}

	എങ്കിൽ (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	എങ്കിൽ (!scriptfile.empty()) {
		എങ്കിൽ (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			പൂർണ്ണസംഖ്യ tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			വേണ്ടി (auto arg : special_args)
				script_args.തള്ളുക_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ശൂന്യ, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ശൂന്യ, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ശൂന്യ, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			എങ്കിൽ (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#അല്ലെങ്കിൽ
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} അല്ലെങ്കിൽ എങ്കിൽ (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			പൂർണ്ണസംഖ്യ py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			എങ്കിൽ (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			എങ്കിൽ (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#അല്ലെങ്കിൽ
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} അല്ലെങ്കിൽ
			run_frontend(scriptfile, "script");
	}

	വേണ്ടി (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	എങ്കിൽ (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#അല്ലെങ്കിൽ
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (run_shell)
			shell(yosys_design);
		അല്ലെങ്കിൽ
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	വേണ്ടി (auto it : saved_designs)
		it.second->check();
	വേണ്ടി (auto it : pushed_designs)
		it->check();

	എങ്കിൽ (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		എങ്കിൽ (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		തർക്കം first = സത്യം;
		വേണ്ടി (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = അസത്യം;
		}
		fprintf(f, ":");
		വേണ്ടി (auto fn : yosys_നിവേശം_files) {
			എങ്കിൽ (yosys_output_files.എണ്ണം(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	എങ്കിൽ (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(അസത്യം);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		എങ്കിൽ (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		എങ്കിൽ (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		എങ്കിൽ (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			വേണ്ടി (auto &[feature, earliest_removal] : logger().get_deprecated())
				എങ്കിൽ (earliest_removal.empty())
					log("    %s\n", feature);
				അല്ലെങ്കിൽ
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(ശൂന്യം)wall_clock_start;
#അല്ലെങ്കിൽ
		std::string meminfo;
		std::string stats_divider = ", ";

		ഘടന rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		എങ്കിൽ (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ഘടന rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#എങ്കിൽ defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#എങ്കിൽ defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#അല്ലെങ്കിലെങ്കിൽ defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		ഇരട്ട wall_seconds = std::chrono::duration<ഇരട്ട>(
				std::chrono::steady_clock::now() - wall_clock_start).എണ്ണം();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, പൂർണ്ണസംഖ്യ, std::string>> timedat;

		വേണ്ടി (auto &it : pass_register)
			എങ്കിൽ (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.തിരുകുക(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		എങ്കിൽ (twine_gc_എണ്ണം) {
			total_ns += twine_gc_ns;
			timedat.തിരുകുക(make_tuple(twine_gc_ns, twine_gc_എണ്ണം, "twine_gc"));
		}

		എങ്കിൽ (timing_details)
		{
			log("Time spent:\n");
			വേണ്ടി (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", പൂർണ്ണസംഖ്യ(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		അല്ലെങ്കിൽ
		{
			പൂർണ്ണസംഖ്യ out_എണ്ണം = 0;
			std::string message = "Time spent:";
			വേണ്ടി (auto it = timedat.rbegin(); it != timedat.rend() && out_എണ്ണം < 4; it++, out_എണ്ണം++) {
				എങ്കിൽ (out_എണ്ണം >= 2 && (std::get<0>(*it) < 1000000000 || പൂർണ്ണസംഖ്യ(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					നിർത്തുക;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_എണ്ണം ? "," : "", പൂർണ്ണസംഖ്യ(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), പൂർണ്ണസംഖ്യ(std::get<0>(*it) / 1000000000));
			}
			message += out_എണ്ണം ? "" : " no commands executed";
			log("%s\n", message);
		}
		എങ്കിൽ(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			എങ്കിൽ (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			തർക്കം first = സത്യം;
			വേണ്ടി (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				എങ്കിൽ (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = അസത്യം;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	എങ്കിൽ (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#എങ്കിൽ defined(_MSC_VER)
	_exit(0);
#അല്ലെങ്കിലെങ്കിൽ defined(_WIN32)
	_Exit(0);
#endif
	തിരികെ 0;
}

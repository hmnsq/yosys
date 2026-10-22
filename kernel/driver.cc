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
#অন্তর্ভাব "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#অন্তর্ভাব "libs/cxxopts/include/cxxopts.hpp"
#অন্তর্ভাব <iostream>
#অন্তর্ভাব <chrono>

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
নামাকাশ py = pybind11;
#endif

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#অন্তর্ভাব <inttypes.h>

#যদি defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#যদি !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#অন্তর্ভাব <fmt/color.h>
#অন্তর্ভাব <fmt/format.h>

USING_YOSYS_NAMESPACE

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
পূর্ণাঙ্ক yosys_history_offset = 0;
std::string yosys_history_file;
#endif

শূন্য yosys_atexit()
{
#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	যদি (!yosys_history_file.empty()) {
#যদি defined(YOSYS_ENABLE_READLINE)
		যদি (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} অন্যথা
			write_history(yosys_history_file.c_str());
#অন্যথা
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#যদি defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	যদি (hist_list != রিক্ত)
		মুক্ত(hist_list);
#endif
#endif
}

#যদি defined(__OpenBSD__)
নামাকাশ Yosys {
বাহ্য বর্ণ *yosys_argv0;
বাহ্য বর্ণ yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
নামাকাশ Yosys {
	বাহ্য পূর্ণাঙ্ক yosys_tcl_interp_init(Tcl_Interp *interp);
	বাহ্য শূন্য yosys_tcl_activate_repl();
};
#endif

নামাকাশ Yosys {
	বাহ্য তার্কিক log_stderr_sink_forced;
};

বর্গ ColorConsoleLogSink : সার্বজনিক LogSink
{
সার্বজনিক:
	explicit ColorConsoleLogSink(তার্কিক err, তার্কিক quiet) : error_output(err), quiet_warnings(quiet) {}
	শূন্য log(ধ্রুব LogMessage &বার্তা) override;
	তার্কিক should_log(ধ্রুব LogMessage &বার্তা) ধ্রুব override;
	শূন্য flush() override;
	FILE *file_handle() override { প্রত্যাগম error_output ? nullptr : stdout; }
গুপ্ত:
	তার্কিক error_output;
	তার্কিক quiet_warnings;
};

তার্কিক no_color = অসত্য;

তার্কিক is_colored(FILE *file)
{
	যদি (no_color)
		প্রত্যাগম অসত্য;

#যদি defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	প্রত্যাগম handle != INVALID_HANDLE_মান &&
			GetConsoleMode(handle, &mode);
#অন্যথা
	প্রত্যাগম isatty(fileno(file));
#endif
}

শূন্য ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

তার্কিক next_print_log = সত্য;

শূন্য ColorConsoleLogSink::log(ধ্রুব LogMessage &বার্তা)
{
	FILE *f = (error_output || বার্তা.severity == LogSeverity::Error) ? stderr : stdout;

	যদি (logger().get_log_time() && next_print_log) {
		auto elapsed = বার্তা.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", পূর্ণাঙ্ক(us.গুনতি() / 1'000'000),পূর্ণাঙ্ক(us.গুনতি() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = অসত্য;
	}

	যদি (!বার্তা.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", বার্তা.src.filename, বার্তা.src.start_সারি);

	বিকল্প (বার্তা.severity) {
		পক্ষ LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", বার্তা.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", বার্তা.message);
			ভঙ্গ;

		পক্ষ LogSeverity::Error:
		পক্ষ LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", বার্তা.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", বার্তা.message);
			ভঙ্গ;

		পক্ষ LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", বার্তা.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", বার্তা.message);
			ভঙ্গ;

		পক্ষ LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", বার্তা.prefix, বার্তা.message);
			ভঙ্গ;

		পক্ষ LogSeverity::Comment:
		পক্ষ LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", বার্তা.prefix, বার্তা.message);
			ভঙ্গ;

		পক্ষ LogSeverity::Info:
		স্বভাব:
			যদি (!বার্তা.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", বার্তা.prefix);
			fmt::print(f, "{}", বার্তা.message);
			ভঙ্গ;
	}
	যদি (!বার্তা.message.empty())
		next_print_log = বার্তা.message.back() == '\n';
}

তার্কিক ColorConsoleLogSink::should_log(ধ্রুব LogMessage &বার্তা) ধ্রুব
{
	প্রত্যাগম (!error_output) || (বার্তা.severity == LogSeverity::Error || বার্তা.severity == LogSeverity::NonFatalError ||
			(বার্তা.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
পূর্ণাঙ্ক wmain(পূর্ণাঙ্ক argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<বর্ণ*> uargs;
	প্রতি (পূর্ণাঙ্ক i = 0; i < argc; i++) {
		পূর্ণাঙ্ক usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, রিক্ত, 0, রিক্ত, রিক্ত);
		বর্ণ *uarg = (বর্ণ *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, রিক্ত, রিক্ত);
		uargs.ঠেলো_back(uarg);
	}
	uargs.ঠেলো_back(রিক্ত);
	বর্ণ **argv = uargs.data();
#অন্যথা
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, বর্ণ **argv)
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
	তার্কিক scriptfile_tcl = অসত্য;
	তার্কিক scriptfile_python = অসত্য;
	তার্কিক print_banner = সত্য;
	তার্কিক print_stats = সত্য;
	তার্কিক call_abort = অসত্য;
	তার্কিক timing_details = অসত্য;
	তার্কিক run_shell = সত্য;
	তার্কিক run_tcl_shell = অসত্য;
	তার্কিক mode_v = অসত্য;
	তার্কিক mode_q = অসত্য;
	তার্কিক quiet_warnings = অসত্য;
	তার্কিক log_stderr = অসত্য;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::মান<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::মান<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::মান<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::মান<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::মান<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::মান<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::মান<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::মান<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::মান<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::মান<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::মান<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::মান<পূর্ণাঙ্ক>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::মান<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::মান<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::মান<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::মান<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::মান<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::মান<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::মান<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::মান<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::মান<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::মান<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::মান<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	যদি (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	যত্ন {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		যদি (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		যদি (result.গুনতি("M")) memhasher_on();
		যদি (result.গুনতি("X")) yosys_xtrace += result.গুনতি("X");
		যদি (result.গুনতি("A")) call_abort = সত্য;
		যদি (result.গুনতি("Q")) print_banner = অসত্য;
		যদি (result.গুনতি("T")) print_stats = অসত্য;
		যদি (result.গুনতি("no-version")) yosys_write_versions = অসত্য;
		যদি (result.গুনতি("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		যদি (result.গুনতি("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		যদি (result.গুনতি("S")) {
			passes_commands.ঠেলো_back("synth");
			run_shell = অসত্য;
		}
		যদি (result.গুনতি("C")) run_tcl_shell = সত্য;
		যদি (result.গুনতি("g")) logger().force_debug_on();
		যদি (result.গুনতি("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		যদি (result.গুনতি("f")) frontend_command = result["f"].as<std::string>();
		যদি (result.গুনতি("H")) {
			passes_commands.ঠেলো_back("help");
			run_shell = অসত্য;
		}
		যদি (result.গুনতি("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.ঠেলো_back("help " + res);
			run_shell = অসত্য;
		}
		যদি (result.গুনতি("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = অসত্য;
		}
		যদি (result.গুনতি("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.ঢোকাও(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = অসত্য;
		}
		যদি (result.গুনতি("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = অসত্য;
		}
		প্রতি (ধ্রুব auto& চাবি : {"l", "L"}) {
			যদি (result.গুনতি(চাবি)) {
				প্রতি (ধ্রুব auto& filename : result[চাবি].as<std::vector<std::string>>()) {
					যত্ন {
						logger().add_sink<FileLogSink>(filename, চাবি[0] == 'L', অসত্য);
					} গৃহাণ (ধ্রুব std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		যদি (result.গুনতি("q")) {
			mode_q = সত্য;
			যদি (log_stderr) quiet_warnings = সত্য;
			log_stderr = সত্য;
		}
		যদি (result.গুনতি("v")) {
			mode_v = সত্য;
			log_stderr = সত্য;
			logger().set_verbose_level(result["v"].as<পূর্ণাঙ্ক>());
		}
		যদি (result.গুনতি("t")) logger().set_log_time(সত্য);
		যদি (result.গুনতি("no-color")) no_color = সত্য;
		যদি (result.গুনতি("d")) timing_details = সত্য;
		প্রতি (ধ্রুব auto& চাবি : {"s", "c"}) {
			যদি (result.গুনতি(চাবি)) {
				scriptfile = result[চাবি].as<std::string>();
				scriptfile_tcl = std::string(চাবি) == "c";
				run_shell = অসত্য;
			}
		}
		যদি (result.গুনতি("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = সত্য;
			run_shell = অসত্য;
		}
		প্রতি (ধ্রুব auto& চাবি : {"W", "w", "e"}) {
			যদি (result.গুনতি(চাবি)) {
				auto regexes = result[চাবি].as<std::vector<std::string>>();
				প্রতি (ধ্রুব auto& regex : regexes) {
					যদি (std::string(চাবি) == "W")
						logger().add_warn(regex);
					যদি (std::string(চাবি) == "w")
						logger().add_nowarn(regex);
					যদি (std::string(চাবি) == "e")
						logger().add_werror(regex);
				}
			}
		}
		যদি (result.গুনতি("r")) topmodule = result["r"].as<std::string>();
		যদি (result.গুনতি("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		যদি (result.গুনতি("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			প্রতি (ধ্রুব auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				যদি (!tokens.empty() && tokens[0] == "ALL") {
					যদি (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(সত্য);
				} অন্যথা {
					যদি (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].বের_করো_back();
					যদি (tokens.size() == 1)
						tokens.ঠেলো_back("yosys_dump_" + tokens[0] + ".il");
					যদি (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		যদি (result.গুনতি("E")) depsfile = result["E"].as<std::string>();
		যদি (result.গুনতি("x")) {
			প্রতি (ধ্রুব auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		যদি (result.গুনতি("perffile")) perffile = result["perffile"].as<std::string>();
		যদি (result.গুনতি("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		যদি (result.গুনতি("autoidx")) {
			পূর্ণাঙ্ক idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		যদি (!is_colored(stdout) || !is_colored(stderr)) {
			যদি (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} অন্যথা {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} অন্যথা
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		যদি (print_banner)
			yosys_banner();

	}
	গৃহাণ (ধ্রুব cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#যদি defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#যদি defined(_WIN32)
		যদি (getenv("HOMEDRIVE") != রিক্ত && getenv("HOMEPATH") != রিক্ত) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} অন্যথা {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#অন্যথা
		যদি (getenv("XDG_STATE_HOME") == রিক্ত || getenv("XDG_STATE_HOME")[0] == '\0') {
			যদি (getenv("HOME") != রিক্ত) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} অন্যথা {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} অন্যথা {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	যদি (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	যদি (print_stats)
		logger().start_hasher();

#যদি defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#যদি defined(__linux__)
	// set stack size to >= 128 MB
	{
		সংরচনা rlimit rl;
		ধ্রুব rlim_t স্তূপ_size = 128L * 1024L * 1024L;
		যদি (getrlimit(RLIMIT_স্তূপ, &rl) == 0 && rl.rlim_cur < স্তূপ_size) {
			rl.rlim_cur = স্তূপ_size;
			setrlimit(RLIMIT_স্তূপ, &rl);
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

	প্রতি (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	যদি (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		প্রতি (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	যদি (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		প্রতি (auto special_arg : special_args)
			frontend_files.ঠেলো_back(special_arg);
	}

	প্রতি (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		যদি (run_frontend((*it).c_str(), frontend_command))
			run_shell = অসত্য;
	}

	যদি (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	যদি (!scriptfile.empty()) {
		যদি (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			পূর্ণাঙ্ক tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			প্রতি (auto arg : special_args)
				script_args.ঠেলো_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), রিক্ত, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), রিক্ত, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), রিক্ত, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			যদি (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#অন্যথা
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} অন্যথা যদি (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			পূর্ণাঙ্ক py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			প্রতি (পূর্ণাঙ্ক i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			যদি (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			যদি (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#অন্যথা
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} অন্যথা
			run_frontend(scriptfile, "script");
	}

	প্রতি (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	যদি (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#অন্যথা
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} অন্যথা {
		যদি (run_shell)
			shell(yosys_design);
		অন্যথা
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	প্রতি (auto it : saved_designs)
		it.second->check();
	প্রতি (auto it : pushed_designs)
		it->check();

	যদি (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		যদি (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		তার্কিক first = সত্য;
		প্রতি (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = অসত্য;
		}
		fprintf(f, ":");
		প্রতি (auto fn : yosys_প্রবেশ_files) {
			যদি (yosys_output_files.গুনতি(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	যদি (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(অসত্য);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		যদি (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		যদি (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		যদি (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			প্রতি (auto &[feature, earliest_removal] : logger().get_deprecated())
				যদি (earliest_removal.empty())
					log("    %s\n", feature);
				অন্যথা
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(শূন্য)wall_clock_start;
#অন্যথা
		std::string meminfo;
		std::string stats_divider = ", ";

		সংরচনা rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		যদি (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			সংরচনা rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#যদি defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#যদি defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#নোচেদ্যদি defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		দ্বিগুণ wall_seconds = std::chrono::duration<দ্বিগুণ>(
				std::chrono::steady_clock::now() - wall_clock_start).গুনতি();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, পূর্ণাঙ্ক, std::string>> timedat;

		প্রতি (auto &it : pass_register)
			যদি (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.ঢোকাও(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		যদি (twine_gc_গুনতি) {
			total_ns += twine_gc_ns;
			timedat.ঢোকাও(make_tuple(twine_gc_ns, twine_gc_গুনতি, "twine_gc"));
		}

		যদি (timing_details)
		{
			log("Time spent:\n");
			প্রতি (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", পূর্ণাঙ্ক(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		অন্যথা
		{
			পূর্ণাঙ্ক out_গুনতি = 0;
			std::string message = "Time spent:";
			প্রতি (auto it = timedat.rbegin(); it != timedat.rend() && out_গুনতি < 4; it++, out_গুনতি++) {
				যদি (out_গুনতি >= 2 && (std::get<0>(*it) < 1000000000 || পূর্ণাঙ্ক(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					ভঙ্গ;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_গুনতি ? "," : "", পূর্ণাঙ্ক(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), পূর্ণাঙ্ক(std::get<0>(*it) / 1000000000));
			}
			message += out_গুনতি ? "" : " no commands executed";
			log("%s\n", message);
		}
		যদি(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			যদি (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			তার্কিক first = সত্য;
			প্রতি (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				যদি (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = অসত্য;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	যদি (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#যদি defined(_MSC_VER)
	_exit(0);
#নোচেদ্যদি defined(_WIN32)
	_Exit(0);
#endif
	প্রত্যাগম 0;
}

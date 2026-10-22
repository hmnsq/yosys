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

#ضمن "kernel/yosys.h"
#ضمن "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#ضمن "libs/cxxopts/include/cxxopts.hpp"
#ضمن <iostream>
#ضمن <chrono>

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
نطاق py = pybind11;
#endif

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <limits.h>
#ضمن <errno.h>
#ضمن <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#ضمن <inttypes.h>

#𐡄𐡍 defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#𐡄𐡍 !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#ضمن <fmt/color.h>
#ضمن <fmt/format.h>

USING_YOSYS_NAMESPACE

#𐡄𐡍 defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحيح yosys_history_offset = 0;
std::string yosys_history_file;
#endif

فراغ yosys_atexit()
{
#𐡄𐡍 defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	𐡄𐡍 (!yosys_history_file.empty()) {
#𐡄𐡍 defined(YOSYS_ENABLE_READLINE)
		𐡄𐡍 (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} وإلا
			write_history(yosys_history_file.c_str());
#وإلا
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#𐡄𐡍 defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	𐡄𐡍 (hist_list != عدم)
		حرر(hist_list);
#endif
#endif
}

#𐡄𐡍 defined(__OpenBSD__)
نطاق Yosys {
خارجي حرف *yosys_argv0;
خارجي حرف yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
نطاق Yosys {
	خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);
	خارجي فراغ yosys_tcl_activate_repl();
};
#endif

نطاق Yosys {
	خارجي منطقي log_stderr_sink_forced;
};

صنف ColorConsoleLogSink : عام LogSink
{
عام:
	explicit ColorConsoleLogSink(منطقي err, منطقي quiet) : error_output(err), quiet_warnings(quiet) {}
	فراغ log(ثابت LogMessage &msg) override;
	منطقي should_log(ثابت LogMessage &msg) ثابت override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع error_output ? nullptr : stdout; }
خاص:
	منطقي error_output;
	منطقي quiet_warnings;
};

منطقي no_color = خطأ;

منطقي is_colored(FILE *file)
{
	𐡄𐡍 (no_color)
		أرجع خطأ;

#𐡄𐡍 defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	أرجع handle != INVALID_HANDLE_VALUE &&
			GetConsoleMode(handle, &mode);
#وإلا
	أرجع isatty(fileno(file));
#endif
}

فراغ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي next_print_log = 𐡒𐡔𐡈;

فراغ ColorConsoleLogSink::log(ثابت LogMessage &msg)
{
	FILE *f = (error_output || msg.severity == LogSeverity::Error) ? stderr : stdout;

	𐡄𐡍 (logger().get_log_time() && next_print_log) {
		auto elapsed = msg.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", صحيح(us.count() / 1'000'000),صحيح(us.count() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = خطأ;
	}

	𐡄𐡍 (!msg.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", msg.src.filename, msg.src.start_line);

	بدل (msg.severity) {
		حالة LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", msg.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", msg.message);
			اكسر;

		حالة LogSeverity::Error:
		حالة LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", msg.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", msg.message);
			اكسر;

		حالة LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", msg.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", msg.message);
			اكسر;

		حالة LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", msg.prefix, msg.message);
			اكسر;

		حالة LogSeverity::Comment:
		حالة LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", msg.prefix, msg.message);
			اكسر;

		حالة LogSeverity::Info:
		افتراضي:
			𐡄𐡍 (!msg.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", msg.prefix);
			fmt::print(f, "{}", msg.message);
			اكسر;
	}
	𐡄𐡍 (!msg.message.empty())
		next_print_log = msg.message.back() == '\n';
}

منطقي ColorConsoleLogSink::should_log(ثابت LogMessage &msg) ثابت
{
	أرجع (!error_output) || (msg.severity == LogSeverity::Error || msg.severity == LogSeverity::NonFatalError ||
			(msg.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
صحيح wmain(صحيح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<حرف*> uargs;
	لكل (صحيح i = 0; i < argc; i++) {
		صحيح usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, عدم, 0, عدم, عدم);
		حرف *uarg = (حرف *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, عدم, عدم);
		uargs.push_back(uarg);
	}
	uargs.push_back(عدم);
	حرف **argv = uargs.data();
#وإلا
صحيح main(صحيح argc, حرف **argv)
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
	منطقي scriptfile_tcl = خطأ;
	منطقي scriptfile_python = خطأ;
	منطقي print_banner = 𐡒𐡔𐡈;
	منطقي print_stats = 𐡒𐡔𐡈;
	منطقي call_abort = خطأ;
	منطقي timing_details = خطأ;
	منطقي run_shell = 𐡒𐡔𐡈;
	منطقي run_tcl_shell = خطأ;
	منطقي mode_v = خطأ;
	منطقي mode_q = خطأ;
	منطقي quiet_warnings = خطأ;
	منطقي log_stderr = خطأ;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::value<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::value<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::value<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::value<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::value<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::value<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::value<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::value<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::value<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::value<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::value<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::value<صحيح>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::value<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::value<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::value<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::value<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::value<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::value<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::value<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::value<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::value<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::value<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::value<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	𐡄𐡍 (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	حاول {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		𐡄𐡍 (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		𐡄𐡍 (result.count("M")) memhasher_on();
		𐡄𐡍 (result.count("X")) yosys_xtrace += result.count("X");
		𐡄𐡍 (result.count("A")) call_abort = 𐡒𐡔𐡈;
		𐡄𐡍 (result.count("Q")) print_banner = خطأ;
		𐡄𐡍 (result.count("T")) print_stats = خطأ;
		𐡄𐡍 (result.count("no-version")) yosys_write_versions = خطأ;
		𐡄𐡍 (result.count("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		𐡄𐡍 (result.count("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		𐡄𐡍 (result.count("S")) {
			passes_commands.push_back("synth");
			run_shell = خطأ;
		}
		𐡄𐡍 (result.count("C")) run_tcl_shell = 𐡒𐡔𐡈;
		𐡄𐡍 (result.count("g")) logger().force_debug_on();
		𐡄𐡍 (result.count("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		𐡄𐡍 (result.count("f")) frontend_command = result["f"].as<std::string>();
		𐡄𐡍 (result.count("H")) {
			passes_commands.push_back("help");
			run_shell = خطأ;
		}
		𐡄𐡍 (result.count("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.push_back("help " + res);
			run_shell = خطأ;
		}
		𐡄𐡍 (result.count("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = خطأ;
		}
		𐡄𐡍 (result.count("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.insert(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = خطأ;
		}
		𐡄𐡍 (result.count("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = خطأ;
		}
		لكل (ثابت auto& key : {"l", "L"}) {
			𐡄𐡍 (result.count(key)) {
				لكل (ثابت auto& filename : result[key].as<std::vector<std::string>>()) {
					حاول {
						logger().add_sink<FileLogSink>(filename, key[0] == 'L', خطأ);
					} امسك (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		𐡄𐡍 (result.count("q")) {
			mode_q = 𐡒𐡔𐡈;
			𐡄𐡍 (log_stderr) quiet_warnings = 𐡒𐡔𐡈;
			log_stderr = 𐡒𐡔𐡈;
		}
		𐡄𐡍 (result.count("v")) {
			mode_v = 𐡒𐡔𐡈;
			log_stderr = 𐡒𐡔𐡈;
			logger().set_verbose_level(result["v"].as<صحيح>());
		}
		𐡄𐡍 (result.count("t")) logger().set_log_time(𐡒𐡔𐡈);
		𐡄𐡍 (result.count("no-color")) no_color = 𐡒𐡔𐡈;
		𐡄𐡍 (result.count("d")) timing_details = 𐡒𐡔𐡈;
		لكل (ثابت auto& key : {"s", "c"}) {
			𐡄𐡍 (result.count(key)) {
				scriptfile = result[key].as<std::string>();
				scriptfile_tcl = std::string(key) == "c";
				run_shell = خطأ;
			}
		}
		𐡄𐡍 (result.count("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = 𐡒𐡔𐡈;
			run_shell = خطأ;
		}
		لكل (ثابت auto& key : {"W", "w", "e"}) {
			𐡄𐡍 (result.count(key)) {
				auto regexes = result[key].as<std::vector<std::string>>();
				لكل (ثابت auto& regex : regexes) {
					𐡄𐡍 (std::string(key) == "W")
						logger().add_warn(regex);
					𐡄𐡍 (std::string(key) == "w")
						logger().add_nowarn(regex);
					𐡄𐡍 (std::string(key) == "e")
						logger().add_werror(regex);
				}
			}
		}
		𐡄𐡍 (result.count("r")) topmodule = result["r"].as<std::string>();
		𐡄𐡍 (result.count("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		𐡄𐡍 (result.count("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			لكل (ثابت auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				𐡄𐡍 (!tokens.empty() && tokens[0] == "ALL") {
					𐡄𐡍 (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(𐡒𐡔𐡈);
				} وإلا {
					𐡄𐡍 (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].pop_back();
					𐡄𐡍 (tokens.size() == 1)
						tokens.push_back("yosys_dump_" + tokens[0] + ".il");
					𐡄𐡍 (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		𐡄𐡍 (result.count("E")) depsfile = result["E"].as<std::string>();
		𐡄𐡍 (result.count("x")) {
			لكل (ثابت auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		𐡄𐡍 (result.count("perffile")) perffile = result["perffile"].as<std::string>();
		𐡄𐡍 (result.count("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		𐡄𐡍 (result.count("autoidx")) {
			صحيح idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		𐡄𐡍 (!is_colored(stdout) || !is_colored(stderr)) {
			𐡄𐡍 (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} وإلا {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} وإلا
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		𐡄𐡍 (print_banner)
			yosys_banner();

	}
	امسك (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#𐡄𐡍 defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#𐡄𐡍 defined(_WIN32)
		𐡄𐡍 (getenv("HOMEDRIVE") != عدم && getenv("HOMEPATH") != عدم) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} وإلا {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#وإلا
		𐡄𐡍 (getenv("XDG_STATE_HOME") == عدم || getenv("XDG_STATE_HOME")[0] == '\0') {
			𐡄𐡍 (getenv("HOME") != عدم) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} وإلا {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} وإلا {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	𐡄𐡍 (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	𐡄𐡍 (print_stats)
		logger().start_hasher();

#𐡄𐡍 defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#𐡄𐡍 defined(__linux__)
	// set stack size to >= 128 MB
	{
		بنية rlimit rl;
		ثابت rlim_t stack_size = 128L * 1024L * 1024L;
		𐡄𐡍 (getrlimit(RLIMIT_STACK, &rl) == 0 && rl.rlim_cur < stack_size) {
			rl.rlim_cur = stack_size;
			setrlimit(RLIMIT_STACK, &rl);
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

	لكل (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	𐡄𐡍 (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		لكل (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	𐡄𐡍 (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		لكل (auto special_arg : special_args)
			frontend_files.push_back(special_arg);
	}

	لكل (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		𐡄𐡍 (run_frontend((*it).c_str(), frontend_command))
			run_shell = خطأ;
	}

	𐡄𐡍 (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	𐡄𐡍 (!scriptfile.empty()) {
		𐡄𐡍 (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			صحيح tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			لكل (auto arg : special_args)
				script_args.push_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			𐡄𐡍 (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#وإلا
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} وإلا 𐡄𐡍 (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحيح py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			لكل (صحيح i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			𐡄𐡍 (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			𐡄𐡍 (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#وإلا
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} وإلا
			run_frontend(scriptfile, "script");
	}

	لكل (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	𐡄𐡍 (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وإلا
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} وإلا {
		𐡄𐡍 (run_shell)
			shell(yosys_design);
		وإلا
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	لكل (auto it : saved_designs)
		it.second->check();
	لكل (auto it : pushed_designs)
		it->check();

	𐡄𐡍 (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		𐡄𐡍 (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقي first = 𐡒𐡔𐡈;
		لكل (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = خطأ;
		}
		fprintf(f, ":");
		لكل (auto fn : yosys_input_files) {
			𐡄𐡍 (yosys_output_files.count(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	𐡄𐡍 (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(خطأ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		𐡄𐡍 (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		𐡄𐡍 (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		𐡄𐡍 (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			لكل (auto &[feature, earliest_removal] : logger().get_deprecated())
				𐡄𐡍 (earliest_removal.empty())
					log("    %s\n", feature);
				وإلا
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(فراغ)wall_clock_start;
#وإلا
		std::string meminfo;
		std::string stats_divider = ", ";

		بنية rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		𐡄𐡍 (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			بنية rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#𐡄𐡍 defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#𐡄𐡍 defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#وإلاإذا defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		مزدوج wall_seconds = std::chrono::duration<مزدوج>(
				std::chrono::steady_clock::now() - wall_clock_start).count();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, صحيح, std::string>> timedat;

		لكل (auto &it : pass_register)
			𐡄𐡍 (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.insert(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		𐡄𐡍 (twine_gc_count) {
			total_ns += twine_gc_ns;
			timedat.insert(make_tuple(twine_gc_ns, twine_gc_count, "twine_gc"));
		}

		𐡄𐡍 (timing_details)
		{
			log("Time spent:\n");
			لكل (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", صحيح(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وإلا
		{
			صحيح out_count = 0;
			std::string message = "Time spent:";
			لكل (auto it = timedat.rbegin(); it != timedat.rend() && out_count < 4; it++, out_count++) {
				𐡄𐡍 (out_count >= 2 && (std::get<0>(*it) < 1000000000 || صحيح(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					اكسر;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_count ? "," : "", صحيح(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحيح(std::get<0>(*it) / 1000000000));
			}
			message += out_count ? "" : " no commands executed";
			log("%s\n", message);
		}
		𐡄𐡍(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			𐡄𐡍 (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			منطقي first = 𐡒𐡔𐡈;
			لكل (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				𐡄𐡍 (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = خطأ;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	𐡄𐡍 (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#𐡄𐡍 defined(_MSC_VER)
	_exit(0);
#وإلاإذا defined(_WIN32)
	_Exit(0);
#endif
	أرجع 0;
}

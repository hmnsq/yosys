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

#شامل "kernel/yosys.h"
#شامل "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#شامل "libs/cxxopts/include/cxxopts.hpp"
#شامل <iostream>
#شامل <chrono>

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
فضاینام py = pybind11;
#endif

#شامل <stdio.h>
#شامل <string.h>
#شامل <limits.h>
#شامل <errno.h>
#شامل <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#شامل <inttypes.h>

#اگر defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#اگر !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#شامل <fmt/color.h>
#شامل <fmt/format.h>

USING_YOSYS_NAMESPACE

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحیح yosys_history_offset = 0;
std::string yosys_history_file;
#endif

تهی yosys_atexit()
{
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	اگر (!yosys_history_file.empty()) {
#اگر defined(YOSYS_ENABLE_READLINE)
		اگر (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} وگرنه
			write_history(yosys_history_file.c_str());
#وگرنه
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#اگر defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	اگر (hist_list != پوچ)
		آزاد(hist_list);
#endif
#endif
}

#اگر defined(__OpenBSD__)
فضاینام Yosys {
بیرونی نویسه *yosys_argv0;
بیرونی نویسه yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
فضاینام Yosys {
	بیرونی صحیح yosys_tcl_interp_init(Tcl_Interp *interp);
	بیرونی تهی yosys_tcl_activate_repl();
};
#endif

فضاینام Yosys {
	بیرونی منطقی log_stderr_sink_forced;
};

رده ColorConsoleLogSink : عمومی LogSink
{
عمومی:
	explicit ColorConsoleLogSink(منطقی err, منطقی quiet) : error_output(err), quiet_warnings(quiet) {}
	تهی log(ثابت LogMessage &پیام) override;
	منطقی should_log(ثابت LogMessage &پیام) ثابت override;
	تهی flush() override;
	FILE *file_handle() override { بازگشت error_output ? nullptr : stdout; }
خصوصی:
	منطقی error_output;
	منطقی quiet_warnings;
};

منطقی no_color = نادرست;

منطقی is_colored(FILE *file)
{
	اگر (no_color)
		بازگشت نادرست;

#اگر defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	بازگشت handle != INVALID_HANDLE_مقدار &&
			GetConsoleMode(handle, &mode);
#وگرنه
	بازگشت isatty(fileno(file));
#endif
}

تهی ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقی next_print_log = درست;

تهی ColorConsoleLogSink::log(ثابت LogMessage &پیام)
{
	FILE *f = (error_output || پیام.severity == LogSeverity::Error) ? stderr : stdout;

	اگر (logger().get_log_time() && next_print_log) {
		auto elapsed = پیام.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", صحیح(us.شمار() / 1'000'000),صحیح(us.شمار() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = نادرست;
	}

	اگر (!پیام.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", پیام.src.filename, پیام.src.start_سطر);

	انتخاب (پیام.severity) {
		حالت LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", پیام.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیام.message);
			بشکن;

		حالت LogSeverity::Error:
		حالت LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", پیام.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیام.message);
			بشکن;

		حالت LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", پیام.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", پیام.message);
			بشکن;

		حالت LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", پیام.prefix, پیام.message);
			بشکن;

		حالت LogSeverity::Comment:
		حالت LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", پیام.prefix, پیام.message);
			بشکن;

		حالت LogSeverity::Info:
		پیشفرض:
			اگر (!پیام.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", پیام.prefix);
			fmt::print(f, "{}", پیام.message);
			بشکن;
	}
	اگر (!پیام.message.empty())
		next_print_log = پیام.message.back() == '\n';
}

منطقی ColorConsoleLogSink::should_log(ثابت LogMessage &پیام) ثابت
{
	بازگشت (!error_output) || (پیام.severity == LogSeverity::Error || پیام.severity == LogSeverity::NonFatalError ||
			(پیام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
صحیح wmain(صحیح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<نویسه*> uargs;
	برای (صحیح i = 0; i < argc; i++) {
		صحیح usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, پوچ, 0, پوچ, پوچ);
		نویسه *uarg = (نویسه *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, پوچ, پوچ);
		uargs.افزودن_back(uarg);
	}
	uargs.افزودن_back(پوچ);
	نویسه **argv = uargs.data();
#وگرنه
صحیح اصلی(صحیح argc, نویسه **argv)
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
	منطقی scriptfile_tcl = نادرست;
	منطقی scriptfile_python = نادرست;
	منطقی print_banner = درست;
	منطقی print_stats = درست;
	منطقی call_abort = نادرست;
	منطقی timing_details = نادرست;
	منطقی run_shell = درست;
	منطقی run_tcl_shell = نادرست;
	منطقی mode_v = نادرست;
	منطقی mode_q = نادرست;
	منطقی quiet_warnings = نادرست;
	منطقی log_stderr = نادرست;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::مقدار<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::مقدار<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::مقدار<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::مقدار<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::مقدار<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::مقدار<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::مقدار<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::مقدار<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::مقدار<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::مقدار<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::مقدار<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::مقدار<صحیح>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::مقدار<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::مقدار<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::مقدار<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::مقدار<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::مقدار<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::مقدار<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::مقدار<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::مقدار<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::مقدار<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::مقدار<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::مقدار<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	اگر (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	تلاش {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		اگر (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		اگر (result.شمار("M")) memhasher_on();
		اگر (result.شمار("X")) yosys_xtrace += result.شمار("X");
		اگر (result.شمار("A")) call_abort = درست;
		اگر (result.شمار("Q")) print_banner = نادرست;
		اگر (result.شمار("T")) print_stats = نادرست;
		اگر (result.شمار("no-version")) yosys_write_versions = نادرست;
		اگر (result.شمار("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		اگر (result.شمار("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		اگر (result.شمار("S")) {
			passes_commands.افزودن_back("synth");
			run_shell = نادرست;
		}
		اگر (result.شمار("C")) run_tcl_shell = درست;
		اگر (result.شمار("g")) logger().force_debug_on();
		اگر (result.شمار("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		اگر (result.شمار("f")) frontend_command = result["f"].as<std::string>();
		اگر (result.شمار("H")) {
			passes_commands.افزودن_back("help");
			run_shell = نادرست;
		}
		اگر (result.شمار("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.افزودن_back("help " + res);
			run_shell = نادرست;
		}
		اگر (result.شمار("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = نادرست;
		}
		اگر (result.شمار("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.درج(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = نادرست;
		}
		اگر (result.شمار("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = نادرست;
		}
		برای (ثابت auto& کلید : {"l", "L"}) {
			اگر (result.شمار(کلید)) {
				برای (ثابت auto& filename : result[کلید].as<std::vector<std::string>>()) {
					تلاش {
						logger().add_sink<FileLogSink>(filename, کلید[0] == 'L', نادرست);
					} بگیر (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		اگر (result.شمار("q")) {
			mode_q = درست;
			اگر (log_stderr) quiet_warnings = درست;
			log_stderr = درست;
		}
		اگر (result.شمار("v")) {
			mode_v = درست;
			log_stderr = درست;
			logger().set_verbose_level(result["v"].as<صحیح>());
		}
		اگر (result.شمار("t")) logger().set_log_time(درست);
		اگر (result.شمار("no-color")) no_color = درست;
		اگر (result.شمار("d")) timing_details = درست;
		برای (ثابت auto& کلید : {"s", "c"}) {
			اگر (result.شمار(کلید)) {
				scriptfile = result[کلید].as<std::string>();
				scriptfile_tcl = std::string(کلید) == "c";
				run_shell = نادرست;
			}
		}
		اگر (result.شمار("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = درست;
			run_shell = نادرست;
		}
		برای (ثابت auto& کلید : {"W", "w", "e"}) {
			اگر (result.شمار(کلید)) {
				auto regexes = result[کلید].as<std::vector<std::string>>();
				برای (ثابت auto& regex : regexes) {
					اگر (std::string(کلید) == "W")
						logger().add_warn(regex);
					اگر (std::string(کلید) == "w")
						logger().add_nowarn(regex);
					اگر (std::string(کلید) == "e")
						logger().add_werror(regex);
				}
			}
		}
		اگر (result.شمار("r")) topmodule = result["r"].as<std::string>();
		اگر (result.شمار("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		اگر (result.شمار("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			برای (ثابت auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				اگر (!tokens.empty() && tokens[0] == "ALL") {
					اگر (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(درست);
				} وگرنه {
					اگر (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].برداشت_back();
					اگر (tokens.size() == 1)
						tokens.افزودن_back("yosys_dump_" + tokens[0] + ".il");
					اگر (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		اگر (result.شمار("E")) depsfile = result["E"].as<std::string>();
		اگر (result.شمار("x")) {
			برای (ثابت auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		اگر (result.شمار("perffile")) perffile = result["perffile"].as<std::string>();
		اگر (result.شمار("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		اگر (result.شمار("autoidx")) {
			صحیح idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		اگر (!is_colored(stdout) || !is_colored(stderr)) {
			اگر (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} وگرنه {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} وگرنه
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		اگر (print_banner)
			yosys_banner();

	}
	بگیر (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#اگر defined(_WIN32)
		اگر (getenv("HOMEDRIVE") != پوچ && getenv("HOMEPATH") != پوچ) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} وگرنه {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#وگرنه
		اگر (getenv("XDG_STATE_HOME") == پوچ || getenv("XDG_STATE_HOME")[0] == '\0') {
			اگر (getenv("HOME") != پوچ) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} وگرنه {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} وگرنه {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	اگر (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	اگر (print_stats)
		logger().start_hasher();

#اگر defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#اگر defined(__linux__)
	// set stack size to >= 128 MB
	{
		ساختار rlimit rl;
		ثابت rlim_t پشته_size = 128L * 1024L * 1024L;
		اگر (getrlimit(RLIMIT_پشته, &rl) == 0 && rl.rlim_cur < پشته_size) {
			rl.rlim_cur = پشته_size;
			setrlimit(RLIMIT_پشته, &rl);
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

	برای (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	اگر (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		برای (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	اگر (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		برای (auto special_arg : special_args)
			frontend_files.افزودن_back(special_arg);
	}

	برای (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		اگر (run_frontend((*it).c_str(), frontend_command))
			run_shell = نادرست;
	}

	اگر (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	اگر (!scriptfile.empty()) {
		اگر (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			صحیح tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			برای (auto arg : special_args)
				script_args.افزودن_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), پوچ, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), پوچ, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), پوچ, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			اگر (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#وگرنه
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} وگرنه اگر (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحیح py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			برای (صحیح i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			اگر (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			اگر (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#وگرنه
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} وگرنه
			run_frontend(scriptfile, "script");
	}

	برای (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	اگر (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وگرنه
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} وگرنه {
		اگر (run_shell)
			shell(yosys_design);
		وگرنه
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	برای (auto it : saved_designs)
		it.second->check();
	برای (auto it : pushed_designs)
		it->check();

	اگر (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		اگر (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقی first = درست;
		برای (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = نادرست;
		}
		fprintf(f, ":");
		برای (auto fn : yosys_درون_داد_files) {
			اگر (yosys_output_files.شمار(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	اگر (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(نادرست);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		اگر (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		اگر (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		اگر (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			برای (auto &[feature, earliest_removal] : logger().get_deprecated())
				اگر (earliest_removal.empty())
					log("    %s\n", feature);
				وگرنه
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(تهی)wall_clock_start;
#وگرنه
		std::string meminfo;
		std::string stats_divider = ", ";

		ساختار rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		اگر (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ساختار rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#اگر defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#اگر defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#وگرنهاگر defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		دوگانه wall_seconds = std::chrono::duration<دوگانه>(
				std::chrono::steady_clock::now() - wall_clock_start).شمار();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, صحیح, std::string>> timedat;

		برای (auto &it : pass_register)
			اگر (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.درج(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		اگر (twine_gc_شمار) {
			total_ns += twine_gc_ns;
			timedat.درج(make_tuple(twine_gc_ns, twine_gc_شمار, "twine_gc"));
		}

		اگر (timing_details)
		{
			log("Time spent:\n");
			برای (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", صحیح(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وگرنه
		{
			صحیح out_شمار = 0;
			std::string message = "Time spent:";
			برای (auto it = timedat.rbegin(); it != timedat.rend() && out_شمار < 4; it++, out_شمار++) {
				اگر (out_شمار >= 2 && (std::get<0>(*it) < 1000000000 || صحیح(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					بشکن;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_شمار ? "," : "", صحیح(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحیح(std::get<0>(*it) / 1000000000));
			}
			message += out_شمار ? "" : " no commands executed";
			log("%s\n", message);
		}
		اگر(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			اگر (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			منطقی first = درست;
			برای (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				اگر (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = نادرست;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	اگر (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#اگر defined(_MSC_VER)
	_exit(0);
#وگرنهاگر defined(_WIN32)
	_Exit(0);
#endif
	بازگشت 0;
}

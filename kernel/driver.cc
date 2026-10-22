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

#إذا defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#ضمن <fmt/color.h>
#ضمن <fmt/format.h>

USING_YOSYS_NAMESPACE

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحيح yosys_history_offset = 0;
std::string yosys_history_file;
#endif

فراغ yosys_atexit()
{
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	إذا (!yosys_history_file.empty()) {
#إذا defined(YOSYS_ENABLE_READLINE)
		إذا (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} وإلا
			write_history(yosys_history_file.c_str());
#وإلا
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#إذا defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	إذا (hist_list != عدم)
		حرر(hist_list);
#endif
#endif
}

#إذا defined(__OpenBSD__)
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
	فراغ log(ثابت LogMessage &رسالة) override;
	منطقي should_log(ثابت LogMessage &رسالة) ثابت override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع error_output ? nullptr : stdout; }
خاص:
	منطقي error_output;
	منطقي quiet_warnings;
};

منطقي no_color = خطأ;

منطقي is_colored(FILE *file)
{
	إذا (no_color)
		أرجع خطأ;

#إذا defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	أرجع handle != INVALID_HANDLE_قيمة &&
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

منطقي next_اطبع_log = true;

فراغ ColorConsoleLogSink::log(ثابت LogMessage &رسالة)
{
	FILE *f = (error_output || رسالة.severity == LogSeverity::Error) ? stderr : stdout;

	إذا (logger().get_log_time() && next_اطبع_log) {
		auto elapsed = رسالة.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", صحيح(us.عدد() / 1'000'000),صحيح(us.عدد() % 1'000'000));
		fmt::اطبع(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_اطبع_log = خطأ;
	}

	إذا (!رسالة.src.filename.empty())
		fmt::اطبع(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", رسالة.src.filename, رسالة.src.start_سطر);

	بدل (رسالة.severity) {
		حالة LogSeverity::Warning:
			fmt::اطبع(f, fg(fmt::terminal_color::bright_yellow), "{}", رسالة.prefix);
			fmt::اطبع(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", رسالة.message);
			اكسر;

		حالة LogSeverity::Error:
		حالة LogSeverity::NonFatalError:
			fmt::اطبع(f, fg(fmt::terminal_color::bright_red), "{}", رسالة.prefix);
			fmt::اطبع(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", رسالة.message);
			اكسر;

		حالة LogSeverity::Header:
			fmt::اطبع(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", رسالة.prefix);
			fmt::اطبع(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", رسالة.message);
			اكسر;

		حالة LogSeverity::Highlight:
			fmt::اطبع(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", رسالة.prefix, رسالة.message);
			اكسر;

		حالة LogSeverity::Comment:
		حالة LogSeverity::Debug:
			fmt::اطبع(f, fg(fmt::terminal_color::bright_black), "{}{}", رسالة.prefix, رسالة.message);
			اكسر;

		حالة LogSeverity::Info:
		افتراضي:
			إذا (!رسالة.prefix.empty())
				fmt::اطبع(f, fg(fmt::terminal_color::bright_black), "{}", رسالة.prefix);
			fmt::اطبع(f, "{}", رسالة.message);
			اكسر;
	}
	إذا (!رسالة.message.empty())
		next_اطبع_log = رسالة.message.back() == '\n';
}

منطقي ColorConsoleLogSink::should_log(ثابت LogMessage &رسالة) ثابت
{
	أرجع (!error_output) || (رسالة.severity == LogSeverity::Error || رسالة.severity == LogSeverity::NonFatalError ||
			(رسالة.severity == LogSeverity::Warning && !quiet_warnings) ||
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
		uargs.ادفع_back(uarg);
	}
	uargs.ادفع_back(عدم);
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
	std::vector<std::string> special_وسائط;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	منطقي scriptfile_tcl = خطأ;
	منطقي scriptfile_python = خطأ;
	منطقي اطبع_banner = true;
	منطقي اطبع_stats = true;
	منطقي call_abort = خطأ;
	منطقي timing_details = خطأ;
	منطقي run_shell = true;
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
			cxxopts::قيمة<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::قيمة<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::قيمة<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::قيمة<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::قيمة<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::قيمة<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::قيمة<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::قيمة<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::قيمة<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::قيمة<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::قيمة<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::قيمة<صحيح>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::قيمة<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::قيمة<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::قيمة<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::قيمة<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::قيمة<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::قيمة<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::قيمة<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::قيمة<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::قيمة<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::قيمة<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::قيمة<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	إذا (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	حاول {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		إذا (it != argv + argc) {
			special_وسائط.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		إذا (result.عدد("M")) memhasher_on();
		إذا (result.عدد("X")) yosys_xtrace += result.عدد("X");
		إذا (result.عدد("A")) call_abort = true;
		إذا (result.عدد("Q")) اطبع_banner = خطأ;
		إذا (result.عدد("T")) اطبع_stats = خطأ;
		إذا (result.عدد("no-version")) yosys_write_versions = خطأ;
		إذا (result.عدد("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		إذا (result.عدد("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		إذا (result.عدد("S")) {
			passes_commands.ادفع_back("synth");
			run_shell = خطأ;
		}
		إذا (result.عدد("C")) run_tcl_shell = true;
		إذا (result.عدد("g")) logger().force_debug_on();
		إذا (result.عدد("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		إذا (result.عدد("f")) frontend_command = result["f"].as<std::string>();
		إذا (result.عدد("H")) {
			passes_commands.ادفع_back("help");
			run_shell = خطأ;
		}
		إذا (result.عدد("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.ادفع_back("help " + res);
			run_shell = خطأ;
		}
		إذا (result.عدد("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = خطأ;
		}
		إذا (result.عدد("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.أدرج(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = خطأ;
		}
		إذا (result.عدد("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = خطأ;
		}
		لكل (ثابت auto& مفتاح : {"l", "L"}) {
			إذا (result.عدد(مفتاح)) {
				لكل (ثابت auto& filename : result[مفتاح].as<std::vector<std::string>>()) {
					حاول {
						logger().add_sink<FileLogSink>(filename, مفتاح[0] == 'L', خطأ);
					} امسك (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		إذا (result.عدد("q")) {
			mode_q = true;
			إذا (log_stderr) quiet_warnings = true;
			log_stderr = true;
		}
		إذا (result.عدد("v")) {
			mode_v = true;
			log_stderr = true;
			logger().set_verbose_level(result["v"].as<صحيح>());
		}
		إذا (result.عدد("t")) logger().set_log_time(true);
		إذا (result.عدد("no-color")) no_color = true;
		إذا (result.عدد("d")) timing_details = true;
		لكل (ثابت auto& مفتاح : {"s", "c"}) {
			إذا (result.عدد(مفتاح)) {
				scriptfile = result[مفتاح].as<std::string>();
				scriptfile_tcl = std::string(مفتاح) == "c";
				run_shell = خطأ;
			}
		}
		إذا (result.عدد("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = true;
			run_shell = خطأ;
		}
		لكل (ثابت auto& مفتاح : {"W", "w", "e"}) {
			إذا (result.عدد(مفتاح)) {
				auto regexes = result[مفتاح].as<std::vector<std::string>>();
				لكل (ثابت auto& regex : regexes) {
					إذا (std::string(مفتاح) == "W")
						logger().add_warn(regex);
					إذا (std::string(مفتاح) == "w")
						logger().add_nowarn(regex);
					إذا (std::string(مفتاح) == "e")
						logger().add_werror(regex);
				}
			}
		}
		إذا (result.عدد("r")) topmodule = result["r"].as<std::string>();
		إذا (result.عدد("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		إذا (result.عدد("P")) {
			auto dump_وسائط = result["P"].as<std::vector<std::string>>();
			لكل (ثابت auto& arg : dump_وسائط) {
				auto tokens = split_tokens(arg, ":");
				إذا (!tokens.empty() && tokens[0] == "ALL") {
					إذا (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(true);
				} وإلا {
					إذا (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].اسحب_back();
					إذا (tokens.size() == 1)
						tokens.ادفع_back("yosys_dump_" + tokens[0] + ".il");
					إذا (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		إذا (result.عدد("E")) depsfile = result["E"].as<std::string>();
		إذا (result.عدد("x")) {
			لكل (ثابت auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		إذا (result.عدد("perffile")) perffile = result["perffile"].as<std::string>();
		إذا (result.عدد("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		إذا (result.عدد("autoidx")) {
			صحيح idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		إذا (!is_colored(stdout) || !is_colored(stderr)) {
			إذا (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} وإلا {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} وإلا
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		إذا (اطبع_banner)
			yosys_banner();

	}
	امسك (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#إذا defined(_WIN32)
		إذا (getenv("HOMEDRIVE") != عدم && getenv("HOMEPATH") != عدم) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} وإلا {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#وإلا
		إذا (getenv("XDG_STATE_HOME") == عدم || getenv("XDG_STATE_HOME")[0] == '\0') {
			إذا (getenv("HOME") != عدم) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} وإلا {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} وإلا {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	إذا (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	إذا (اطبع_stats)
		logger().start_hasher();

#إذا defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#إذا defined(__linux__)
	// set stack size to >= 128 MB
	{
		بنية rlimit rl;
		ثابت rlim_t مكدس_size = 128L * 1024L * 1024L;
		إذا (getrlimit(RLIMIT_مكدس, &rl) == 0 && rl.rlim_cur < مكدس_size) {
			rl.rlim_cur = مكدس_size;
			setrlimit(RLIMIT_مكدس, &rl);
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

	إذا (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		لكل (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	إذا (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		لكل (auto special_arg : special_وسائط)
			frontend_files.ادفع_back(special_arg);
	}

	لكل (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		إذا (run_frontend((*it).c_str(), frontend_command))
			run_shell = خطأ;
	}

	إذا (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	إذا (!scriptfile.empty()) {
		إذا (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			صحيح tcl_argc = special_وسائط.size();
			std::vector<Tcl_Obj*> script_وسائط;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			لكل (auto arg : special_وسائط)
				script_وسائط.ادفع_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(tcl_argc, script_وسائط.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			إذا (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#وإلا
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} وإلا إذا (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحيح py_argc = special_وسائط.size() + 1;
			new_argv.append(scriptfile);
			لكل (صحيح i = 1; i < py_argc; ++i)
				new_argv.append(special_وسائط[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			إذا (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			إذا (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				Py_Err_اطبع();
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

	إذا (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وإلا
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} وإلا {
		إذا (run_shell)
			shell(yosys_design);
		وإلا
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	لكل (auto it : saved_designs)
		it.second->check();
	لكل (auto it : pushed_designs)
		it->check();

	إذا (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		إذا (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقي first = true;
		لكل (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = خطأ;
		}
		fprintf(f, ":");
		لكل (auto fn : yosys_مدخل_files) {
			إذا (yosys_output_files.عدد(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	إذا (اطبع_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(خطأ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		إذا (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		إذا (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		إذا (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			لكل (auto &[feature, earliest_removal] : logger().get_deprecated())
				إذا (earliest_removal.empty())
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
		إذا (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			بنية rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#إذا defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#إذا defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#وإلاإذا defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		مزدوج wall_seconds = std::chrono::duration<مزدوج>(
				std::chrono::steady_clock::now() - wall_clock_start).عدد();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, صحيح, std::string>> timedat;

		لكل (auto &it : pass_register)
			إذا (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.أدرج(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		إذا (twine_gc_عدد) {
			total_ns += twine_gc_ns;
			timedat.أدرج(make_tuple(twine_gc_ns, twine_gc_عدد, "twine_gc"));
		}

		إذا (timing_details)
		{
			log("Time spent:\n");
			لكل (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", صحيح(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وإلا
		{
			صحيح out_عدد = 0;
			std::string message = "Time spent:";
			لكل (auto it = timedat.rbegin(); it != timedat.rend() && out_عدد < 4; it++, out_عدد++) {
				إذا (out_عدد >= 2 && (std::get<0>(*it) < 1000000000 || صحيح(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					اكسر;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_عدد ? "," : "", صحيح(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحيح(std::get<0>(*it) / 1000000000));
			}
			message += out_عدد ? "" : " no commands executed";
			log("%s\n", message);
		}
		إذا(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			إذا (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			منطقي first = true;
			لكل (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				إذا (!first)
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
	إذا (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#إذا defined(_MSC_VER)
	_exit(0);
#وإلاإذا defined(_WIN32)
	_Exit(0);
#endif
	أرجع 0;
}

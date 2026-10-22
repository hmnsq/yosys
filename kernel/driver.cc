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
#அந்தர்பாவ "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#அந்தர்பாவ "libs/cxxopts/include/cxxopts.hpp"
#அந்தர்பாவ <iostream>
#அந்தர்பாவ <chrono>

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
நாமாகாஶ py = pybind11;
#endif

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#அந்தர்பாவ <inttypes.h>

#எனில் defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#எனில் !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#அந்தர்பாவ <fmt/color.h>
#அந்தர்பாவ <fmt/format.h>

USING_YOSYS_NAMESPACE

#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
முழு yosys_history_offset = 0;
std::string yosys_history_file;
#endif

வெறுமை yosys_atexit()
{
#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	எனில் (!yosys_history_file.empty()) {
#எனில் defined(YOSYS_ENABLE_READLINE)
		எனில் (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} இல்லையெனில்
			write_history(yosys_history_file.c_str());
#இல்லையெனில்
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#எனில் defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	எனில் (hist_list != வெற்று)
		விடுவி(hist_list);
#endif
#endif
}

#எனில் defined(__OpenBSD__)
நாமாகாஶ Yosys {
வெளி எழுத்து *yosys_argv0;
வெளி எழுத்து yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
நாமாகாஶ Yosys {
	வெளி முழு yosys_tcl_interp_init(Tcl_Interp *interp);
	வெளி வெறுமை yosys_tcl_activate_repl();
};
#endif

நாமாகாஶ Yosys {
	வெளி தருக்க log_stderr_sink_forced;
};

வகுப்பு ColorConsoleLogSink : பொது LogSink
{
பொது:
	explicit ColorConsoleLogSink(தருக்க err, தருக்க quiet) : error_output(err), quiet_warnings(quiet) {}
	வெறுமை log(மாறிலி LogMessage &செய்தி) override;
	தருக்க should_log(மாறிலி LogMessage &செய்தி) மாறிலி override;
	வெறுமை flush() override;
	FILE *file_handle() override { திரும்பு error_output ? nullptr : stdout; }
தனிப்பட்ட:
	தருக்க error_output;
	தருக்க quiet_warnings;
};

தருக்க no_color = பொய்;

தருக்க is_colored(FILE *file)
{
	எனில் (no_color)
		திரும்பு பொய்;

#எனில் defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	திரும்பு handle != INVALID_HANDLE_மதிப்பு &&
			GetConsoleMode(handle, &mode);
#இல்லையெனில்
	திரும்பு isatty(fileno(file));
#endif
}

வெறுமை ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

தருக்க next_print_log = உண்மை;

வெறுமை ColorConsoleLogSink::log(மாறிலி LogMessage &செய்தி)
{
	FILE *f = (error_output || செய்தி.severity == LogSeverity::Error) ? stderr : stdout;

	எனில் (logger().get_log_time() && next_print_log) {
		auto elapsed = செய்தி.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", முழு(us.மொத்தம்() / 1'000'000),முழு(us.மொத்தம்() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = பொய்;
	}

	எனில் (!செய்தி.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", செய்தி.src.filename, செய்தி.src.start_வரி);

	தேர்வு (செய்தி.severity) {
		நிலை LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", செய்தி.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", செய்தி.message);
			நிறுத்து;

		நிலை LogSeverity::Error:
		நிலை LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", செய்தி.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", செய்தி.message);
			நிறுத்து;

		நிலை LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", செய்தி.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", செய்தி.message);
			நிறுத்து;

		நிலை LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", செய்தி.prefix, செய்தி.message);
			நிறுத்து;

		நிலை LogSeverity::Comment:
		நிலை LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", செய்தி.prefix, செய்தி.message);
			நிறுத்து;

		நிலை LogSeverity::Info:
		இயல்பு:
			எனில் (!செய்தி.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", செய்தி.prefix);
			fmt::print(f, "{}", செய்தி.message);
			நிறுத்து;
	}
	எனில் (!செய்தி.message.empty())
		next_print_log = செய்தி.message.back() == '\n';
}

தருக்க ColorConsoleLogSink::should_log(மாறிலி LogMessage &செய்தி) மாறிலி
{
	திரும்பு (!error_output) || (செய்தி.severity == LogSeverity::Error || செய்தி.severity == LogSeverity::NonFatalError ||
			(செய்தி.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
முழு wmain(முழு argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<எழுத்து*> uargs;
	க்கு (முழு i = 0; i < argc; i++) {
		முழு usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, வெற்று, 0, வெற்று, வெற்று);
		எழுத்து *uarg = (எழுத்து *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, வெற்று, வெற்று);
		uargs.தள்ளு_back(uarg);
	}
	uargs.தள்ளு_back(வெற்று);
	எழுத்து **argv = uargs.data();
#இல்லையெனில்
முழு முதன்மை(முழு argc, எழுத்து **argv)
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
	தருக்க scriptfile_tcl = பொய்;
	தருக்க scriptfile_python = பொய்;
	தருக்க print_banner = உண்மை;
	தருக்க print_stats = உண்மை;
	தருக்க call_abort = பொய்;
	தருக்க timing_details = பொய்;
	தருக்க run_shell = உண்மை;
	தருக்க run_tcl_shell = பொய்;
	தருக்க mode_v = பொய்;
	தருக்க mode_q = பொய்;
	தருக்க quiet_warnings = பொய்;
	தருக்க log_stderr = பொய்;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::மதிப்பு<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::மதிப்பு<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::மதிப்பு<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::மதிப்பு<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::மதிப்பு<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::மதிப்பு<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::மதிப்பு<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::மதிப்பு<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::மதிப்பு<முழு>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::மதிப்பு<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::மதிப்பு<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::மதிப்பு<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::மதிப்பு<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::மதிப்பு<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::மதிப்பு<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	எனில் (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	முயல் {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		எனில் (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		எனில் (result.மொத்தம்("M")) memhasher_on();
		எனில் (result.மொத்தம்("X")) yosys_xtrace += result.மொத்தம்("X");
		எனில் (result.மொத்தம்("A")) call_abort = உண்மை;
		எனில் (result.மொத்தம்("Q")) print_banner = பொய்;
		எனில் (result.மொத்தம்("T")) print_stats = பொய்;
		எனில் (result.மொத்தம்("no-version")) yosys_write_versions = பொய்;
		எனில் (result.மொத்தம்("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		எனில் (result.மொத்தம்("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		எனில் (result.மொத்தம்("S")) {
			passes_commands.தள்ளு_back("synth");
			run_shell = பொய்;
		}
		எனில் (result.மொத்தம்("C")) run_tcl_shell = உண்மை;
		எனில் (result.மொத்தம்("g")) logger().force_debug_on();
		எனில் (result.மொத்தம்("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		எனில் (result.மொத்தம்("f")) frontend_command = result["f"].as<std::string>();
		எனில் (result.மொத்தம்("H")) {
			passes_commands.தள்ளு_back("help");
			run_shell = பொய்;
		}
		எனில் (result.மொத்தம்("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.தள்ளு_back("help " + res);
			run_shell = பொய்;
		}
		எனில் (result.மொத்தம்("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = பொய்;
		}
		எனில் (result.மொத்தம்("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.செருகு(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = பொய்;
		}
		எனில் (result.மொத்தம்("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = பொய்;
		}
		க்கு (மாறிலி auto& சாவி : {"l", "L"}) {
			எனில் (result.மொத்தம்(சாவி)) {
				க்கு (மாறிலி auto& filename : result[சாவி].as<std::vector<std::string>>()) {
					முயல் {
						logger().add_sink<FileLogSink>(filename, சாவி[0] == 'L', பொய்);
					} பிடி (மாறிலி std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		எனில் (result.மொத்தம்("q")) {
			mode_q = உண்மை;
			எனில் (log_stderr) quiet_warnings = உண்மை;
			log_stderr = உண்மை;
		}
		எனில் (result.மொத்தம்("v")) {
			mode_v = உண்மை;
			log_stderr = உண்மை;
			logger().set_verbose_level(result["v"].as<முழு>());
		}
		எனில் (result.மொத்தம்("t")) logger().set_log_time(உண்மை);
		எனில் (result.மொத்தம்("no-color")) no_color = உண்மை;
		எனில் (result.மொத்தம்("d")) timing_details = உண்மை;
		க்கு (மாறிலி auto& சாவி : {"s", "c"}) {
			எனில் (result.மொத்தம்(சாவி)) {
				scriptfile = result[சாவி].as<std::string>();
				scriptfile_tcl = std::string(சாவி) == "c";
				run_shell = பொய்;
			}
		}
		எனில் (result.மொத்தம்("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = உண்மை;
			run_shell = பொய்;
		}
		க்கு (மாறிலி auto& சாவி : {"W", "w", "e"}) {
			எனில் (result.மொத்தம்(சாவி)) {
				auto regexes = result[சாவி].as<std::vector<std::string>>();
				க்கு (மாறிலி auto& regex : regexes) {
					எனில் (std::string(சாவி) == "W")
						logger().add_warn(regex);
					எனில் (std::string(சாவி) == "w")
						logger().add_nowarn(regex);
					எனில் (std::string(சாவி) == "e")
						logger().add_werror(regex);
				}
			}
		}
		எனில் (result.மொத்தம்("r")) topmodule = result["r"].as<std::string>();
		எனில் (result.மொத்தம்("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		எனில் (result.மொத்தம்("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			க்கு (மாறிலி auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				எனில் (!tokens.empty() && tokens[0] == "ALL") {
					எனில் (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(உண்மை);
				} இல்லையெனில் {
					எனில் (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].எடு_back();
					எனில் (tokens.size() == 1)
						tokens.தள்ளு_back("yosys_dump_" + tokens[0] + ".il");
					எனில் (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		எனில் (result.மொத்தம்("E")) depsfile = result["E"].as<std::string>();
		எனில் (result.மொத்தம்("x")) {
			க்கு (மாறிலி auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		எனில் (result.மொத்தம்("perffile")) perffile = result["perffile"].as<std::string>();
		எனில் (result.மொத்தம்("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		எனில் (result.மொத்தம்("autoidx")) {
			முழு idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		எனில் (!is_colored(stdout) || !is_colored(stderr)) {
			எனில் (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} இல்லையெனில் {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} இல்லையெனில்
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		எனில் (print_banner)
			yosys_banner();

	}
	பிடி (மாறிலி cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#எனில் defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#எனில் defined(_WIN32)
		எனில் (getenv("HOMEDRIVE") != வெற்று && getenv("HOMEPATH") != வெற்று) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} இல்லையெனில் {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#இல்லையெனில்
		எனில் (getenv("XDG_STATE_HOME") == வெற்று || getenv("XDG_STATE_HOME")[0] == '\0') {
			எனில் (getenv("HOME") != வெற்று) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} இல்லையெனில் {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} இல்லையெனில் {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	எனில் (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	எனில் (print_stats)
		logger().start_hasher();

#எனில் defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#எனில் defined(__linux__)
	// set stack size to >= 128 MB
	{
		கட்டமைப்பு rlimit rl;
		மாறிலி rlim_t அடுக்கு_size = 128L * 1024L * 1024L;
		எனில் (getrlimit(RLIMIT_அடுக்கு, &rl) == 0 && rl.rlim_cur < அடுக்கு_size) {
			rl.rlim_cur = அடுக்கு_size;
			setrlimit(RLIMIT_அடுக்கு, &rl);
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

	க்கு (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	எனில் (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		க்கு (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	எனில் (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		க்கு (auto special_arg : special_args)
			frontend_files.தள்ளு_back(special_arg);
	}

	க்கு (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		எனில் (run_frontend((*it).c_str(), frontend_command))
			run_shell = பொய்;
	}

	எனில் (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	எனில் (!scriptfile.empty()) {
		எனில் (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			முழு tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			க்கு (auto arg : special_args)
				script_args.தள்ளு_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), வெற்று, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), வெற்று, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), வெற்று, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			எனில் (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#இல்லையெனில்
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} இல்லையெனில் எனில் (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			முழு py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			க்கு (முழு i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			எனில் (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			எனில் (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#இல்லையெனில்
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} இல்லையெனில்
			run_frontend(scriptfile, "script");
	}

	க்கு (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	எனில் (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#இல்லையெனில்
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} இல்லையெனில் {
		எனில் (run_shell)
			shell(yosys_design);
		இல்லையெனில்
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	க்கு (auto it : saved_designs)
		it.second->check();
	க்கு (auto it : pushed_designs)
		it->check();

	எனில் (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		எனில் (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		தருக்க first = உண்மை;
		க்கு (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = பொய்;
		}
		fprintf(f, ":");
		க்கு (auto fn : yosys_உள்ளீடு_files) {
			எனில் (yosys_output_files.மொத்தம்(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	எனில் (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(பொய்);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		எனில் (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		எனில் (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		எனில் (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			க்கு (auto &[feature, earliest_removal] : logger().get_deprecated())
				எனில் (earliest_removal.empty())
					log("    %s\n", feature);
				இல்லையெனில்
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(வெறுமை)wall_clock_start;
#இல்லையெனில்
		std::string meminfo;
		std::string stats_divider = ", ";

		கட்டமைப்பு rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		எனில் (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			கட்டமைப்பு rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#எனில் defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#எனில் defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#அல்லதெனில் defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		இரட்டை wall_seconds = std::chrono::duration<இரட்டை>(
				std::chrono::steady_clock::now() - wall_clock_start).மொத்தம்();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, முழு, std::string>> timedat;

		க்கு (auto &it : pass_register)
			எனில் (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.செருகு(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		எனில் (twine_gc_மொத்தம்) {
			total_ns += twine_gc_ns;
			timedat.செருகு(make_tuple(twine_gc_ns, twine_gc_மொத்தம், "twine_gc"));
		}

		எனில் (timing_details)
		{
			log("Time spent:\n");
			க்கு (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", முழு(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		இல்லையெனில்
		{
			முழு out_மொத்தம் = 0;
			std::string message = "Time spent:";
			க்கு (auto it = timedat.rbegin(); it != timedat.rend() && out_மொத்தம் < 4; it++, out_மொத்தம்++) {
				எனில் (out_மொத்தம் >= 2 && (std::get<0>(*it) < 1000000000 || முழு(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					நிறுத்து;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_மொத்தம் ? "," : "", முழு(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), முழு(std::get<0>(*it) / 1000000000));
			}
			message += out_மொத்தம் ? "" : " no commands executed";
			log("%s\n", message);
		}
		எனில்(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			எனில் (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			தருக்க first = உண்மை;
			க்கு (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				எனில் (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = பொய்;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	எனில் (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#எனில் defined(_MSC_VER)
	_exit(0);
#அல்லதெனில் defined(_WIN32)
	_Exit(0);
#endif
	திரும்பு 0;
}

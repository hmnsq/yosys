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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#અન્તર્ભાવ "libs/cxxopts/include/cxxopts.hpp"
#અન્તર્ભાવ <iostream>
#અન્તર્ભાવ <chrono>

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
નામાકાશ py = pybind11;
#endif

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#અન્તર્ભાવ <inttypes.h>

#જો defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#જો !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#અન્તર્ભાવ <fmt/color.h>
#અન્તર્ભાવ <fmt/format.h>

USING_YOSYS_NAMESPACE

#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
પૂર્ણાંક yosys_history_offset = 0;
std::string yosys_history_file;
#endif

શૂન્ય yosys_atexit()
{
#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	જો (!yosys_history_file.empty()) {
#જો defined(YOSYS_ENABLE_READLINE)
		જો (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} નહીંતર
			write_history(yosys_history_file.c_str());
#નહીંતર
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#જો defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	જો (hist_list != ખાલી)
		મુક્ત(hist_list);
#endif
#endif
}

#જો defined(__OpenBSD__)
નામાકાશ Yosys {
બાહ્ય અક્ષર *yosys_argv0;
બાહ્ય અક્ષર yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
નામાકાશ Yosys {
	બાહ્ય પૂર્ણાંક yosys_tcl_interp_init(Tcl_Interp *interp);
	બાહ્ય શૂન્ય yosys_tcl_activate_repl();
};
#endif

નામાકાશ Yosys {
	બાહ્ય તાર્કિક log_stderr_sink_forced;
};

વર્ગ ColorConsoleLogSink : જાહેર LogSink
{
જાહેર:
	explicit ColorConsoleLogSink(તાર્કિક err, તાર્કિક quiet) : error_output(err), quiet_warnings(quiet) {}
	શૂન્ય log(અચલ LogMessage &સંદેશ) override;
	તાર્કિક should_log(અચલ LogMessage &સંદેશ) અચલ override;
	શૂન્ય flush() override;
	FILE *file_handle() override { પાછા error_output ? nullptr : stdout; }
ખાનગી:
	તાર્કિક error_output;
	તાર્કિક quiet_warnings;
};

તાર્કિક no_color = ખોટું;

તાર્કિક is_colored(FILE *file)
{
	જો (no_color)
		પાછા ખોટું;

#જો defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	પાછા handle != INVALID_HANDLE_કિંમત &&
			GetConsoleMode(handle, &mode);
#નહીંતર
	પાછા isatty(fileno(file));
#endif
}

શૂન્ય ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

તાર્કિક next_print_log = સાચું;

શૂન્ય ColorConsoleLogSink::log(અચલ LogMessage &સંદેશ)
{
	FILE *f = (error_output || સંદેશ.severity == LogSeverity::Error) ? stderr : stdout;

	જો (logger().get_log_time() && next_print_log) {
		auto elapsed = સંદેશ.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", પૂર્ણાંક(us.ગણતરી() / 1'000'000),પૂર્ણાંક(us.ગણતરી() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = ખોટું;
	}

	જો (!સંદેશ.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", સંદેશ.src.filename, સંદેશ.src.start_લીટી);

	પસંદગી (સંદેશ.severity) {
		સ્થિતિ LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", સંદેશ.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", સંદેશ.message);
			તોડો;

		સ્થિતિ LogSeverity::Error:
		સ્થિતિ LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", સંદેશ.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", સંદેશ.message);
			તોડો;

		સ્થિતિ LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", સંદેશ.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", સંદેશ.message);
			તોડો;

		સ્થિતિ LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", સંદેશ.prefix, સંદેશ.message);
			તોડો;

		સ્થિતિ LogSeverity::Comment:
		સ્થિતિ LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", સંદેશ.prefix, સંદેશ.message);
			તોડો;

		સ્થિતિ LogSeverity::Info:
		મૂળભૂત:
			જો (!સંદેશ.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", સંદેશ.prefix);
			fmt::print(f, "{}", સંદેશ.message);
			તોડો;
	}
	જો (!સંદેશ.message.empty())
		next_print_log = સંદેશ.message.back() == '\n';
}

તાર્કિક ColorConsoleLogSink::should_log(અચલ LogMessage &સંદેશ) અચલ
{
	પાછા (!error_output) || (સંદેશ.severity == LogSeverity::Error || સંદેશ.severity == LogSeverity::NonFatalError ||
			(સંદેશ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
પૂર્ણાંક wmain(પૂર્ણાંક argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<અક્ષર*> uargs;
	માટે (પૂર્ણાંક i = 0; i < argc; i++) {
		પૂર્ણાંક usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, ખાલી, 0, ખાલી, ખાલી);
		અક્ષર *uarg = (અક્ષર *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ખાલી, ખાલી);
		uargs.ધકેલો_back(uarg);
	}
	uargs.ધકેલો_back(ખાલી);
	અક્ષર **argv = uargs.data();
#નહીંતર
પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
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
	તાર્કિક scriptfile_tcl = ખોટું;
	તાર્કિક scriptfile_python = ખોટું;
	તાર્કિક print_banner = સાચું;
	તાર્કિક print_stats = સાચું;
	તાર્કિક call_abort = ખોટું;
	તાર્કિક timing_details = ખોટું;
	તાર્કિક run_shell = સાચું;
	તાર્કિક run_tcl_shell = ખોટું;
	તાર્કિક mode_v = ખોટું;
	તાર્કિક mode_q = ખોટું;
	તાર્કિક quiet_warnings = ખોટું;
	તાર્કિક log_stderr = ખોટું;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::કિંમત<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::કિંમત<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::કિંમત<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::કિંમત<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::કિંમત<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::કિંમત<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::કિંમત<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::કિંમત<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::કિંમત<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::કિંમત<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::કિંમત<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::કિંમત<પૂર્ણાંક>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::કિંમત<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::કિંમત<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::કિંમત<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::કિંમત<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::કિંમત<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::કિંમત<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::કિંમત<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::કિંમત<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::કિંમત<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::કિંમત<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::કિંમત<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	જો (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	પ્રયાસ {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		જો (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		જો (result.ગણતરી("M")) memhasher_on();
		જો (result.ગણતરી("X")) yosys_xtrace += result.ગણતરી("X");
		જો (result.ગણતરી("A")) call_abort = સાચું;
		જો (result.ગણતરી("Q")) print_banner = ખોટું;
		જો (result.ગણતરી("T")) print_stats = ખોટું;
		જો (result.ગણતરી("no-version")) yosys_write_versions = ખોટું;
		જો (result.ગણતરી("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		જો (result.ગણતરી("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		જો (result.ગણતરી("S")) {
			passes_commands.ધકેલો_back("synth");
			run_shell = ખોટું;
		}
		જો (result.ગણતરી("C")) run_tcl_shell = સાચું;
		જો (result.ગણતરી("g")) logger().force_debug_on();
		જો (result.ગણતરી("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		જો (result.ગણતરી("f")) frontend_command = result["f"].as<std::string>();
		જો (result.ગણતરી("H")) {
			passes_commands.ધકેલો_back("help");
			run_shell = ખોટું;
		}
		જો (result.ગણતરી("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.ધકેલો_back("help " + res);
			run_shell = ખોટું;
		}
		જો (result.ગણતરી("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = ખોટું;
		}
		જો (result.ગણતરી("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.ઉમેરો(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = ખોટું;
		}
		જો (result.ગણતરી("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = ખોટું;
		}
		માટે (અચલ auto& ચાવી : {"l", "L"}) {
			જો (result.ગણતરી(ચાવી)) {
				માટે (અચલ auto& filename : result[ચાવી].as<std::vector<std::string>>()) {
					પ્રયાસ {
						logger().add_sink<FileLogSink>(filename, ચાવી[0] == 'L', ખોટું);
					} પકડો (અચલ std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		જો (result.ગણતરી("q")) {
			mode_q = સાચું;
			જો (log_stderr) quiet_warnings = સાચું;
			log_stderr = સાચું;
		}
		જો (result.ગણતરી("v")) {
			mode_v = સાચું;
			log_stderr = સાચું;
			logger().set_verbose_level(result["v"].as<પૂર્ણાંક>());
		}
		જો (result.ગણતરી("t")) logger().set_log_time(સાચું);
		જો (result.ગણતરી("no-color")) no_color = સાચું;
		જો (result.ગણતરી("d")) timing_details = સાચું;
		માટે (અચલ auto& ચાવી : {"s", "c"}) {
			જો (result.ગણતરી(ચાવી)) {
				scriptfile = result[ચાવી].as<std::string>();
				scriptfile_tcl = std::string(ચાવી) == "c";
				run_shell = ખોટું;
			}
		}
		જો (result.ગણતરી("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = સાચું;
			run_shell = ખોટું;
		}
		માટે (અચલ auto& ચાવી : {"W", "w", "e"}) {
			જો (result.ગણતરી(ચાવી)) {
				auto regexes = result[ચાવી].as<std::vector<std::string>>();
				માટે (અચલ auto& regex : regexes) {
					જો (std::string(ચાવી) == "W")
						logger().add_warn(regex);
					જો (std::string(ચાવી) == "w")
						logger().add_nowarn(regex);
					જો (std::string(ચાવી) == "e")
						logger().add_werror(regex);
				}
			}
		}
		જો (result.ગણતરી("r")) topmodule = result["r"].as<std::string>();
		જો (result.ગણતરી("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		જો (result.ગણતરી("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			માટે (અચલ auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				જો (!tokens.empty() && tokens[0] == "ALL") {
					જો (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(સાચું);
				} નહીંતર {
					જો (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].કાઢો_back();
					જો (tokens.size() == 1)
						tokens.ધકેલો_back("yosys_dump_" + tokens[0] + ".il");
					જો (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		જો (result.ગણતરી("E")) depsfile = result["E"].as<std::string>();
		જો (result.ગણતરી("x")) {
			માટે (અચલ auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		જો (result.ગણતરી("perffile")) perffile = result["perffile"].as<std::string>();
		જો (result.ગણતરી("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		જો (result.ગણતરી("autoidx")) {
			પૂર્ણાંક idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		જો (!is_colored(stdout) || !is_colored(stderr)) {
			જો (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} નહીંતર {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} નહીંતર
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		જો (print_banner)
			yosys_banner();

	}
	પકડો (અચલ cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#જો defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#જો defined(_WIN32)
		જો (getenv("HOMEDRIVE") != ખાલી && getenv("HOMEPATH") != ખાલી) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} નહીંતર {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#નહીંતર
		જો (getenv("XDG_STATE_HOME") == ખાલી || getenv("XDG_STATE_HOME")[0] == '\0') {
			જો (getenv("HOME") != ખાલી) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} નહીંતર {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} નહીંતર {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	જો (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	જો (print_stats)
		logger().start_hasher();

#જો defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#જો defined(__linux__)
	// set stack size to >= 128 MB
	{
		સંરચના rlimit rl;
		અચલ rlim_t ઢગલો_size = 128L * 1024L * 1024L;
		જો (getrlimit(RLIMIT_ઢગલો, &rl) == 0 && rl.rlim_cur < ઢગલો_size) {
			rl.rlim_cur = ઢગલો_size;
			setrlimit(RLIMIT_ઢગલો, &rl);
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

	માટે (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	જો (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		માટે (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	જો (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		માટે (auto special_arg : special_args)
			frontend_files.ધકેલો_back(special_arg);
	}

	માટે (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		જો (run_frontend((*it).c_str(), frontend_command))
			run_shell = ખોટું;
	}

	જો (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	જો (!scriptfile.empty()) {
		જો (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			પૂર્ણાંક tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			માટે (auto arg : special_args)
				script_args.ધકેલો_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ખાલી, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ખાલી, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ખાલી, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			જો (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#નહીંતર
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} નહીંતર જો (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			પૂર્ણાંક py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			માટે (પૂર્ણાંક i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			જો (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			જો (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#નહીંતર
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} નહીંતર
			run_frontend(scriptfile, "script");
	}

	માટે (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	જો (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#નહીંતર
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} નહીંતર {
		જો (run_shell)
			shell(yosys_design);
		નહીંતર
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	માટે (auto it : saved_designs)
		it.second->check();
	માટે (auto it : pushed_designs)
		it->check();

	જો (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		જો (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		તાર્કિક first = સાચું;
		માટે (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = ખોટું;
		}
		fprintf(f, ":");
		માટે (auto fn : yosys_આગત_files) {
			જો (yosys_output_files.ગણતરી(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	જો (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(ખોટું);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		જો (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		જો (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		જો (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			માટે (auto &[feature, earliest_removal] : logger().get_deprecated())
				જો (earliest_removal.empty())
					log("    %s\n", feature);
				નહીંતર
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(શૂન્ય)wall_clock_start;
#નહીંતર
		std::string meminfo;
		std::string stats_divider = ", ";

		સંરચના rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		જો (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			સંરચના rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#જો defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#જો defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#અથવાજો defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		દ્વિગુણ wall_seconds = std::chrono::duration<દ્વિગુણ>(
				std::chrono::steady_clock::now() - wall_clock_start).ગણતરી();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, પૂર્ણાંક, std::string>> timedat;

		માટે (auto &it : pass_register)
			જો (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.ઉમેરો(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		જો (twine_gc_ગણતરી) {
			total_ns += twine_gc_ns;
			timedat.ઉમેરો(make_tuple(twine_gc_ns, twine_gc_ગણતરી, "twine_gc"));
		}

		જો (timing_details)
		{
			log("Time spent:\n");
			માટે (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", પૂર્ણાંક(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		નહીંતર
		{
			પૂર્ણાંક out_ગણતરી = 0;
			std::string message = "Time spent:";
			માટે (auto it = timedat.rbegin(); it != timedat.rend() && out_ગણતરી < 4; it++, out_ગણતરી++) {
				જો (out_ગણતરી >= 2 && (std::get<0>(*it) < 1000000000 || પૂર્ણાંક(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					તોડો;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_ગણતરી ? "," : "", પૂર્ણાંક(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), પૂર્ણાંક(std::get<0>(*it) / 1000000000));
			}
			message += out_ગણતરી ? "" : " no commands executed";
			log("%s\n", message);
		}
		જો(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			જો (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			તાર્કિક first = સાચું;
			માટે (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				જો (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = ખોટું;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	જો (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#જો defined(_MSC_VER)
	_exit(0);
#અથવાજો defined(_WIN32)
	_Exit(0);
#endif
	પાછા 0;
}

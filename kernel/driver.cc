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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#ಅನ್ತರ್ಭಾವ "libs/cxxopts/include/cxxopts.hpp"
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <chrono>

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
ನಾಮಾಕಾಶ py = pybind11;
#endif

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#ಅನ್ತರ್ಭಾವ <inttypes.h>

#ಯದಿ defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#ಯದಿ !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#ಅನ್ತರ್ಭಾವ <fmt/color.h>
#ಅನ್ತರ್ಭಾವ <fmt/format.h>

USING_YOSYS_NAMESPACE

#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ಪೂರ್ಣಾಙ್ಕ yosys_history_offset = 0;
std::string yosys_history_file;
#endif

ಶೂನ್ಯ yosys_atexit()
{
#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ಯದಿ (!yosys_history_file.empty()) {
#ಯದಿ defined(YOSYS_ENABLE_READLINE)
		ಯದಿ (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} ಅನ್ಯಥಾ
			write_history(yosys_history_file.c_str());
#ಅನ್ಯಥಾ
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#ಯದಿ defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	ಯದಿ (hist_list != ರಿಕ್ತ)
		ಮುಕ್ತ(hist_list);
#endif
#endif
}

#ಯದಿ defined(__OpenBSD__)
ನಾಮಾಕಾಶ Yosys {
ಬಾಹ್ಯ ವರ್ಣ *yosys_argv0;
ಬಾಹ್ಯ ವರ್ಣ yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
ನಾಮಾಕಾಶ Yosys {
	ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ yosys_tcl_interp_init(Tcl_Interp *interp);
	ಬಾಹ್ಯ ಶೂನ್ಯ yosys_tcl_activate_repl();
};
#endif

ನಾಮಾಕಾಶ Yosys {
	ಬಾಹ್ಯ ತಾರ್ಕಿಕ log_stderr_sink_forced;
};

ವರ್ಗ ColorConsoleLogSink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	explicit ColorConsoleLogSink(ತಾರ್ಕಿಕ err, ತಾರ್ಕಿಕ quiet) : error_output(err), quiet_warnings(quiet) {}
	ಶೂನ್ಯ log(ಧ್ರುವ LogMessage &ಸಂದೇಶ) override;
	ತಾರ್ಕಿಕ should_log(ಧ್ರುವ LogMessage &ಸಂದೇಶ) ಧ್ರುವ override;
	ಶೂನ್ಯ flush() override;
	FILE *file_handle() override { ಪ್ರತ್ಯಾಗಮ error_output ? nullptr : stdout; }
ಗುಪ್ತ:
	ತಾರ್ಕಿಕ error_output;
	ತಾರ್ಕಿಕ quiet_warnings;
};

ತಾರ್ಕಿಕ no_color = ಅಸತ್ಯ;

ತಾರ್ಕಿಕ is_colored(FILE *file)
{
	ಯದಿ (no_color)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

#ಯದಿ defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	ಪ್ರತ್ಯಾಗಮ handle != INVALID_HANDLE_ಮೌಲ್ಯ &&
			GetConsoleMode(handle, &mode);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ isatty(fileno(file));
#endif
}

ಶೂನ್ಯ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ತಾರ್ಕಿಕ next_print_log = ಸತ್ಯ;

ಶೂನ್ಯ ColorConsoleLogSink::log(ಧ್ರುವ LogMessage &ಸಂದೇಶ)
{
	FILE *f = (error_output || ಸಂದೇಶ.severity == LogSeverity::Error) ? stderr : stdout;

	ಯದಿ (logger().get_log_time() && next_print_log) {
		auto elapsed = ಸಂದೇಶ.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", ಪೂರ್ಣಾಙ್ಕ(us.ಎಣಿಕೆ() / 1'000'000),ಪೂರ್ಣಾಙ್ಕ(us.ಎಣಿಕೆ() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = ಅಸತ್ಯ;
	}

	ಯದಿ (!ಸಂದೇಶ.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", ಸಂದೇಶ.src.filename, ಸಂದೇಶ.src.start_ಸಾಲು);

	ವಿಕಲ್ಪ (ಸಂದೇಶ.severity) {
		ಪಕ್ಷ LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", ಸಂದೇಶ.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.message);
			ಭಙ್ಗ;

		ಪಕ್ಷ LogSeverity::Error:
		ಪಕ್ಷ LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", ಸಂದೇಶ.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.message);
			ಭಙ್ಗ;

		ಪಕ್ಷ LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.message);
			ಭಙ್ಗ;

		ಪಕ್ಷ LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", ಸಂದೇಶ.prefix, ಸಂದೇಶ.message);
			ಭಙ್ಗ;

		ಪಕ್ಷ LogSeverity::Comment:
		ಪಕ್ಷ LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", ಸಂದೇಶ.prefix, ಸಂದೇಶ.message);
			ಭಙ್ಗ;

		ಪಕ್ಷ LogSeverity::Info:
		ಸ್ವಭಾವ:
			ಯದಿ (!ಸಂದೇಶ.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", ಸಂದೇಶ.prefix);
			fmt::print(f, "{}", ಸಂದೇಶ.message);
			ಭಙ್ಗ;
	}
	ಯದಿ (!ಸಂದೇಶ.message.empty())
		next_print_log = ಸಂದೇಶ.message.back() == '\n';
}

ತಾರ್ಕಿಕ ColorConsoleLogSink::should_log(ಧ್ರುವ LogMessage &ಸಂದೇಶ) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ (!error_output) || (ಸಂದೇಶ.severity == LogSeverity::Error || ಸಂದೇಶ.severity == LogSeverity::NonFatalError ||
			(ಸಂದೇಶ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
ಪೂರ್ಣಾಙ್ಕ wmain(ಪೂರ್ಣಾಙ್ಕ argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<ವರ್ಣ*> uargs;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < argc; i++) {
		ಪೂರ್ಣಾಙ್ಕ usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ);
		ವರ್ಣ *uarg = (ವರ್ಣ *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ರಿಕ್ತ, ರಿಕ್ತ);
		uargs.ತಳ್ಳು_back(uarg);
	}
	uargs.ತಳ್ಳು_back(ರಿಕ್ತ);
	ವರ್ಣ **argv = uargs.data();
#ಅನ್ಯಥಾ
ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
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
	ತಾರ್ಕಿಕ scriptfile_tcl = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ scriptfile_python = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ print_banner = ಸತ್ಯ;
	ತಾರ್ಕಿಕ print_stats = ಸತ್ಯ;
	ತಾರ್ಕಿಕ call_abort = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ timing_details = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ run_shell = ಸತ್ಯ;
	ತಾರ್ಕಿಕ run_tcl_shell = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ mode_v = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ mode_q = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ quiet_warnings = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ log_stderr = ಅಸತ್ಯ;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::ಮೌಲ್ಯ<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::ಮೌಲ್ಯ<ಪೂರ್ಣಾಙ್ಕ>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::ಮೌಲ್ಯ<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::ಮೌಲ್ಯ<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::ಮೌಲ್ಯ<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	ಯದಿ (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	ಯತ್ನ {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		ಯದಿ (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		ಯದಿ (result.ಎಣಿಕೆ("M")) memhasher_on();
		ಯದಿ (result.ಎಣಿಕೆ("X")) yosys_xtrace += result.ಎಣಿಕೆ("X");
		ಯದಿ (result.ಎಣಿಕೆ("A")) call_abort = ಸತ್ಯ;
		ಯದಿ (result.ಎಣಿಕೆ("Q")) print_banner = ಅಸತ್ಯ;
		ಯದಿ (result.ಎಣಿಕೆ("T")) print_stats = ಅಸತ್ಯ;
		ಯದಿ (result.ಎಣಿಕೆ("no-version")) yosys_write_versions = ಅಸತ್ಯ;
		ಯದಿ (result.ಎಣಿಕೆ("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		ಯದಿ (result.ಎಣಿಕೆ("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		ಯದಿ (result.ಎಣಿಕೆ("S")) {
			passes_commands.ತಳ್ಳು_back("synth");
			run_shell = ಅಸತ್ಯ;
		}
		ಯದಿ (result.ಎಣಿಕೆ("C")) run_tcl_shell = ಸತ್ಯ;
		ಯದಿ (result.ಎಣಿಕೆ("g")) logger().force_debug_on();
		ಯದಿ (result.ಎಣಿಕೆ("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		ಯದಿ (result.ಎಣಿಕೆ("f")) frontend_command = result["f"].as<std::string>();
		ಯದಿ (result.ಎಣಿಕೆ("H")) {
			passes_commands.ತಳ್ಳು_back("help");
			run_shell = ಅಸತ್ಯ;
		}
		ಯದಿ (result.ಎಣಿಕೆ("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.ತಳ್ಳು_back("help " + res);
			run_shell = ಅಸತ್ಯ;
		}
		ಯದಿ (result.ಎಣಿಕೆ("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = ಅಸತ್ಯ;
		}
		ಯದಿ (result.ಎಣಿಕೆ("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.ಸೇರಿಸು(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = ಅಸತ್ಯ;
		}
		ಯದಿ (result.ಎಣಿಕೆ("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = ಅಸತ್ಯ;
		}
		ಪ್ರತಿ (ಧ್ರುವ auto& ಕೀಲಿ : {"l", "L"}) {
			ಯದಿ (result.ಎಣಿಕೆ(ಕೀಲಿ)) {
				ಪ್ರತಿ (ಧ್ರುವ auto& filename : result[ಕೀಲಿ].as<std::vector<std::string>>()) {
					ಯತ್ನ {
						logger().add_sink<FileLogSink>(filename, ಕೀಲಿ[0] == 'L', ಅಸತ್ಯ);
					} ಗೃಹಾಣ (ಧ್ರುವ std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		ಯದಿ (result.ಎಣಿಕೆ("q")) {
			mode_q = ಸತ್ಯ;
			ಯದಿ (log_stderr) quiet_warnings = ಸತ್ಯ;
			log_stderr = ಸತ್ಯ;
		}
		ಯದಿ (result.ಎಣಿಕೆ("v")) {
			mode_v = ಸತ್ಯ;
			log_stderr = ಸತ್ಯ;
			logger().set_verbose_level(result["v"].as<ಪೂರ್ಣಾಙ್ಕ>());
		}
		ಯದಿ (result.ಎಣಿಕೆ("t")) logger().set_log_time(ಸತ್ಯ);
		ಯದಿ (result.ಎಣಿಕೆ("no-color")) no_color = ಸತ್ಯ;
		ಯದಿ (result.ಎಣಿಕೆ("d")) timing_details = ಸತ್ಯ;
		ಪ್ರತಿ (ಧ್ರುವ auto& ಕೀಲಿ : {"s", "c"}) {
			ಯದಿ (result.ಎಣಿಕೆ(ಕೀಲಿ)) {
				scriptfile = result[ಕೀಲಿ].as<std::string>();
				scriptfile_tcl = std::string(ಕೀಲಿ) == "c";
				run_shell = ಅಸತ್ಯ;
			}
		}
		ಯದಿ (result.ಎಣಿಕೆ("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = ಸತ್ಯ;
			run_shell = ಅಸತ್ಯ;
		}
		ಪ್ರತಿ (ಧ್ರುವ auto& ಕೀಲಿ : {"W", "w", "e"}) {
			ಯದಿ (result.ಎಣಿಕೆ(ಕೀಲಿ)) {
				auto regexes = result[ಕೀಲಿ].as<std::vector<std::string>>();
				ಪ್ರತಿ (ಧ್ರುವ auto& regex : regexes) {
					ಯದಿ (std::string(ಕೀಲಿ) == "W")
						logger().add_warn(regex);
					ಯದಿ (std::string(ಕೀಲಿ) == "w")
						logger().add_nowarn(regex);
					ಯದಿ (std::string(ಕೀಲಿ) == "e")
						logger().add_werror(regex);
				}
			}
		}
		ಯದಿ (result.ಎಣಿಕೆ("r")) topmodule = result["r"].as<std::string>();
		ಯದಿ (result.ಎಣಿಕೆ("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		ಯದಿ (result.ಎಣಿಕೆ("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			ಪ್ರತಿ (ಧ್ರುವ auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				ಯದಿ (!tokens.empty() && tokens[0] == "ALL") {
					ಯದಿ (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(ಸತ್ಯ);
				} ಅನ್ಯಥಾ {
					ಯದಿ (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].ತೆಗೆ_back();
					ಯದಿ (tokens.size() == 1)
						tokens.ತಳ್ಳು_back("yosys_dump_" + tokens[0] + ".il");
					ಯದಿ (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		ಯದಿ (result.ಎಣಿಕೆ("E")) depsfile = result["E"].as<std::string>();
		ಯದಿ (result.ಎಣಿಕೆ("x")) {
			ಪ್ರತಿ (ಧ್ರುವ auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		ಯದಿ (result.ಎಣಿಕೆ("perffile")) perffile = result["perffile"].as<std::string>();
		ಯದಿ (result.ಎಣಿಕೆ("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		ಯದಿ (result.ಎಣಿಕೆ("autoidx")) {
			ಪೂರ್ಣಾಙ್ಕ idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		ಯದಿ (!is_colored(stdout) || !is_colored(stderr)) {
			ಯದಿ (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} ಅನ್ಯಥಾ {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} ಅನ್ಯಥಾ
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		ಯದಿ (print_banner)
			yosys_banner();

	}
	ಗೃಹಾಣ (ಧ್ರುವ cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#ಯದಿ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#ಯದಿ defined(_WIN32)
		ಯದಿ (getenv("HOMEDRIVE") != ರಿಕ್ತ && getenv("HOMEPATH") != ರಿಕ್ತ) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} ಅನ್ಯಥಾ {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#ಅನ್ಯಥಾ
		ಯದಿ (getenv("XDG_STATE_HOME") == ರಿಕ್ತ || getenv("XDG_STATE_HOME")[0] == '\0') {
			ಯದಿ (getenv("HOME") != ರಿಕ್ತ) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} ಅನ್ಯಥಾ {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} ಅನ್ಯಥಾ {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	ಯದಿ (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	ಯದಿ (print_stats)
		logger().start_hasher();

#ಯದಿ defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#ಯದಿ defined(__linux__)
	// set stack size to >= 128 MB
	{
		ಸಂರಚನಾ rlimit rl;
		ಧ್ರುವ rlim_t ರಾಶಿ_size = 128L * 1024L * 1024L;
		ಯದಿ (getrlimit(RLIMIT_ರಾಶಿ, &rl) == 0 && rl.rlim_cur < ರಾಶಿ_size) {
			rl.rlim_cur = ರಾಶಿ_size;
			setrlimit(RLIMIT_ರಾಶಿ, &rl);
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

	ಪ್ರತಿ (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	ಯದಿ (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		ಪ್ರತಿ (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	ಯದಿ (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		ಪ್ರತಿ (auto special_arg : special_args)
			frontend_files.ತಳ್ಳು_back(special_arg);
	}

	ಪ್ರತಿ (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		ಯದಿ (run_frontend((*it).c_str(), frontend_command))
			run_shell = ಅಸತ್ಯ;
	}

	ಯದಿ (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	ಯದಿ (!scriptfile.empty()) {
		ಯದಿ (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			ಪೂರ್ಣಾಙ್ಕ tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			ಪ್ರತಿ (auto arg : special_args)
				script_args.ತಳ್ಳು_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ರಿಕ್ತ, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ರಿಕ್ತ, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ರಿಕ್ತ, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			ಯದಿ (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#ಅನ್ಯಥಾ
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} ಅನ್ಯಥಾ ಯದಿ (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			ಪೂರ್ಣಾಙ್ಕ py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			ಯದಿ (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			ಯದಿ (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#ಅನ್ಯಥಾ
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} ಅನ್ಯಥಾ
			run_frontend(scriptfile, "script");
	}

	ಪ್ರತಿ (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	ಯದಿ (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#ಅನ್ಯಥಾ
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} ಅನ್ಯಥಾ {
		ಯದಿ (run_shell)
			shell(yosys_design);
		ಅನ್ಯಥಾ
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	ಪ್ರತಿ (auto it : saved_designs)
		it.second->check();
	ಪ್ರತಿ (auto it : pushed_designs)
		it->check();

	ಯದಿ (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		ಯದಿ (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		ತಾರ್ಕಿಕ first = ಸತ್ಯ;
		ಪ್ರತಿ (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = ಅಸತ್ಯ;
		}
		fprintf(f, ":");
		ಪ್ರತಿ (auto fn : yosys_ಒಳಹರಿವು_files) {
			ಯದಿ (yosys_output_files.ಎಣಿಕೆ(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	ಯದಿ (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(ಅಸತ್ಯ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		ಯದಿ (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		ಯದಿ (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		ಯದಿ (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			ಪ್ರತಿ (auto &[feature, earliest_removal] : logger().get_deprecated())
				ಯದಿ (earliest_removal.empty())
					log("    %s\n", feature);
				ಅನ್ಯಥಾ
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(ಶೂನ್ಯ)wall_clock_start;
#ಅನ್ಯಥಾ
		std::string meminfo;
		std::string stats_divider = ", ";

		ಸಂರಚನಾ rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		ಯದಿ (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ಸಂರಚನಾ rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#ಯದಿ defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#ಯದಿ defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#ನೋಚೇದ್ಯದಿ defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		ದ್ವಿಗುಣ wall_seconds = std::chrono::duration<ದ್ವಿಗುಣ>(
				std::chrono::steady_clock::now() - wall_clock_start).ಎಣಿಕೆ();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, ಪೂರ್ಣಾಙ್ಕ, std::string>> timedat;

		ಪ್ರತಿ (auto &it : pass_register)
			ಯದಿ (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.ಸೇರಿಸು(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		ಯದಿ (twine_gc_ಎಣಿಕೆ) {
			total_ns += twine_gc_ns;
			timedat.ಸೇರಿಸು(make_tuple(twine_gc_ns, twine_gc_ಎಣಿಕೆ, "twine_gc"));
		}

		ಯದಿ (timing_details)
		{
			log("Time spent:\n");
			ಪ್ರತಿ (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", ಪೂರ್ಣಾಙ್ಕ(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		ಅನ್ಯಥಾ
		{
			ಪೂರ್ಣಾಙ್ಕ out_ಎಣಿಕೆ = 0;
			std::string message = "Time spent:";
			ಪ್ರತಿ (auto it = timedat.rbegin(); it != timedat.rend() && out_ಎಣಿಕೆ < 4; it++, out_ಎಣಿಕೆ++) {
				ಯದಿ (out_ಎಣಿಕೆ >= 2 && (std::get<0>(*it) < 1000000000 || ಪೂರ್ಣಾಙ್ಕ(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					ಭಙ್ಗ;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_ಎಣಿಕೆ ? "," : "", ಪೂರ್ಣಾಙ್ಕ(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), ಪೂರ್ಣಾಙ್ಕ(std::get<0>(*it) / 1000000000));
			}
			message += out_ಎಣಿಕೆ ? "" : " no commands executed";
			log("%s\n", message);
		}
		ಯದಿ(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			ಯದಿ (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			ತಾರ್ಕಿಕ first = ಸತ್ಯ;
			ಪ್ರತಿ (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				ಯದಿ (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = ಅಸತ್ಯ;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	ಯದಿ (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#ಯದಿ defined(_MSC_VER)
	_exit(0);
#ನೋಚೇದ್ಯದಿ defined(_WIN32)
	_Exit(0);
#endif
	ಪ್ರತ್ಯಾಗಮ 0;
}

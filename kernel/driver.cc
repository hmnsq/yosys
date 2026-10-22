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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/hashlib.h"
#define CXXOPTS_VECTOR_DELIMITER '\0'
#ଅନ୍ତର୍ଭାଵ "libs/cxxopts/include/cxxopts.hpp"
#ଅନ୍ତର୍ଭାଵ <iostream>
#ଅନ୍ତର୍ଭାଵ <chrono>

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
ନାମାକାଶ py = pybind11;
#endif

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif
#ଅନ୍ତର୍ଭାଵ <inttypes.h>

#ଯଦି defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#endif

#ଯଦି !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#endif

#ଅନ୍ତର୍ଭାଵ <fmt/color.h>
#ଅନ୍ତର୍ଭାଵ <fmt/format.h>

USING_YOSYS_NAMESPACE

#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା yosys_history_offset = 0;
std::string yosys_history_file;
#endif

ଶୂନ୍ୟ yosys_atexit()
{
#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ଯଦି (!yosys_history_file.empty()) {
#ଯଦି defined(YOSYS_ENABLE_READLINE)
		ଯଦି (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} ଅନ୍ୟଥା
			write_history(yosys_history_file.c_str());
#ଅନ୍ୟଥା
		write_history(yosys_history_file.c_str());
#endif
	}

	clear_history();
#ଯଦି defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	ଯଦି (hist_list != ଖାଲି)
		ମୁକ୍ତ(hist_list);
#endif
#endif
}

#ଯଦି defined(__OpenBSD__)
ନାମାକାଶ Yosys {
ବାହ୍ୟ ଅକ୍ଷର *yosys_argv0;
ବାହ୍ୟ ଅକ୍ଷର yosys_path[PATH_MAX];
};
#endif
#ifdef YOSYS_ENABLE_TCL
ନାମାକାଶ Yosys {
	ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା yosys_tcl_interp_init(Tcl_Interp *interp);
	ବାହ୍ୟ ଶୂନ୍ୟ yosys_tcl_activate_repl();
};
#endif

ନାମାକାଶ Yosys {
	ବାହ୍ୟ ତାର୍କିକ log_stderr_sink_forced;
};

ଶ୍ରେଣୀ ColorConsoleLogSink : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	explicit ColorConsoleLogSink(ତାର୍କିକ err, ତାର୍କିକ quiet) : error_output(err), quiet_warnings(quiet) {}
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override;
	ତାର୍କିକ should_log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) ଅଚଳ override;
	ଶୂନ୍ୟ flush() override;
	FILE *file_handle() override { ଫେରନ୍ତୁ error_output ? nullptr : stdout; }
ବ୍ୟକ୍ତିଗତ:
	ତାର୍କିକ error_output;
	ତାର୍କିକ quiet_warnings;
};

ତାର୍କିକ no_color = ମିଥ୍ୟା;

ତାର୍କିକ is_colored(FILE *file)
{
	ଯଦି (no_color)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

#ଯଦି defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	ଫେରନ୍ତୁ handle != INVALID_HANDLE_ମୂଲ୍ୟ &&
			GetConsoleMode(handle, &mode);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ isatty(fileno(file));
#endif
}

ଶୂନ୍ୟ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ତାର୍କିକ next_print_log = ସତ୍ୟ;

ଶୂନ୍ୟ ColorConsoleLogSink::log(ଅଚଳ LogMessage &ସନ୍ଦେଶ)
{
	FILE *f = (error_output || ସନ୍ଦେଶ.severity == LogSeverity::Error) ? stderr : stdout;

	ଯଦି (logger().get_log_time() && next_print_log) {
		auto elapsed = ସନ୍ଦେଶ.timestamp - logger().get_initial_time();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", ପୂର୍ଣ୍ଣସଂଖ୍ୟା(us.ଗଣନା_2() / 1'000'000),ପୂର୍ଣ୍ଣସଂଖ୍ୟା(us.ଗଣନା_2() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		next_print_log = ମିଥ୍ୟା;
	}

	ଯଦି (!ସନ୍ଦେଶ.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", ସନ୍ଦେଶ.src.filename, ସନ୍ଦେଶ.src.start_ଧାଡ଼ି);

	ବାଛ (ସନ୍ଦେଶ.severity) {
		ସ୍ଥିତି LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", ସନ୍ଦେଶ.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ସନ୍ଦେଶ.message);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LogSeverity::Error:
		ସ୍ଥିତି LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", ସନ୍ଦେଶ.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ସନ୍ଦେଶ.message);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", ସନ୍ଦେଶ.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", ସନ୍ଦେଶ.message);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", ସନ୍ଦେଶ.prefix, ସନ୍ଦେଶ.message);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LogSeverity::Comment:
		ସ୍ଥିତି LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", ସନ୍ଦେଶ.prefix, ସନ୍ଦେଶ.message);
			ଭାଙ୍ଗ;

		ସ୍ଥିତି LogSeverity::Info:
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଯଦି (!ସନ୍ଦେଶ.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", ସନ୍ଦେଶ.prefix);
			fmt::print(f, "{}", ସନ୍ଦେଶ.message);
			ଭାଙ୍ଗ;
	}
	ଯଦି (!ସନ୍ଦେଶ.message.empty())
		next_print_log = ସନ୍ଦେଶ.message.back() == '\n';
}

ତାର୍କିକ ColorConsoleLogSink::should_log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) ଅଚଳ
{
	ଫେରନ୍ତୁ (!error_output) || (ସନ୍ଦେଶ.severity == LogSeverity::Error || ସନ୍ଦେଶ.severity == LogSeverity::NonFatalError ||
			(ସନ୍ଦେଶ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ifdef _WIN32
ପୂର୍ଣ୍ଣସଂଖ୍ୟା wmain(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ifdef _UCRT
	setlocale(LC_ALL, ".UTF-8");
#endif
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<ଅକ୍ଷର*> uargs;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argc; i++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, ଖାଲି, 0, ଖାଲି, ଖାଲି);
		ଅକ୍ଷର *uarg = (ଅକ୍ଷର *)malloc(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ଖାଲି, ଖାଲି);
		uargs.ଠେଲ_back(uarg);
	}
	uargs.ଠେଲ_back(ଖାଲି);
	ଅକ୍ଷର **argv = uargs.data();
#ଅନ୍ୟଥା
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
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
	ତାର୍କିକ scriptfile_tcl = ମିଥ୍ୟା;
	ତାର୍କିକ scriptfile_python = ମିଥ୍ୟା;
	ତାର୍କିକ print_banner = ସତ୍ୟ;
	ତାର୍କିକ print_stats = ସତ୍ୟ;
	ତାର୍କିକ call_abort = ମିଥ୍ୟା;
	ତାର୍କିକ timing_details = ମିଥ୍ୟା;
	ତାର୍କିକ run_shell = ସତ୍ୟ;
	ତାର୍କିକ run_tcl_shell = ମିଥ୍ୟା;
	ତାର୍କିକ mode_v = ମିଥ୍ୟା;
	ତାର୍କିକ mode_q = ମିଥ୍ୟା;
	ତାର୍କିକ quiet_warnings = ମିଥ୍ୟା;
	ତାର୍କିକ log_stderr = ମିଥ୍ୟା;
	no_color = std::getenv("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<scriptfile>")
#ifdef YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::ମୂଲ୍ୟ<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#endif // YOSYS_ENABLE_TCL
#ifdef YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<script>")
#endif // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::ମୂଲ୍ୟ<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::ମୂଲ୍ୟ<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::ମୂଲ୍ୟ<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::ମୂଲ୍ୟ<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::ମୂଲ୍ୟ<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	ଯଦି (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	ଚେଷ୍ଟା {
		// Check for "--" in arguments
		auto it = std::find(argv, argv + argc, std::string("--"));
		ଯଦି (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		auto result = options.parse(argc, argv);

		ଯଦି (result.ଗଣନା_2("M")) memhasher_on();
		ଯଦି (result.ଗଣନା_2("X")) yosys_xtrace += result.ଗଣନା_2("X");
		ଯଦି (result.ଗଣନା_2("A")) call_abort = ସତ୍ୟ;
		ଯଦି (result.ଗଣନା_2("Q")) print_banner = ମିଥ୍ୟା;
		ଯଦି (result.ଗଣନା_2("T")) print_stats = ମିଥ୍ୟା;
		ଯଦି (result.ଗଣନା_2("no-version")) yosys_write_versions = ମିଥ୍ୟା;
		ଯଦି (result.ଗଣନା_2("V")) {
			std::cout << yosys_version_str << std::endl;
			exit(0);
		}
		ଯଦି (result.ଗଣନା_2("git-hash")) {
			std::cout << yosys_git_hash_str << std::endl;
			exit(0);
		}
		ଯଦି (result.ଗଣନା_2("S")) {
			passes_commands.ଠେଲ_back("synth");
			run_shell = ମିଥ୍ୟା;
		}
		ଯଦି (result.ଗଣନା_2("C")) run_tcl_shell = ସତ୍ୟ;
		ଯଦି (result.ଗଣନା_2("g")) logger().force_debug_on();
		ଯଦି (result.ଗଣନା_2("m")) plugin_filenames = result["m"].as<std::vector<std::string>>();
		ଯଦି (result.ଗଣନା_2("f")) frontend_command = result["f"].as<std::string>();
		ଯଦି (result.ଗଣନା_2("H")) {
			passes_commands.ଠେଲ_back("help");
			run_shell = ମିଥ୍ୟା;
		}
		ଯଦି (result.ଗଣନା_2("h")) {
			std::string res = result["h"].as<std::string>();
			passes_commands.ଠେଲ_back("help " + res);
			run_shell = ମିଥ୍ୟା;
		}
		ଯଦି (result.ଗଣନା_2("b")) {
			backend_command = result["b"].as<std::string>();
			run_shell = ମିଥ୍ୟା;
		}
		ଯଦି (result.ଗଣନା_2("p")) {
			auto cmds = result["p"].as<std::vector<std::string>>();
			passes_commands.ଭର୍ତ୍ତି(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = ମିଥ୍ୟା;
		}
		ଯଦି (result.ଗଣନା_2("o")) {
			output_filename = result["o"].as<std::string>();
			run_shell = ମିଥ୍ୟା;
		}
		ପାଇଁ (ଅଚଳ auto& ଚାବି : {"l", "L"}) {
			ଯଦି (result.ଗଣନା_2(ଚାବି)) {
				ପାଇଁ (ଅଚଳ auto& filename : result[ଚାବି].as<std::vector<std::string>>()) {
					ଚେଷ୍ଟା {
						logger().add_sink<FileLogSink>(filename, ଚାବି[0] == 'L', ମିଥ୍ୟା);
					} ଧର (ଅଚଳ std::runtime_error &e) {
 					   std::cerr << e.what();
					   exit(1);
					}
				}
			}
		}
		ଯଦି (result.ଗଣନା_2("q")) {
			mode_q = ସତ୍ୟ;
			ଯଦି (log_stderr) quiet_warnings = ସତ୍ୟ;
			log_stderr = ସତ୍ୟ;
		}
		ଯଦି (result.ଗଣନା_2("v")) {
			mode_v = ସତ୍ୟ;
			log_stderr = ସତ୍ୟ;
			logger().set_verbose_level(result["v"].as<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>());
		}
		ଯଦି (result.ଗଣନା_2("t")) logger().set_log_time(ସତ୍ୟ);
		ଯଦି (result.ଗଣନା_2("no-color")) no_color = ସତ୍ୟ;
		ଯଦି (result.ଗଣନା_2("d")) timing_details = ସତ୍ୟ;
		ପାଇଁ (ଅଚଳ auto& ଚାବି : {"s", "c"}) {
			ଯଦି (result.ଗଣନା_2(ଚାବି)) {
				scriptfile = result[ଚାବି].as<std::string>();
				scriptfile_tcl = std::string(ଚାବି) == "c";
				run_shell = ମିଥ୍ୟା;
			}
		}
		ଯଦି (result.ଗଣନା_2("y")) {
			scriptfile = result["y"].as<std::string>();
			scriptfile_python = ସତ୍ୟ;
			run_shell = ମିଥ୍ୟା;
		}
		ପାଇଁ (ଅଚଳ auto& ଚାବି : {"W", "w", "e"}) {
			ଯଦି (result.ଗଣନା_2(ଚାବି)) {
				auto regexes = result[ଚାବି].as<std::vector<std::string>>();
				ପାଇଁ (ଅଚଳ auto& regex : regexes) {
					ଯଦି (std::string(ଚାବି) == "W")
						logger().add_warn(regex);
					ଯଦି (std::string(ଚାବି) == "w")
						logger().add_nowarn(regex);
					ଯଦି (std::string(ଚାବି) == "e")
						logger().add_werror(regex);
				}
			}
		}
		ଯଦି (result.ଗଣନା_2("r")) topmodule = result["r"].as<std::string>();
		ଯଦି (result.ଗଣନା_2("D")) vlog_defines = result["D"].as<std::vector<std::string>>();
		ଯଦି (result.ଗଣନା_2("P")) {
			auto dump_args = result["P"].as<std::vector<std::string>>();
			ପାଇଁ (ଅଚଳ auto& arg : dump_args) {
				auto tokens = split_tokens(arg, ":");
				ଯଦି (!tokens.empty() && tokens[0] == "ALL") {
					ଯଦି (tokens.size() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::endl;
						exit(1);
					}
					logger().set_hdump_all(ସତ୍ୟ);
				} ଅନ୍ୟଥା {
					ଯଦି (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].କାଢ଼_back();
					ଯଦି (tokens.size() == 1)
						tokens.ଠେଲ_back("yosys_dump_" + tokens[0] + ".il");
					ଯଦି (tokens.size() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::endl;
						exit(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		ଯଦି (result.ଗଣନା_2("E")) depsfile = result["E"].as<std::string>();
		ଯଦି (result.ଗଣନା_2("x")) {
			ପାଇଁ (ଅଚଳ auto &ignore : result["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		ଯଦି (result.ଗଣନା_2("perffile")) perffile = result["perffile"].as<std::string>();
		ଯଦି (result.ଗଣନା_2("infile")) {
			frontend_files = result["infile"].as<std::vector<std::string>>();
		}
		ଯଦି (result.ଗଣନା_2("autoidx")) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = result["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		ଯଦି (!is_colored(stdout) || !is_colored(stderr)) {
			ଯଦି (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} ଅନ୍ୟଥା {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} ଅନ୍ୟଥା
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		ଯଦି (print_banner)
			yosys_banner();

	}
	ଧର (ଅଚଳ cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::endl;
		exit(1);
	}

#ଯଦି defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#ଯଦି defined(_WIN32)
		ଯଦି (getenv("HOMEDRIVE") != ଖାଲି && getenv("HOMEPATH") != ଖାଲି) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} ଅନ୍ୟଥା {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#ଅନ୍ୟଥା
		ଯଦି (getenv("XDG_STATE_HOME") == ଖାଲି || getenv("XDG_STATE_HOME")[0] == '\0') {
			ଯଦି (getenv("HOME") != ଖାଲି) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} ଅନ୍ୟଥା {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} ଅନ୍ୟଥା {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#endif

	ଯଦି (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#endif

	ଯଦି (print_stats)
		logger().start_hasher();

#ଯଦି defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#endif

#ଯଦି defined(__linux__)
	// set stack size to >= 128 MB
	{
		ସଂରଚନା rlimit rl;
		ଅଚଳ rlim_t ଗଦା_size = 128L * 1024L * 1024L;
		ଯଦି (getrlimit(RLIMIT_ଗଦା, &rl) == 0 && rl.rlim_cur < ଗଦା_size) {
			rl.rlim_cur = ଗଦା_size;
			setrlimit(RLIMIT_ଗଦା, &rl);
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

	ପାଇଁ (auto &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	ଯଦି (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		ପାଇଁ (auto vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	ଯଦି (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		ପାଇଁ (auto special_arg : special_args)
			frontend_files.ଠେଲ_back(special_arg);
	}

	ପାଇଁ (auto it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		ଯଦି (run_frontend((*it).c_str(), frontend_command))
			run_shell = ମିଥ୍ୟା;
	}

	ଯଦି (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	ଯଦି (!scriptfile.empty()) {
		ଯଦି (scriptfile_tcl) {
#ifdef YOSYS_ENABLE_TCL
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା tcl_argc = special_args.size();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			ପାଇଁ (auto arg : special_args)
				script_args.ଠେଲ_back(Tcl_NewStringObj(arg.c_str(), arg.length()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), ଖାଲି, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), ଖାଲି, Tcl_NewListObj(tcl_argc, script_args.data()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), ଖାଲି, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.length()), 0);

			ଯଦି (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_GetStringResult(yosys_get_tcl_interp()));
#ଅନ୍ୟଥା
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#endif
		} ଅନ୍ୟଥା ଯଦି (scriptfile_python) {
#ifdef YOSYS_ENABLE_PYTHON
			py::list new_argv;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା py_argc = special_args.size() + 1;
			new_argv.append(scriptfile);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			ଯଦି (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			ଯଦି (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#ଅନ୍ୟଥା
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#endif
		} ଅନ୍ୟଥା
			run_frontend(scriptfile, "script");
	}

	ପାଇଁ (auto it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	ଯଦି (run_tcl_shell) {
#ifdef YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#ଅନ୍ୟଥା
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#endif
	} ଅନ୍ୟଥା {
		ଯଦି (run_shell)
			shell(yosys_design);
		ଅନ୍ୟଥା
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	ପାଇଁ (auto it : saved_designs)
		it.second->check();
	ପାଇଁ (auto it : pushed_designs)
		it->check();

	ଯଦି (!depsfile.empty())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		ଯଦି (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		ତାର୍କିକ first = ସତ୍ୟ;
		ପାଇଁ (auto fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = ମିଥ୍ୟା;
		}
		fprintf(f, ":");
		ପାଇଁ (auto fn : yosys_ନିବେଶ_files) {
			ଯଦି (yosys_output_files.ଗଣନା_2(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	ଯଦି (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(ମିଥ୍ୟା);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		ଯଦି (logger().get_warnings_total())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_total());

		ଯଦି (GetSize(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", GetSize(logger().get_experimental()));

		ଯଦି (GetSize(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			ପାଇଁ (auto &[feature, earliest_removal] : logger().get_deprecated())
				ଯଦି (earliest_removal.empty())
					log("    %s\n", feature);
				ଅନ୍ୟଥା
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ifdef _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(ଶୂନ୍ୟ)wall_clock_start;
#ଅନ୍ୟଥା
		std::string meminfo;
		std::string stats_divider = ", ";

		ସଂରଚନା rusage ru_buffer;
		getrusage(RUSAGE_SELF, &ru_buffer);
		ଯଦି (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ସଂରଚନା rusage ru_buffer_children;
			getrusage(RUSAGE_CHILDREN, &ru_buffer_children);
			ru_buffer.ru_utime.tv_sec += ru_buffer_children.ru_utime.tv_sec;
			ru_buffer.ru_utime.tv_usec += ru_buffer_children.ru_utime.tv_usec;
			ru_buffer.ru_stime.tv_sec += ru_buffer_children.ru_stime.tv_sec;
			ru_buffer.ru_stime.tv_usec += ru_buffer_children.ru_stime.tv_usec;
#ଯଦି defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_buffer.ru_maxrss = std::max(ru_buffer.ru_maxrss, ru_buffer_children.ru_maxrss);
#endif
		}
#ଯଦି defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / 1024.0);
#ନଚେତଯଦି defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_buffer.ru_maxrss / (1024.0 * 1024.0));
#endif
		ଦ୍ୱିଗୁଣ wall_seconds = std::chrono::duration<ଦ୍ୱିଗୁଣ>(
				std::chrono::steady_clock::now() - wall_clock_start).ଗଣନା_2();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_buffer.ru_utime.tv_sec + 1e-6 * ru_buffer.ru_utime.tv_usec,
				ru_buffer.ru_stime.tv_sec + 1e-6 * ru_buffer.ru_stime.tv_usec, meminfo.c_str());
#endif
		log("%s\n", yosys_maybe_version());

		int64_t total_ns = 0;
		std::set<tuple<int64_t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, std::string>> timedat;

		ପାଇଁ (auto &it : pass_register)
			ଯଦି (it.second->call_counter) {
				auto pass_ns = it.second->runtime_ns + 1;
				total_ns += pass_ns;
				timedat.ଭର୍ତ୍ତି(make_tuple(pass_ns, it.second->call_counter, it.first));
			}
		ଯଦି (twine_gc_ଗଣନା) {
			total_ns += twine_gc_ns;
			timedat.ଭର୍ତ୍ତି(make_tuple(twine_gc_ns, twine_gc_ଗଣନା, "twine_gc"));
		}

		ଯଦି (timing_details)
		{
			log("Time spent:\n");
			ପାଇଁ (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", ପୂର୍ଣ୍ଣସଂଖ୍ୟା(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		ଅନ୍ୟଥା
		{
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_ଗଣନା = 0;
			std::string message = "Time spent:";
			ପାଇଁ (auto it = timedat.rbegin(); it != timedat.rend() && out_ଗଣନା < 4; it++, out_ଗଣନା++) {
				ଯଦି (out_ଗଣନା >= 2 && (std::get<0>(*it) < 1000000000 || ପୂର୍ଣ୍ଣସଂଖ୍ୟା(100*std::get<0>(*it) / total_ns) < 20)) {
					message += ", ...";
					ଭାଙ୍ଗ;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_ଗଣନା ? "," : "", ପୂର୍ଣ୍ଣସଂଖ୍ୟା(100*std::get<0>(*it) / total_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), ପୂର୍ଣ୍ଣସଂଖ୍ୟା(std::get<0>(*it) / 1000000000));
			}
			message += out_ଗଣନା ? "" : " no commands executed";
			log("%s\n", message);
		}
		ଯଦି(!perffile.empty())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			ଯଦି (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", total_ns);
			fprintf(f, "  \"passes\": {");

			ତାର୍କିକ first = ସତ୍ୟ;
			ପାଇଁ (auto it = timedat.rbegin(); it != timedat.rend(); it++) {
				ଯଦି (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = ମିଥ୍ୟା;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	ଯଦି (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#ଯଦି defined(_MSC_VER)
	_exit(0);
#ନଚେତଯଦି defined(_WIN32)
	_Exit(0);
#endif
	ଫେରନ୍ତୁ 0;
}

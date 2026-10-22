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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/hashlib.h"
#నిర్వచించు CXXOPTS_సదిశ_DELIMITER '\0'
#అన్తర్భావ "libs/cxxopts/include/cxxopts.hpp"
#అన్తర్భావ <iostream>
#అన్తర్భావ <chrono>

#నిర్వచితమైతే YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_TCL
#  include <tcl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
నామాకాశ py = pybind11;
#అయితే_ముగింపు

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <limits.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <fcntl.h>
#నిర్వచితం_కాకపోతే __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#అయితే_ముగింపు
#అన్తర్భావ <inttypes.h>

#ఒకవేళ defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#అయితే_ముగింపు

#ఒకవేళ !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#అయితే_ముగింపు

#అన్తర్భావ <fmt/color.h>
#అన్తర్భావ <fmt/format.h>

USING_YOSYS_NAMESPACE

#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
పూర్ణాంకం yosys_history_స్థానభ్రంశం = 0;
std::string yosys_history_file;
#అయితే_ముగింపు

శూన్య yosys_atexit()
{
#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ఒకవేళ (!yosys_history_file.ఖాళీలు()) {
#ఒకవేళ defined(YOSYS_ENABLE_READLINE)
		ఒకవేళ (yosys_history_స్థానభ్రంశం > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_స్థానభ్రంశం, yosys_history_file.c_str());
		} లేకపోతే
			write_history(yosys_history_file.c_str());
#లేకపోతే
		write_history(yosys_history_file.c_str());
#అయితే_ముగింపు
	}

	clear_history();
#ఒకవేళ defined(YOSYS_ENABLE_READLINE)
	HIST_నమోదు **hist_list = history_list();
	ఒకవేళ (hist_list != ఖాళీ)
		విడుదల(hist_list);
#అయితే_ముగింపు
#అయితే_ముగింపు
}

#ఒకవేళ defined(__OpenBSD__)
నామాకాశ Yosys {
బాహ్య అక్షరం *yosys_argv0;
బాహ్య అక్షరం yosys_మార్గం[మార్గం_MAX];
};
#అయితే_ముగింపు
#నిర్వచితమైతే YOSYS_ENABLE_TCL
నామాకాశ Yosys {
	బాహ్య పూర్ణాంకం yosys_tcl_interp_init(Tcl_Interp *interp);
	బాహ్య శూన్య yosys_tcl_activate_repl();
};
#అయితే_ముగింపు

నామాకాశ Yosys {
	బాహ్య తార్కిక log_stderr_sink_forced;
};

తరగతి ColorConsoleLogSink : బహిరంగ LogSink
{
బహిరంగ:
	స్పష్టమైన ColorConsoleLogSink(తార్కిక err, తార్కిక quiet) : error_output(err), quiet_warnings(quiet) {}
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override;
	తార్కిక should_log(స్థిరరాశి Log_సందేశం &సందేశం) స్థిరరాశి override;
	శూన్య flush() override;
	FILE *file_handle() override { తిరిగి error_output ? శూన్యసూచిక : stdout; }
అంతరంగ:
	తార్కిక error_output;
	తార్కిక quiet_warnings;
};

తార్కిక no_color = అబద్ధం;

తార్కిక is_colored(FILE *file)
{
	ఒకవేళ (no_color)
		తిరిగి అబద్ధం;

#ఒకవేళ defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD బహుళకం;
	తిరిగి handle != INVALID_HANDLE_విలువ &&
			Get_Console_బహుళకం(handle, &బహుళకం);
#లేకపోతే
	తిరిగి isatty(fileno(file));
#అయితే_ముగింపు
}

శూన్య ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

తార్కిక తదుపరి_print_log = నిజం;

శూన్య ColorConsoleLogSink::సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం)
{
	FILE *f = (error_output || సందేశం.severity == LogSeverity::Error) ? stderr : stdout;

	ఒకవేళ (logger().get_log_సమయం() && తదుపరి_print_log) {
		స్వయంచాలక elapsed = సందేశం.timestamp - logger().get_initial_సమయం();
		స్వయంచాలక us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string సమయం_str = stringf("[%05d.%06d] ", పూర్ణాంకం(us.లెక్క() / 1'000'000),పూర్ణాంకం(us.లెక్క() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", సమయం_str);
		తదుపరి_print_log = అబద్ధం;
	}

	ఒకవేళ (!సందేశం.src.filename.ఖాళీలు())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", సందేశం.src.filename, సందేశం.src.ప్రారంభం_పంక్తి);

	ఎంపిక (సందేశం.severity) {
		స్థితి LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", సందేశం.ఉపసర్గ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", సందేశం.సందేశం_2);
			ఆపు;

		స్థితి LogSeverity::Error:
		స్థితి LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", సందేశం.ఉపసర్గ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", సందేశం.సందేశం_2);
			ఆపు;

		స్థితి LogSeverity::శీర్షిక:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", సందేశం.ఉపసర్గ_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", సందేశం.సందేశం_2);
			ఆపు;

		స్థితి LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", సందేశం.ఉపసర్గ_2, సందేశం.సందేశం_2);
			ఆపు;

		స్థితి LogSeverity::Comment:
		స్థితి LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", సందేశం.ఉపసర్గ_2, సందేశం.సందేశం_2);
			ఆపు;

		స్థితి LogSeverity::Info:
		అప్రమేయ:
			ఒకవేళ (!సందేశం.ఉపసర్గ_2.ఖాళీలు())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", సందేశం.ఉపసర్గ_2);
			fmt::print(f, "{}", సందేశం.సందేశం_2);
			ఆపు;
	}
	ఒకవేళ (!సందేశం.సందేశం_2.ఖాళీలు())
		తదుపరి_print_log = సందేశం.సందేశం_2.వెనుక() == '\n';
}

తార్కిక ColorConsoleLogSink::should_log(స్థిరరాశి Log_సందేశం &సందేశం) స్థిరరాశి
{
	తిరిగి (!error_output) || (సందేశం.severity == LogSeverity::Error || సందేశం.severity == LogSeverity::NonFatalError ||
			(సందేశం.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#నిర్వచితమైతే _WIN32
పూర్ణాంకం wmain(పూర్ణాంకం argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#నిర్వచితమైతే _UCRT
	setlocale(LC_ALL, ".UTF-8");
#అయితే_ముగింపు
	// Configure the Win32 console to use UTF-8.
	సమితి_Console_CP(CP_UTF8);
	సమితి_Console_Output_CP(CP_UTF8);

	std::సదిశ<అక్షరం*> uargs;
	కొరకు (పూర్ణాంకం i = 0; i < argc; i++) {
		పూర్ణాంకం usize = Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, wargv[i], -1, ఖాళీ, 0, ఖాళీ, ఖాళీ);
		అక్షరం *uarg = (అక్షరం *)జ్ఞాపకం_ఇవ్వు(usize);
		Wide_Char_గమ్యం_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ఖాళీ, ఖాళీ);
		uargs.నెట్టు_వెనుక(uarg);
	}
	uargs.నెట్టు_వెనుక(ఖాళీ);
	అక్షరం **argv = uargs.దత్తాంశం();
#లేకపోతే
పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
#అయితే_ముగింపు
	స్వయంచాలక wall_clock_ప్రారంభం = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::సదిశ<std::string> vlog_నిర్వచిస్తుంది;
	std::సదిశ<std::string> passes_commands;
	std::సదిశ<std::string> frontend_files;
	std::సదిశ<std::string> plugin_filenames;
	std::సదిశ<std::string> special_వాదనలు;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	తార్కిక scriptfile_tcl = అబద్ధం;
	తార్కిక scriptfile_python = అబద్ధం;
	తార్కిక print_banner = నిజం;
	తార్కిక print_stats = నిజం;
	తార్కిక call_abort = అబద్ధం;
	తార్కిక timing_details = అబద్ధం;
	తార్కిక నిరంతరం_shell = నిజం;
	తార్కిక నిరంతరం_tcl_shell = అబద్ధం;
	తార్కిక బహుళకం_v = అబద్ధం;
	తార్కిక బహుళకం_q = అబద్ధం;
	తార్కిక quiet_warnings = అబద్ధం;
	తార్కిక log_stderr = అబద్ధం;
	no_color = std::getenv("NO_COLOR") != శూన్యసూచిక;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.సమితి_వెడల్పు(SIZE_MAX);

	options.కూడు_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::విలువ<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::విలువ<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::విలువ<std::string>(), "<scriptfile>")
#నిర్వచితమైతే YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::విలువ<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#అయితే_ముగింపు // YOSYS_ENABLE_TCL
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::విలువ<std::string>(), "<script>")
#అయితే_ముగింపు // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::విలువ<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::విలువ<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::విలువ<std::సదిశ<std::string>>())
	;
	options.కూడు_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::విలువ<పూర్ణాంకం>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::విలువ<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::విలువ<std::సదిశ<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::విలువ<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.కూడు_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::విలువ<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::విలువ<std::సదిశ<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::విలువ<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	ఒకవేళ (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::బయటకు << options.help() << std::పంక్తి_ముగింపు;
		నిష్క్రమించు(0);
	}
	ప్రయత్నించు {
		// Check for "--" in arguments
		స్వయంచాలక it = std::కనుగొను(argv, argv + argc, std::string("--"));
		ఒకవేళ (it != argv + argc) {
			special_వాదనలు.కేటాయించు(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::దూరం(argv, it);
		}

		స్వయంచాలక ఫలితం = options.parse(argc, argv);

		ఒకవేళ (ఫలితం.లెక్క("M")) memhasher_on();
		ఒకవేళ (ఫలితం.లెక్క("X")) yosys_xtrace += ఫలితం.లెక్క("X");
		ఒకవేళ (ఫలితం.లెక్క("A")) call_abort = నిజం;
		ఒకవేళ (ఫలితం.లెక్క("Q")) print_banner = అబద్ధం;
		ఒకవేళ (ఫలితం.లెక్క("T")) print_stats = అబద్ధం;
		ఒకవేళ (ఫలితం.లెక్క("no-version")) yosys_write_versions = అబద్ధం;
		ఒకవేళ (ఫలితం.లెక్క("V")) {
			std::బయటకు << yosys_సంస్కరణ_str << std::పంక్తి_ముగింపు;
			నిష్క్రమించు(0);
		}
		ఒకవేళ (ఫలితం.లెక్క("git-hash")) {
			std::బయటకు << yosys_git_hash_str << std::పంక్తి_ముగింపు;
			నిష్క్రమించు(0);
		}
		ఒకవేళ (ఫలితం.లెక్క("S")) {
			passes_commands.నెట్టు_వెనుక("synth");
			నిరంతరం_shell = అబద్ధం;
		}
		ఒకవేళ (ఫలితం.లెక్క("C")) నిరంతరం_tcl_shell = నిజం;
		ఒకవేళ (ఫలితం.లెక్క("g")) logger().force_debug_on();
		ఒకవేళ (ఫలితం.లెక్క("m")) plugin_filenames = ఫలితం["m"].as<std::సదిశ<std::string>>();
		ఒకవేళ (ఫలితం.లెక్క("f")) frontend_command = ఫలితం["f"].as<std::string>();
		ఒకవేళ (ఫలితం.లెక్క("H")) {
			passes_commands.నెట్టు_వెనుక("help");
			నిరంతరం_shell = అబద్ధం;
		}
		ఒకవేళ (ఫలితం.లెక్క("h")) {
			std::string res = ఫలితం["h"].as<std::string>();
			passes_commands.నెట్టు_వెనుక("help " + res);
			నిరంతరం_shell = అబద్ధం;
		}
		ఒకవేళ (ఫలితం.లెక్క("b")) {
			backend_command = ఫలితం["b"].as<std::string>();
			నిరంతరం_shell = అబద్ధం;
		}
		ఒకవేళ (ఫలితం.లెక్క("p")) {
			స్వయంచాలక cmds = ఫలితం["p"].as<std::సదిశ<std::string>>();
			passes_commands.చొప్పించు(passes_commands.చివర(), cmds.begin(), cmds.చివర());
			నిరంతరం_shell = అబద్ధం;
		}
		ఒకవేళ (ఫలితం.లెక్క("o")) {
			output_filename = ఫలితం["o"].as<std::string>();
			నిరంతరం_shell = అబద్ధం;
		}
		కొరకు (స్థిరరాశి స్వయంచాలక& కీలకం : {"l", "L"}) {
			ఒకవేళ (ఫలితం.లెక్క(కీలకం)) {
				కొరకు (స్థిరరాశి స్వయంచాలక& filename : ఫలితం[కీలకం].as<std::సదిశ<std::string>>()) {
					ప్రయత్నించు {
						logger().కూడు_sink<FileLogSink>(filename, కీలకం[0] == 'L', అబద్ధం);
					} పట్టుకో (స్థిరరాశి std::runtime_error &e) {
 					   std::cerr << e.what();
					   నిష్క్రమించు(1);
					}
				}
			}
		}
		ఒకవేళ (ఫలితం.లెక్క("q")) {
			బహుళకం_q = నిజం;
			ఒకవేళ (log_stderr) quiet_warnings = నిజం;
			log_stderr = నిజం;
		}
		ఒకవేళ (ఫలితం.లెక్క("v")) {
			బహుళకం_v = నిజం;
			log_stderr = నిజం;
			logger().సమితి_verbose_level(ఫలితం["v"].as<పూర్ణాంకం>());
		}
		ఒకవేళ (ఫలితం.లెక్క("t")) logger().సమితి_log_సమయం(నిజం);
		ఒకవేళ (ఫలితం.లెక్క("no-color")) no_color = నిజం;
		ఒకవేళ (ఫలితం.లెక్క("d")) timing_details = నిజం;
		కొరకు (స్థిరరాశి స్వయంచాలక& కీలకం : {"s", "c"}) {
			ఒకవేళ (ఫలితం.లెక్క(కీలకం)) {
				scriptfile = ఫలితం[కీలకం].as<std::string>();
				scriptfile_tcl = std::string(కీలకం) == "c";
				నిరంతరం_shell = అబద్ధం;
			}
		}
		ఒకవేళ (ఫలితం.లెక్క("y")) {
			scriptfile = ఫలితం["y"].as<std::string>();
			scriptfile_python = నిజం;
			నిరంతరం_shell = అబద్ధం;
		}
		కొరకు (స్థిరరాశి స్వయంచాలక& కీలకం : {"W", "w", "e"}) {
			ఒకవేళ (ఫలితం.లెక్క(కీలకం)) {
				స్వయంచాలక regexes = ఫలితం[కీలకం].as<std::సదిశ<std::string>>();
				కొరకు (స్థిరరాశి స్వయంచాలక& regex : regexes) {
					ఒకవేళ (std::string(కీలకం) == "W")
						logger().కూడు_warn(regex);
					ఒకవేళ (std::string(కీలకం) == "w")
						logger().కూడు_nowarn(regex);
					ఒకవేళ (std::string(కీలకం) == "e")
						logger().కూడు_werror(regex);
				}
			}
		}
		ఒకవేళ (ఫలితం.లెక్క("r")) topmodule = ఫలితం["r"].as<std::string>();
		ఒకవేళ (ఫలితం.లెక్క("D")) vlog_నిర్వచిస్తుంది = ఫలితం["D"].as<std::సదిశ<std::string>>();
		ఒకవేళ (ఫలితం.లెక్క("P")) {
			స్వయంచాలక dump_వాదనలు = ఫలితం["P"].as<std::సదిశ<std::string>>();
			కొరకు (స్థిరరాశి స్వయంచాలక& arg : dump_వాదనలు) {
				స్వయంచాలక టోకెన్లు = విభజించు_టోకెన్లు(arg, ":");
				ఒకవేళ (!టోకెన్లు.ఖాళీలు() && టోకెన్లు[0] == "ALL") {
					ఒకవేళ (టోకెన్లు.కొలత() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::పంక్తి_ముగింపు;
						నిష్క్రమించు(1);
					}
					logger().సమితి_hdump_all(నిజం);
				} లేకపోతే {
					ఒకవేళ (!టోకెన్లు.ఖాళీలు() && !టోకెన్లు[0].ఖాళీలు() && టోకెన్లు[0].వెనుక() == '.')
						టోకెన్లు[0].తీయి_వెనుక();
					ఒకవేళ (టోకెన్లు.కొలత() == 1)
						టోకెన్లు.నెట్టు_వెనుక("yosys_dump_" + టోకెన్లు[0] + ".il");
					ఒకవేళ (టోకెన్లు.కొలత() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::పంక్తి_ముగింపు;
						నిష్క్రమించు(1);
					}
					logger().కూడు_hdump(టోకెన్లు[0],టోకెన్లు[1]);
				}
			}
		}
		ఒకవేళ (ఫలితం.లెక్క("E")) depsfile = ఫలితం["E"].as<std::string>();
		ఒకవేళ (ఫలితం.లెక్క("x")) {
			కొరకు (స్థిరరాశి స్వయంచాలక &ignore : ఫలితం["x"].as<std::సదిశ<std::string>>())
    			logger().కూడు_experimental_ignore(ignore);
		}
		ఒకవేళ (ఫలితం.లెక్క("perffile")) perffile = ఫలితం["perffile"].as<std::string>();
		ఒకవేళ (ఫలితం.లెక్క("infile")) {
			frontend_files = ఫలితం["infile"].as<std::సదిశ<std::string>>();
		}
		ఒకవేళ (ఫలితం.లెక్క("autoidx")) {
			పూర్ణాంకం idx = ఫలితం["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		ఒకవేళ (!is_colored(stdout) || !is_colored(stderr)) {
			ఒకవేళ (!log_stderr) {
				logger().కూడు_sink<ConsoleLogSink>();
			} లేకపోతే {
				logger().కూడు_sink<StderrLogSink>(quiet_warnings);
			}
		} లేకపోతే
			logger().కూడు_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		ఒకవేళ (print_banner)
			yosys_banner();

	}
	పట్టుకో (స్థిరరాశి cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::పంక్తి_ముగింపు;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::పంక్తి_ముగింపు;
		నిష్క్రమించు(1);
	}

#ఒకవేళ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#ఒకవేళ defined(_WIN32)
		ఒకవేళ (getenv("HOMEDRIVE") != ఖాళీ && getenv("HOMEPATH") != ఖాళీ) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} లేకపోతే {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#లేకపోతే
		ఒకవేళ (getenv("XDG_STATE_HOME") == ఖాళీ || getenv("XDG_STATE_HOME")[0] == '\0') {
			ఒకవేళ (getenv("HOME") != ఖాళీ) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} లేకపోతే {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} లేకపోతే {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#అయితే_ముగింపు

	ఒకవేళ (!state_dir.ఖాళీలు()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_స్థానభ్రంశం = where_history();
	}
#అయితే_ముగింపు

	ఒకవేళ (print_stats)
		logger().ప్రారంభం_hasher();

#ఒకవేళ defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_మార్గం);
#అయితే_ముగింపు

#ఒకవేళ defined(__linux__)
	// set stack size to >= 128 MB
	{
		నిర్మాణం rlimit rl;
		స్థిరరాశి rlim_t దొంతర_కొలత = 128L * 1024L * 1024L;
		ఒకవేళ (getrlimit(RLIMIT_దొంతర, &rl) == 0 && rl.rlim_cur < దొంతర_కొలత) {
			rl.rlim_cur = దొంతర_కొలత;
			setrlimit(RLIMIT_దొంతర, &rl);
		}
	}
#అయితే_ముగింపు

	yosys_అమరిక();
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#అయితే_ముగింపు
	log_error_atexit = yosys_atexit;

	కొరకు (స్వయంచాలక &fn : plugin_filenames)
		ఎక్కించు_plugin(fn, {});

	log_suppressed();

	ఒకవేళ (!vlog_నిర్వచిస్తుంది.ఖాళీలు()) {
		std::string vdef_cmd = "read -define";
		కొరకు (స్వయంచాలక vdef : vlog_నిర్వచిస్తుంది)
			vdef_cmd += " " + vdef;
		నిరంతరం_pass(vdef_cmd);
	}

	ఒకవేళ (scriptfile.ఖాళీలు() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		కొరకు (స్వయంచాలక special_arg : special_వాదనలు)
			frontend_files.నెట్టు_వెనుక(special_arg);
	}

	కొరకు (స్వయంచాలక it = frontend_files.begin(); it != frontend_files.చివర(); ++it) {
		ఒకవేళ (నిరంతరం_frontend((*it).c_str(), frontend_command))
			నిరంతరం_shell = అబద్ధం;
	}

	ఒకవేళ (!topmodule.ఖాళీలు())
		నిరంతరం_pass("hierarchy -top " + topmodule);
	ఒకవేళ (!scriptfile.ఖాళీలు()) {
		ఒకవేళ (scriptfile_tcl) {
#నిర్వచితమైతే YOSYS_ENABLE_TCL
			పూర్ణాంకం tcl_argc = special_వాదనలు.కొలత();
			std::సదిశ<Tcl_Obj*> script_వాదనలు;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			కొరకు (స్వయంచాలక arg : special_వాదనలు)
				script_వాదనలు.నెట్టు_వెనుక(Tcl_NewStringObj(arg.c_str(), arg.నిడివి()));

			Tcl_Obj_సమితి_Var2(interp, Tcl_NewStringObj("argc", 4), ఖాళీ, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_సమితి_Var2(interp, Tcl_NewStringObj("argv", 4), ఖాళీ, Tcl_NewListObj(tcl_argc, script_వాదనలు.దత్తాంశం()), 0);
			Tcl_Obj_సమితి_Var2(interp, Tcl_NewStringObj("argv0", 5), ఖాళీ, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.నిడివి()), 0);

			ఒకవేళ (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ఫలితం(yosys_get_tcl_interp()));
#లేకపోతే
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#అయితే_ముగింపు
		} లేకపోతే ఒకవేళ (scriptfile_python) {
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
			py::list new_argv;
			పూర్ణాంకం py_argc = special_వాదనలు.కొలత() + 1;
			new_argv.append(scriptfile);
			కొరకు (పూర్ణాంకం i = 1; i < py_argc; ++i)
				new_argv.append(special_వాదనలు[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object మార్గం = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_మార్గం = మార్గం(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_మార్గం));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			ఒకవేళ (scriptfp == శూన్యసూచిక) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			ఒకవేళ (Py_నిరంతరం_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#లేకపోతే
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#అయితే_ముగింపు
		} లేకపోతే
			నిరంతరం_frontend(scriptfile, "script");
	}

	కొరకు (స్వయంచాలక it = passes_commands.begin(); it != passes_commands.చివర(); it++)
		నిరంతరం_pass(*it);

	ఒకవేళ (నిరంతరం_tcl_shell) {
#నిర్వచితమైతే YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#లేకపోతే
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#అయితే_ముగింపు
	} లేకపోతే {
		ఒకవేళ (నిరంతరం_shell)
			shell(yosys_design);
		లేకపోతే
			నిరంతరం_backend(output_filename, backend_command);
	}

	yosys_design->check();
	కొరకు (స్వయంచాలక it : saved_designs)
		it.రెండవ->check();
	కొరకు (స్వయంచాలక it : pushed_designs)
		it->check();

	ఒకవేళ (!depsfile.ఖాళీలు())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		ఒకవేళ (f == శూన్యసూచిక)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		తార్కిక మొదటి = నిజం;
		కొరకు (స్వయంచాలక fn : yosys_output_files) {
			fprintf(f, "%s%s", మొదటి ? "" : " ", escape_filename_spaces(fn).c_str());
			మొదటి = అబద్ధం;
		}
		fprintf(f, ":");
		కొరకు (స్వయంచాలక fn : yosys_ఆదానం_files) {
			ఒకవేళ (yosys_output_files.లెక్క(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	ఒకవేళ (print_stats)
	{
		std::string hash = logger().ముగింపు_hasher();
		logger().సమితి_log_సమయం(అబద్ధం);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = బహుళకం_v && !బహుళకం_q;
		ఒకవేళ (logger().get_warnings_మొత్తం())
			సంవర్గమానం("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_మొత్తం());

		ఒకవేళ (Get_కొలత(logger().get_experimental()) != 0)
			సంవర్గమానం("Warnings: %d experimental features used (not excluded with -x).\n", Get_కొలత(logger().get_experimental()));

		ఒకవేళ (Get_కొలత(logger().get_deprecated()) != 0) {
			సంవర్గమానం("Deprecated features used:\n");
			కొరకు (స్వయంచాలక &[feature, earliest_removal] : logger().get_deprecated())
				ఒకవేళ (earliest_removal.ఖాళీలు())
					సంవర్గమానం("    %s\n", feature);
				లేకపోతే
					సంవర్గమానం("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#నిర్వచితమైతే _WIN32
		సంవర్గమానం("End of script. Logfile hash: %s\n", hash);
		(శూన్య)wall_clock_ప్రారంభం;
#లేకపోతే
		std::string meminfo;
		std::string stats_divider = ", ";

		నిర్మాణం rusage ru_బఫరు;
		getrusage(RUSAGE_SELF, &ru_బఫరు);
		ఒకవేళ (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			నిర్మాణం rusage ru_బఫరు_children;
			getrusage(RUSAGE_CHILDREN, &ru_బఫరు_children);
			ru_బఫరు.ru_utime.tv_sec += ru_బఫరు_children.ru_utime.tv_sec;
			ru_బఫరు.ru_utime.tv_usec += ru_బఫరు_children.ru_utime.tv_usec;
			ru_బఫరు.ru_stime.tv_sec += ru_బఫరు_children.ru_stime.tv_sec;
			ru_బఫరు.ru_stime.tv_usec += ru_బఫరు_children.ru_stime.tv_usec;
#ఒకవేళ defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_బఫరు.ru_maxrss = std::max(ru_బఫరు.ru_maxrss, ru_బఫరు_children.ru_maxrss);
#అయితే_ముగింపు
		}
#ఒకవేళ defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_బఫరు.ru_maxrss / 1024.0);
#లేదావేళ defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_బఫరు.ru_maxrss / (1024.0 * 1024.0));
#అయితే_ముగింపు
		ద్విగుణ wall_seconds = std::chrono::duration<ద్విగుణ>(
				std::chrono::steady_clock::now() - wall_clock_ప్రారంభం).లెక్క();

		సంవర్గమానం("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_బఫరు.ru_utime.tv_sec + 1e-6 * ru_బఫరు.ru_utime.tv_usec,
				ru_బఫరు.ru_stime.tv_sec + 1e-6 * ru_బఫరు.ru_stime.tv_usec, meminfo.c_str());
#అయితే_ముగింపు
		సంవర్గమానం("%s\n", yosys_maybe_సంస్కరణ());

		int64_t మొత్తం_ns = 0;
		std::సమితి<tuple<int64_t, పూర్ణాంకం, std::string>> timedat;

		కొరకు (స్వయంచాలక &it : pass_రిజిస్టరు)
			ఒకవేళ (it.రెండవ->call_counter) {
				స్వయంచాలక pass_ns = it.రెండవ->runtime_ns + 1;
				మొత్తం_ns += pass_ns;
				timedat.చొప్పించు(make_tuple(pass_ns, it.రెండవ->call_counter, it.మొదటి));
			}
		ఒకవేళ (twine_gc_లెక్క) {
			మొత్తం_ns += twine_gc_ns;
			timedat.చొప్పించు(make_tuple(twine_gc_ns, twine_gc_లెక్క, "twine_gc"));
		}

		ఒకవేళ (timing_details)
		{
			సంవర్గమానం("Time spent:\n");
			కొరకు (స్వయంచాలక it = timedat.rbegin(); it != timedat.rend(); it++) {
				సంవర్గమానం("%5d%% %5d calls %8.3f sec %s\n", పూర్ణాంకం(100*std::get<0>(*it) / మొత్తం_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		లేకపోతే
		{
			పూర్ణాంకం బహిర్గతం_లెక్క = 0;
			std::string సందేశం_2 = "Time spent:";
			కొరకు (స్వయంచాలక it = timedat.rbegin(); it != timedat.rend() && బహిర్గతం_లెక్క < 4; it++, బహిర్గతం_లెక్క++) {
				ఒకవేళ (బహిర్గతం_లెక్క >= 2 && (std::get<0>(*it) < 1000000000 || పూర్ణాంకం(100*std::get<0>(*it) / మొత్తం_ns) < 20)) {
					సందేశం_2 += ", ...";
					ఆపు;
				}
				సందేశం_2 += stringf("%s %d%% %dx %s (%d sec)", బహిర్గతం_లెక్క ? "," : "", పూర్ణాంకం(100*std::get<0>(*it) / మొత్తం_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), పూర్ణాంకం(std::get<0>(*it) / 1000000000));
			}
			సందేశం_2 += బహిర్గతం_లెక్క ? "" : " no commands executed";
			సంవర్గమానం("%s\n", సందేశం_2);
		}
		ఒకవేళ(!perffile.ఖాళీలు())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			ఒకవేళ (f == శూన్యసూచిక)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_సంస్కరణ());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", మొత్తం_ns);
			fprintf(f, "  \"passes\": {");

			తార్కిక మొదటి = నిజం;
			కొరకు (స్వయంచాలక it = timedat.rbegin(); it != timedat.rend(); it++) {
				ఒకవేళ (!మొదటి)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				మొదటి = అబద్ధం;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	ఒకవేళ (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#ఒకవేళ defined(_MSC_VER)
	_exit(0);
#లేదావేళ defined(_WIN32)
	_Exit(0);
#అయితే_ముగింపు
	తిరిగి 0;
}

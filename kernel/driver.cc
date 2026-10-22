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
#تعریف_کړه CXXOPTS_ویکټور_DELIMITER '\0'
#ضمن "libs/cxxopts/include/cxxopts.hpp"
#ضمن <iostream>
#ضمن <chrono>

#که_تعریف_شوی YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_TCL
#  include <tcl.h>
#د_که_پای

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#د_که_پای

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <limits.h>
#ضمن <errno.h>
#ضمن <fcntl.h>
#که_نه_تعریف_شوی __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#د_که_پای
#ضمن <inttypes.h>

#إذا defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#د_که_پای

#که_تعریف_شوی __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#د_که_پای

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#د_که_پای

#ضمن <fmt/color.h>
#ضمن <fmt/format.h>

USING_YOSYS_NAMESPACE

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحيح yosys_history_دننه_واټن = 0;
std::string yosys_history_file;
#د_که_پای

فراغ yosys_atexit()
{
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	إذا (!yosys_history_file.تشه()) {
#إذا defined(YOSYS_ENABLE_READLINE)
		إذا (yosys_history_دننه_واټن > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_دننه_واټن, yosys_history_file.c_str());
		} وإلا
			write_history(yosys_history_file.c_str());
#وإلا
		write_history(yosys_history_file.c_str());
#د_که_پای
	}

	clear_history();
#إذا defined(YOSYS_ENABLE_READLINE)
	HIST_ثبت **hist_list = history_list();
	إذا (hist_list != عدم)
		حرر(hist_list);
#د_که_پای
#د_که_پای
}

#إذا defined(__OpenBSD__)
نطاق Yosys {
خارجي حرف *yosys_argv0;
خارجي حرف yosys_لار[لار_MAX];
};
#د_که_پای
#که_تعریف_شوی YOSYS_ENABLE_TCL
نطاق Yosys {
	خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);
	خارجي فراغ yosys_tcl_activate_repl();
};
#د_که_پای

نطاق Yosys {
	خارجي منطقي log_stderr_sink_forced;
};

صنف ColorConsoleLogSink : عام LogSink
{
عام:
	څرګند ColorConsoleLogSink(منطقي err, منطقي quiet) : error_output(err), quiet_warnings(quiet) {}
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override;
	منطقي should_log(ثابت Log_پیغام &پیغام) ثابت override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع error_output ? تش_اشاره : stdout; }
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
	DWORD ډېرپېښ;
	أرجع handle != INVALID_HANDLE_ارزښت &&
			Get_Console_ډېرپېښ(handle, &ډېرپېښ);
#وإلا
	أرجع isatty(fileno(file));
#د_که_پای
}

فراغ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي بل_print_log = ریښتیا;

فراغ ColorConsoleLogSink::لوګاریتم(ثابت Log_پیغام &پیغام)
{
	FILE *f = (error_output || پیغام.severity == LogSeverity::Error) ? stderr : stdout;

	إذا (logger().get_log_وخت() && بل_print_log) {
		خپلکاری elapsed = پیغام.timestamp - logger().get_initial_وخت();
		خپلکاری us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string وخت_str = stringf("[%05d.%06d] ", صحيح(us.شمېر() / 1'000'000),صحيح(us.شمېر() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", وخت_str);
		بل_print_log = خطأ;
	}

	إذا (!پیغام.src.filename.تشه())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", پیغام.src.filename, پیغام.src.پیل_کرښه);

	بدل (پیغام.severity) {
		حالة LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", پیغام.مختاړی_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیغام.پیغام_2);
			اكسر;

		حالة LogSeverity::Error:
		حالة LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", پیغام.مختاړی_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیغام.پیغام_2);
			اكسر;

		حالة LogSeverity::سرلیک:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", پیغام.مختاړی_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", پیغام.پیغام_2);
			اكسر;

		حالة LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", پیغام.مختاړی_2, پیغام.پیغام_2);
			اكسر;

		حالة LogSeverity::Comment:
		حالة LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", پیغام.مختاړی_2, پیغام.پیغام_2);
			اكسر;

		حالة LogSeverity::Info:
		افتراضي:
			إذا (!پیغام.مختاړی_2.تشه())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", پیغام.مختاړی_2);
			fmt::print(f, "{}", پیغام.پیغام_2);
			اكسر;
	}
	إذا (!پیغام.پیغام_2.تشه())
		بل_print_log = پیغام.پیغام_2.شا() == '\n';
}

منطقي ColorConsoleLogSink::should_log(ثابت Log_پیغام &پیغام) ثابت
{
	أرجع (!error_output) || (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#که_تعریف_شوی _WIN32
صحيح wmain(صحيح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#که_تعریف_شوی _UCRT
	setlocale(LC_ALL, ".UTF-8");
#د_که_پای
	// Configure the Win32 console to use UTF-8.
	سېټ_Console_CP(CP_UTF8);
	سېټ_Console_Output_CP(CP_UTF8);

	std::ویکټور<حرف*> uargs;
	لكل (صحيح i = 0; i < argc; i++) {
		صحيح usize = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wargv[i], -1, عدم, 0, عدم, عدم);
		حرف *uarg = (حرف *)حافظه_ورکړه(usize);
		Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, عدم, عدم);
		uargs.ټېل_شا(uarg);
	}
	uargs.ټېل_شا(عدم);
	حرف **argv = uargs.معلومات();
#وإلا
صحيح اصلي(صحيح argc, حرف **argv)
{
#د_که_پای
	خپلکاری wall_clock_پیل = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::ویکټور<std::string> vlog_تعریفوي;
	std::ویکټور<std::string> passes_commands;
	std::ویکټور<std::string> frontend_files;
	std::ویکټور<std::string> plugin_filenames;
	std::ویکټور<std::string> special_دلیلونه;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	منطقي scriptfile_tcl = خطأ;
	منطقي scriptfile_python = خطأ;
	منطقي print_banner = ریښتیا;
	منطقي print_stats = ریښتیا;
	منطقي call_abort = خطأ;
	منطقي timing_details = خطأ;
	منطقي پرلپسې_shell = ریښتیا;
	منطقي پرلپسې_tcl_shell = خطأ;
	منطقي ډېرپېښ_v = خطأ;
	منطقي ډېرپېښ_q = خطأ;
	منطقي quiet_warnings = خطأ;
	منطقي log_stderr = خطأ;
	no_color = std::getenv("NO_COLOR") != تش_اشاره;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.سېټ_پلنوالی(SIZE_MAX);

	options.جمع_کړه_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::ارزښت<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::ارزښت<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::ارزښت<std::string>(), "<scriptfile>")
#که_تعریف_شوی YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::ارزښت<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#د_که_پای // YOSYS_ENABLE_TCL
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::ارزښت<std::string>(), "<script>")
#د_که_پای // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::ارزښت<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::ارزښت<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::ارزښت<std::ویکټور<std::string>>())
	;
	options.جمع_کړه_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::ارزښت<صحيح>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::ارزښت<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::ارزښت<std::ویکټور<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::ارزښت<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.جمع_کړه_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::ارزښت<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::ارزښت<std::ویکټور<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::ارزښت<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	إذا (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::وتنه << options.help() << std::کرښه_پای;
		وتل(0);
	}
	حاول {
		// Check for "--" in arguments
		خپلکاری it = std::پیدا_کړه(argv, argv + argc, std::string("--"));
		إذا (it != argv + argc) {
			special_دلیلونه.ټاکل(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::واټن(argv, it);
		}

		خپلکاری پایله = options.parse(argc, argv);

		إذا (پایله.شمېر("M")) memhasher_on();
		إذا (پایله.شمېر("X")) yosys_xtrace += پایله.شمېر("X");
		إذا (پایله.شمېر("A")) call_abort = ریښتیا;
		إذا (پایله.شمېر("Q")) print_banner = خطأ;
		إذا (پایله.شمېر("T")) print_stats = خطأ;
		إذا (پایله.شمېر("no-version")) yosys_write_versions = خطأ;
		إذا (پایله.شمېر("V")) {
			std::وتنه << yosys_نسخه_str << std::کرښه_پای;
			وتل(0);
		}
		إذا (پایله.شمېر("git-hash")) {
			std::وتنه << yosys_git_hash_str << std::کرښه_پای;
			وتل(0);
		}
		إذا (پایله.شمېر("S")) {
			passes_commands.ټېل_شا("synth");
			پرلپسې_shell = خطأ;
		}
		إذا (پایله.شمېر("C")) پرلپسې_tcl_shell = ریښتیا;
		إذا (پایله.شمېر("g")) logger().force_debug_on();
		إذا (پایله.شمېر("m")) plugin_filenames = پایله["m"].as<std::ویکټور<std::string>>();
		إذا (پایله.شمېر("f")) frontend_command = پایله["f"].as<std::string>();
		إذا (پایله.شمېر("H")) {
			passes_commands.ټېل_شا("help");
			پرلپسې_shell = خطأ;
		}
		إذا (پایله.شمېر("h")) {
			std::string res = پایله["h"].as<std::string>();
			passes_commands.ټېل_شا("help " + res);
			پرلپسې_shell = خطأ;
		}
		إذا (پایله.شمېر("b")) {
			backend_command = پایله["b"].as<std::string>();
			پرلپسې_shell = خطأ;
		}
		إذا (پایله.شمېر("p")) {
			خپلکاری cmds = پایله["p"].as<std::ویکټور<std::string>>();
			passes_commands.ورننباسه(passes_commands.پای_ټکی(), cmds.begin(), cmds.پای_ټکی());
			پرلپسې_shell = خطأ;
		}
		إذا (پایله.شمېر("o")) {
			output_filename = پایله["o"].as<std::string>();
			پرلپسې_shell = خطأ;
		}
		لكل (ثابت خپلکاری& کلی : {"l", "L"}) {
			إذا (پایله.شمېر(کلی)) {
				لكل (ثابت خپلکاری& filename : پایله[کلی].as<std::ویکټور<std::string>>()) {
					حاول {
						logger().جمع_کړه_sink<FileLogSink>(filename, کلی[0] == 'L', خطأ);
					} امسك (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   وتل(1);
					}
				}
			}
		}
		إذا (پایله.شمېر("q")) {
			ډېرپېښ_q = ریښتیا;
			إذا (log_stderr) quiet_warnings = ریښتیا;
			log_stderr = ریښتیا;
		}
		إذا (پایله.شمېر("v")) {
			ډېرپېښ_v = ریښتیا;
			log_stderr = ریښتیا;
			logger().سېټ_verbose_level(پایله["v"].as<صحيح>());
		}
		إذا (پایله.شمېر("t")) logger().سېټ_log_وخت(ریښتیا);
		إذا (پایله.شمېر("no-color")) no_color = ریښتیا;
		إذا (پایله.شمېر("d")) timing_details = ریښتیا;
		لكل (ثابت خپلکاری& کلی : {"s", "c"}) {
			إذا (پایله.شمېر(کلی)) {
				scriptfile = پایله[کلی].as<std::string>();
				scriptfile_tcl = std::string(کلی) == "c";
				پرلپسې_shell = خطأ;
			}
		}
		إذا (پایله.شمېر("y")) {
			scriptfile = پایله["y"].as<std::string>();
			scriptfile_python = ریښتیا;
			پرلپسې_shell = خطأ;
		}
		لكل (ثابت خپلکاری& کلی : {"W", "w", "e"}) {
			إذا (پایله.شمېر(کلی)) {
				خپلکاری regexes = پایله[کلی].as<std::ویکټور<std::string>>();
				لكل (ثابت خپلکاری& regex : regexes) {
					إذا (std::string(کلی) == "W")
						logger().جمع_کړه_warn(regex);
					إذا (std::string(کلی) == "w")
						logger().جمع_کړه_nowarn(regex);
					إذا (std::string(کلی) == "e")
						logger().جمع_کړه_werror(regex);
				}
			}
		}
		إذا (پایله.شمېر("r")) topmodule = پایله["r"].as<std::string>();
		إذا (پایله.شمېر("D")) vlog_تعریفوي = پایله["D"].as<std::ویکټور<std::string>>();
		إذا (پایله.شمېر("P")) {
			خپلکاری dump_دلیلونه = پایله["P"].as<std::ویکټور<std::string>>();
			لكل (ثابت خپلکاری& arg : dump_دلیلونه) {
				خپلکاری نښې = وېشل_نښې(arg, ":");
				إذا (!نښې.تشه() && نښې[0] == "ALL") {
					إذا (نښې.اندازه() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::کرښه_پای;
						وتل(1);
					}
					logger().سېټ_hdump_all(ریښتیا);
				} وإلا {
					إذا (!نښې.تشه() && !نښې[0].تشه() && نښې[0].شا() == '.')
						نښې[0].راوباسه_شا();
					إذا (نښې.اندازه() == 1)
						نښې.ټېل_شا("yosys_dump_" + نښې[0] + ".il");
					إذا (نښې.اندازه() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::کرښه_پای;
						وتل(1);
					}
					logger().جمع_کړه_hdump(نښې[0],نښې[1]);
				}
			}
		}
		إذا (پایله.شمېر("E")) depsfile = پایله["E"].as<std::string>();
		إذا (پایله.شمېر("x")) {
			لكل (ثابت خپلکاری &ignore : پایله["x"].as<std::ویکټور<std::string>>())
    			logger().جمع_کړه_experimental_ignore(ignore);
		}
		إذا (پایله.شمېر("perffile")) perffile = پایله["perffile"].as<std::string>();
		إذا (پایله.شمېر("infile")) {
			frontend_files = پایله["infile"].as<std::ویکټور<std::string>>();
		}
		إذا (پایله.شمېر("autoidx")) {
			صحيح idx = پایله["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		إذا (!is_colored(stdout) || !is_colored(stderr)) {
			إذا (!log_stderr) {
				logger().جمع_کړه_sink<ConsoleLogSink>();
			} وإلا {
				logger().جمع_کړه_sink<StderrLogSink>(quiet_warnings);
			}
		} وإلا
			logger().جمع_کړه_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		إذا (print_banner)
			yosys_banner();

	}
	امسك (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::کرښه_پای;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::کرښه_پای;
		وتل(1);
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
	#د_که_پای

	إذا (!state_dir.تشه()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_دننه_واټن = where_history();
	}
#د_که_پای

	إذا (print_stats)
		logger().پیل_hasher();

#إذا defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_لار);
#د_که_پای

#إذا defined(__linux__)
	// set stack size to >= 128 MB
	{
		بنية rlimit rl;
		ثابت rlim_t ډېری_اندازه = 128L * 1024L * 1024L;
		إذا (getrlimit(RLIMIT_ډېری, &rl) == 0 && rl.rlim_cur < ډېری_اندازه) {
			rl.rlim_cur = ډېری_اندازه;
			setrlimit(RLIMIT_ډېری, &rl);
		}
	}
#د_که_پای

	yosys_تیاری();
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#د_که_پای
	log_error_atexit = yosys_atexit;

	لكل (خپلکاری &fn : plugin_filenames)
		بارول_plugin(fn, {});

	log_suppressed();

	إذا (!vlog_تعریفوي.تشه()) {
		std::string vdef_cmd = "read -define";
		لكل (خپلکاری vdef : vlog_تعریفوي)
			vdef_cmd += " " + vdef;
		پرلپسې_pass(vdef_cmd);
	}

	إذا (scriptfile.تشه() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		لكل (خپلکاری special_arg : special_دلیلونه)
			frontend_files.ټېل_شا(special_arg);
	}

	لكل (خپلکاری it = frontend_files.begin(); it != frontend_files.پای_ټکی(); ++it) {
		إذا (پرلپسې_frontend((*it).c_str(), frontend_command))
			پرلپسې_shell = خطأ;
	}

	إذا (!topmodule.تشه())
		پرلپسې_pass("hierarchy -top " + topmodule);
	إذا (!scriptfile.تشه()) {
		إذا (scriptfile_tcl) {
#که_تعریف_شوی YOSYS_ENABLE_TCL
			صحيح tcl_argc = special_دلیلونه.اندازه();
			std::ویکټور<Tcl_Obj*> script_دلیلونه;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			لكل (خپلکاری arg : special_دلیلونه)
				script_دلیلونه.ټېل_شا(Tcl_NewStringObj(arg.c_str(), arg.اوږدتیا()));

			Tcl_Obj_سېټ_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_سېټ_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(tcl_argc, script_دلیلونه.معلومات()), 0);
			Tcl_Obj_سېټ_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.اوږدتیا()), 0);

			إذا (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_پایله(yosys_get_tcl_interp()));
#وإلا
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#د_که_پای
		} وإلا إذا (scriptfile_python) {
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحيح py_argc = special_دلیلونه.اندازه() + 1;
			new_argv.append(scriptfile);
			لكل (صحيح i = 1; i < py_argc; ++i)
				new_argv.append(special_دلیلونه[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object لار = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_لار = لار(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_لار));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			إذا (scriptfp == تش_اشاره) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			إذا (Py_پرلپسې_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#وإلا
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#د_که_پای
		} وإلا
			پرلپسې_frontend(scriptfile, "script");
	}

	لكل (خپلکاری it = passes_commands.begin(); it != passes_commands.پای_ټکی(); it++)
		پرلپسې_pass(*it);

	إذا (پرلپسې_tcl_shell) {
#که_تعریف_شوی YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وإلا
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#د_که_پای
	} وإلا {
		إذا (پرلپسې_shell)
			shell(yosys_design);
		وإلا
			پرلپسې_backend(output_filename, backend_command);
	}

	yosys_design->check();
	لكل (خپلکاری it : saved_designs)
		it.دوهم->check();
	لكل (خپلکاری it : pushed_designs)
		it->check();

	إذا (!depsfile.تشه())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		إذا (f == تش_اشاره)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقي لومړی = ریښتیا;
		لكل (خپلکاری fn : yosys_output_files) {
			fprintf(f, "%s%s", لومړی ? "" : " ", escape_filename_spaces(fn).c_str());
			لومړی = خطأ;
		}
		fprintf(f, ":");
		لكل (خپلکاری fn : yosys_ننوت_files) {
			إذا (yosys_output_files.شمېر(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	إذا (print_stats)
	{
		std::string hash = logger().پای_hasher();
		logger().سېټ_log_وخت(خطأ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = ډېرپېښ_v && !ډېرپېښ_q;
		إذا (logger().get_warnings_ټول())
			لوګاریتم("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_ټول());

		إذا (Get_اندازه(logger().get_experimental()) != 0)
			لوګاریتم("Warnings: %d experimental features used (not excluded with -x).\n", Get_اندازه(logger().get_experimental()));

		إذا (Get_اندازه(logger().get_deprecated()) != 0) {
			لوګاریتم("Deprecated features used:\n");
			لكل (خپلکاری &[feature, earliest_removal] : logger().get_deprecated())
				إذا (earliest_removal.تشه())
					لوګاریتم("    %s\n", feature);
				وإلا
					لوګاریتم("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#که_تعریف_شوی _WIN32
		لوګاریتم("End of script. Logfile hash: %s\n", hash);
		(فراغ)wall_clock_پیل;
#وإلا
		std::string meminfo;
		std::string stats_divider = ", ";

		بنية rusage ru_بفر;
		getrusage(RUSAGE_SELF, &ru_بفر);
		إذا (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			بنية rusage ru_بفر_children;
			getrusage(RUSAGE_CHILDREN, &ru_بفر_children);
			ru_بفر.ru_utime.tv_sec += ru_بفر_children.ru_utime.tv_sec;
			ru_بفر.ru_utime.tv_usec += ru_بفر_children.ru_utime.tv_usec;
			ru_بفر.ru_stime.tv_sec += ru_بفر_children.ru_stime.tv_sec;
			ru_بفر.ru_stime.tv_usec += ru_بفر_children.ru_stime.tv_usec;
#إذا defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_بفر.ru_maxrss = std::max(ru_بفر.ru_maxrss, ru_بفر_children.ru_maxrss);
#د_که_پای
		}
#إذا defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / 1024.0);
#وإلاإذا defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / (1024.0 * 1024.0));
#د_که_پای
		مزدوج wall_seconds = std::chrono::duration<مزدوج>(
				std::chrono::steady_clock::now() - wall_clock_پیل).شمېر();

		لوګاریتم("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_بفر.ru_utime.tv_sec + 1e-6 * ru_بفر.ru_utime.tv_usec,
				ru_بفر.ru_stime.tv_sec + 1e-6 * ru_بفر.ru_stime.tv_usec, meminfo.c_str());
#د_که_پای
		لوګاریتم("%s\n", yosys_maybe_نسخه());

		int64_t ټول_ns = 0;
		std::سېټ<tuple<int64_t, صحيح, std::string>> timedat;

		لكل (خپلکاری &it : pass_ثبتوونکی)
			إذا (it.دوهم->call_counter) {
				خپلکاری pass_ns = it.دوهم->runtime_ns + 1;
				ټول_ns += pass_ns;
				timedat.ورننباسه(make_tuple(pass_ns, it.دوهم->call_counter, it.لومړی));
			}
		إذا (twine_gc_شمېر) {
			ټول_ns += twine_gc_ns;
			timedat.ورننباسه(make_tuple(twine_gc_ns, twine_gc_شمېر, "twine_gc"));
		}

		إذا (timing_details)
		{
			لوګاریتم("Time spent:\n");
			لكل (خپلکاری it = timedat.rbegin(); it != timedat.rend(); it++) {
				لوګاریتم("%5d%% %5d calls %8.3f sec %s\n", صحيح(100*std::get<0>(*it) / ټول_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وإلا
		{
			صحيح بهر_شمېر = 0;
			std::string پیغام_2 = "Time spent:";
			لكل (خپلکاری it = timedat.rbegin(); it != timedat.rend() && بهر_شمېر < 4; it++, بهر_شمېر++) {
				إذا (بهر_شمېر >= 2 && (std::get<0>(*it) < 1000000000 || صحيح(100*std::get<0>(*it) / ټول_ns) < 20)) {
					پیغام_2 += ", ...";
					اكسر;
				}
				پیغام_2 += stringf("%s %d%% %dx %s (%d sec)", بهر_شمېر ? "," : "", صحيح(100*std::get<0>(*it) / ټول_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحيح(std::get<0>(*it) / 1000000000));
			}
			پیغام_2 += بهر_شمېر ? "" : " no commands executed";
			لوګاریتم("%s\n", پیغام_2);
		}
		إذا(!perffile.تشه())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			إذا (f == تش_اشاره)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_نسخه());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", ټول_ns);
			fprintf(f, "  \"passes\": {");

			منطقي لومړی = ریښتیا;
			لكل (خپلکاری it = timedat.rbegin(); it != timedat.rend(); it++) {
				إذا (!لومړی)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				لومړی = خطأ;
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
#د_که_پای
	أرجع 0;
}

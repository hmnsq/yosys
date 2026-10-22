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
#تعریف_کرو CXXOPTS_سمتیہ_DELIMITER '\0'
#ضمن "libs/cxxopts/include/cxxopts.hpp"
#ضمن <iostream>
#ضمن <chrono>

#جے_تعریف_شدہ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_TCL
#  include <tcl.h>
#جے_انت

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#جے_انت

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <limits.h>
#ضمن <errno.h>
#ضمن <fcntl.h>
#جے_بنا_تعریف __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#جے_انت
#ضمن <inttypes.h>

#إذا defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#جے_انت

#جے_تعریف_شدہ __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#جے_انت

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#جے_انت

#ضمن <fmt/color.h>
#ضمن <fmt/format.h>

USING_YOSYS_NAMESPACE

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحيح yosys_history_وتھ = 0;
std::string yosys_history_file;
#جے_انت

فراغ yosys_atexit()
{
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	إذا (!yosys_history_file.خالی_تھاواں()) {
#إذا defined(YOSYS_ENABLE_READLINE)
		إذا (yosys_history_وتھ > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_وتھ, yosys_history_file.c_str());
		} وإلا
			write_history(yosys_history_file.c_str());
#وإلا
		write_history(yosys_history_file.c_str());
#جے_انت
	}

	clear_history();
#إذا defined(YOSYS_ENABLE_READLINE)
	HIST_اندراج **hist_list = history_list();
	إذا (hist_list != عدم)
		حرر(hist_list);
#جے_انت
#جے_انت
}

#إذا defined(__OpenBSD__)
نطاق Yosys {
خارجي حرف *yosys_argv0;
خارجي حرف yosys_رستہ[رستہ_MAX];
};
#جے_انت
#جے_تعریف_شدہ YOSYS_ENABLE_TCL
نطاق Yosys {
	خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);
	خارجي فراغ yosys_tcl_activate_repl();
};
#جے_انت

نطاق Yosys {
	خارجي منطقي log_stderr_sink_forced;
};

صنف ColorConsoleLogSink : عام LogSink
{
عام:
	واضح ColorConsoleLogSink(منطقي err, منطقي quiet) : error_output(err), quiet_warnings(quiet) {}
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override;
	منطقي should_log(ثابت Log_سنیہا &پیغام) ثابت override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع error_output ? خالی_سوچک : stdout; }
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
	DWORD منوال;
	أرجع handle != INVALID_HANDLE_قدر &&
			Get_Console_منوال(handle, &منوال);
#وإلا
	أرجع isatty(fileno(file));
#جے_انت
}

فراغ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي اگلا_print_log = سچ;

فراغ ColorConsoleLogSink::لوگارتھم(ثابت Log_سنیہا &پیغام)
{
	FILE *f = (error_output || پیغام.severity == LogSeverity::Error) ? stderr : stdout;

	إذا (logger().get_log_ویلا() && اگلا_print_log) {
		خودکار elapsed = پیغام.timestamp - logger().get_initial_ویلا();
		خودکار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string ویلا_str = stringf("[%05d.%06d] ", صحيح(us.گنتی() / 1'000'000),صحيح(us.گنتی() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", ویلا_str);
		اگلا_print_log = خطأ;
	}

	إذا (!پیغام.src.filename.خالی_تھاواں())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", پیغام.src.filename, پیغام.src.شروع_سطر);

	بدل (پیغام.severity) {
		حالة LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", پیغام.اگیتر_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیغام.سنیہا);
			اكسر;

		حالة LogSeverity::Error:
		حالة LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", پیغام.اگیتر_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیغام.سنیہا);
			اكسر;

		حالة LogSeverity::سرخی:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", پیغام.اگیتر_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", پیغام.سنیہا);
			اكسر;

		حالة LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", پیغام.اگیتر_2, پیغام.سنیہا);
			اكسر;

		حالة LogSeverity::Comment:
		حالة LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", پیغام.اگیتر_2, پیغام.سنیہا);
			اكسر;

		حالة LogSeverity::Info:
		افتراضي:
			إذا (!پیغام.اگیتر_2.خالی_تھاواں())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", پیغام.اگیتر_2);
			fmt::print(f, "{}", پیغام.سنیہا);
			اكسر;
	}
	إذا (!پیغام.سنیہا.خالی_تھاواں())
		اگلا_print_log = پیغام.سنیہا.پٹھ() == '\n';
}

منطقي ColorConsoleLogSink::should_log(ثابت Log_سنیہا &پیغام) ثابت
{
	أرجع (!error_output) || (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#جے_تعریف_شدہ _WIN32
صحيح wmain(صحيح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#جے_تعریف_شدہ _UCRT
	setlocale(LC_ALL, ".UTF-8");
#جے_انت
	// Configure the Win32 console to use UTF-8.
	سیٹ_Console_CP(CP_UTF8);
	سیٹ_Console_Output_CP(CP_UTF8);

	std::سمتیہ<حرف*> uargs;
	لكل (صحيح i = 0; i < argc; i++) {
		صحيح usize = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wargv[i], -1, عدم, 0, عدم, عدم);
		حرف *uarg = (حرف *)حافظہ_دیو(usize);
		Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, عدم, عدم);
		uargs.ڈالو_پٹھ(uarg);
	}
	uargs.ڈالو_پٹھ(عدم);
	حرف **argv = uargs.انکڑے();
#وإلا
صحيح مکھ(صحيح argc, حرف **argv)
{
#جے_انت
	خودکار wall_clock_شروع = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::سمتیہ<std::string> vlog_مقرر_کردا;
	std::سمتیہ<std::string> passes_commands;
	std::سمتیہ<std::string> frontend_files;
	std::سمتیہ<std::string> plugin_filenames;
	std::سمتیہ<std::string> special_دلیلاں;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	منطقي scriptfile_tcl = خطأ;
	منطقي scriptfile_python = خطأ;
	منطقي print_banner = سچ;
	منطقي print_stats = سچ;
	منطقي call_abort = خطأ;
	منطقي timing_details = خطأ;
	منطقي لگاتار_shell = سچ;
	منطقي لگاتار_tcl_shell = خطأ;
	منطقي منوال_v = خطأ;
	منطقي منوال_q = خطأ;
	منطقي quiet_warnings = خطأ;
	منطقي log_stderr = خطأ;
	no_color = std::getenv("NO_COLOR") != خالی_سوچک;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.سیٹ_چوڑائی(SIZE_MAX);

	options.جوڑو_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::قدر<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::قدر<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::قدر<std::string>(), "<scriptfile>")
#جے_تعریف_شدہ YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::قدر<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#جے_انت // YOSYS_ENABLE_TCL
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::قدر<std::string>(), "<script>")
#جے_انت // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::قدر<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::قدر<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::قدر<std::سمتیہ<std::string>>())
	;
	options.جوڑو_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::قدر<صحيح>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::قدر<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::قدر<std::سمتیہ<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::قدر<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.جوڑو_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::قدر<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::قدر<std::سمتیہ<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::قدر<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	إذا (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::باہر << options.help() << std::سطر_انت;
		باہر_نکلو(0);
	}
	حاول {
		// Check for "--" in arguments
		خودکار it = std::لبھو(argv, argv + argc, std::string("--"));
		إذا (it != argv + argc) {
			special_دلیلاں.سونپو(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::فاصلہ(argv, it);
		}

		خودکار نتیجہ = options.parse(argc, argv);

		إذا (نتیجہ.گنتی("M")) memhasher_on();
		إذا (نتیجہ.گنتی("X")) yosys_xtrace += نتیجہ.گنتی("X");
		إذا (نتیجہ.گنتی("A")) call_abort = سچ;
		إذا (نتیجہ.گنتی("Q")) print_banner = خطأ;
		إذا (نتیجہ.گنتی("T")) print_stats = خطأ;
		إذا (نتیجہ.گنتی("no-version")) yosys_write_versions = خطأ;
		إذا (نتیجہ.گنتی("V")) {
			std::باہر << yosys_نسخہ_str << std::سطر_انت;
			باہر_نکلو(0);
		}
		إذا (نتیجہ.گنتی("git-hash")) {
			std::باہر << yosys_git_hash_str << std::سطر_انت;
			باہر_نکلو(0);
		}
		إذا (نتیجہ.گنتی("S")) {
			passes_commands.ڈالو_پٹھ("synth");
			لگاتار_shell = خطأ;
		}
		إذا (نتیجہ.گنتی("C")) لگاتار_tcl_shell = سچ;
		إذا (نتیجہ.گنتی("g")) logger().force_debug_on();
		إذا (نتیجہ.گنتی("m")) plugin_filenames = نتیجہ["m"].as<std::سمتیہ<std::string>>();
		إذا (نتیجہ.گنتی("f")) frontend_command = نتیجہ["f"].as<std::string>();
		إذا (نتیجہ.گنتی("H")) {
			passes_commands.ڈالو_پٹھ("help");
			لگاتار_shell = خطأ;
		}
		إذا (نتیجہ.گنتی("h")) {
			std::string res = نتیجہ["h"].as<std::string>();
			passes_commands.ڈالو_پٹھ("help " + res);
			لگاتار_shell = خطأ;
		}
		إذا (نتیجہ.گنتی("b")) {
			backend_command = نتیجہ["b"].as<std::string>();
			لگاتار_shell = خطأ;
		}
		إذا (نتیجہ.گنتی("p")) {
			خودکار cmds = نتیجہ["p"].as<std::سمتیہ<std::string>>();
			passes_commands.داخل_کرو(passes_commands.سرا(), cmds.begin(), cmds.سرا());
			لگاتار_shell = خطأ;
		}
		إذا (نتیجہ.گنتی("o")) {
			output_filename = نتیجہ["o"].as<std::string>();
			لگاتار_shell = خطأ;
		}
		لكل (ثابت خودکار& کلید : {"l", "L"}) {
			إذا (نتیجہ.گنتی(کلید)) {
				لكل (ثابت خودکار& filename : نتیجہ[کلید].as<std::سمتیہ<std::string>>()) {
					حاول {
						logger().جوڑو_sink<FileLogSink>(filename, کلید[0] == 'L', خطأ);
					} امسك (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   باہر_نکلو(1);
					}
				}
			}
		}
		إذا (نتیجہ.گنتی("q")) {
			منوال_q = سچ;
			إذا (log_stderr) quiet_warnings = سچ;
			log_stderr = سچ;
		}
		إذا (نتیجہ.گنتی("v")) {
			منوال_v = سچ;
			log_stderr = سچ;
			logger().سیٹ_verbose_level(نتیجہ["v"].as<صحيح>());
		}
		إذا (نتیجہ.گنتی("t")) logger().سیٹ_log_ویلا(سچ);
		إذا (نتیجہ.گنتی("no-color")) no_color = سچ;
		إذا (نتیجہ.گنتی("d")) timing_details = سچ;
		لكل (ثابت خودکار& کلید : {"s", "c"}) {
			إذا (نتیجہ.گنتی(کلید)) {
				scriptfile = نتیجہ[کلید].as<std::string>();
				scriptfile_tcl = std::string(کلید) == "c";
				لگاتار_shell = خطأ;
			}
		}
		إذا (نتیجہ.گنتی("y")) {
			scriptfile = نتیجہ["y"].as<std::string>();
			scriptfile_python = سچ;
			لگاتار_shell = خطأ;
		}
		لكل (ثابت خودکار& کلید : {"W", "w", "e"}) {
			إذا (نتیجہ.گنتی(کلید)) {
				خودکار regexes = نتیجہ[کلید].as<std::سمتیہ<std::string>>();
				لكل (ثابت خودکار& regex : regexes) {
					إذا (std::string(کلید) == "W")
						logger().جوڑو_warn(regex);
					إذا (std::string(کلید) == "w")
						logger().جوڑو_nowarn(regex);
					إذا (std::string(کلید) == "e")
						logger().جوڑو_werror(regex);
				}
			}
		}
		إذا (نتیجہ.گنتی("r")) topmodule = نتیجہ["r"].as<std::string>();
		إذا (نتیجہ.گنتی("D")) vlog_مقرر_کردا = نتیجہ["D"].as<std::سمتیہ<std::string>>();
		إذا (نتیجہ.گنتی("P")) {
			خودکار dump_دلیلاں = نتیجہ["P"].as<std::سمتیہ<std::string>>();
			لكل (ثابت خودکار& arg : dump_دلیلاں) {
				خودکار ٹوکن = ونڈو_ٹوکن(arg, ":");
				إذا (!ٹوکن.خالی_تھاواں() && ٹوکن[0] == "ALL") {
					إذا (ٹوکن.ناپ() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::سطر_انت;
						باہر_نکلو(1);
					}
					logger().سیٹ_hdump_all(سچ);
				} وإلا {
					إذا (!ٹوکن.خالی_تھاواں() && !ٹوکن[0].خالی_تھاواں() && ٹوکن[0].پٹھ() == '.')
						ٹوکن[0].نکالو_پٹھ();
					إذا (ٹوکن.ناپ() == 1)
						ٹوکن.ڈالو_پٹھ("yosys_dump_" + ٹوکن[0] + ".il");
					إذا (ٹوکن.ناپ() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::سطر_انت;
						باہر_نکلو(1);
					}
					logger().جوڑو_hdump(ٹوکن[0],ٹوکن[1]);
				}
			}
		}
		إذا (نتیجہ.گنتی("E")) depsfile = نتیجہ["E"].as<std::string>();
		إذا (نتیجہ.گنتی("x")) {
			لكل (ثابت خودکار &ignore : نتیجہ["x"].as<std::سمتیہ<std::string>>())
    			logger().جوڑو_experimental_ignore(ignore);
		}
		إذا (نتیجہ.گنتی("perffile")) perffile = نتیجہ["perffile"].as<std::string>();
		إذا (نتیجہ.گنتی("infile")) {
			frontend_files = نتیجہ["infile"].as<std::سمتیہ<std::string>>();
		}
		إذا (نتیجہ.گنتی("autoidx")) {
			صحيح idx = نتیجہ["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		إذا (!is_colored(stdout) || !is_colored(stderr)) {
			إذا (!log_stderr) {
				logger().جوڑو_sink<ConsoleLogSink>();
			} وإلا {
				logger().جوڑو_sink<StderrLogSink>(quiet_warnings);
			}
		} وإلا
			logger().جوڑو_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		إذا (print_banner)
			yosys_banner();

	}
	امسك (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::سطر_انت;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::سطر_انت;
		باہر_نکلو(1);
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
	#جے_انت

	إذا (!state_dir.خالی_تھاواں()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_وتھ = where_history();
	}
#جے_انت

	إذا (print_stats)
		logger().شروع_hasher();

#إذا defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_رستہ);
#جے_انت

#إذا defined(__linux__)
	// set stack size to >= 128 MB
	{
		بنية rlimit rl;
		ثابت rlim_t ڈھیر_ناپ = 128L * 1024L * 1024L;
		إذا (getrlimit(RLIMIT_ڈھیر, &rl) == 0 && rl.rlim_cur < ڈھیر_ناپ) {
			rl.rlim_cur = ڈھیر_ناپ;
			setrlimit(RLIMIT_ڈھیر, &rl);
		}
	}
#جے_انت

	yosys_تیاری();
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#جے_انت
	log_error_atexit = yosys_atexit;

	لكل (خودکار &fn : plugin_filenames)
		لدو_plugin(fn, {});

	log_suppressed();

	إذا (!vlog_مقرر_کردا.خالی_تھاواں()) {
		std::string vdef_cmd = "read -define";
		لكل (خودکار vdef : vlog_مقرر_کردا)
			vdef_cmd += " " + vdef;
		لگاتار_pass(vdef_cmd);
	}

	إذا (scriptfile.خالی_تھاواں() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		لكل (خودکار special_arg : special_دلیلاں)
			frontend_files.ڈالو_پٹھ(special_arg);
	}

	لكل (خودکار it = frontend_files.begin(); it != frontend_files.سرا(); ++it) {
		إذا (لگاتار_frontend((*it).c_str(), frontend_command))
			لگاتار_shell = خطأ;
	}

	إذا (!topmodule.خالی_تھاواں())
		لگاتار_pass("hierarchy -top " + topmodule);
	إذا (!scriptfile.خالی_تھاواں()) {
		إذا (scriptfile_tcl) {
#جے_تعریف_شدہ YOSYS_ENABLE_TCL
			صحيح tcl_argc = special_دلیلاں.ناپ();
			std::سمتیہ<Tcl_Obj*> script_دلیلاں;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			لكل (خودکار arg : special_دلیلاں)
				script_دلیلاں.ڈالو_پٹھ(Tcl_NewStringObj(arg.c_str(), arg.لمائی()));

			Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(tcl_argc, script_دلیلاں.انکڑے()), 0);
			Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.لمائی()), 0);

			إذا (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتیجہ(yosys_get_tcl_interp()));
#وإلا
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#جے_انت
		} وإلا إذا (scriptfile_python) {
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحيح py_argc = special_دلیلاں.ناپ() + 1;
			new_argv.append(scriptfile);
			لكل (صحيح i = 1; i < py_argc; ++i)
				new_argv.append(special_دلیلاں[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object رستہ = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_رستہ = رستہ(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_رستہ));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			إذا (scriptfp == خالی_سوچک) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			إذا (Py_لگاتار_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#وإلا
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#جے_انت
		} وإلا
			لگاتار_frontend(scriptfile, "script");
	}

	لكل (خودکار it = passes_commands.begin(); it != passes_commands.سرا(); it++)
		لگاتار_pass(*it);

	إذا (لگاتار_tcl_shell) {
#جے_تعریف_شدہ YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وإلا
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#جے_انت
	} وإلا {
		إذا (لگاتار_shell)
			shell(yosys_design);
		وإلا
			لگاتار_backend(output_filename, backend_command);
	}

	yosys_design->check();
	لكل (خودکار it : saved_designs)
		it.دوجا->check();
	لكل (خودکار it : pushed_designs)
		it->check();

	إذا (!depsfile.خالی_تھاواں())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		إذا (f == خالی_سوچک)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقي پہلا = سچ;
		لكل (خودکار fn : yosys_output_files) {
			fprintf(f, "%s%s", پہلا ? "" : " ", escape_filename_spaces(fn).c_str());
			پہلا = خطأ;
		}
		fprintf(f, ":");
		لكل (خودکار fn : yosys_داخلہ_files) {
			إذا (yosys_output_files.گنتی(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	إذا (print_stats)
	{
		std::string hash = logger().انت_hasher();
		logger().سیٹ_log_ویلا(خطأ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = منوال_v && !منوال_q;
		إذا (logger().get_warnings_کل())
			لوگارتھم("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_کل());

		إذا (Get_ناپ(logger().get_experimental()) != 0)
			لوگارتھم("Warnings: %d experimental features used (not excluded with -x).\n", Get_ناپ(logger().get_experimental()));

		إذا (Get_ناپ(logger().get_deprecated()) != 0) {
			لوگارتھم("Deprecated features used:\n");
			لكل (خودکار &[feature, earliest_removal] : logger().get_deprecated())
				إذا (earliest_removal.خالی_تھاواں())
					لوگارتھم("    %s\n", feature);
				وإلا
					لوگارتھم("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#جے_تعریف_شدہ _WIN32
		لوگارتھم("End of script. Logfile hash: %s\n", hash);
		(فراغ)wall_clock_شروع;
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
#جے_انت
		}
#إذا defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / 1024.0);
#وإلاإذا defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / (1024.0 * 1024.0));
#جے_انت
		مزدوج wall_seconds = std::chrono::duration<مزدوج>(
				std::chrono::steady_clock::now() - wall_clock_شروع).گنتی();

		لوگارتھم("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_بفر.ru_utime.tv_sec + 1e-6 * ru_بفر.ru_utime.tv_usec,
				ru_بفر.ru_stime.tv_sec + 1e-6 * ru_بفر.ru_stime.tv_usec, meminfo.c_str());
#جے_انت
		لوگارتھم("%s\n", yosys_maybe_نسخہ());

		int64_t کل_ns = 0;
		std::سیٹ<tuple<int64_t, صحيح, std::string>> timedat;

		لكل (خودکار &it : pass_رجسٹر)
			إذا (it.دوجا->call_counter) {
				خودکار pass_ns = it.دوجا->runtime_ns + 1;
				کل_ns += pass_ns;
				timedat.داخل_کرو(make_tuple(pass_ns, it.دوجا->call_counter, it.پہلا));
			}
		إذا (twine_gc_گنتی) {
			کل_ns += twine_gc_ns;
			timedat.داخل_کرو(make_tuple(twine_gc_ns, twine_gc_گنتی, "twine_gc"));
		}

		إذا (timing_details)
		{
			لوگارتھم("Time spent:\n");
			لكل (خودکار it = timedat.rbegin(); it != timedat.rend(); it++) {
				لوگارتھم("%5d%% %5d calls %8.3f sec %s\n", صحيح(100*std::get<0>(*it) / کل_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وإلا
		{
			صحيح باہرلا_گنتی = 0;
			std::string سنیہا = "Time spent:";
			لكل (خودکار it = timedat.rbegin(); it != timedat.rend() && باہرلا_گنتی < 4; it++, باہرلا_گنتی++) {
				إذا (باہرلا_گنتی >= 2 && (std::get<0>(*it) < 1000000000 || صحيح(100*std::get<0>(*it) / کل_ns) < 20)) {
					سنیہا += ", ...";
					اكسر;
				}
				سنیہا += stringf("%s %d%% %dx %s (%d sec)", باہرلا_گنتی ? "," : "", صحيح(100*std::get<0>(*it) / کل_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحيح(std::get<0>(*it) / 1000000000));
			}
			سنیہا += باہرلا_گنتی ? "" : " no commands executed";
			لوگارتھم("%s\n", سنیہا);
		}
		إذا(!perffile.خالی_تھاواں())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			إذا (f == خالی_سوچک)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_نسخہ());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", کل_ns);
			fprintf(f, "  \"passes\": {");

			منطقي پہلا = سچ;
			لكل (خودکار it = timedat.rbegin(); it != timedat.rend(); it++) {
				إذا (!پہلا)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				پہلا = خطأ;
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
#جے_انت
	أرجع 0;
}

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
#تعریف_کن CXXOPTS_وکتور_DELIMITER '\0'
#ضمن "libs/cxxopts/include/cxxopts.hpp"
#ضمن <iostream>
#ضمن <chrono>

#اگر_تعریف_شده YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_TCL
#  include <tcl.h>
#پایان_اگر

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#پایان_اگر

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <limits.h>
#ضمن <errno.h>
#ضمن <fcntl.h>
#اگر_تعریف_نشده __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#پایان_اگر
#ضمن <inttypes.h>

#إذا defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#پایان_اگر

#اگر_تعریف_شده __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#پایان_اگر

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#پایان_اگر

#ضمن <fmt/color.h>
#ضمن <fmt/format.h>

USING_YOSYS_NAMESPACE

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحيح yosys_history_جابه_جایی = 0;
std::string yosys_history_file;
#پایان_اگر

فراغ yosys_atexit()
{
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	إذا (!yosys_history_file.خالی()) {
#إذا defined(YOSYS_ENABLE_READLINE)
		إذا (yosys_history_جابه_جایی > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_جابه_جایی, yosys_history_file.c_str());
		} وإلا
			write_history(yosys_history_file.c_str());
#وإلا
		write_history(yosys_history_file.c_str());
#پایان_اگر
	}

	clear_history();
#إذا defined(YOSYS_ENABLE_READLINE)
	HIST_مدخل **hist_list = history_list();
	إذا (hist_list != عدم)
		حرر(hist_list);
#پایان_اگر
#پایان_اگر
}

#إذا defined(__OpenBSD__)
نطاق Yosys {
خارجي حرف *yosys_argv0;
خارجي حرف yosys_مسیر[مسیر_MAX];
};
#پایان_اگر
#اگر_تعریف_شده YOSYS_ENABLE_TCL
نطاق Yosys {
	خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);
	خارجي فراغ yosys_tcl_activate_repl();
};
#پایان_اگر

نطاق Yosys {
	خارجي منطقي log_stderr_sink_forced;
};

صنف ColorConsoleLogSink : عام LogSink
{
عام:
	صریح ColorConsoleLogSink(منطقي err, منطقي quiet) : error_output(err), quiet_warnings(quiet) {}
	فراغ لگاریتم(ثابت Log_پیام &پیام) override;
	منطقي should_log(ثابت Log_پیام &پیام) ثابت override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع error_output ? اشاره_تهی : stdout; }
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
	DWORD نما;
	أرجع handle != INVALID_HANDLE_مقدار &&
			Get_Console_نما(handle, &نما);
#وإلا
	أرجع isatty(fileno(file));
#پایان_اگر
}

فراغ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي بعدی_print_log = درست;

فراغ ColorConsoleLogSink::لگاریتم(ثابت Log_پیام &پیام)
{
	FILE *f = (error_output || پیام.severity == LogSeverity::Error) ? stderr : stdout;

	إذا (logger().get_log_زمان() && بعدی_print_log) {
		خودکار elapsed = پیام.timestamp - logger().get_initial_زمان();
		خودکار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string زمان_str = stringf("[%05d.%06d] ", صحيح(us.شمار() / 1'000'000),صحيح(us.شمار() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", زمان_str);
		بعدی_print_log = خطأ;
	}

	إذا (!پیام.src.filename.خالی())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", پیام.src.filename, پیام.src.شروع_سطر);

	بدل (پیام.severity) {
		حالة LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", پیام.پیشوند_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیام.پیام_2);
			اكسر;

		حالة LogSeverity::Error:
		حالة LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", پیام.پیشوند_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیام.پیام_2);
			اكسر;

		حالة LogSeverity::سرآیند:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", پیام.پیشوند_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", پیام.پیام_2);
			اكسر;

		حالة LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", پیام.پیشوند_2, پیام.پیام_2);
			اكسر;

		حالة LogSeverity::Comment:
		حالة LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", پیام.پیشوند_2, پیام.پیام_2);
			اكسر;

		حالة LogSeverity::Info:
		افتراضي:
			إذا (!پیام.پیشوند_2.خالی())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", پیام.پیشوند_2);
			fmt::print(f, "{}", پیام.پیام_2);
			اكسر;
	}
	إذا (!پیام.پیام_2.خالی())
		بعدی_print_log = پیام.پیام_2.پشت() == '\n';
}

منطقي ColorConsoleLogSink::should_log(ثابت Log_پیام &پیام) ثابت
{
	أرجع (!error_output) || (پیام.severity == LogSeverity::Error || پیام.severity == LogSeverity::NonFatalError ||
			(پیام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#اگر_تعریف_شده _WIN32
صحيح wmain(صحيح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#اگر_تعریف_شده _UCRT
	setlocale(LC_ALL, ".UTF-8");
#پایان_اگر
	// Configure the Win32 console to use UTF-8.
	مجموعه_Console_CP(CP_UTF8);
	مجموعه_Console_Output_CP(CP_UTF8);

	std::وکتور<حرف*> uargs;
	لكل (صحيح i = 0; i < argc; i++) {
		صحيح usize = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wargv[i], -1, عدم, 0, عدم, عدم);
		حرف *uarg = (حرف *)حافظه_بده(usize);
		Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, عدم, عدم);
		uargs.افزودن_پشت(uarg);
	}
	uargs.افزودن_پشت(عدم);
	حرف **argv = uargs.داده();
#وإلا
صحيح اصلی(صحيح argc, حرف **argv)
{
#پایان_اگر
	خودکار wall_clock_شروع = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::وکتور<std::string> vlog_تعریف_میکند;
	std::وکتور<std::string> passes_commands;
	std::وکتور<std::string> frontend_files;
	std::وکتور<std::string> plugin_filenames;
	std::وکتور<std::string> special_آرگومانها;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	منطقي scriptfile_tcl = خطأ;
	منطقي scriptfile_python = خطأ;
	منطقي print_banner = درست;
	منطقي print_stats = درست;
	منطقي call_abort = خطأ;
	منطقي timing_details = خطأ;
	منطقي پیاپی_shell = درست;
	منطقي پیاپی_tcl_shell = خطأ;
	منطقي نما_v = خطأ;
	منطقي نما_q = خطأ;
	منطقي quiet_warnings = خطأ;
	منطقي log_stderr = خطأ;
	no_color = std::getenv("NO_COLOR") != اشاره_تهی;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.مجموعه_عرض(SIZE_MAX);

	options.جمع_کن_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::مقدار<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::مقدار<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::مقدار<std::string>(), "<scriptfile>")
#اگر_تعریف_شده YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::مقدار<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#پایان_اگر // YOSYS_ENABLE_TCL
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::مقدار<std::string>(), "<script>")
#پایان_اگر // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::مقدار<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<define>[=<value>]")
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
		("infile", "input files", cxxopts::مقدار<std::وکتور<std::string>>())
	;
	options.جمع_کن_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::مقدار<صحيح>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::مقدار<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::مقدار<std::وکتور<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::مقدار<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.جمع_کن_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::مقدار<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::مقدار<std::وکتور<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::مقدار<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	إذا (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::خروجی << options.help() << std::پایان_سطر;
		خروج(0);
	}
	حاول {
		// Check for "--" in arguments
		خودکار it = std::پیدا_کن(argv, argv + argc, std::string("--"));
		إذا (it != argv + argc) {
			special_آرگومانها.تخصیص_بده(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::فاصله(argv, it);
		}

		خودکار نتیجه = options.parse(argc, argv);

		إذا (نتیجه.شمار("M")) memhasher_on();
		إذا (نتیجه.شمار("X")) yosys_xtrace += نتیجه.شمار("X");
		إذا (نتیجه.شمار("A")) call_abort = درست;
		إذا (نتیجه.شمار("Q")) print_banner = خطأ;
		إذا (نتیجه.شمار("T")) print_stats = خطأ;
		إذا (نتیجه.شمار("no-version")) yosys_write_versions = خطأ;
		إذا (نتیجه.شمار("V")) {
			std::خروجی << yosys_نسخه_str << std::پایان_سطر;
			خروج(0);
		}
		إذا (نتیجه.شمار("git-hash")) {
			std::خروجی << yosys_git_hash_str << std::پایان_سطر;
			خروج(0);
		}
		إذا (نتیجه.شمار("S")) {
			passes_commands.افزودن_پشت("synth");
			پیاپی_shell = خطأ;
		}
		إذا (نتیجه.شمار("C")) پیاپی_tcl_shell = درست;
		إذا (نتیجه.شمار("g")) logger().force_debug_on();
		إذا (نتیجه.شمار("m")) plugin_filenames = نتیجه["m"].as<std::وکتور<std::string>>();
		إذا (نتیجه.شمار("f")) frontend_command = نتیجه["f"].as<std::string>();
		إذا (نتیجه.شمار("H")) {
			passes_commands.افزودن_پشت("help");
			پیاپی_shell = خطأ;
		}
		إذا (نتیجه.شمار("h")) {
			std::string res = نتیجه["h"].as<std::string>();
			passes_commands.افزودن_پشت("help " + res);
			پیاپی_shell = خطأ;
		}
		إذا (نتیجه.شمار("b")) {
			backend_command = نتیجه["b"].as<std::string>();
			پیاپی_shell = خطأ;
		}
		إذا (نتیجه.شمار("p")) {
			خودکار cmds = نتیجه["p"].as<std::وکتور<std::string>>();
			passes_commands.درج(passes_commands.انتها(), cmds.begin(), cmds.انتها());
			پیاپی_shell = خطأ;
		}
		إذا (نتیجه.شمار("o")) {
			output_filename = نتیجه["o"].as<std::string>();
			پیاپی_shell = خطأ;
		}
		لكل (ثابت خودکار& کلید : {"l", "L"}) {
			إذا (نتیجه.شمار(کلید)) {
				لكل (ثابت خودکار& filename : نتیجه[کلید].as<std::وکتور<std::string>>()) {
					حاول {
						logger().جمع_کن_sink<FileLogSink>(filename, کلید[0] == 'L', خطأ);
					} امسك (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   خروج(1);
					}
				}
			}
		}
		إذا (نتیجه.شمار("q")) {
			نما_q = درست;
			إذا (log_stderr) quiet_warnings = درست;
			log_stderr = درست;
		}
		إذا (نتیجه.شمار("v")) {
			نما_v = درست;
			log_stderr = درست;
			logger().مجموعه_verbose_level(نتیجه["v"].as<صحيح>());
		}
		إذا (نتیجه.شمار("t")) logger().مجموعه_log_زمان(درست);
		إذا (نتیجه.شمار("no-color")) no_color = درست;
		إذا (نتیجه.شمار("d")) timing_details = درست;
		لكل (ثابت خودکار& کلید : {"s", "c"}) {
			إذا (نتیجه.شمار(کلید)) {
				scriptfile = نتیجه[کلید].as<std::string>();
				scriptfile_tcl = std::string(کلید) == "c";
				پیاپی_shell = خطأ;
			}
		}
		إذا (نتیجه.شمار("y")) {
			scriptfile = نتیجه["y"].as<std::string>();
			scriptfile_python = درست;
			پیاپی_shell = خطأ;
		}
		لكل (ثابت خودکار& کلید : {"W", "w", "e"}) {
			إذا (نتیجه.شمار(کلید)) {
				خودکار regexes = نتیجه[کلید].as<std::وکتور<std::string>>();
				لكل (ثابت خودکار& regex : regexes) {
					إذا (std::string(کلید) == "W")
						logger().جمع_کن_warn(regex);
					إذا (std::string(کلید) == "w")
						logger().جمع_کن_nowarn(regex);
					إذا (std::string(کلید) == "e")
						logger().جمع_کن_werror(regex);
				}
			}
		}
		إذا (نتیجه.شمار("r")) topmodule = نتیجه["r"].as<std::string>();
		إذا (نتیجه.شمار("D")) vlog_تعریف_میکند = نتیجه["D"].as<std::وکتور<std::string>>();
		إذا (نتیجه.شمار("P")) {
			خودکار dump_آرگومانها = نتیجه["P"].as<std::وکتور<std::string>>();
			لكل (ثابت خودکار& arg : dump_آرگومانها) {
				خودکار نشانهها = تقسیم_نشانهها(arg, ":");
				إذا (!نشانهها.خالی() && نشانهها[0] == "ALL") {
					إذا (نشانهها.اندازه() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::پایان_سطر;
						خروج(1);
					}
					logger().مجموعه_hdump_all(درست);
				} وإلا {
					إذا (!نشانهها.خالی() && !نشانهها[0].خالی() && نشانهها[0].پشت() == '.')
						نشانهها[0].برداشت_پشت();
					إذا (نشانهها.اندازه() == 1)
						نشانهها.افزودن_پشت("yosys_dump_" + نشانهها[0] + ".il");
					إذا (نشانهها.اندازه() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::پایان_سطر;
						خروج(1);
					}
					logger().جمع_کن_hdump(نشانهها[0],نشانهها[1]);
				}
			}
		}
		إذا (نتیجه.شمار("E")) depsfile = نتیجه["E"].as<std::string>();
		إذا (نتیجه.شمار("x")) {
			لكل (ثابت خودکار &ignore : نتیجه["x"].as<std::وکتور<std::string>>())
    			logger().جمع_کن_experimental_ignore(ignore);
		}
		إذا (نتیجه.شمار("perffile")) perffile = نتیجه["perffile"].as<std::string>();
		إذا (نتیجه.شمار("infile")) {
			frontend_files = نتیجه["infile"].as<std::وکتور<std::string>>();
		}
		إذا (نتیجه.شمار("autoidx")) {
			صحيح idx = نتیجه["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		إذا (!is_colored(stdout) || !is_colored(stderr)) {
			إذا (!log_stderr) {
				logger().جمع_کن_sink<ConsoleLogSink>();
			} وإلا {
				logger().جمع_کن_sink<StderrLogSink>(quiet_warnings);
			}
		} وإلا
			logger().جمع_کن_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		إذا (print_banner)
			yosys_banner();

	}
	امسك (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::پایان_سطر;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::پایان_سطر;
		خروج(1);
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
	#پایان_اگر

	إذا (!state_dir.خالی()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_جابه_جایی = where_history();
	}
#پایان_اگر

	إذا (print_stats)
		logger().شروع_hasher();

#إذا defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_مسیر);
#پایان_اگر

#إذا defined(__linux__)
	// set stack size to >= 128 MB
	{
		بنية rlimit rl;
		ثابت rlim_t پشته_اندازه = 128L * 1024L * 1024L;
		إذا (getrlimit(RLIMIT_پشته, &rl) == 0 && rl.rlim_cur < پشته_اندازه) {
			rl.rlim_cur = پشته_اندازه;
			setrlimit(RLIMIT_پشته, &rl);
		}
	}
#پایان_اگر

	yosys_راهاندازی();
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#پایان_اگر
	log_error_atexit = yosys_atexit;

	لكل (خودکار &fn : plugin_filenames)
		بارگیری_plugin(fn, {});

	log_suppressed();

	إذا (!vlog_تعریف_میکند.خالی()) {
		std::string vdef_cmd = "read -define";
		لكل (خودکار vdef : vlog_تعریف_میکند)
			vdef_cmd += " " + vdef;
		پیاپی_pass(vdef_cmd);
	}

	إذا (scriptfile.خالی() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		لكل (خودکار special_arg : special_آرگومانها)
			frontend_files.افزودن_پشت(special_arg);
	}

	لكل (خودکار it = frontend_files.begin(); it != frontend_files.انتها(); ++it) {
		إذا (پیاپی_frontend((*it).c_str(), frontend_command))
			پیاپی_shell = خطأ;
	}

	إذا (!topmodule.خالی())
		پیاپی_pass("hierarchy -top " + topmodule);
	إذا (!scriptfile.خالی()) {
		إذا (scriptfile_tcl) {
#اگر_تعریف_شده YOSYS_ENABLE_TCL
			صحيح tcl_argc = special_آرگومانها.اندازه();
			std::وکتور<Tcl_Obj*> script_آرگومانها;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			لكل (خودکار arg : special_آرگومانها)
				script_آرگومانها.افزودن_پشت(Tcl_NewStringObj(arg.c_str(), arg.درازی()));

			Tcl_Obj_مجموعه_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_مجموعه_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(tcl_argc, script_آرگومانها.داده()), 0);
			Tcl_Obj_مجموعه_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.درازی()), 0);

			إذا (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتیجه(yosys_get_tcl_interp()));
#وإلا
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#پایان_اگر
		} وإلا إذا (scriptfile_python) {
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحيح py_argc = special_آرگومانها.اندازه() + 1;
			new_argv.append(scriptfile);
			لكل (صحيح i = 1; i < py_argc; ++i)
				new_argv.append(special_آرگومانها[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object مسیر = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_مسیر = مسیر(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_مسیر));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			إذا (scriptfp == اشاره_تهی) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			إذا (Py_پیاپی_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#وإلا
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#پایان_اگر
		} وإلا
			پیاپی_frontend(scriptfile, "script");
	}

	لكل (خودکار it = passes_commands.begin(); it != passes_commands.انتها(); it++)
		پیاپی_pass(*it);

	إذا (پیاپی_tcl_shell) {
#اگر_تعریف_شده YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وإلا
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#پایان_اگر
	} وإلا {
		إذا (پیاپی_shell)
			shell(yosys_design);
		وإلا
			پیاپی_backend(output_filename, backend_command);
	}

	yosys_design->check();
	لكل (خودکار it : saved_designs)
		it.دومی->check();
	لكل (خودکار it : pushed_designs)
		it->check();

	إذا (!depsfile.خالی())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		إذا (f == اشاره_تهی)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقي اولی = درست;
		لكل (خودکار fn : yosys_output_files) {
			fprintf(f, "%s%s", اولی ? "" : " ", escape_filename_spaces(fn).c_str());
			اولی = خطأ;
		}
		fprintf(f, ":");
		لكل (خودکار fn : yosys_درون_داد_files) {
			إذا (yosys_output_files.شمار(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	إذا (print_stats)
	{
		std::string hash = logger().پایان_hasher();
		logger().مجموعه_log_زمان(خطأ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = نما_v && !نما_q;
		إذا (logger().get_warnings_مجموع())
			لگاریتم("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_مجموع());

		إذا (Get_اندازه(logger().get_experimental()) != 0)
			لگاریتم("Warnings: %d experimental features used (not excluded with -x).\n", Get_اندازه(logger().get_experimental()));

		إذا (Get_اندازه(logger().get_deprecated()) != 0) {
			لگاریتم("Deprecated features used:\n");
			لكل (خودکار &[feature, earliest_removal] : logger().get_deprecated())
				إذا (earliest_removal.خالی())
					لگاریتم("    %s\n", feature);
				وإلا
					لگاریتم("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#اگر_تعریف_شده _WIN32
		لگاریتم("End of script. Logfile hash: %s\n", hash);
		(فراغ)wall_clock_شروع;
#وإلا
		std::string meminfo;
		std::string stats_divider = ", ";

		بنية rusage ru_میانگیر;
		getrusage(RUSAGE_SELF, &ru_میانگیر);
		إذا (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			بنية rusage ru_میانگیر_children;
			getrusage(RUSAGE_CHILDREN, &ru_میانگیر_children);
			ru_میانگیر.ru_utime.tv_sec += ru_میانگیر_children.ru_utime.tv_sec;
			ru_میانگیر.ru_utime.tv_usec += ru_میانگیر_children.ru_utime.tv_usec;
			ru_میانگیر.ru_stime.tv_sec += ru_میانگیر_children.ru_stime.tv_sec;
			ru_میانگیر.ru_stime.tv_usec += ru_میانگیر_children.ru_stime.tv_usec;
#إذا defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_میانگیر.ru_maxrss = std::max(ru_میانگیر.ru_maxrss, ru_میانگیر_children.ru_maxrss);
#پایان_اگر
		}
#إذا defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_میانگیر.ru_maxrss / 1024.0);
#وإلاإذا defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_میانگیر.ru_maxrss / (1024.0 * 1024.0));
#پایان_اگر
		مزدوج wall_seconds = std::chrono::duration<مزدوج>(
				std::chrono::steady_clock::now() - wall_clock_شروع).شمار();

		لگاریتم("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_میانگیر.ru_utime.tv_sec + 1e-6 * ru_میانگیر.ru_utime.tv_usec,
				ru_میانگیر.ru_stime.tv_sec + 1e-6 * ru_میانگیر.ru_stime.tv_usec, meminfo.c_str());
#پایان_اگر
		لگاریتم("%s\n", yosys_maybe_نسخه());

		int64_t مجموع_ns = 0;
		std::مجموعه<tuple<int64_t, صحيح, std::string>> timedat;

		لكل (خودکار &it : pass_ثبات)
			إذا (it.دومی->call_counter) {
				خودکار pass_ns = it.دومی->runtime_ns + 1;
				مجموع_ns += pass_ns;
				timedat.درج(make_tuple(pass_ns, it.دومی->call_counter, it.اولی));
			}
		إذا (twine_gc_شمار) {
			مجموع_ns += twine_gc_ns;
			timedat.درج(make_tuple(twine_gc_ns, twine_gc_شمار, "twine_gc"));
		}

		إذا (timing_details)
		{
			لگاریتم("Time spent:\n");
			لكل (خودکار it = timedat.rbegin(); it != timedat.rend(); it++) {
				لگاریتم("%5d%% %5d calls %8.3f sec %s\n", صحيح(100*std::get<0>(*it) / مجموع_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وإلا
		{
			صحيح بیرون_شمار = 0;
			std::string پیام_2 = "Time spent:";
			لكل (خودکار it = timedat.rbegin(); it != timedat.rend() && بیرون_شمار < 4; it++, بیرون_شمار++) {
				إذا (بیرون_شمار >= 2 && (std::get<0>(*it) < 1000000000 || صحيح(100*std::get<0>(*it) / مجموع_ns) < 20)) {
					پیام_2 += ", ...";
					اكسر;
				}
				پیام_2 += stringf("%s %d%% %dx %s (%d sec)", بیرون_شمار ? "," : "", صحيح(100*std::get<0>(*it) / مجموع_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحيح(std::get<0>(*it) / 1000000000));
			}
			پیام_2 += بیرون_شمار ? "" : " no commands executed";
			لگاریتم("%s\n", پیام_2);
		}
		إذا(!perffile.خالی())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			إذا (f == اشاره_تهی)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_نسخه());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", مجموع_ns);
			fprintf(f, "  \"passes\": {");

			منطقي اولی = درست;
			لكل (خودکار it = timedat.rbegin(); it != timedat.rend(); it++) {
				إذا (!اولی)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				اولی = خطأ;
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
#پایان_اگر
	أرجع 0;
}

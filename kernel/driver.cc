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
#تعریف_کرو CXXOPTS_سمتیہ_DELIMITER '\0'
#شامل "libs/cxxopts/include/cxxopts.hpp"
#شامل <iostream>
#شامل <chrono>

#اگر_تعریف_شدہ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
#  include <tcl.h>
#اگر_ختم

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نامخلا py = pybind11;
#اگر_ختم

#شامل <stdio.h>
#شامل <string.h>
#شامل <limits.h>
#شامل <errno.h>
#شامل <fcntl.h>
#اگر_غیر_تعریف_شدہ __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#اگر_ختم
#شامل <inttypes.h>

#اگر defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#اگر_ختم

#اگر_تعریف_شدہ __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#اگر_ختم

#اگر !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#اگر_ختم

#شامل <fmt/color.h>
#شامل <fmt/format.h>

USING_YOSYS_NAMESPACE

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحیح yosys_history_انحراف = 0;
std::string yosys_history_file;
#اگر_ختم

خالی yosys_atexit()
{
#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	اگر (!yosys_history_file.خالی_خانے()) {
#اگر defined(YOSYS_ENABLE_READLINE)
		اگر (yosys_history_انحراف > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_انحراف, yosys_history_file.c_str());
		} ورنہ
			write_history(yosys_history_file.c_str());
#ورنہ
		write_history(yosys_history_file.c_str());
#اگر_ختم
	}

	clear_history();
#اگر defined(YOSYS_ENABLE_READLINE)
	HIST_اندراج **hist_list = history_list();
	اگر (hist_list != تہی)
		آزاد(hist_list);
#اگر_ختم
#اگر_ختم
}

#اگر defined(__OpenBSD__)
نامخلا Yosys {
خارجی حرف *yosys_argv0;
خارجی حرف yosys_راستہ[راستہ_MAX];
};
#اگر_ختم
#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
نامخلا Yosys {
	خارجی صحیح yosys_tcl_interp_init(Tcl_Interp *interp);
	خارجی خالی yosys_tcl_activate_repl();
};
#اگر_ختم

نامخلا Yosys {
	خارجی منطقی log_stderr_sink_forced;
};

جماعت ColorConsoleLogSink : عوامی LogSink
{
عوامی:
	صریح ColorConsoleLogSink(منطقی err, منطقی quiet) : error_output(err), quiet_warnings(quiet) {}
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override;
	منطقی should_log(ثابت Log_پیغام &پیغام) ثابت override;
	خالی flush() override;
	FILE *file_handle() override { واپس error_output ? خالی_اشاریہ : stdout; }
نجی:
	منطقی error_output;
	منطقی quiet_warnings;
};

منطقی no_color = جھوٹ;

منطقی is_colored(FILE *file)
{
	اگر (no_color)
		واپس جھوٹ;

#اگر defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD منوال;
	واپس handle != INVALID_HANDLE_قدر &&
			Get_Console_منوال(handle, &منوال);
#ورنہ
	واپس isatty(fileno(file));
#اگر_ختم
}

خالی ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقی اگلا_print_log = سچ;

خالی ColorConsoleLogSink::لوگارتھم(ثابت Log_پیغام &پیغام)
{
	FILE *f = (error_output || پیغام.severity == LogSeverity::Error) ? stderr : stdout;

	اگر (logger().get_log_وقت() && اگلا_print_log) {
		خودکار elapsed = پیغام.timestamp - logger().get_initial_وقت();
		خودکار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string وقت_str = stringf("[%05d.%06d] ", صحیح(us.گنتی() / 1'000'000),صحیح(us.گنتی() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", وقت_str);
		اگلا_print_log = جھوٹ;
	}

	اگر (!پیغام.src.filename.خالی_خانے())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", پیغام.src.filename, پیغام.src.آغاز_سطر);

	انتخاب (پیغام.severity) {
		حالت LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", پیغام.سابقہ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیغام.پیغام_2);
			توڑو;

		حالت LogSeverity::Error:
		حالت LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", پیغام.سابقہ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", پیغام.پیغام_2);
			توڑو;

		حالت LogSeverity::سرخی:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", پیغام.سابقہ_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", پیغام.پیغام_2);
			توڑو;

		حالت LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", پیغام.سابقہ_2, پیغام.پیغام_2);
			توڑو;

		حالت LogSeverity::Comment:
		حالت LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", پیغام.سابقہ_2, پیغام.پیغام_2);
			توڑو;

		حالت LogSeverity::Info:
		طےشدہ:
			اگر (!پیغام.سابقہ_2.خالی_خانے())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", پیغام.سابقہ_2);
			fmt::print(f, "{}", پیغام.پیغام_2);
			توڑو;
	}
	اگر (!پیغام.پیغام_2.خالی_خانے())
		اگلا_print_log = پیغام.پیغام_2.عقب() == '\n';
}

منطقی ColorConsoleLogSink::should_log(ثابت Log_پیغام &پیغام) ثابت
{
	واپس (!error_output) || (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#اگر_تعریف_شدہ _WIN32
صحیح wmain(صحیح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#اگر_تعریف_شدہ _UCRT
	setlocale(LC_ALL, ".UTF-8");
#اگر_ختم
	// Configure the Win32 console to use UTF-8.
	سیٹ_Console_CP(CP_UTF8);
	سیٹ_Console_Output_CP(CP_UTF8);

	std::سمتیہ<حرف*> uargs;
	برائے (صحیح i = 0; i < argc; i++) {
		صحیح usize = Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wargv[i], -1, تہی, 0, تہی, تہی);
		حرف *uarg = (حرف *)حافظہ_دو(usize);
		Wide_Char_مقصد_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, تہی, تہی);
		uargs.ڈالو_عقب(uarg);
	}
	uargs.ڈالو_عقب(تہی);
	حرف **argv = uargs.ڈیٹا();
#ورنہ
صحیح مین(صحیح argc, حرف **argv)
{
#اگر_ختم
	خودکار wall_clock_آغاز = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::سمتیہ<std::string> vlog_متعین_کرتا;
	std::سمتیہ<std::string> passes_commands;
	std::سمتیہ<std::string> frontend_files;
	std::سمتیہ<std::string> plugin_filenames;
	std::سمتیہ<std::string> special_دلائل;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	منطقی scriptfile_tcl = جھوٹ;
	منطقی scriptfile_python = جھوٹ;
	منطقی print_banner = سچ;
	منطقی print_stats = سچ;
	منطقی call_abort = جھوٹ;
	منطقی timing_details = جھوٹ;
	منطقی تسلسل_shell = سچ;
	منطقی تسلسل_tcl_shell = جھوٹ;
	منطقی منوال_v = جھوٹ;
	منطقی منوال_q = جھوٹ;
	منطقی quiet_warnings = جھوٹ;
	منطقی log_stderr = جھوٹ;
	no_color = std::getenv("NO_COLOR") != خالی_اشاریہ;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.سیٹ_چوڑائی(SIZE_MAX);

	options.جوڑو_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::قدر<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::قدر<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::قدر<std::string>(), "<scriptfile>")
#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::قدر<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#اگر_ختم // YOSYS_ENABLE_TCL
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::قدر<std::string>(), "<script>")
#اگر_ختم // YOSYS_ENABLE_PYTHON
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
			cxxopts::قدر<صحیح>(), "<level>")
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
	اگر (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::خارج << options.help() << std::سطر_ختم;
		باہر_نکلو(0);
	}
	کوشش {
		// Check for "--" in arguments
		خودکار it = std::ڈھونڈو(argv, argv + argc, std::string("--"));
		اگر (it != argv + argc) {
			special_دلائل.تفویض(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::فاصلہ(argv, it);
		}

		خودکار نتیجہ = options.parse(argc, argv);

		اگر (نتیجہ.گنتی("M")) memhasher_on();
		اگر (نتیجہ.گنتی("X")) yosys_xtrace += نتیجہ.گنتی("X");
		اگر (نتیجہ.گنتی("A")) call_abort = سچ;
		اگر (نتیجہ.گنتی("Q")) print_banner = جھوٹ;
		اگر (نتیجہ.گنتی("T")) print_stats = جھوٹ;
		اگر (نتیجہ.گنتی("no-version")) yosys_write_versions = جھوٹ;
		اگر (نتیجہ.گنتی("V")) {
			std::خارج << yosys_نسخہ_str << std::سطر_ختم;
			باہر_نکلو(0);
		}
		اگر (نتیجہ.گنتی("git-hash")) {
			std::خارج << yosys_git_hash_str << std::سطر_ختم;
			باہر_نکلو(0);
		}
		اگر (نتیجہ.گنتی("S")) {
			passes_commands.ڈالو_عقب("synth");
			تسلسل_shell = جھوٹ;
		}
		اگر (نتیجہ.گنتی("C")) تسلسل_tcl_shell = سچ;
		اگر (نتیجہ.گنتی("g")) logger().force_debug_on();
		اگر (نتیجہ.گنتی("m")) plugin_filenames = نتیجہ["m"].as<std::سمتیہ<std::string>>();
		اگر (نتیجہ.گنتی("f")) frontend_command = نتیجہ["f"].as<std::string>();
		اگر (نتیجہ.گنتی("H")) {
			passes_commands.ڈالو_عقب("help");
			تسلسل_shell = جھوٹ;
		}
		اگر (نتیجہ.گنتی("h")) {
			std::string res = نتیجہ["h"].as<std::string>();
			passes_commands.ڈالو_عقب("help " + res);
			تسلسل_shell = جھوٹ;
		}
		اگر (نتیجہ.گنتی("b")) {
			backend_command = نتیجہ["b"].as<std::string>();
			تسلسل_shell = جھوٹ;
		}
		اگر (نتیجہ.گنتی("p")) {
			خودکار cmds = نتیجہ["p"].as<std::سمتیہ<std::string>>();
			passes_commands.داخل_کرو(passes_commands.انتہا(), cmds.begin(), cmds.انتہا());
			تسلسل_shell = جھوٹ;
		}
		اگر (نتیجہ.گنتی("o")) {
			output_filename = نتیجہ["o"].as<std::string>();
			تسلسل_shell = جھوٹ;
		}
		برائے (ثابت خودکار& کلید : {"l", "L"}) {
			اگر (نتیجہ.گنتی(کلید)) {
				برائے (ثابت خودکار& filename : نتیجہ[کلید].as<std::سمتیہ<std::string>>()) {
					کوشش {
						logger().جوڑو_sink<FileLogSink>(filename, کلید[0] == 'L', جھوٹ);
					} پکڑو (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   باہر_نکلو(1);
					}
				}
			}
		}
		اگر (نتیجہ.گنتی("q")) {
			منوال_q = سچ;
			اگر (log_stderr) quiet_warnings = سچ;
			log_stderr = سچ;
		}
		اگر (نتیجہ.گنتی("v")) {
			منوال_v = سچ;
			log_stderr = سچ;
			logger().سیٹ_verbose_level(نتیجہ["v"].as<صحیح>());
		}
		اگر (نتیجہ.گنتی("t")) logger().سیٹ_log_وقت(سچ);
		اگر (نتیجہ.گنتی("no-color")) no_color = سچ;
		اگر (نتیجہ.گنتی("d")) timing_details = سچ;
		برائے (ثابت خودکار& کلید : {"s", "c"}) {
			اگر (نتیجہ.گنتی(کلید)) {
				scriptfile = نتیجہ[کلید].as<std::string>();
				scriptfile_tcl = std::string(کلید) == "c";
				تسلسل_shell = جھوٹ;
			}
		}
		اگر (نتیجہ.گنتی("y")) {
			scriptfile = نتیجہ["y"].as<std::string>();
			scriptfile_python = سچ;
			تسلسل_shell = جھوٹ;
		}
		برائے (ثابت خودکار& کلید : {"W", "w", "e"}) {
			اگر (نتیجہ.گنتی(کلید)) {
				خودکار regexes = نتیجہ[کلید].as<std::سمتیہ<std::string>>();
				برائے (ثابت خودکار& regex : regexes) {
					اگر (std::string(کلید) == "W")
						logger().جوڑو_warn(regex);
					اگر (std::string(کلید) == "w")
						logger().جوڑو_nowarn(regex);
					اگر (std::string(کلید) == "e")
						logger().جوڑو_werror(regex);
				}
			}
		}
		اگر (نتیجہ.گنتی("r")) topmodule = نتیجہ["r"].as<std::string>();
		اگر (نتیجہ.گنتی("D")) vlog_متعین_کرتا = نتیجہ["D"].as<std::سمتیہ<std::string>>();
		اگر (نتیجہ.گنتی("P")) {
			خودکار dump_دلائل = نتیجہ["P"].as<std::سمتیہ<std::string>>();
			برائے (ثابت خودکار& arg : dump_دلائل) {
				خودکار علامات = بانٹو_علامات(arg, ":");
				اگر (!علامات.خالی_خانے() && علامات[0] == "ALL") {
					اگر (علامات.سائز() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::سطر_ختم;
						باہر_نکلو(1);
					}
					logger().سیٹ_hdump_all(سچ);
				} ورنہ {
					اگر (!علامات.خالی_خانے() && !علامات[0].خالی_خانے() && علامات[0].عقب() == '.')
						علامات[0].نکالو_عقب();
					اگر (علامات.سائز() == 1)
						علامات.ڈالو_عقب("yosys_dump_" + علامات[0] + ".il");
					اگر (علامات.سائز() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::سطر_ختم;
						باہر_نکلو(1);
					}
					logger().جوڑو_hdump(علامات[0],علامات[1]);
				}
			}
		}
		اگر (نتیجہ.گنتی("E")) depsfile = نتیجہ["E"].as<std::string>();
		اگر (نتیجہ.گنتی("x")) {
			برائے (ثابت خودکار &ignore : نتیجہ["x"].as<std::سمتیہ<std::string>>())
    			logger().جوڑو_experimental_ignore(ignore);
		}
		اگر (نتیجہ.گنتی("perffile")) perffile = نتیجہ["perffile"].as<std::string>();
		اگر (نتیجہ.گنتی("infile")) {
			frontend_files = نتیجہ["infile"].as<std::سمتیہ<std::string>>();
		}
		اگر (نتیجہ.گنتی("autoidx")) {
			صحیح idx = نتیجہ["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		اگر (!is_colored(stdout) || !is_colored(stderr)) {
			اگر (!log_stderr) {
				logger().جوڑو_sink<ConsoleLogSink>();
			} ورنہ {
				logger().جوڑو_sink<StderrLogSink>(quiet_warnings);
			}
		} ورنہ
			logger().جوڑو_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		اگر (print_banner)
			yosys_banner();

	}
	پکڑو (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::سطر_ختم;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::سطر_ختم;
		باہر_نکلو(1);
	}

#اگر defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#اگر defined(_WIN32)
		اگر (getenv("HOMEDRIVE") != تہی && getenv("HOMEPATH") != تہی) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} ورنہ {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#ورنہ
		اگر (getenv("XDG_STATE_HOME") == تہی || getenv("XDG_STATE_HOME")[0] == '\0') {
			اگر (getenv("HOME") != تہی) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} ورنہ {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} ورنہ {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#اگر_ختم

	اگر (!state_dir.خالی_خانے()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_انحراف = where_history();
	}
#اگر_ختم

	اگر (print_stats)
		logger().آغاز_hasher();

#اگر defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_راستہ);
#اگر_ختم

#اگر defined(__linux__)
	// set stack size to >= 128 MB
	{
		ساخت rlimit rl;
		ثابت rlim_t ڈھیر_سائز = 128L * 1024L * 1024L;
		اگر (getrlimit(RLIMIT_ڈھیر, &rl) == 0 && rl.rlim_cur < ڈھیر_سائز) {
			rl.rlim_cur = ڈھیر_سائز;
			setrlimit(RLIMIT_ڈھیر, &rl);
		}
	}
#اگر_ختم

	yosys_تیاری();
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#اگر_ختم
	log_error_atexit = yosys_atexit;

	برائے (خودکار &fn : plugin_filenames)
		بھرو_plugin(fn, {});

	log_suppressed();

	اگر (!vlog_متعین_کرتا.خالی_خانے()) {
		std::string vdef_cmd = "read -define";
		برائے (خودکار vdef : vlog_متعین_کرتا)
			vdef_cmd += " " + vdef;
		تسلسل_pass(vdef_cmd);
	}

	اگر (scriptfile.خالی_خانے() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		برائے (خودکار special_arg : special_دلائل)
			frontend_files.ڈالو_عقب(special_arg);
	}

	برائے (خودکار it = frontend_files.begin(); it != frontend_files.انتہا(); ++it) {
		اگر (تسلسل_frontend((*it).c_str(), frontend_command))
			تسلسل_shell = جھوٹ;
	}

	اگر (!topmodule.خالی_خانے())
		تسلسل_pass("hierarchy -top " + topmodule);
	اگر (!scriptfile.خالی_خانے()) {
		اگر (scriptfile_tcl) {
#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
			صحیح tcl_argc = special_دلائل.سائز();
			std::سمتیہ<Tcl_Obj*> script_دلائل;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			برائے (خودکار arg : special_دلائل)
				script_دلائل.ڈالو_عقب(Tcl_NewStringObj(arg.c_str(), arg.لمبائی_2()));

			Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argc", 4), تہی, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv", 4), تہی, Tcl_NewListObj(tcl_argc, script_دلائل.ڈیٹا()), 0);
			Tcl_Obj_سیٹ_Var2(interp, Tcl_NewStringObj("argv0", 5), تہی, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.لمبائی_2()), 0);

			اگر (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتیجہ(yosys_get_tcl_interp()));
#ورنہ
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#اگر_ختم
		} ورنہ اگر (scriptfile_python) {
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحیح py_argc = special_دلائل.سائز() + 1;
			new_argv.append(scriptfile);
			برائے (صحیح i = 1; i < py_argc; ++i)
				new_argv.append(special_دلائل[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object راستہ = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_راستہ = راستہ(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_راستہ));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			اگر (scriptfp == خالی_اشاریہ) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			اگر (Py_تسلسل_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#ورنہ
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#اگر_ختم
		} ورنہ
			تسلسل_frontend(scriptfile, "script");
	}

	برائے (خودکار it = passes_commands.begin(); it != passes_commands.انتہا(); it++)
		تسلسل_pass(*it);

	اگر (تسلسل_tcl_shell) {
#اگر_تعریف_شدہ YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#ورنہ
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#اگر_ختم
	} ورنہ {
		اگر (تسلسل_shell)
			shell(yosys_design);
		ورنہ
			تسلسل_backend(output_filename, backend_command);
	}

	yosys_design->check();
	برائے (خودکار it : saved_designs)
		it.دوسرا->check();
	برائے (خودکار it : pushed_designs)
		it->check();

	اگر (!depsfile.خالی_خانے())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		اگر (f == خالی_اشاریہ)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقی پہلا = سچ;
		برائے (خودکار fn : yosys_output_files) {
			fprintf(f, "%s%s", پہلا ? "" : " ", escape_filename_spaces(fn).c_str());
			پہلا = جھوٹ;
		}
		fprintf(f, ":");
		برائے (خودکار fn : yosys_داخلہ_files) {
			اگر (yosys_output_files.گنتی(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	اگر (print_stats)
	{
		std::string hash = logger().اختتام_hasher();
		logger().سیٹ_log_وقت(جھوٹ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = منوال_v && !منوال_q;
		اگر (logger().get_warnings_کل())
			لوگارتھم("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_کل());

		اگر (Get_سائز(logger().get_experimental()) != 0)
			لوگارتھم("Warnings: %d experimental features used (not excluded with -x).\n", Get_سائز(logger().get_experimental()));

		اگر (Get_سائز(logger().get_deprecated()) != 0) {
			لوگارتھم("Deprecated features used:\n");
			برائے (خودکار &[feature, earliest_removal] : logger().get_deprecated())
				اگر (earliest_removal.خالی_خانے())
					لوگارتھم("    %s\n", feature);
				ورنہ
					لوگارتھم("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#اگر_تعریف_شدہ _WIN32
		لوگارتھم("End of script. Logfile hash: %s\n", hash);
		(خالی)wall_clock_آغاز;
#ورنہ
		std::string meminfo;
		std::string stats_divider = ", ";

		ساخت rusage ru_بفر;
		getrusage(RUSAGE_SELF, &ru_بفر);
		اگر (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ساخت rusage ru_بفر_children;
			getrusage(RUSAGE_CHILDREN, &ru_بفر_children);
			ru_بفر.ru_utime.tv_sec += ru_بفر_children.ru_utime.tv_sec;
			ru_بفر.ru_utime.tv_usec += ru_بفر_children.ru_utime.tv_usec;
			ru_بفر.ru_stime.tv_sec += ru_بفر_children.ru_stime.tv_sec;
			ru_بفر.ru_stime.tv_usec += ru_بفر_children.ru_stime.tv_usec;
#اگر defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_بفر.ru_maxrss = std::max(ru_بفر.ru_maxrss, ru_بفر_children.ru_maxrss);
#اگر_ختم
		}
#اگر defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / 1024.0);
#وگرنہ_اگر defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / (1024.0 * 1024.0));
#اگر_ختم
		دوہرا wall_seconds = std::chrono::duration<دوہرا>(
				std::chrono::steady_clock::now() - wall_clock_آغاز).گنتی();

		لوگارتھم("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_بفر.ru_utime.tv_sec + 1e-6 * ru_بفر.ru_utime.tv_usec,
				ru_بفر.ru_stime.tv_sec + 1e-6 * ru_بفر.ru_stime.tv_usec, meminfo.c_str());
#اگر_ختم
		لوگارتھم("%s\n", yosys_maybe_نسخہ());

		int64_t کل_ns = 0;
		std::سیٹ<tuple<int64_t, صحیح, std::string>> timedat;

		برائے (خودکار &it : pass_رجسٹر)
			اگر (it.دوسرا->call_شمار) {
				خودکار pass_ns = it.دوسرا->runtime_ns + 1;
				کل_ns += pass_ns;
				timedat.داخل_کرو(make_tuple(pass_ns, it.دوسرا->call_شمار, it.پہلا));
			}
		اگر (twine_gc_گنتی) {
			کل_ns += twine_gc_ns;
			timedat.داخل_کرو(make_tuple(twine_gc_ns, twine_gc_گنتی, "twine_gc"));
		}

		اگر (timing_details)
		{
			لوگارتھم("Time spent:\n");
			برائے (خودکار it = timedat.rbegin(); it != timedat.rend(); it++) {
				لوگارتھم("%5d%% %5d calls %8.3f sec %s\n", صحیح(100*std::get<0>(*it) / کل_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		ورنہ
		{
			صحیح برآمد_گنتی = 0;
			std::string پیغام_2 = "Time spent:";
			برائے (خودکار it = timedat.rbegin(); it != timedat.rend() && برآمد_گنتی < 4; it++, برآمد_گنتی++) {
				اگر (برآمد_گنتی >= 2 && (std::get<0>(*it) < 1000000000 || صحیح(100*std::get<0>(*it) / کل_ns) < 20)) {
					پیغام_2 += ", ...";
					توڑو;
				}
				پیغام_2 += stringf("%s %d%% %dx %s (%d sec)", برآمد_گنتی ? "," : "", صحیح(100*std::get<0>(*it) / کل_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحیح(std::get<0>(*it) / 1000000000));
			}
			پیغام_2 += برآمد_گنتی ? "" : " no commands executed";
			لوگارتھم("%s\n", پیغام_2);
		}
		اگر(!perffile.خالی_خانے())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			اگر (f == خالی_اشاریہ)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_نسخہ());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", کل_ns);
			fprintf(f, "  \"passes\": {");

			منطقی پہلا = سچ;
			برائے (خودکار it = timedat.rbegin(); it != timedat.rend(); it++) {
				اگر (!پہلا)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				پہلا = جھوٹ;
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
#وگرنہ_اگر defined(_WIN32)
	_Exit(0);
#اگر_ختم
	واپس 0;
}

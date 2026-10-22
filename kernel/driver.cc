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
#وصف_ڪريو CXXOPTS_ويڪٽر_DELIMITER '\0'
#ضمن "libs/cxxopts/include/cxxopts.hpp"
#ضمن <iostream>
#ضمن <chrono>

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
#  include <tcl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
نطاق py = pybind11;
#جيڪڏهن_پورو

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <limits.h>
#ضمن <errno.h>
#ضمن <fcntl.h>
#جيڪڏهن_وصف_نه_ٿيل __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#جيڪڏهن_پورو
#ضمن <inttypes.h>

#إذا defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#جيڪڏهن_پورو

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#جيڪڏهن_پورو

#ضمن <fmt/color.h>
#ضمن <fmt/format.h>

USING_YOSYS_NAMESPACE

#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
صحيح yosys_history_وٿي = 0;
std::string yosys_history_file;
#جيڪڏهن_پورو

فراغ yosys_atexit()
{
#إذا defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	إذا (!yosys_history_file.خالي_جايون()) {
#إذا defined(YOSYS_ENABLE_READLINE)
		إذا (yosys_history_وٿي > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_وٿي, yosys_history_file.c_str());
		} وإلا
			write_history(yosys_history_file.c_str());
#وإلا
		write_history(yosys_history_file.c_str());
#جيڪڏهن_پورو
	}

	clear_history();
#إذا defined(YOSYS_ENABLE_READLINE)
	HIST_داخلا **hist_list = history_list();
	إذا (hist_list != عدم)
		حرر(hist_list);
#جيڪڏهن_پورو
#جيڪڏهن_پورو
}

#إذا defined(__OpenBSD__)
نطاق Yosys {
خارجي حرف *yosys_argv0;
خارجي حرف yosys_رستو[رستو_MAX];
};
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
نطاق Yosys {
	خارجي صحيح yosys_tcl_interp_init(Tcl_Interp *interp);
	خارجي فراغ yosys_tcl_activate_repl();
};
#جيڪڏهن_پورو

نطاق Yosys {
	خارجي منطقي log_stderr_sink_forced;
};

صنف ColorConsoleLogSink : عام LogSink
{
عام:
	واضح ColorConsoleLogSink(منطقي err, منطقي quiet) : error_output(err), quiet_warnings(quiet) {}
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override;
	منطقي should_log(ثابت Log_پيغام &نياپو) ثابت override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع error_output ? خالي_اشارو : stdout; }
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
	DWORD گهڻائي;
	أرجع handle != INVALID_HANDLE_قدر &&
			Get_Console_گهڻائي(handle, &گهڻائي);
#وإلا
	أرجع isatty(fileno(file));
#جيڪڏهن_پورو
}

فراغ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي ايندڙ_print_log = سچ;

فراغ ColorConsoleLogSink::لاگرٿم(ثابت Log_پيغام &نياپو)
{
	FILE *f = (error_output || نياپو.severity == LogSeverity::Error) ? stderr : stdout;

	إذا (logger().get_log_وقت() && ايندڙ_print_log) {
		خودڪار elapsed = نياپو.timestamp - logger().get_initial_وقت();
		خودڪار us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string وقت_str = stringf("[%05d.%06d] ", صحيح(us.ڳڻپ() / 1'000'000),صحيح(us.ڳڻپ() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", وقت_str);
		ايندڙ_print_log = خطأ;
	}

	إذا (!نياپو.src.filename.خالي_جايون())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", نياپو.src.filename, نياپو.src.شروعات_سٽ);

	بدل (نياپو.severity) {
		حالة LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", نياپو.اڳياڙي_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", نياپو.پيغام);
			اكسر;

		حالة LogSeverity::Error:
		حالة LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", نياپو.اڳياڙي_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", نياپو.پيغام);
			اكسر;

		حالة LogSeverity::سرو:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", نياپو.اڳياڙي_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", نياپو.پيغام);
			اكسر;

		حالة LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", نياپو.اڳياڙي_2, نياپو.پيغام);
			اكسر;

		حالة LogSeverity::Comment:
		حالة LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", نياپو.اڳياڙي_2, نياپو.پيغام);
			اكسر;

		حالة LogSeverity::Info:
		افتراضي:
			إذا (!نياپو.اڳياڙي_2.خالي_جايون())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", نياپو.اڳياڙي_2);
			fmt::print(f, "{}", نياپو.پيغام);
			اكسر;
	}
	إذا (!نياپو.پيغام.خالي_جايون())
		ايندڙ_print_log = نياپو.پيغام.پٺ() == '\n';
}

منطقي ColorConsoleLogSink::should_log(ثابت Log_پيغام &نياپو) ثابت
{
	أرجع (!error_output) || (نياپو.severity == LogSeverity::Error || نياپو.severity == LogSeverity::NonFatalError ||
			(نياپو.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#جيڪڏهن_وصف_ٿيل _WIN32
صحيح wmain(صحيح argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#جيڪڏهن_وصف_ٿيل _UCRT
	setlocale(LC_ALL, ".UTF-8");
#جيڪڏهن_پورو
	// Configure the Win32 console to use UTF-8.
	سيٽ_Console_CP(CP_UTF8);
	سيٽ_Console_Output_CP(CP_UTF8);

	std::ويڪٽر<حرف*> uargs;
	لكل (صحيح i = 0; i < argc; i++) {
		صحيح usize = Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wargv[i], -1, عدم, 0, عدم, عدم);
		حرف *uarg = (حرف *)يادگيري_ڏيو(usize);
		Wide_Char_منزل_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, عدم, عدم);
		uargs.ڌڪو_پٺ(uarg);
	}
	uargs.ڌڪو_پٺ(عدم);
	حرف **argv = uargs.معلومات();
#وإلا
صحيح مکيه(صحيح argc, حرف **argv)
{
#جيڪڏهن_پورو
	خودڪار wall_clock_شروعات = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::ويڪٽر<std::string> vlog_مقرر_ڪري;
	std::ويڪٽر<std::string> passes_commands;
	std::ويڪٽر<std::string> frontend_files;
	std::ويڪٽر<std::string> plugin_filenames;
	std::ويڪٽر<std::string> special_دليلون;
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
	منطقي لاڳيتو_shell = سچ;
	منطقي لاڳيتو_tcl_shell = خطأ;
	منطقي گهڻائي_v = خطأ;
	منطقي گهڻائي_q = خطأ;
	منطقي quiet_warnings = خطأ;
	منطقي log_stderr = خطأ;
	no_color = std::getenv("NO_COLOR") != خالي_اشارو;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.سيٽ_ويڪر(SIZE_MAX);

	options.جوڙيو_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::قدر<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::قدر<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::قدر<std::string>(), "<scriptfile>")
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::قدر<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#جيڪڏهن_پورو // YOSYS_ENABLE_TCL
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::قدر<std::string>(), "<script>")
#جيڪڏهن_پورو // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::قدر<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<define>[=<value>]")
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
		("infile", "input files", cxxopts::قدر<std::ويڪٽر<std::string>>())
	;
	options.جوڙيو_options("logging")
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
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::قدر<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::قدر<std::ويڪٽر<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::قدر<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.جوڙيو_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::قدر<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::قدر<std::ويڪٽر<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::قدر<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	إذا (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::ٻاهر << options.help() << std::سٽ_پڄاڻي;
		ٻاهر_نڪرو(0);
	}
	حاول {
		// Check for "--" in arguments
		خودڪار it = std::ڳوليو(argv, argv + argc, std::string("--"));
		إذا (it != argv + argc) {
			special_دليلون.سونپيو(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::مفاصلو(argv, it);
		}

		خودڪار نتيجو = options.parse(argc, argv);

		إذا (نتيجو.ڳڻپ("M")) memhasher_on();
		إذا (نتيجو.ڳڻپ("X")) yosys_xtrace += نتيجو.ڳڻپ("X");
		إذا (نتيجو.ڳڻپ("A")) call_abort = سچ;
		إذا (نتيجو.ڳڻپ("Q")) print_banner = خطأ;
		إذا (نتيجو.ڳڻپ("T")) print_stats = خطأ;
		إذا (نتيجو.ڳڻپ("no-version")) yosys_write_versions = خطأ;
		إذا (نتيجو.ڳڻپ("V")) {
			std::ٻاهر << yosys_نسخو_str << std::سٽ_پڄاڻي;
			ٻاهر_نڪرو(0);
		}
		إذا (نتيجو.ڳڻپ("git-hash")) {
			std::ٻاهر << yosys_git_hash_str << std::سٽ_پڄاڻي;
			ٻاهر_نڪرو(0);
		}
		إذا (نتيجو.ڳڻپ("S")) {
			passes_commands.ڌڪو_پٺ("synth");
			لاڳيتو_shell = خطأ;
		}
		إذا (نتيجو.ڳڻپ("C")) لاڳيتو_tcl_shell = سچ;
		إذا (نتيجو.ڳڻپ("g")) logger().force_debug_on();
		إذا (نتيجو.ڳڻپ("m")) plugin_filenames = نتيجو["m"].as<std::ويڪٽر<std::string>>();
		إذا (نتيجو.ڳڻپ("f")) frontend_command = نتيجو["f"].as<std::string>();
		إذا (نتيجو.ڳڻپ("H")) {
			passes_commands.ڌڪو_پٺ("help");
			لاڳيتو_shell = خطأ;
		}
		إذا (نتيجو.ڳڻپ("h")) {
			std::string res = نتيجو["h"].as<std::string>();
			passes_commands.ڌڪو_پٺ("help " + res);
			لاڳيتو_shell = خطأ;
		}
		إذا (نتيجو.ڳڻپ("b")) {
			backend_command = نتيجو["b"].as<std::string>();
			لاڳيتو_shell = خطأ;
		}
		إذا (نتيجو.ڳڻپ("p")) {
			خودڪار cmds = نتيجو["p"].as<std::ويڪٽر<std::string>>();
			passes_commands.وجهو(passes_commands.پڇاڙي(), cmds.begin(), cmds.پڇاڙي());
			لاڳيتو_shell = خطأ;
		}
		إذا (نتيجو.ڳڻپ("o")) {
			output_filename = نتيجو["o"].as<std::string>();
			لاڳيتو_shell = خطأ;
		}
		لكل (ثابت خودڪار& ڪنجي : {"l", "L"}) {
			إذا (نتيجو.ڳڻپ(ڪنجي)) {
				لكل (ثابت خودڪار& filename : نتيجو[ڪنجي].as<std::ويڪٽر<std::string>>()) {
					حاول {
						logger().جوڙيو_sink<FileLogSink>(filename, ڪنجي[0] == 'L', خطأ);
					} امسك (ثابت std::runtime_error &e) {
 					   std::cerr << e.what();
					   ٻاهر_نڪرو(1);
					}
				}
			}
		}
		إذا (نتيجو.ڳڻپ("q")) {
			گهڻائي_q = سچ;
			إذا (log_stderr) quiet_warnings = سچ;
			log_stderr = سچ;
		}
		إذا (نتيجو.ڳڻپ("v")) {
			گهڻائي_v = سچ;
			log_stderr = سچ;
			logger().سيٽ_verbose_level(نتيجو["v"].as<صحيح>());
		}
		إذا (نتيجو.ڳڻپ("t")) logger().سيٽ_log_وقت(سچ);
		إذا (نتيجو.ڳڻپ("no-color")) no_color = سچ;
		إذا (نتيجو.ڳڻپ("d")) timing_details = سچ;
		لكل (ثابت خودڪار& ڪنجي : {"s", "c"}) {
			إذا (نتيجو.ڳڻپ(ڪنجي)) {
				scriptfile = نتيجو[ڪنجي].as<std::string>();
				scriptfile_tcl = std::string(ڪنجي) == "c";
				لاڳيتو_shell = خطأ;
			}
		}
		إذا (نتيجو.ڳڻپ("y")) {
			scriptfile = نتيجو["y"].as<std::string>();
			scriptfile_python = سچ;
			لاڳيتو_shell = خطأ;
		}
		لكل (ثابت خودڪار& ڪنجي : {"W", "w", "e"}) {
			إذا (نتيجو.ڳڻپ(ڪنجي)) {
				خودڪار regexes = نتيجو[ڪنجي].as<std::ويڪٽر<std::string>>();
				لكل (ثابت خودڪار& regex : regexes) {
					إذا (std::string(ڪنجي) == "W")
						logger().جوڙيو_warn(regex);
					إذا (std::string(ڪنجي) == "w")
						logger().جوڙيو_nowarn(regex);
					إذا (std::string(ڪنجي) == "e")
						logger().جوڙيو_werror(regex);
				}
			}
		}
		إذا (نتيجو.ڳڻپ("r")) topmodule = نتيجو["r"].as<std::string>();
		إذا (نتيجو.ڳڻپ("D")) vlog_مقرر_ڪري = نتيجو["D"].as<std::ويڪٽر<std::string>>();
		إذا (نتيجو.ڳڻپ("P")) {
			خودڪار dump_دليلون = نتيجو["P"].as<std::ويڪٽر<std::string>>();
			لكل (ثابت خودڪار& arg : dump_دليلون) {
				خودڪار ٽوڪن = ورهايو_ٽوڪن(arg, ":");
				إذا (!ٽوڪن.خالي_جايون() && ٽوڪن[0] == "ALL") {
					إذا (ٽوڪن.سائيز() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::سٽ_پڄاڻي;
						ٻاهر_نڪرو(1);
					}
					logger().سيٽ_hdump_all(سچ);
				} وإلا {
					إذا (!ٽوڪن.خالي_جايون() && !ٽوڪن[0].خالي_جايون() && ٽوڪن[0].پٺ() == '.')
						ٽوڪن[0].ڪڍو_پٺ();
					إذا (ٽوڪن.سائيز() == 1)
						ٽوڪن.ڌڪو_پٺ("yosys_dump_" + ٽوڪن[0] + ".il");
					إذا (ٽوڪن.سائيز() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::سٽ_پڄاڻي;
						ٻاهر_نڪرو(1);
					}
					logger().جوڙيو_hdump(ٽوڪن[0],ٽوڪن[1]);
				}
			}
		}
		إذا (نتيجو.ڳڻپ("E")) depsfile = نتيجو["E"].as<std::string>();
		إذا (نتيجو.ڳڻپ("x")) {
			لكل (ثابت خودڪار &ignore : نتيجو["x"].as<std::ويڪٽر<std::string>>())
    			logger().جوڙيو_experimental_ignore(ignore);
		}
		إذا (نتيجو.ڳڻپ("perffile")) perffile = نتيجو["perffile"].as<std::string>();
		إذا (نتيجو.ڳڻپ("infile")) {
			frontend_files = نتيجو["infile"].as<std::ويڪٽر<std::string>>();
		}
		إذا (نتيجو.ڳڻپ("autoidx")) {
			صحيح idx = نتيجو["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		إذا (!is_colored(stdout) || !is_colored(stderr)) {
			إذا (!log_stderr) {
				logger().جوڙيو_sink<ConsoleLogSink>();
			} وإلا {
				logger().جوڙيو_sink<StderrLogSink>(quiet_warnings);
			}
		} وإلا
			logger().جوڙيو_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		إذا (print_banner)
			yosys_banner();

	}
	امسك (ثابت cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::سٽ_پڄاڻي;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::سٽ_پڄاڻي;
		ٻاهر_نڪرو(1);
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
	#جيڪڏهن_پورو

	إذا (!state_dir.خالي_جايون()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_وٿي = where_history();
	}
#جيڪڏهن_پورو

	إذا (print_stats)
		logger().شروعات_hasher();

#إذا defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_رستو);
#جيڪڏهن_پورو

#إذا defined(__linux__)
	// set stack size to >= 128 MB
	{
		بنية rlimit rl;
		ثابت rlim_t ڍير_سائيز = 128L * 1024L * 1024L;
		إذا (getrlimit(RLIMIT_ڍير, &rl) == 0 && rl.rlim_cur < ڍير_سائيز) {
			rl.rlim_cur = ڍير_سائيز;
			setrlimit(RLIMIT_ڍير, &rl);
		}
	}
#جيڪڏهن_پورو

	yosys_تياري();
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#جيڪڏهن_پورو
	log_error_atexit = yosys_atexit;

	لكل (خودڪار &fn : plugin_filenames)
		ڀريو_plugin(fn, {});

	log_suppressed();

	إذا (!vlog_مقرر_ڪري.خالي_جايون()) {
		std::string vdef_cmd = "read -define";
		لكل (خودڪار vdef : vlog_مقرر_ڪري)
			vdef_cmd += " " + vdef;
		لاڳيتو_pass(vdef_cmd);
	}

	إذا (scriptfile.خالي_جايون() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		لكل (خودڪار special_arg : special_دليلون)
			frontend_files.ڌڪو_پٺ(special_arg);
	}

	لكل (خودڪار it = frontend_files.begin(); it != frontend_files.پڇاڙي(); ++it) {
		إذا (لاڳيتو_frontend((*it).c_str(), frontend_command))
			لاڳيتو_shell = خطأ;
	}

	إذا (!topmodule.خالي_جايون())
		لاڳيتو_pass("hierarchy -top " + topmodule);
	إذا (!scriptfile.خالي_جايون()) {
		إذا (scriptfile_tcl) {
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
			صحيح tcl_argc = special_دليلون.سائيز();
			std::ويڪٽر<Tcl_Obj*> script_دليلون;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			لكل (خودڪار arg : special_دليلون)
				script_دليلون.ڌڪو_پٺ(Tcl_NewStringObj(arg.c_str(), arg.ڊيگهائي()));

			Tcl_Obj_سيٽ_Var2(interp, Tcl_NewStringObj("argc", 4), عدم, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_سيٽ_Var2(interp, Tcl_NewStringObj("argv", 4), عدم, Tcl_NewListObj(tcl_argc, script_دليلون.معلومات()), 0);
			Tcl_Obj_سيٽ_Var2(interp, Tcl_NewStringObj("argv0", 5), عدم, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.ڊيگهائي()), 0);

			إذا (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_نتيجو(yosys_get_tcl_interp()));
#وإلا
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#جيڪڏهن_پورو
		} وإلا إذا (scriptfile_python) {
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
			py::list new_argv;
			صحيح py_argc = special_دليلون.سائيز() + 1;
			new_argv.append(scriptfile);
			لكل (صحيح i = 1; i < py_argc; ++i)
				new_argv.append(special_دليلون[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object رستو = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_رستو = رستو(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_رستو));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			إذا (scriptfp == خالي_اشارو) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			إذا (Py_لاڳيتو_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#وإلا
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#جيڪڏهن_پورو
		} وإلا
			لاڳيتو_frontend(scriptfile, "script");
	}

	لكل (خودڪار it = passes_commands.begin(); it != passes_commands.پڇاڙي(); it++)
		لاڳيتو_pass(*it);

	إذا (لاڳيتو_tcl_shell) {
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#وإلا
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#جيڪڏهن_پورو
	} وإلا {
		إذا (لاڳيتو_shell)
			shell(yosys_design);
		وإلا
			لاڳيتو_backend(output_filename, backend_command);
	}

	yosys_design->check();
	لكل (خودڪار it : saved_designs)
		it.ٻيو->check();
	لكل (خودڪار it : pushed_designs)
		it->check();

	إذا (!depsfile.خالي_جايون())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		إذا (f == خالي_اشارو)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		منطقي پهريون = سچ;
		لكل (خودڪار fn : yosys_output_files) {
			fprintf(f, "%s%s", پهريون ? "" : " ", escape_filename_spaces(fn).c_str());
			پهريون = خطأ;
		}
		fprintf(f, ":");
		لكل (خودڪار fn : yosys_داخلا_files) {
			إذا (yosys_output_files.ڳڻپ(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	إذا (print_stats)
	{
		std::string hash = logger().پڄاڻي_hasher();
		logger().سيٽ_log_وقت(خطأ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = گهڻائي_v && !گهڻائي_q;
		إذا (logger().get_warnings_ڪل())
			لاگرٿم("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_ڪل());

		إذا (Get_سائيز(logger().get_experimental()) != 0)
			لاگرٿم("Warnings: %d experimental features used (not excluded with -x).\n", Get_سائيز(logger().get_experimental()));

		إذا (Get_سائيز(logger().get_deprecated()) != 0) {
			لاگرٿم("Deprecated features used:\n");
			لكل (خودڪار &[feature, earliest_removal] : logger().get_deprecated())
				إذا (earliest_removal.خالي_جايون())
					لاگرٿم("    %s\n", feature);
				وإلا
					لاگرٿم("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#جيڪڏهن_وصف_ٿيل _WIN32
		لاگرٿم("End of script. Logfile hash: %s\n", hash);
		(فراغ)wall_clock_شروعات;
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
#جيڪڏهن_پورو
		}
#إذا defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / 1024.0);
#وإلاإذا defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_بفر.ru_maxrss / (1024.0 * 1024.0));
#جيڪڏهن_پورو
		مزدوج wall_seconds = std::chrono::duration<مزدوج>(
				std::chrono::steady_clock::now() - wall_clock_شروعات).ڳڻپ();

		لاگرٿم("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_بفر.ru_utime.tv_sec + 1e-6 * ru_بفر.ru_utime.tv_usec,
				ru_بفر.ru_stime.tv_sec + 1e-6 * ru_بفر.ru_stime.tv_usec, meminfo.c_str());
#جيڪڏهن_پورو
		لاگرٿم("%s\n", yosys_maybe_نسخو());

		int64_t ڪل_ns = 0;
		std::سيٽ<tuple<int64_t, صحيح, std::string>> timedat;

		لكل (خودڪار &it : pass_رجسٽر)
			إذا (it.ٻيو->call_counter) {
				خودڪار pass_ns = it.ٻيو->runtime_ns + 1;
				ڪل_ns += pass_ns;
				timedat.وجهو(make_tuple(pass_ns, it.ٻيو->call_counter, it.پهريون));
			}
		إذا (twine_gc_ڳڻپ) {
			ڪل_ns += twine_gc_ns;
			timedat.وجهو(make_tuple(twine_gc_ns, twine_gc_ڳڻپ, "twine_gc"));
		}

		إذا (timing_details)
		{
			لاگرٿم("Time spent:\n");
			لكل (خودڪار it = timedat.rbegin(); it != timedat.rend(); it++) {
				لاگرٿم("%5d%% %5d calls %8.3f sec %s\n", صحيح(100*std::get<0>(*it) / ڪل_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		وإلا
		{
			صحيح ٻاهريون_ڳڻپ = 0;
			std::string پيغام = "Time spent:";
			لكل (خودڪار it = timedat.rbegin(); it != timedat.rend() && ٻاهريون_ڳڻپ < 4; it++, ٻاهريون_ڳڻپ++) {
				إذا (ٻاهريون_ڳڻپ >= 2 && (std::get<0>(*it) < 1000000000 || صحيح(100*std::get<0>(*it) / ڪل_ns) < 20)) {
					پيغام += ", ...";
					اكسر;
				}
				پيغام += stringf("%s %d%% %dx %s (%d sec)", ٻاهريون_ڳڻپ ? "," : "", صحيح(100*std::get<0>(*it) / ڪل_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), صحيح(std::get<0>(*it) / 1000000000));
			}
			پيغام += ٻاهريون_ڳڻپ ? "" : " no commands executed";
			لاگرٿم("%s\n", پيغام);
		}
		إذا(!perffile.خالي_جايون())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			إذا (f == خالي_اشارو)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_نسخو());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", ڪل_ns);
			fprintf(f, "  \"passes\": {");

			منطقي پهريون = سچ;
			لكل (خودڪار it = timedat.rbegin(); it != timedat.rend(); it++) {
				إذا (!پهريون)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				پهريون = خطأ;
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
#جيڪڏهن_پورو
	أرجع 0;
}

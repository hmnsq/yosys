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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/hashlib.h"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CXXOPTS_ਸਦਿਸ਼_DELIMITER '\0'
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/cxxopts/include/cxxopts.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>
#ਅਨ੍ਤਰ੍ਭਾਵ <chrono>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
#  include <tcl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
ਨਾਮਾਕਾਸ਼ py = pybind11;
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>

#ਜੇ defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#ਜੇ_ਅੰਤ

#ਜੇ !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <fmt/color.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fmt/format.h>

USING_YOSYS_NAMESPACE

#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ਪੂਰਨ yosys_history_ਵਿੱਥ = 0;
std::string yosys_history_file;
#ਜੇ_ਅੰਤ

ਖਾਲੀ yosys_atexit()
{
#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ਜੇ (!yosys_history_file.ਖਾਲੀ_ਥਾਵਾਂ()) {
#ਜੇ defined(YOSYS_ENABLE_READLINE)
		ਜੇ (yosys_history_ਵਿੱਥ > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_ਵਿੱਥ, yosys_history_file.c_str());
		} ਨਹੀਂਤਾਂ
			write_history(yosys_history_file.c_str());
#ਨਹੀਂਤਾਂ
		write_history(yosys_history_file.c_str());
#ਜੇ_ਅੰਤ
	}

	clear_history();
#ਜੇ defined(YOSYS_ENABLE_READLINE)
	HIST_ਇੰਦਰਾਜ **hist_list = history_list();
	ਜੇ (hist_list != ਨਿਰਾ)
		ਮੁਕਤ(hist_list);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(__OpenBSD__)
ਨਾਮਾਕਾਸ਼ Yosys {
ਬਾਹਰੀ ਅੱਖਰ *yosys_argv0;
ਬਾਹਰੀ ਅੱਖਰ yosys_ਰਸਤਾ[ਰਸਤਾ_MAX];
};
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
ਨਾਮਾਕਾਸ਼ Yosys {
	ਬਾਹਰੀ ਪੂਰਨ yosys_tcl_interp_init(Tcl_Interp *interp);
	ਬਾਹਰੀ ਖਾਲੀ yosys_tcl_activate_repl();
};
#ਜੇ_ਅੰਤ

ਨਾਮਾਕਾਸ਼ Yosys {
	ਬਾਹਰੀ ਤਾਰਕਿਕ log_stderr_sink_forced;
};

ਵਰਗ ColorConsoleLogSink : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	ਸਪਸ਼ਟ ColorConsoleLogSink(ਤਾਰਕਿਕ err, ਤਾਰਕਿਕ quiet) : error_output(err), quiet_warnings(quiet) {}
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override;
	ਤਾਰਕਿਕ should_log(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) ਅਚਲ override;
	ਖਾਲੀ flush() override;
	FILE *file_handle() override { ਵਾਪਸ error_output ? ਖਾਲੀ_ਸੂਚਕ : stdout; }
ਨਿਜੀ:
	ਤਾਰਕਿਕ error_output;
	ਤਾਰਕਿਕ quiet_warnings;
};

ਤਾਰਕਿਕ no_color = ਝੂਠ;

ਤਾਰਕਿਕ is_colored(FILE *file)
{
	ਜੇ (no_color)
		ਵਾਪਸ ਝੂਠ;

#ਜੇ defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD ਬਹੁਲਕ;
	ਵਾਪਸ handle != INVALID_HANDLE_ਮੁੱਲ &&
			Get_Console_ਬਹੁਲਕ(handle, &ਬਹੁਲਕ);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ isatty(fileno(file));
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ਤਾਰਕਿਕ ਅਗਲਾ_print_log = ਸੱਚ;

ਖਾਲੀ ColorConsoleLogSink::ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ)
{
	FILE *f = (error_output || ਸੁਨੇਹਾ.severity == LogSeverity::Error) ? stderr : stdout;

	ਜੇ (logger().get_log_ਸਮਾਂ() && ਅਗਲਾ_print_log) {
		ਸਵੈਚਾਲਿਤ elapsed = ਸੁਨੇਹਾ.timestamp - logger().get_initial_ਸਮਾਂ();
		ਸਵੈਚਾਲਿਤ us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string ਸਮਾਂ_str = stringf("[%05d.%06d] ", ਪੂਰਨ(us.ਗਿਣਤੀ_2() / 1'000'000),ਪੂਰਨ(us.ਗਿਣਤੀ_2() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", ਸਮਾਂ_str);
		ਅਗਲਾ_print_log = ਝੂਠ;
	}

	ਜੇ (!ਸੁਨੇਹਾ.src.filename.ਖਾਲੀ_ਥਾਵਾਂ())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", ਸੁਨੇਹਾ.src.filename, ਸੁਨੇਹਾ.src.ਸ਼ੁਰੂ_ਸਤਰ);

	ਚੋਣ (ਸੁਨੇਹਾ.severity) {
		ਹਾਲਤ LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", ਸੁਨੇਹਾ.ਅਗੇਤਰ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2);
			ਰੋਕੋ;

		ਹਾਲਤ LogSeverity::Error:
		ਹਾਲਤ LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", ਸੁਨੇਹਾ.ਅਗੇਤਰ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2);
			ਰੋਕੋ;

		ਹਾਲਤ LogSeverity::ਸਿਰਲੇਖ:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", ਸੁਨੇਹਾ.ਅਗੇਤਰ_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2);
			ਰੋਕੋ;

		ਹਾਲਤ LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", ਸੁਨੇਹਾ.ਅਗੇਤਰ_2, ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2);
			ਰੋਕੋ;

		ਹਾਲਤ LogSeverity::Comment:
		ਹਾਲਤ LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", ਸੁਨੇਹਾ.ਅਗੇਤਰ_2, ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2);
			ਰੋਕੋ;

		ਹਾਲਤ LogSeverity::Info:
		ਮੂਲ:
			ਜੇ (!ਸੁਨੇਹਾ.ਅਗੇਤਰ_2.ਖਾਲੀ_ਥਾਵਾਂ())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", ਸੁਨੇਹਾ.ਅਗੇਤਰ_2);
			fmt::print(f, "{}", ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2);
			ਰੋਕੋ;
	}
	ਜੇ (!ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2.ਖਾਲੀ_ਥਾਵਾਂ())
		ਅਗਲਾ_print_log = ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2.ਪਿੱਠ() == '\n';
}

ਤਾਰਕਿਕ ColorConsoleLogSink::should_log(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) ਅਚਲ
{
	ਵਾਪਸ (!error_output) || (ਸੁਨੇਹਾ.severity == LogSeverity::Error || ਸੁਨੇਹਾ.severity == LogSeverity::NonFatalError ||
			(ਸੁਨੇਹਾ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
ਪੂਰਨ wmain(ਪੂਰਨ argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _UCRT
	setlocale(LC_ALL, ".UTF-8");
#ਜੇ_ਅੰਤ
	// Configure the Win32 console to use UTF-8.
	ਸੈੱਟ_Console_CP(CP_UTF8);
	ਸੈੱਟ_Console_Output_CP(CP_UTF8);

	std::ਸਦਿਸ਼<ਅੱਖਰ*> uargs;
	ਲਈ (ਪੂਰਨ i = 0; i < argc; i++) {
		ਪੂਰਨ usize = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, wargv[i], -1, ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ);
		ਅੱਖਰ *uarg = (ਅੱਖਰ *)ਸਮ੍ਰਿਤੀ_ਦਿਓ(usize);
		Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ਨਿਰਾ, ਨਿਰਾ);
		uargs.ਧੱਕੋ_ਪਿੱਠ(uarg);
	}
	uargs.ਧੱਕੋ_ਪਿੱਠ(ਨਿਰਾ);
	ਅੱਖਰ **argv = uargs.ਅੰਕੜੇ();
#ਨਹੀਂਤਾਂ
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
#ਜੇ_ਅੰਤ
	ਸਵੈਚਾਲਿਤ wall_clock_ਸ਼ੁਰੂ = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::ਸਦਿਸ਼<std::string> vlog_ਨਿਰਧਾਰਤ_ਕਰਦਾ;
	std::ਸਦਿਸ਼<std::string> passes_commands;
	std::ਸਦਿਸ਼<std::string> frontend_files;
	std::ਸਦਿਸ਼<std::string> plugin_filenames;
	std::ਸਦਿਸ਼<std::string> special_ਦਲੀਲਾਂ;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	ਤਾਰਕਿਕ scriptfile_tcl = ਝੂਠ;
	ਤਾਰਕਿਕ scriptfile_python = ਝੂਠ;
	ਤਾਰਕਿਕ print_banner = ਸੱਚ;
	ਤਾਰਕਿਕ print_stats = ਸੱਚ;
	ਤਾਰਕਿਕ call_abort = ਝੂਠ;
	ਤਾਰਕਿਕ timing_details = ਝੂਠ;
	ਤਾਰਕਿਕ ਲੜੀਵਾਰ_shell = ਸੱਚ;
	ਤਾਰਕਿਕ ਲੜੀਵਾਰ_tcl_shell = ਝੂਠ;
	ਤਾਰਕਿਕ ਬਹੁਲਕ_v = ਝੂਠ;
	ਤਾਰਕਿਕ ਬਹੁਲਕ_q = ਝੂਠ;
	ਤਾਰਕਿਕ quiet_warnings = ਝੂਠ;
	ਤਾਰਕਿਕ log_stderr = ਝੂਠ;
	no_color = std::getenv("NO_COLOR") != ਖਾਲੀ_ਸੂਚਕ;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.ਸੈੱਟ_ਚੌੜਾਈ(SIZE_MAX);

	options.ਜੋੜੋ_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::ਮੁੱਲ<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::ਮੁੱਲ<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::ਮੁੱਲ<std::string>(), "<scriptfile>")
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::ਮੁੱਲ<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#ਜੇ_ਅੰਤ // YOSYS_ENABLE_TCL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::ਮੁੱਲ<std::string>(), "<script>")
#ਜੇ_ਅੰਤ // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::ਮੁੱਲ<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::ਮੁੱਲ<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>())
	;
	options.ਜੋੜੋ_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::ਮੁੱਲ<ਪੂਰਨ>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::ਮੁੱਲ<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::ਮੁੱਲ<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.ਜੋੜੋ_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::ਮੁੱਲ<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::ਮੁੱਲ<std::ਸਦਿਸ਼<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::ਮੁੱਲ<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	ਜੇ (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::ਬਾਹਰ << options.help() << std::ਸਤਰ_ਅੰਤ;
		ਬਾਹਰ_ਨਿਕਲੋ(0);
	}
	ਕੋਸ਼ਿਸ਼ {
		// Check for "--" in arguments
		ਸਵੈਚਾਲਿਤ it = std::ਲੱਭੋ(argv, argv + argc, std::string("--"));
		ਜੇ (it != argv + argc) {
			special_ਦਲੀਲਾਂ.ਸੌਂਪੋ(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::ਦੂਰੀ(argv, it);
		}

		ਸਵੈਚਾਲਿਤ ਨਤੀਜਾ = options.parse(argc, argv);

		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("M")) memhasher_on();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("X")) yosys_xtrace += ਨਤੀਜਾ.ਗਿਣਤੀ_2("X");
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("A")) call_abort = ਸੱਚ;
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("Q")) print_banner = ਝੂਠ;
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("T")) print_stats = ਝੂਠ;
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("no-version")) yosys_write_versions = ਝੂਠ;
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("V")) {
			std::ਬਾਹਰ << yosys_ਸੰਸਕਰਣ_str << std::ਸਤਰ_ਅੰਤ;
			ਬਾਹਰ_ਨਿਕਲੋ(0);
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("git-hash")) {
			std::ਬਾਹਰ << yosys_git_hash_str << std::ਸਤਰ_ਅੰਤ;
			ਬਾਹਰ_ਨਿਕਲੋ(0);
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("S")) {
			passes_commands.ਧੱਕੋ_ਪਿੱਠ("synth");
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("C")) ਲੜੀਵਾਰ_tcl_shell = ਸੱਚ;
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("g")) logger().force_debug_on();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("m")) plugin_filenames = ਨਤੀਜਾ["m"].as<std::ਸਦਿਸ਼<std::string>>();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("f")) frontend_command = ਨਤੀਜਾ["f"].as<std::string>();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("H")) {
			passes_commands.ਧੱਕੋ_ਪਿੱਠ("help");
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("h")) {
			std::string res = ਨਤੀਜਾ["h"].as<std::string>();
			passes_commands.ਧੱਕੋ_ਪਿੱਠ("help " + res);
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("b")) {
			backend_command = ਨਤੀਜਾ["b"].as<std::string>();
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("p")) {
			ਸਵੈਚਾਲਿਤ cmds = ਨਤੀਜਾ["p"].as<std::ਸਦਿਸ਼<std::string>>();
			passes_commands.ਪਾਓ(passes_commands.ਸਿਰਾ(), cmds.begin(), cmds.ਸਿਰਾ());
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("o")) {
			output_filename = ਨਤੀਜਾ["o"].as<std::string>();
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& ਕੁੰਜੀ : {"l", "L"}) {
			ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2(ਕੁੰਜੀ)) {
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& filename : ਨਤੀਜਾ[ਕੁੰਜੀ].as<std::ਸਦਿਸ਼<std::string>>()) {
					ਕੋਸ਼ਿਸ਼ {
						logger().ਜੋੜੋ_sink<FileLogSink>(filename, ਕੁੰਜੀ[0] == 'L', ਝੂਠ);
					} ਫੜੋ (ਅਚਲ std::runtime_error &e) {
 					   std::cerr << e.what();
					   ਬਾਹਰ_ਨਿਕਲੋ(1);
					}
				}
			}
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("q")) {
			ਬਹੁਲਕ_q = ਸੱਚ;
			ਜੇ (log_stderr) quiet_warnings = ਸੱਚ;
			log_stderr = ਸੱਚ;
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("v")) {
			ਬਹੁਲਕ_v = ਸੱਚ;
			log_stderr = ਸੱਚ;
			logger().ਸੈੱਟ_verbose_level(ਨਤੀਜਾ["v"].as<ਪੂਰਨ>());
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("t")) logger().ਸੈੱਟ_log_ਸਮਾਂ(ਸੱਚ);
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("no-color")) no_color = ਸੱਚ;
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("d")) timing_details = ਸੱਚ;
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& ਕੁੰਜੀ : {"s", "c"}) {
			ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2(ਕੁੰਜੀ)) {
				scriptfile = ਨਤੀਜਾ[ਕੁੰਜੀ].as<std::string>();
				scriptfile_tcl = std::string(ਕੁੰਜੀ) == "c";
				ਲੜੀਵਾਰ_shell = ਝੂਠ;
			}
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("y")) {
			scriptfile = ਨਤੀਜਾ["y"].as<std::string>();
			scriptfile_python = ਸੱਚ;
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
		}
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& ਕੁੰਜੀ : {"W", "w", "e"}) {
			ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2(ਕੁੰਜੀ)) {
				ਸਵੈਚਾਲਿਤ regexes = ਨਤੀਜਾ[ਕੁੰਜੀ].as<std::ਸਦਿਸ਼<std::string>>();
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& regex : regexes) {
					ਜੇ (std::string(ਕੁੰਜੀ) == "W")
						logger().ਜੋੜੋ_warn(regex);
					ਜੇ (std::string(ਕੁੰਜੀ) == "w")
						logger().ਜੋੜੋ_nowarn(regex);
					ਜੇ (std::string(ਕੁੰਜੀ) == "e")
						logger().ਜੋੜੋ_werror(regex);
				}
			}
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("r")) topmodule = ਨਤੀਜਾ["r"].as<std::string>();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("D")) vlog_ਨਿਰਧਾਰਤ_ਕਰਦਾ = ਨਤੀਜਾ["D"].as<std::ਸਦਿਸ਼<std::string>>();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("P")) {
			ਸਵੈਚਾਲਿਤ dump_ਦਲੀਲਾਂ = ਨਤੀਜਾ["P"].as<std::ਸਦਿਸ਼<std::string>>();
			ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& arg : dump_ਦਲੀਲਾਂ) {
				ਸਵੈਚਾਲਿਤ ਟੋਕਨ = ਵੰਡੋ_ਟੋਕਨ(arg, ":");
				ਜੇ (!ਟੋਕਨ.ਖਾਲੀ_ਥਾਵਾਂ() && ਟੋਕਨ[0] == "ALL") {
					ਜੇ (ਟੋਕਨ.ਮਾਪ() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::ਸਤਰ_ਅੰਤ;
						ਬਾਹਰ_ਨਿਕਲੋ(1);
					}
					logger().ਸੈੱਟ_hdump_all(ਸੱਚ);
				} ਨਹੀਂਤਾਂ {
					ਜੇ (!ਟੋਕਨ.ਖਾਲੀ_ਥਾਵਾਂ() && !ਟੋਕਨ[0].ਖਾਲੀ_ਥਾਵਾਂ() && ਟੋਕਨ[0].ਪਿੱਠ() == '.')
						ਟੋਕਨ[0].ਕੱਢੋ_ਪਿੱਠ();
					ਜੇ (ਟੋਕਨ.ਮਾਪ() == 1)
						ਟੋਕਨ.ਧੱਕੋ_ਪਿੱਠ("yosys_dump_" + ਟੋਕਨ[0] + ".il");
					ਜੇ (ਟੋਕਨ.ਮਾਪ() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::ਸਤਰ_ਅੰਤ;
						ਬਾਹਰ_ਨਿਕਲੋ(1);
					}
					logger().ਜੋੜੋ_hdump(ਟੋਕਨ[0],ਟੋਕਨ[1]);
				}
			}
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("E")) depsfile = ਨਤੀਜਾ["E"].as<std::string>();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("x")) {
			ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ignore : ਨਤੀਜਾ["x"].as<std::ਸਦਿਸ਼<std::string>>())
    			logger().ਜੋੜੋ_experimental_ignore(ignore);
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("perffile")) perffile = ਨਤੀਜਾ["perffile"].as<std::string>();
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("infile")) {
			frontend_files = ਨਤੀਜਾ["infile"].as<std::ਸਦਿਸ਼<std::string>>();
		}
		ਜੇ (ਨਤੀਜਾ.ਗਿਣਤੀ_2("autoidx")) {
			ਪੂਰਨ idx = ਨਤੀਜਾ["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		ਜੇ (!is_colored(stdout) || !is_colored(stderr)) {
			ਜੇ (!log_stderr) {
				logger().ਜੋੜੋ_sink<ConsoleLogSink>();
			} ਨਹੀਂਤਾਂ {
				logger().ਜੋੜੋ_sink<StderrLogSink>(quiet_warnings);
			}
		} ਨਹੀਂਤਾਂ
			logger().ਜੋੜੋ_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		ਜੇ (print_banner)
			yosys_banner();

	}
	ਫੜੋ (ਅਚਲ cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::ਸਤਰ_ਅੰਤ;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::ਸਤਰ_ਅੰਤ;
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}

#ਜੇ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#ਜੇ defined(_WIN32)
		ਜੇ (getenv("HOMEDRIVE") != ਨਿਰਾ && getenv("HOMEPATH") != ਨਿਰਾ) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} ਨਹੀਂਤਾਂ {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#ਨਹੀਂਤਾਂ
		ਜੇ (getenv("XDG_STATE_HOME") == ਨਿਰਾ || getenv("XDG_STATE_HOME")[0] == '\0') {
			ਜੇ (getenv("HOME") != ਨਿਰਾ) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} ਨਹੀਂਤਾਂ {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} ਨਹੀਂਤਾਂ {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#ਜੇ_ਅੰਤ

	ਜੇ (!state_dir.ਖਾਲੀ_ਥਾਵਾਂ()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_ਵਿੱਥ = where_history();
	}
#ਜੇ_ਅੰਤ

	ਜੇ (print_stats)
		logger().ਸ਼ੁਰੂ_hasher();

#ਜੇ defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_ਰਸਤਾ);
#ਜੇ_ਅੰਤ

#ਜੇ defined(__linux__)
	// set stack size to >= 128 MB
	{
		ਬਣਤਰ rlimit rl;
		ਅਚਲ rlim_t ਢੇਰ_ਮਾਪ = 128L * 1024L * 1024L;
		ਜੇ (getrlimit(RLIMIT_ਢੇਰ, &rl) == 0 && rl.rlim_cur < ਢੇਰ_ਮਾਪ) {
			rl.rlim_cur = ਢੇਰ_ਮਾਪ;
			setrlimit(RLIMIT_ਢੇਰ, &rl);
		}
	}
#ਜੇ_ਅੰਤ

	yosys_ਤਿਆਰੀ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#ਜੇ_ਅੰਤ
	log_error_atexit = yosys_atexit;

	ਲਈ (ਸਵੈਚਾਲਿਤ &fn : plugin_filenames)
		ਲੱਦੋ_plugin(fn, {});

	log_suppressed();

	ਜੇ (!vlog_ਨਿਰਧਾਰਤ_ਕਰਦਾ.ਖਾਲੀ_ਥਾਵਾਂ()) {
		std::string vdef_cmd = "read -define";
		ਲਈ (ਸਵੈਚਾਲਿਤ vdef : vlog_ਨਿਰਧਾਰਤ_ਕਰਦਾ)
			vdef_cmd += " " + vdef;
		ਲੜੀਵਾਰ_pass(vdef_cmd);
	}

	ਜੇ (scriptfile.ਖਾਲੀ_ਥਾਵਾਂ() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		ਲਈ (ਸਵੈਚਾਲਿਤ special_arg : special_ਦਲੀਲਾਂ)
			frontend_files.ਧੱਕੋ_ਪਿੱਠ(special_arg);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ it = frontend_files.begin(); it != frontend_files.ਸਿਰਾ(); ++it) {
		ਜੇ (ਲੜੀਵਾਰ_frontend((*it).c_str(), frontend_command))
			ਲੜੀਵਾਰ_shell = ਝੂਠ;
	}

	ਜੇ (!topmodule.ਖਾਲੀ_ਥਾਵਾਂ())
		ਲੜੀਵਾਰ_pass("hierarchy -top " + topmodule);
	ਜੇ (!scriptfile.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਜੇ (scriptfile_tcl) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
			ਪੂਰਨ tcl_argc = special_ਦਲੀਲਾਂ.ਮਾਪ();
			std::ਸਦਿਸ਼<Tcl_Obj*> script_ਦਲੀਲਾਂ;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			ਲਈ (ਸਵੈਚਾਲਿਤ arg : special_ਦਲੀਲਾਂ)
				script_ਦਲੀਲਾਂ.ਧੱਕੋ_ਪਿੱਠ(Tcl_NewStringObj(arg.c_str(), arg.ਲੰਮਾਈ()));

			Tcl_Obj_ਸੈੱਟ_Var2(interp, Tcl_NewStringObj("argc", 4), ਨਿਰਾ, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_ਸੈੱਟ_Var2(interp, Tcl_NewStringObj("argv", 4), ਨਿਰਾ, Tcl_NewListObj(tcl_argc, script_ਦਲੀਲਾਂ.ਅੰਕੜੇ()), 0);
			Tcl_Obj_ਸੈੱਟ_Var2(interp, Tcl_NewStringObj("argv0", 5), ਨਿਰਾ, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.ਲੰਮਾਈ()), 0);

			ਜੇ (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ਨਤੀਜਾ(yosys_get_tcl_interp()));
#ਨਹੀਂਤਾਂ
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#ਜੇ_ਅੰਤ
		} ਨਹੀਂਤਾਂ ਜੇ (scriptfile_python) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
			py::list new_argv;
			ਪੂਰਨ py_argc = special_ਦਲੀਲਾਂ.ਮਾਪ() + 1;
			new_argv.append(scriptfile);
			ਲਈ (ਪੂਰਨ i = 1; i < py_argc; ++i)
				new_argv.append(special_ਦਲੀਲਾਂ[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object ਰਸਤਾ = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_ਰਸਤਾ = ਰਸਤਾ(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_ਰਸਤਾ));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			ਜੇ (scriptfp == ਖਾਲੀ_ਸੂਚਕ) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			ਜੇ (Py_ਲੜੀਵਾਰ_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#ਨਹੀਂਤਾਂ
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#ਜੇ_ਅੰਤ
		} ਨਹੀਂਤਾਂ
			ਲੜੀਵਾਰ_frontend(scriptfile, "script");
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ it = passes_commands.begin(); it != passes_commands.ਸਿਰਾ(); it++)
		ਲੜੀਵਾਰ_pass(*it);

	ਜੇ (ਲੜੀਵਾਰ_tcl_shell) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#ਨਹੀਂਤਾਂ
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#ਜੇ_ਅੰਤ
	} ਨਹੀਂਤਾਂ {
		ਜੇ (ਲੜੀਵਾਰ_shell)
			shell(yosys_design);
		ਨਹੀਂਤਾਂ
			ਲੜੀਵਾਰ_backend(output_filename, backend_command);
	}

	yosys_design->check();
	ਲਈ (ਸਵੈਚਾਲਿਤ it : saved_designs)
		it.ਦੂਜਾ->check();
	ਲਈ (ਸਵੈਚਾਲਿਤ it : pushed_designs)
		it->check();

	ਜੇ (!depsfile.ਖਾਲੀ_ਥਾਵਾਂ())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		ਜੇ (f == ਖਾਲੀ_ਸੂਚਕ)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		ਤਾਰਕਿਕ ਪਹਿਲਾ = ਸੱਚ;
		ਲਈ (ਸਵੈਚਾਲਿਤ fn : yosys_output_files) {
			fprintf(f, "%s%s", ਪਹਿਲਾ ? "" : " ", escape_filename_spaces(fn).c_str());
			ਪਹਿਲਾ = ਝੂਠ;
		}
		fprintf(f, ":");
		ਲਈ (ਸਵੈਚਾਲਿਤ fn : yosys_ਨਿਵੇਸ਼_files) {
			ਜੇ (yosys_output_files.ਗਿਣਤੀ_2(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	ਜੇ (print_stats)
	{
		std::string hash = logger().ਅੰਤ_hasher();
		logger().ਸੈੱਟ_log_ਸਮਾਂ(ਝੂਠ);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = ਬਹੁਲਕ_v && !ਬਹੁਲਕ_q;
		ਜੇ (logger().get_warnings_ਕੁੱਲ())
			ਲਘੂਗਣਕ("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_ਕੁੱਲ());

		ਜੇ (Get_ਮਾਪ(logger().get_experimental()) != 0)
			ਲਘੂਗਣਕ("Warnings: %d experimental features used (not excluded with -x).\n", Get_ਮਾਪ(logger().get_experimental()));

		ਜੇ (Get_ਮਾਪ(logger().get_deprecated()) != 0) {
			ਲਘੂਗਣਕ("Deprecated features used:\n");
			ਲਈ (ਸਵੈਚਾਲਿਤ &[feature, earliest_removal] : logger().get_deprecated())
				ਜੇ (earliest_removal.ਖਾਲੀ_ਥਾਵਾਂ())
					ਲਘੂਗਣਕ("    %s\n", feature);
				ਨਹੀਂਤਾਂ
					ਲਘੂਗਣਕ("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
		ਲਘੂਗਣਕ("End of script. Logfile hash: %s\n", hash);
		(ਖਾਲੀ)wall_clock_ਸ਼ੁਰੂ;
#ਨਹੀਂਤਾਂ
		std::string meminfo;
		std::string stats_divider = ", ";

		ਬਣਤਰ rusage ru_ਬਫਰ;
		getrusage(RUSAGE_SELF, &ru_ਬਫਰ);
		ਜੇ (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ਬਣਤਰ rusage ru_ਬਫਰ_children;
			getrusage(RUSAGE_CHILDREN, &ru_ਬਫਰ_children);
			ru_ਬਫਰ.ru_utime.tv_sec += ru_ਬਫਰ_children.ru_utime.tv_sec;
			ru_ਬਫਰ.ru_utime.tv_usec += ru_ਬਫਰ_children.ru_utime.tv_usec;
			ru_ਬਫਰ.ru_stime.tv_sec += ru_ਬਫਰ_children.ru_stime.tv_sec;
			ru_ਬਫਰ.ru_stime.tv_usec += ru_ਬਫਰ_children.ru_stime.tv_usec;
#ਜੇ defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_ਬਫਰ.ru_maxrss = std::max(ru_ਬਫਰ.ru_maxrss, ru_ਬਫਰ_children.ru_maxrss);
#ਜੇ_ਅੰਤ
		}
#ਜੇ defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_ਬਫਰ.ru_maxrss / 1024.0);
#ਜਾਂਜੇ defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_ਬਫਰ.ru_maxrss / (1024.0 * 1024.0));
#ਜੇ_ਅੰਤ
		ਦੁੱਗਣਾ wall_seconds = std::chrono::duration<ਦੁੱਗਣਾ>(
				std::chrono::steady_clock::now() - wall_clock_ਸ਼ੁਰੂ).ਗਿਣਤੀ_2();

		ਲਘੂਗਣਕ("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_ਬਫਰ.ru_utime.tv_sec + 1e-6 * ru_ਬਫਰ.ru_utime.tv_usec,
				ru_ਬਫਰ.ru_stime.tv_sec + 1e-6 * ru_ਬਫਰ.ru_stime.tv_usec, meminfo.c_str());
#ਜੇ_ਅੰਤ
		ਲਘੂਗਣਕ("%s\n", yosys_maybe_ਸੰਸਕਰਣ());

		int64_t ਕੁੱਲ_ns = 0;
		std::ਸੈੱਟ<tuple<int64_t, ਪੂਰਨ, std::string>> timedat;

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : pass_ਰਜਿਸਟਰ)
			ਜੇ (it.ਦੂਜਾ->call_counter) {
				ਸਵੈਚਾਲਿਤ pass_ns = it.ਦੂਜਾ->runtime_ns + 1;
				ਕੁੱਲ_ns += pass_ns;
				timedat.ਪਾਓ(make_tuple(pass_ns, it.ਦੂਜਾ->call_counter, it.ਪਹਿਲਾ));
			}
		ਜੇ (twine_gc_ਗਿਣਤੀ) {
			ਕੁੱਲ_ns += twine_gc_ns;
			timedat.ਪਾਓ(make_tuple(twine_gc_ns, twine_gc_ਗਿਣਤੀ, "twine_gc"));
		}

		ਜੇ (timing_details)
		{
			ਲਘੂਗਣਕ("Time spent:\n");
			ਲਈ (ਸਵੈਚਾਲਿਤ it = timedat.rbegin(); it != timedat.rend(); it++) {
				ਲਘੂਗਣਕ("%5d%% %5d calls %8.3f sec %s\n", ਪੂਰਨ(100*std::get<0>(*it) / ਕੁੱਲ_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		ਨਹੀਂਤਾਂ
		{
			ਪੂਰਨ ਬਾਹਰਲਾ_ਗਿਣਤੀ = 0;
			std::string ਸੁਨੇਹਾ_2 = "Time spent:";
			ਲਈ (ਸਵੈਚਾਲਿਤ it = timedat.rbegin(); it != timedat.rend() && ਬਾਹਰਲਾ_ਗਿਣਤੀ < 4; it++, ਬਾਹਰਲਾ_ਗਿਣਤੀ++) {
				ਜੇ (ਬਾਹਰਲਾ_ਗਿਣਤੀ >= 2 && (std::get<0>(*it) < 1000000000 || ਪੂਰਨ(100*std::get<0>(*it) / ਕੁੱਲ_ns) < 20)) {
					ਸੁਨੇਹਾ_2 += ", ...";
					ਰੋਕੋ;
				}
				ਸੁਨੇਹਾ_2 += stringf("%s %d%% %dx %s (%d sec)", ਬਾਹਰਲਾ_ਗਿਣਤੀ ? "," : "", ਪੂਰਨ(100*std::get<0>(*it) / ਕੁੱਲ_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), ਪੂਰਨ(std::get<0>(*it) / 1000000000));
			}
			ਸੁਨੇਹਾ_2 += ਬਾਹਰਲਾ_ਗਿਣਤੀ ? "" : " no commands executed";
			ਲਘੂਗਣਕ("%s\n", ਸੁਨੇਹਾ_2);
		}
		ਜੇ(!perffile.ਖਾਲੀ_ਥਾਵਾਂ())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			ਜੇ (f == ਖਾਲੀ_ਸੂਚਕ)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_ਸੰਸਕਰਣ());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", ਕੁੱਲ_ns);
			fprintf(f, "  \"passes\": {");

			ਤਾਰਕਿਕ ਪਹਿਲਾ = ਸੱਚ;
			ਲਈ (ਸਵੈਚਾਲਿਤ it = timedat.rbegin(); it != timedat.rend(); it++) {
				ਜੇ (!ਪਹਿਲਾ)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				ਪਹਿਲਾ = ਝੂਠ;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	ਜੇ (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#ਜੇ defined(_MSC_VER)
	_exit(0);
#ਜਾਂਜੇ defined(_WIN32)
	_Exit(0);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ 0;
}

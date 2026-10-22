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
#ವ್ಯಾಖ್ಯಾನಿಸು CXXOPTS_ಸದಿಶ_DELIMITER '\0'
#ಅನ್ತರ್ಭಾವ "libs/cxxopts/include/cxxopts.hpp"
#ಅನ್ತರ್ಭಾವ <iostream>
#ಅನ್ತರ್ಭಾವ <chrono>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
#  include <tcl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
ನಾಮಾಕಾಶ py = pybind11;
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <inttypes.h>

#ಆದರೆ defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <fmt/color.h>
#ಅನ್ತರ್ಭಾವ <fmt/format.h>

USING_YOSYS_NAMESPACE

#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
ಪೂರ್ಣಾಂಕ yosys_history_ಸ್ಥಾನಾಂತರ = 0;
std::string yosys_history_file;
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ yosys_atexit()
{
#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	ಆದರೆ (!yosys_history_file.ಖಾಲಿಗಳು()) {
#ಆದರೆ defined(YOSYS_ENABLE_READLINE)
		ಆದರೆ (yosys_history_ಸ್ಥಾನಾಂತರ > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_ಸ್ಥಾನಾಂತರ, yosys_history_file.c_str());
		} ಇಲ್ಲದಿದ್ದರೆ
			write_history(yosys_history_file.c_str());
#ಇಲ್ಲದಿದ್ದರೆ
		write_history(yosys_history_file.c_str());
#ಆದರೆ_ಅಂತ್ಯ
	}

	clear_history();
#ಆದರೆ defined(YOSYS_ENABLE_READLINE)
	HIST_ನಮೂದು **hist_list = history_list();
	ಆದರೆ (hist_list != ಖಾಲಿ)
		ಬಿಡುಗಡೆ(hist_list);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(__OpenBSD__)
ನಾಮಾಕಾಶ Yosys {
ಬಾಹ್ಯ ಅಕ್ಷರ *yosys_argv0;
ಬಾಹ್ಯ ಅಕ್ಷರ yosys_ಮಾರ್ಗ[ಮಾರ್ಗ_MAX];
};
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
ನಾಮಾಕಾಶ Yosys {
	ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ yosys_tcl_interp_init(Tcl_Interp *interp);
	ಬಾಹ್ಯ ಶೂನ್ಯ yosys_tcl_activate_repl();
};
#ಆದರೆ_ಅಂತ್ಯ

ನಾಮಾಕಾಶ Yosys {
	ಬಾಹ್ಯ ತಾರ್ಕಿಕ log_stderr_sink_forced;
};

ವರ್ಗ ColorConsoleLogSink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	ಸ್ಪಷ್ಟ ColorConsoleLogSink(ತಾರ್ಕಿಕ err, ತಾರ್ಕಿಕ quiet) : error_output(err), quiet_warnings(quiet) {}
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override;
	ತಾರ್ಕಿಕ should_log(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) ಸ್ಥಿರಾಂಕ override;
	ಶೂನ್ಯ flush() override;
	FILE *file_handle() override { ಹಿಂತಿರುಗಿ error_output ? ಶೂನ್ಯಸೂಚಕ : stdout; }
ಖಾಸಗಿ:
	ತಾರ್ಕಿಕ error_output;
	ತಾರ್ಕಿಕ quiet_warnings;
};

ತಾರ್ಕಿಕ no_color = ಸುಳ್ಳು;

ತಾರ್ಕಿಕ is_colored(FILE *file)
{
	ಆದರೆ (no_color)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

#ಆದರೆ defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD ಬಹುಲಕ;
	ಹಿಂತಿರುಗಿ handle != INVALID_HANDLE_ಮೌಲ್ಯ &&
			Get_Console_ಬಹುಲಕ(handle, &ಬಹುಲಕ);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ isatty(fileno(file));
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ತಾರ್ಕಿಕ ಮುಂದಿನ_print_log = ನಿಜ;

ಶೂನ್ಯ ColorConsoleLogSink::ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ)
{
	FILE *f = (error_output || ಸಂದೇಶ.severity == LogSeverity::Error) ? stderr : stdout;

	ಆದರೆ (logger().get_log_ಸಮಯ() && ಮುಂದಿನ_print_log) {
		ಸ್ವಯಂಚಾಲಿತ elapsed = ಸಂದೇಶ.timestamp - logger().get_initial_ಸಮಯ();
		ಸ್ವಯಂಚಾಲಿತ us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string ಸಮಯ_str = stringf("[%05d.%06d] ", ಪೂರ್ಣಾಂಕ(us.ಎಣಿಕೆ_2() / 1'000'000),ಪೂರ್ಣಾಂಕ(us.ಎಣಿಕೆ_2() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", ಸಮಯ_str);
		ಮುಂದಿನ_print_log = ಸುಳ್ಳು;
	}

	ಆದರೆ (!ಸಂದೇಶ.src.filename.ಖಾಲಿಗಳು())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", ಸಂದೇಶ.src.filename, ಸಂದೇಶ.src.ಆರಂಭ_ಸಾಲು);

	ಆಯ್ಕೆ (ಸಂದೇಶ.severity) {
		ಸ್ಥಿತಿ LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.ಸಂದೇಶ_2);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LogSeverity::Error:
		ಸ್ಥಿತಿ LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.ಸಂದೇಶ_2);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LogSeverity::ಶೀರ್ಷಿಕೆ:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", ಸಂದೇಶ.ಸಂದೇಶ_2);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಸಂದೇಶ.ಸಂದೇಶ_2);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LogSeverity::Comment:
		ಸ್ಥಿತಿ LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಸಂದೇಶ.ಸಂದೇಶ_2);
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ LogSeverity::Info:
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಆದರೆ (!ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2.ಖಾಲಿಗಳು())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", ಸಂದೇಶ.ಪೂರ್ವಪ್ರತ್ಯಯ_2);
			fmt::print(f, "{}", ಸಂದೇಶ.ಸಂದೇಶ_2);
			ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (!ಸಂದೇಶ.ಸಂದೇಶ_2.ಖಾಲಿಗಳು())
		ಮುಂದಿನ_print_log = ಸಂದೇಶ.ಸಂದೇಶ_2.ಹಿಂಭಾಗ() == '\n';
}

ತಾರ್ಕಿಕ ColorConsoleLogSink::should_log(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ (!error_output) || (ಸಂದೇಶ.severity == LogSeverity::Error || ಸಂದೇಶ.severity == LogSeverity::NonFatalError ||
			(ಸಂದೇಶ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
ಪೂರ್ಣಾಂಕ wmain(ಪೂರ್ಣಾಂಕ argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _UCRT
	setlocale(LC_ALL, ".UTF-8");
#ಆದರೆ_ಅಂತ್ಯ
	// Configure the Win32 console to use UTF-8.
	ಗಣ_Console_CP(CP_UTF8);
	ಗಣ_Console_Output_CP(CP_UTF8);

	std::ಸದಿಶ<ಅಕ್ಷರ*> uargs;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < argc; i++) {
		ಪೂರ್ಣಾಂಕ usize = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, wargv[i], -1, ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ);
		ಅಕ್ಷರ *uarg = (ಅಕ್ಷರ *)ಸ್ಮರಣೆ_ಕೊಡು(usize);
		Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, ಖಾಲಿ, ಖಾಲಿ);
		uargs.ತಳ್ಳು_ಹಿಂಭಾಗ(uarg);
	}
	uargs.ತಳ್ಳು_ಹಿಂಭಾಗ(ಖಾಲಿ);
	ಅಕ್ಷರ **argv = uargs.ದತ್ತಾಂಶ();
#ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ವಯಂಚಾಲಿತ wall_clock_ಆರಂಭ = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::ಸದಿಶ<std::string> vlog_ನಿರೂಪಿಸುತ್ತದೆ;
	std::ಸದಿಶ<std::string> passes_commands;
	std::ಸದಿಶ<std::string> frontend_files;
	std::ಸದಿಶ<std::string> plugin_filenames;
	std::ಸದಿಶ<std::string> special_ವಾದಗಳು;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	ತಾರ್ಕಿಕ scriptfile_tcl = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ scriptfile_python = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ print_banner = ನಿಜ;
	ತಾರ್ಕಿಕ print_stats = ನಿಜ;
	ತಾರ್ಕಿಕ call_abort = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ timing_details = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ ಸತತ_shell = ನಿಜ;
	ತಾರ್ಕಿಕ ಸತತ_tcl_shell = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ ಬಹುಲಕ_v = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ ಬಹುಲಕ_q = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ quiet_warnings = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ log_stderr = ಸುಳ್ಳು;
	no_color = std::getenv("NO_COLOR") != ಶೂನ್ಯಸೂಚಕ;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.ಗಣ_ಅಗಲ(SIZE_MAX);

	options.ಕೂಡಿಸು_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<scriptfile>")
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::ಮೌಲ್ಯ<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#ಆದರೆ_ಅಂತ್ಯ // YOSYS_ENABLE_TCL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<script>")
#ಆದರೆ_ಅಂತ್ಯ // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<define>[=<value>]")
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
		("infile", "input files", cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>())
	;
	options.ಕೂಡಿಸು_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::ಮೌಲ್ಯ<ಪೂರ್ಣಾಂಕ>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::ಮೌಲ್ಯ<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.ಕೂಡಿಸು_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::ಮೌಲ್ಯ<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::ಮೌಲ್ಯ<std::ಸದಿಶ<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::ಮೌಲ್ಯ<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	ಆದರೆ (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help") || !strcmp(argv[1], "--help"))) {
		std::ಹೊರಗೆ << options.help() << std::ಸಾಲು_ಅಂತ್ಯ;
		ನಿರ್ಗಮಿಸು(0);
	}
	ಪ್ರಯತ್ನಿಸು {
		// Check for "--" in arguments
		ಸ್ವಯಂಚಾಲಿತ it = std::ಕಂಡುಹಿಡಿ(argv, argv + argc, std::string("--"));
		ಆದರೆ (it != argv + argc) {
			special_ವಾದಗಳು.ನಿಯೋಜಿಸು(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::ದೂರ(argv, it);
		}

		ಸ್ವಯಂಚಾಲಿತ ಫಲಿತಾಂಶ = options.parse(argc, argv);

		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("M")) memhasher_on();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("X")) yosys_xtrace += ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("X");
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("A")) call_abort = ನಿಜ;
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("Q")) print_banner = ಸುಳ್ಳು;
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("T")) print_stats = ಸುಳ್ಳು;
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("no-version")) yosys_write_versions = ಸುಳ್ಳು;
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("V")) {
			std::ಹೊರಗೆ << yosys_ಆವೃತ್ತಿ_str << std::ಸಾಲು_ಅಂತ್ಯ;
			ನಿರ್ಗಮಿಸು(0);
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("git-hash")) {
			std::ಹೊರಗೆ << yosys_git_hash_str << std::ಸಾಲು_ಅಂತ್ಯ;
			ನಿರ್ಗಮಿಸು(0);
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("S")) {
			passes_commands.ತಳ್ಳು_ಹಿಂಭಾಗ("synth");
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("C")) ಸತತ_tcl_shell = ನಿಜ;
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("g")) logger().force_debug_on();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("m")) plugin_filenames = ಫಲಿತಾಂಶ["m"].as<std::ಸದಿಶ<std::string>>();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("f")) frontend_command = ಫಲಿತಾಂಶ["f"].as<std::string>();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("H")) {
			passes_commands.ತಳ್ಳು_ಹಿಂಭಾಗ("help");
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("h")) {
			std::string res = ಫಲಿತಾಂಶ["h"].as<std::string>();
			passes_commands.ತಳ್ಳು_ಹಿಂಭಾಗ("help " + res);
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("b")) {
			backend_command = ಫಲಿತಾಂಶ["b"].as<std::string>();
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("p")) {
			ಸ್ವಯಂಚಾಲಿತ cmds = ಫಲಿತಾಂಶ["p"].as<std::ಸದಿಶ<std::string>>();
			passes_commands.ಸೇರಿಸು(passes_commands.ಕೊನೆ(), cmds.begin(), cmds.ಕೊನೆ());
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("o")) {
			output_filename = ಫಲಿತಾಂಶ["o"].as<std::string>();
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ಕೀಲಿ : {"l", "L"}) {
			ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2(ಕೀಲಿ)) {
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& filename : ಫಲಿತಾಂಶ[ಕೀಲಿ].as<std::ಸದಿಶ<std::string>>()) {
					ಪ್ರಯತ್ನಿಸು {
						logger().ಕೂಡಿಸು_sink<FileLogSink>(filename, ಕೀಲಿ[0] == 'L', ಸುಳ್ಳು);
					} ಹಿಡಿ (ಸ್ಥಿರಾಂಕ std::runtime_error &e) {
 					   std::cerr << e.what();
					   ನಿರ್ಗಮಿಸು(1);
					}
				}
			}
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("q")) {
			ಬಹುಲಕ_q = ನಿಜ;
			ಆದರೆ (log_stderr) quiet_warnings = ನಿಜ;
			log_stderr = ನಿಜ;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("v")) {
			ಬಹುಲಕ_v = ನಿಜ;
			log_stderr = ನಿಜ;
			logger().ಗಣ_verbose_level(ಫಲಿತಾಂಶ["v"].as<ಪೂರ್ಣಾಂಕ>());
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("t")) logger().ಗಣ_log_ಸಮಯ(ನಿಜ);
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("no-color")) no_color = ನಿಜ;
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("d")) timing_details = ನಿಜ;
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ಕೀಲಿ : {"s", "c"}) {
			ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2(ಕೀಲಿ)) {
				scriptfile = ಫಲಿತಾಂಶ[ಕೀಲಿ].as<std::string>();
				scriptfile_tcl = std::string(ಕೀಲಿ) == "c";
				ಸತತ_shell = ಸುಳ್ಳು;
			}
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("y")) {
			scriptfile = ಫಲಿತಾಂಶ["y"].as<std::string>();
			scriptfile_python = ನಿಜ;
			ಸತತ_shell = ಸುಳ್ಳು;
		}
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ಕೀಲಿ : {"W", "w", "e"}) {
			ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2(ಕೀಲಿ)) {
				ಸ್ವಯಂಚಾಲಿತ regexes = ಫಲಿತಾಂಶ[ಕೀಲಿ].as<std::ಸದಿಶ<std::string>>();
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& regex : regexes) {
					ಆದರೆ (std::string(ಕೀಲಿ) == "W")
						logger().ಕೂಡಿಸು_warn(regex);
					ಆದರೆ (std::string(ಕೀಲಿ) == "w")
						logger().ಕೂಡಿಸು_nowarn(regex);
					ಆದರೆ (std::string(ಕೀಲಿ) == "e")
						logger().ಕೂಡಿಸು_werror(regex);
				}
			}
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("r")) topmodule = ಫಲಿತಾಂಶ["r"].as<std::string>();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("D")) vlog_ನಿರೂಪಿಸುತ್ತದೆ = ಫಲಿತಾಂಶ["D"].as<std::ಸದಿಶ<std::string>>();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("P")) {
			ಸ್ವಯಂಚಾಲಿತ dump_ವಾದಗಳು = ಫಲಿತಾಂಶ["P"].as<std::ಸದಿಶ<std::string>>();
			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& arg : dump_ವಾದಗಳು) {
				ಸ್ವಯಂಚಾಲಿತ ಟೋಕನುಗಳು = ವಿಭಜಿಸು_ಟೋಕನುಗಳು(arg, ":");
				ಆದರೆ (!ಟೋಕನುಗಳು.ಖಾಲಿಗಳು() && ಟೋಕನುಗಳು[0] == "ALL") {
					ಆದರೆ (ಟೋಕನುಗಳು.ಗಾತ್ರ_2() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::ಸಾಲು_ಅಂತ್ಯ;
						ನಿರ್ಗಮಿಸು(1);
					}
					logger().ಗಣ_hdump_all(ನಿಜ);
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಆದರೆ (!ಟೋಕನುಗಳು.ಖಾಲಿಗಳು() && !ಟೋಕನುಗಳು[0].ಖಾಲಿಗಳು() && ಟೋಕನುಗಳು[0].ಹಿಂಭಾಗ() == '.')
						ಟೋಕನುಗಳು[0].ತೆಗೆ_ಹಿಂಭಾಗ();
					ಆದರೆ (ಟೋಕನುಗಳು.ಗಾತ್ರ_2() == 1)
						ಟೋಕನುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ("yosys_dump_" + ಟೋಕನುಗಳು[0] + ".il");
					ಆದರೆ (ಟೋಕನುಗಳು.ಗಾತ್ರ_2() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::ಸಾಲು_ಅಂತ್ಯ;
						ನಿರ್ಗಮಿಸು(1);
					}
					logger().ಕೂಡಿಸು_hdump(ಟೋಕನುಗಳು[0],ಟೋಕನುಗಳು[1]);
				}
			}
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("E")) depsfile = ಫಲಿತಾಂಶ["E"].as<std::string>();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("x")) {
			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ignore : ಫಲಿತಾಂಶ["x"].as<std::ಸದಿಶ<std::string>>())
    			logger().ಕೂಡಿಸು_experimental_ignore(ignore);
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("perffile")) perffile = ಫಲಿತಾಂಶ["perffile"].as<std::string>();
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("infile")) {
			frontend_files = ಫಲಿತಾಂಶ["infile"].as<std::ಸದಿಶ<std::string>>();
		}
		ಆದರೆ (ಫಲಿತಾಂಶ.ಎಣಿಕೆ_2("autoidx")) {
			ಪೂರ್ಣಾಂಕ idx = ಫಲಿತಾಂಶ["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		ಆದರೆ (!is_colored(stdout) || !is_colored(stderr)) {
			ಆದರೆ (!log_stderr) {
				logger().ಕೂಡಿಸು_sink<ConsoleLogSink>();
			} ಇಲ್ಲದಿದ್ದರೆ {
				logger().ಕೂಡಿಸು_sink<StderrLogSink>(quiet_warnings);
			}
		} ಇಲ್ಲದಿದ್ದರೆ
			logger().ಕೂಡಿಸು_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		ಆದರೆ (print_banner)
			yosys_banner();

	}
	ಹಿಡಿ (ಸ್ಥಿರಾಂಕ cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::ಸಾಲು_ಅಂತ್ಯ;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::ಸಾಲು_ಅಂತ್ಯ;
		ನಿರ್ಗಮಿಸು(1);
	}

#ಆದರೆ defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#ಆದರೆ defined(_WIN32)
		ಆದರೆ (getenv("HOMEDRIVE") != ಖಾಲಿ && getenv("HOMEPATH") != ಖಾಲಿ) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} ಇಲ್ಲದಿದ್ದರೆ {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (getenv("XDG_STATE_HOME") == ಖಾಲಿ || getenv("XDG_STATE_HOME")[0] == '\0') {
			ಆದರೆ (getenv("HOME") != ಖಾಲಿ) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} ಇಲ್ಲದಿದ್ದರೆ {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (!state_dir.ಖಾಲಿಗಳು()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_ಸ್ಥಾನಾಂತರ = where_history();
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (print_stats)
		logger().ಆರಂಭ_hasher();

#ಆದರೆ defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__linux__)
	// set stack size to >= 128 MB
	{
		ರಚನೆ rlimit rl;
		ಸ್ಥಿರಾಂಕ rlim_t ರಾಶಿ_ಗಾತ್ರ = 128L * 1024L * 1024L;
		ಆದರೆ (getrlimit(RLIMIT_ರಾಶಿ, &rl) == 0 && rl.rlim_cur < ರಾಶಿ_ಗಾತ್ರ) {
			rl.rlim_cur = ರಾಶಿ_ಗಾತ್ರ;
			setrlimit(RLIMIT_ರಾಶಿ, &rl);
		}
	}
#ಆದರೆ_ಅಂತ್ಯ

	yosys_ಸಿದ್ಧತೆ();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#ಆದರೆ_ಅಂತ್ಯ
	log_error_atexit = yosys_atexit;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &fn : plugin_filenames)
		ತುಂಬು_plugin(fn, {});

	log_suppressed();

	ಆದರೆ (!vlog_ನಿರೂಪಿಸುತ್ತದೆ.ಖಾಲಿಗಳು()) {
		std::string vdef_cmd = "read -define";
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ vdef : vlog_ನಿರೂಪಿಸುತ್ತದೆ)
			vdef_cmd += " " + vdef;
		ಸತತ_pass(vdef_cmd);
	}

	ಆದರೆ (scriptfile.ಖಾಲಿಗಳು() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ special_arg : special_ವಾದಗಳು)
			frontend_files.ತಳ್ಳು_ಹಿಂಭಾಗ(special_arg);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = frontend_files.begin(); it != frontend_files.ಕೊನೆ(); ++it) {
		ಆದರೆ (ಸತತ_frontend((*it).c_str(), frontend_command))
			ಸತತ_shell = ಸುಳ್ಳು;
	}

	ಆದರೆ (!topmodule.ಖಾಲಿಗಳು())
		ಸತತ_pass("hierarchy -top " + topmodule);
	ಆದರೆ (!scriptfile.ಖಾಲಿಗಳು()) {
		ಆದರೆ (scriptfile_tcl) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
			ಪೂರ್ಣಾಂಕ tcl_argc = special_ವಾದಗಳು.ಗಾತ್ರ_2();
			std::ಸದಿಶ<Tcl_Obj*> script_ವಾದಗಳು;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ arg : special_ವಾದಗಳು)
				script_ವಾದಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(Tcl_NewStringObj(arg.c_str(), arg.ಉದ್ದಳತೆ()));

			Tcl_Obj_ಗಣ_Var2(interp, Tcl_NewStringObj("argc", 4), ಖಾಲಿ, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_ಗಣ_Var2(interp, Tcl_NewStringObj("argv", 4), ಖಾಲಿ, Tcl_NewListObj(tcl_argc, script_ವಾದಗಳು.ದತ್ತಾಂಶ()), 0);
			Tcl_Obj_ಗಣ_Var2(interp, Tcl_NewStringObj("argv0", 5), ಖಾಲಿ, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.ಉದ್ದಳತೆ()), 0);

			ಆದರೆ (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_ಫಲಿತಾಂಶ(yosys_get_tcl_interp()));
#ಇಲ್ಲದಿದ್ದರೆ
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#ಆದರೆ_ಅಂತ್ಯ
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scriptfile_python) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
			py::list new_argv;
			ಪೂರ್ಣಾಂಕ py_argc = special_ವಾದಗಳು.ಗಾತ್ರ_2() + 1;
			new_argv.append(scriptfile);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < py_argc; ++i)
				new_argv.append(special_ವಾದಗಳು[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object ಮಾರ್ಗ = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_ಮಾರ್ಗ = ಮಾರ್ಗ(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_ಮಾರ್ಗ));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			ಆದರೆ (scriptfp == ಶೂನ್ಯಸೂಚಕ) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			ಆದರೆ (Py_ಸತತ_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#ಇಲ್ಲದಿದ್ದರೆ
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#ಆದರೆ_ಅಂತ್ಯ
		} ಇಲ್ಲದಿದ್ದರೆ
			ಸತತ_frontend(scriptfile, "script");
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = passes_commands.begin(); it != passes_commands.ಕೊನೆ(); it++)
		ಸತತ_pass(*it);

	ಆದರೆ (ಸತತ_tcl_shell) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#ಇಲ್ಲದಿದ್ದರೆ
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#ಆದರೆ_ಅಂತ್ಯ
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (ಸತತ_shell)
			shell(yosys_design);
		ಇಲ್ಲದಿದ್ದರೆ
			ಸತತ_backend(output_filename, backend_command);
	}

	yosys_design->check();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : saved_designs)
		it.ಎರಡನೇ->check();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : pushed_designs)
		it->check();

	ಆದರೆ (!depsfile.ಖಾಲಿಗಳು())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		ಆದರೆ (f == ಶೂನ್ಯಸೂಚಕ)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		ತಾರ್ಕಿಕ ಮೊದಲ = ನಿಜ;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ fn : yosys_output_files) {
			fprintf(f, "%s%s", ಮೊದಲ ? "" : " ", escape_filename_spaces(fn).c_str());
			ಮೊದಲ = ಸುಳ್ಳು;
		}
		fprintf(f, ":");
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ fn : yosys_ಒಳಹರಿವು_files) {
			ಆದರೆ (yosys_output_files.ಎಣಿಕೆ_2(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	ಆದರೆ (print_stats)
	{
		std::string hash = logger().ಮುಕ್ತಾಯ_hasher();
		logger().ಗಣ_log_ಸಮಯ(ಸುಳ್ಳು);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = ಬಹುಲಕ_v && !ಬಹುಲಕ_q;
		ಆದರೆ (logger().get_warnings_ಒಟ್ಟು())
			ಲಘುಗಣಕ("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_ಒಟ್ಟು());

		ಆದರೆ (Get_ಗಾತ್ರ(logger().get_experimental()) != 0)
			ಲಘುಗಣಕ("Warnings: %d experimental features used (not excluded with -x).\n", Get_ಗಾತ್ರ(logger().get_experimental()));

		ಆದರೆ (Get_ಗಾತ್ರ(logger().get_deprecated()) != 0) {
			ಲಘುಗಣಕ("Deprecated features used:\n");
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[feature, earliest_removal] : logger().get_deprecated())
				ಆದರೆ (earliest_removal.ಖಾಲಿಗಳು())
					ಲಘುಗಣಕ("    %s\n", feature);
				ಇಲ್ಲದಿದ್ದರೆ
					ಲಘುಗಣಕ("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
		ಲಘುಗಣಕ("End of script. Logfile hash: %s\n", hash);
		(ಶೂನ್ಯ)wall_clock_ಆರಂಭ;
#ಇಲ್ಲದಿದ್ದರೆ
		std::string meminfo;
		std::string stats_divider = ", ";

		ರಚನೆ rusage ru_ಬಫರ್;
		getrusage(RUSAGE_SELF, &ru_ಬಫರ್);
		ಆದರೆ (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			ರಚನೆ rusage ru_ಬಫರ್_children;
			getrusage(RUSAGE_CHILDREN, &ru_ಬಫರ್_children);
			ru_ಬಫರ್.ru_utime.tv_sec += ru_ಬಫರ್_children.ru_utime.tv_sec;
			ru_ಬಫರ್.ru_utime.tv_usec += ru_ಬಫರ್_children.ru_utime.tv_usec;
			ru_ಬಫರ್.ru_stime.tv_sec += ru_ಬಫರ್_children.ru_stime.tv_sec;
			ru_ಬಫರ್.ru_stime.tv_usec += ru_ಬಫರ್_children.ru_stime.tv_usec;
#ಆದರೆ defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_ಬಫರ್.ru_maxrss = std::max(ru_ಬಫರ್.ru_maxrss, ru_ಬಫರ್_children.ru_maxrss);
#ಆದರೆ_ಅಂತ್ಯ
		}
#ಆದರೆ defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_ಬಫರ್.ru_maxrss / 1024.0);
#ಅಥವಾಆದರೆ defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_ಬಫರ್.ru_maxrss / (1024.0 * 1024.0));
#ಆದರೆ_ಅಂತ್ಯ
		ದ್ವಿಗುಣ wall_seconds = std::chrono::duration<ದ್ವಿಗುಣ>(
				std::chrono::steady_clock::now() - wall_clock_ಆರಂಭ).ಎಣಿಕೆ_2();

		ಲಘುಗಣಕ("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_ಬಫರ್.ru_utime.tv_sec + 1e-6 * ru_ಬಫರ್.ru_utime.tv_usec,
				ru_ಬಫರ್.ru_stime.tv_sec + 1e-6 * ru_ಬಫರ್.ru_stime.tv_usec, meminfo.c_str());
#ಆದರೆ_ಅಂತ್ಯ
		ಲಘುಗಣಕ("%s\n", yosys_maybe_ಆವೃತ್ತಿ());

		int64_t ಒಟ್ಟು_ns = 0;
		std::ಗಣ<tuple<int64_t, ಪೂರ್ಣಾಂಕ, std::string>> timedat;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : pass_ರಿಜಿಸ್ಟರ್)
			ಆದರೆ (it.ಎರಡನೇ->call_counter) {
				ಸ್ವಯಂಚಾಲಿತ pass_ns = it.ಎರಡನೇ->runtime_ns + 1;
				ಒಟ್ಟು_ns += pass_ns;
				timedat.ಸೇರಿಸು(make_tuple(pass_ns, it.ಎರಡನೇ->call_counter, it.ಮೊದಲ));
			}
		ಆದರೆ (twine_gc_ಎಣಿಕೆ) {
			ಒಟ್ಟು_ns += twine_gc_ns;
			timedat.ಸೇರಿಸು(make_tuple(twine_gc_ns, twine_gc_ಎಣಿಕೆ, "twine_gc"));
		}

		ಆದರೆ (timing_details)
		{
			ಲಘುಗಣಕ("Time spent:\n");
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = timedat.rbegin(); it != timedat.rend(); it++) {
				ಲಘುಗಣಕ("%5d%% %5d calls %8.3f sec %s\n", ಪೂರ್ಣಾಂಕ(100*std::get<0>(*it) / ಒಟ್ಟು_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಎಣಿಕೆ = 0;
			std::string ಸಂದೇಶ_2 = "Time spent:";
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = timedat.rbegin(); it != timedat.rend() && ಹೊರಹೋಗು_ಎಣಿಕೆ < 4; it++, ಹೊರಹೋಗು_ಎಣಿಕೆ++) {
				ಆದರೆ (ಹೊರಹೋಗು_ಎಣಿಕೆ >= 2 && (std::get<0>(*it) < 1000000000 || ಪೂರ್ಣಾಂಕ(100*std::get<0>(*it) / ಒಟ್ಟು_ns) < 20)) {
					ಸಂದೇಶ_2 += ", ...";
					ನಿಲ್ಲಿಸು;
				}
				ಸಂದೇಶ_2 += stringf("%s %d%% %dx %s (%d sec)", ಹೊರಹೋಗು_ಎಣಿಕೆ ? "," : "", ಪೂರ್ಣಾಂಕ(100*std::get<0>(*it) / ಒಟ್ಟು_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), ಪೂರ್ಣಾಂಕ(std::get<0>(*it) / 1000000000));
			}
			ಸಂದೇಶ_2 += ಹೊರಹೋಗು_ಎಣಿಕೆ ? "" : " no commands executed";
			ಲಘುಗಣಕ("%s\n", ಸಂದೇಶ_2);
		}
		ಆದರೆ(!perffile.ಖಾಲಿಗಳು())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			ಆದರೆ (f == ಶೂನ್ಯಸೂಚಕ)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_ಆವೃತ್ತಿ());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", ಒಟ್ಟು_ns);
			fprintf(f, "  \"passes\": {");

			ತಾರ್ಕಿಕ ಮೊದಲ = ನಿಜ;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = timedat.rbegin(); it != timedat.rend(); it++) {
				ಆದರೆ (!ಮೊದಲ)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				ಮೊದಲ = ಸುಳ್ಳು;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	ಆದರೆ (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#ಆದರೆ defined(_MSC_VER)
	_exit(0);
#ಅಥವಾಆದರೆ defined(_WIN32)
	_Exit(0);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ 0;
}

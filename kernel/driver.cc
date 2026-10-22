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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/hashlib.h"
#परिभाषय CXXOPTS_सदिशम्_DELIMITER '\0'
#अन्तर्भाव "libs/cxxopts/include/cxxopts.hpp"
#अन्तर्भाव <iostream>
#अन्तर्भाव <chrono>

#यदि_परिभाषितम् YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_TCL
#  include <tcl.h>
#यदि_अन्तः

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
नामाकाश py = pybind11;
#यदि_अन्तः

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव <fcntl.h>
#यदि_अपरिभाषितम् __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#यदि_अन्तः
#अन्तर्भाव <inttypes.h>

#यदि defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#यदि_अन्तः

#यदि_परिभाषितम् __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#यदि_अन्तः

#यदि !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#यदि_अन्तः

#अन्तर्भाव <fmt/color.h>
#अन्तर्भाव <fmt/format.h>

USING_YOSYS_NAMESPACE

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
पूर्णाङ्क yosys_history_विस्थापनम् = 0;
std::string yosys_history_file;
#यदि_अन्तः

शून्य yosys_atexit()
{
#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	यदि (!yosys_history_file.रिक्तानि()) {
#यदि defined(YOSYS_ENABLE_READLINE)
		यदि (yosys_history_विस्थापनम् > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_विस्थापनम्, yosys_history_file.c_str());
		} अन्यथा
			write_history(yosys_history_file.c_str());
#अन्यथा
		write_history(yosys_history_file.c_str());
#यदि_अन्तः
	}

	clear_history();
#यदि defined(YOSYS_ENABLE_READLINE)
	HIST_प्रविष्टिः **hist_list = history_list();
	यदि (hist_list != रिक्त)
		मुक्त(hist_list);
#यदि_अन्तः
#यदि_अन्तः
}

#यदि defined(__OpenBSD__)
नामाकाश Yosys {
बाह्य वर्ण *yosys_argv0;
बाह्य वर्ण yosys_मार्गः[मार्गः_MAX];
};
#यदि_अन्तः
#यदि_परिभाषितम् YOSYS_ENABLE_TCL
नामाकाश Yosys {
	बाह्य पूर्णाङ्क yosys_tcl_interp_init(Tcl_Interp *interp);
	बाह्य शून्य yosys_tcl_activate_repl();
};
#यदि_अन्तः

नामाकाश Yosys {
	बाह्य तार्किक log_stderr_sink_forced;
};

वर्ग ColorConsoleLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	स्पष्ट ColorConsoleLogSink(तार्किक err, तार्किक quiet) : error_output(err), quiet_warnings(quiet) {}
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override;
	तार्किक should_log(ध्रुव Log_सन्देशः &सन्देशः) ध्रुव override;
	शून्य flush() override;
	FILE *file_handle() override { प्रत्यागम error_output ? शून्यसूचकः : stdout; }
गुप्त:
	तार्किक error_output;
	तार्किक quiet_warnings;
};

तार्किक no_color = असत्य;

तार्किक is_colored(FILE *file)
{
	यदि (no_color)
		प्रत्यागम असत्य;

#यदि defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD बहुलकम्;
	प्रत्यागम handle != INVALID_HANDLE_मानम् &&
			Get_Console_बहुलकम्(handle, &बहुलकम्);
#अन्यथा
	प्रत्यागम isatty(fileno(file));
#यदि_अन्तः
}

शून्य ColorConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

तार्किक अग्र_print_log = सत्य;

शून्य ColorConsoleLogSink::लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः)
{
	FILE *f = (error_output || सन्देशः.severity == LogSeverity::Error) ? stderr : stdout;

	यदि (logger().get_log_time() && अग्र_print_log) {
		स्वचालित elapsed = सन्देशः.timestamp - logger().get_initial_time();
		स्वचालित us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", पूर्णाङ्क(us.गणनम्() / 1'000'000),पूर्णाङ्क(us.गणनम्() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		अग्र_print_log = असत्य;
	}

	यदि (!सन्देशः.src.filename.रिक्तानि())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", सन्देशः.src.filename, सन्देशः.src.start_पङ्क्तिः);

	विकल्प (सन्देशः.severity) {
		पक्ष LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", सन्देशः.उपसर्गः_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", सन्देशः.सन्देशः_2);
			भङ्ग;

		पक्ष LogSeverity::Error:
		पक्ष LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", सन्देशः.उपसर्गः_2);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", सन्देशः.सन्देशः_2);
			भङ्ग;

		पक्ष LogSeverity::शीर्षकम्:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", सन्देशः.उपसर्गः_2);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", सन्देशः.सन्देशः_2);
			भङ्ग;

		पक्ष LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", सन्देशः.उपसर्गः_2, सन्देशः.सन्देशः_2);
			भङ्ग;

		पक्ष LogSeverity::Comment:
		पक्ष LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", सन्देशः.उपसर्गः_2, सन्देशः.सन्देशः_2);
			भङ्ग;

		पक्ष LogSeverity::Info:
		स्वभाव:
			यदि (!सन्देशः.उपसर्गः_2.रिक्तानि())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", सन्देशः.उपसर्गः_2);
			fmt::print(f, "{}", सन्देशः.सन्देशः_2);
			भङ्ग;
	}
	यदि (!सन्देशः.सन्देशः_2.रिक्तानि())
		अग्र_print_log = सन्देशः.सन्देशः_2.पृष्ठभागः() == '\n';
}

तार्किक ColorConsoleLogSink::should_log(ध्रुव Log_सन्देशः &सन्देशः) ध्रुव
{
	प्रत्यागम (!error_output) || (सन्देशः.severity == LogSeverity::Error || सन्देशः.severity == LogSeverity::NonFatalError ||
			(सन्देशः.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#यदि_परिभाषितम् _WIN32
पूर्णाङ्क wmain(पूर्णाङ्क argc, wchar_t **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#यदि_परिभाषितम् _UCRT
	setlocale(LC_ALL, ".UTF-8");
#यदि_अन्तः
	// Configure the Win32 console to use UTF-8.
	समुच्चयः_Console_CP(CP_UTF8);
	समुच्चयः_Console_Output_CP(CP_UTF8);

	std::सदिशम्<वर्ण*> uargs;
	प्रति (पूर्णाङ्क i = 0; i < argc; i++) {
		पूर्णाङ्क usize = Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, wargv[i], -1, रिक्त, 0, रिक्त, रिक्त);
		वर्ण *uarg = (वर्ण *)स्मृतिं_देहि(usize);
		Wide_Char_गन्तव्यम्_Multi_Byte(CP_UTF8, 0, wargv[i], -1, uarg, usize, रिक्त, रिक्त);
		uargs.प्रक्षिप_पृष्ठभागः(uarg);
	}
	uargs.प्रक्षिप_पृष्ठभागः(रिक्त);
	वर्ण **argv = uargs.दत्त();
#अन्यथा
पूर्णाङ्क मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
#यदि_अन्तः
	स्वचालित wall_clock_start = std::chrono::steady_clock::now();
	std::string frontend_command = "auto";
	std::string backend_command = "auto";
	std::सदिशम्<std::string> vlog_परिभाषते;
	std::सदिशम्<std::string> passes_commands;
	std::सदिशम्<std::string> frontend_files;
	std::सदिशम्<std::string> plugin_filenames;
	std::सदिशम्<std::string> special_तर्काः;
	std::string output_filename = "";
	std::string scriptfile = "";
	std::string depsfile = "";
	std::string topmodule = "";
	std::string perffile = "";
	तार्किक scriptfile_tcl = असत्य;
	तार्किक scriptfile_python = असत्य;
	तार्किक print_banner = सत्य;
	तार्किक print_stats = सत्य;
	तार्किक call_abort = असत्य;
	तार्किक timing_details = असत्य;
	तार्किक सततम्_shell = सत्य;
	तार्किक सततम्_tcl_shell = असत्य;
	तार्किक बहुलकम्_v = असत्य;
	तार्किक बहुलकम्_q = असत्य;
	तार्किक quiet_warnings = असत्य;
	तार्किक log_stderr = असत्य;
	no_color = std::getenv("NO_COLOR") != शून्यसूचकः;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.समुच्चयः_विस्तारः(SIZE_MAX);

	options.सङ्कलय_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::मानम्<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::मानम्<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::मानम्<std::string>(), "<scriptfile>")
#यदि_परिभाषितम् YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::मानम्<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#यदि_अन्तः // YOSYS_ENABLE_TCL
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::मानम्<std::string>(), "<script>")
#यदि_अन्तः // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::मानम्<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::मानम्<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::मानम्<std::सदिशम्<std::string>>())
	;
	options.सङ्कलय_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::मानम्<पूर्णाङ्क>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::मानम्<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::मानम्<std::सदिशम्<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::मानम्<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.सङ्कलय_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::मानम्<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::मानम्<std::सदिशम्<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::मानम्<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	यदि (argc == 2 && (!वाक्यं_तुलय(argv[1], "-h") || !वाक्यं_तुलय(argv[1], "-help") || !वाक्यं_तुलय(argv[1], "--help"))) {
		std::निर्गमः << options.help() << std::पङ्क्त्यन्तः;
		निर्गच्छ(0);
	}
	यत्न {
		// Check for "--" in arguments
		स्वचालित it = std::find(argv, argv + argc, std::string("--"));
		यदि (it != argv + argc) {
			special_तर्काः.नियोजय(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		स्वचालित परिणाम = options.parse(argc, argv);

		यदि (परिणाम.गणनम्("M")) memhasher_on();
		यदि (परिणाम.गणनम्("X")) yosys_xtrace += परिणाम.गणनम्("X");
		यदि (परिणाम.गणनम्("A")) call_abort = सत्य;
		यदि (परिणाम.गणनम्("Q")) print_banner = असत्य;
		यदि (परिणाम.गणनम्("T")) print_stats = असत्य;
		यदि (परिणाम.गणनम्("no-version")) yosys_write_versions = असत्य;
		यदि (परिणाम.गणनम्("V")) {
			std::निर्गमः << yosys_संस्करणम्_str << std::पङ्क्त्यन्तः;
			निर्गच्छ(0);
		}
		यदि (परिणाम.गणनम्("git-hash")) {
			std::निर्गमः << yosys_git_hash_str << std::पङ्क्त्यन्तः;
			निर्गच्छ(0);
		}
		यदि (परिणाम.गणनम्("S")) {
			passes_commands.प्रक्षिप_पृष्ठभागः("synth");
			सततम्_shell = असत्य;
		}
		यदि (परिणाम.गणनम्("C")) सततम्_tcl_shell = सत्य;
		यदि (परिणाम.गणनम्("g")) logger().force_debug_on();
		यदि (परिणाम.गणनम्("m")) plugin_filenames = परिणाम["m"].as<std::सदिशम्<std::string>>();
		यदि (परिणाम.गणनम्("f")) frontend_command = परिणाम["f"].as<std::string>();
		यदि (परिणाम.गणनम्("H")) {
			passes_commands.प्रक्षिप_पृष्ठभागः("help");
			सततम्_shell = असत्य;
		}
		यदि (परिणाम.गणनम्("h")) {
			std::string res = परिणाम["h"].as<std::string>();
			passes_commands.प्रक्षिप_पृष्ठभागः("help " + res);
			सततम्_shell = असत्य;
		}
		यदि (परिणाम.गणनम्("b")) {
			backend_command = परिणाम["b"].as<std::string>();
			सततम्_shell = असत्य;
		}
		यदि (परिणाम.गणनम्("p")) {
			स्वचालित cmds = परिणाम["p"].as<std::सदिशम्<std::string>>();
			passes_commands.निवेशय(passes_commands.अन्तः(), cmds.begin(), cmds.अन्तः());
			सततम्_shell = असत्य;
		}
		यदि (परिणाम.गणनम्("o")) {
			output_filename = परिणाम["o"].as<std::string>();
			सततम्_shell = असत्य;
		}
		प्रति (ध्रुव स्वचालित& कुञ्चिका : {"l", "L"}) {
			यदि (परिणाम.गणनम्(कुञ्चिका)) {
				प्रति (ध्रुव स्वचालित& filename : परिणाम[कुञ्चिका].as<std::सदिशम्<std::string>>()) {
					यत्न {
						logger().सङ्कलय_sink<FileLogSink>(filename, कुञ्चिका[0] == 'L', असत्य);
					} गृहाण (ध्रुव std::runtime_error &e) {
 					   std::cerr << e.what();
					   निर्गच्छ(1);
					}
				}
			}
		}
		यदि (परिणाम.गणनम्("q")) {
			बहुलकम्_q = सत्य;
			यदि (log_stderr) quiet_warnings = सत्य;
			log_stderr = सत्य;
		}
		यदि (परिणाम.गणनम्("v")) {
			बहुलकम्_v = सत्य;
			log_stderr = सत्य;
			logger().समुच्चयः_verbose_level(परिणाम["v"].as<पूर्णाङ्क>());
		}
		यदि (परिणाम.गणनम्("t")) logger().समुच्चयः_log_time(सत्य);
		यदि (परिणाम.गणनम्("no-color")) no_color = सत्य;
		यदि (परिणाम.गणनम्("d")) timing_details = सत्य;
		प्रति (ध्रुव स्वचालित& कुञ्चिका : {"s", "c"}) {
			यदि (परिणाम.गणनम्(कुञ्चिका)) {
				scriptfile = परिणाम[कुञ्चिका].as<std::string>();
				scriptfile_tcl = std::string(कुञ्चिका) == "c";
				सततम्_shell = असत्य;
			}
		}
		यदि (परिणाम.गणनम्("y")) {
			scriptfile = परिणाम["y"].as<std::string>();
			scriptfile_python = सत्य;
			सततम्_shell = असत्य;
		}
		प्रति (ध्रुव स्वचालित& कुञ्चिका : {"W", "w", "e"}) {
			यदि (परिणाम.गणनम्(कुञ्चिका)) {
				स्वचालित regexes = परिणाम[कुञ्चिका].as<std::सदिशम्<std::string>>();
				प्रति (ध्रुव स्वचालित& regex : regexes) {
					यदि (std::string(कुञ्चिका) == "W")
						logger().सङ्कलय_warn(regex);
					यदि (std::string(कुञ्चिका) == "w")
						logger().सङ्कलय_nowarn(regex);
					यदि (std::string(कुञ्चिका) == "e")
						logger().सङ्कलय_werror(regex);
				}
			}
		}
		यदि (परिणाम.गणनम्("r")) topmodule = परिणाम["r"].as<std::string>();
		यदि (परिणाम.गणनम्("D")) vlog_परिभाषते = परिणाम["D"].as<std::सदिशम्<std::string>>();
		यदि (परिणाम.गणनम्("P")) {
			स्वचालित dump_तर्काः = परिणाम["P"].as<std::सदिशम्<std::string>>();
			प्रति (ध्रुव स्वचालित& arg : dump_तर्काः) {
				स्वचालित tokens = विभज_tokens(arg, ":");
				यदि (!tokens.रिक्तानि() && tokens[0] == "ALL") {
					यदि (tokens.आकार() != 1) {
						std::cerr << "Invalid number of tokens in -P ALL." << std::पङ्क्त्यन्तः;
						निर्गच्छ(1);
					}
					logger().समुच्चयः_hdump_all(सत्य);
				} अन्यथा {
					यदि (!tokens.रिक्तानि() && !tokens[0].रिक्तानि() && tokens[0].पृष्ठभागः() == '.')
						tokens[0].निष्कासय_पृष्ठभागः();
					यदि (tokens.आकार() == 1)
						tokens.प्रक्षिप_पृष्ठभागः("yosys_dump_" + tokens[0] + ".il");
					यदि (tokens.आकार() != 2) {
						std::cerr << "Invalid number of tokens in -P." << std::पङ्क्त्यन्तः;
						निर्गच्छ(1);
					}
					logger().सङ्कलय_hdump(tokens[0],tokens[1]);
				}
			}
		}
		यदि (परिणाम.गणनम्("E")) depsfile = परिणाम["E"].as<std::string>();
		यदि (परिणाम.गणनम्("x")) {
			प्रति (ध्रुव स्वचालित &ignore : परिणाम["x"].as<std::सदिशम्<std::string>>())
    			logger().सङ्कलय_experimental_ignore(ignore);
		}
		यदि (परिणाम.गणनम्("perffile")) perffile = परिणाम["perffile"].as<std::string>();
		यदि (परिणाम.गणनम्("infile")) {
			frontend_files = परिणाम["infile"].as<std::सदिशम्<std::string>>();
		}
		यदि (परिणाम.गणनम्("autoidx")) {
			पूर्णाङ्क idx = परिणाम["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		यदि (!is_colored(stdout) || !is_colored(stderr)) {
			यदि (!log_stderr) {
				logger().सङ्कलय_sink<ConsoleLogSink>();
			} अन्यथा {
				logger().सङ्कलय_sink<StderrLogSink>(quiet_warnings);
			}
		} अन्यथा
			logger().सङ्कलय_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		यदि (print_banner)
			yosys_banner();

	}
	गृहाण (ध्रुव cxxopts::exceptions::parsing& e) {
		std::cerr << "Error parsing options: " << e.what() << std::पङ्क्त्यन्तः;
		std::cerr << "Run '" << argv[0] << " --help' for help." << std::पङ्क्त्यन्तः;
		निर्गच्छ(1);
	}

#यदि defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#यदि defined(_WIN32)
		यदि (getenv("HOMEDRIVE") != रिक्त && getenv("HOMEPATH") != रिक्त) {
			state_dir = stringf("%s%s/.local/state", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
		} अन्यथा {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#अन्यथा
		यदि (getenv("XDG_STATE_HOME") == रिक्त || getenv("XDG_STATE_HOME")[0] == '\0') {
			यदि (getenv("HOME") != रिक्त) {
				state_dir = stringf("%s/.local/state", getenv("HOME"));
			} अन्यथा {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} अन्यथा {
			state_dir = stringf("%s", getenv("XDG_STATE_HOME"));
		}
	#यदि_अन्तः

	यदि (!state_dir.रिक्तानि()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_विस्थापनम् = where_history();
	}
#यदि_अन्तः

	यदि (print_stats)
		logger().start_hasher();

#यदि defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_मार्गः);
#यदि_अन्तः

#यदि defined(__linux__)
	// set stack size to >= 128 MB
	{
		संरचना rlimit rl;
		ध्रुव rlim_t राशिः_आकार = 128L * 1024L * 1024L;
		यदि (getrlimit(RLIMIT_राशिः, &rl) == 0 && rl.rlim_cur < राशिः_आकार) {
			rl.rlim_cur = राशिः_आकार;
			setrlimit(RLIMIT_राशिः, &rl);
		}
	}
#यदि_अन्तः

	yosys_सज्जता();
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#यदि_अन्तः
	log_error_atexit = yosys_atexit;

	प्रति (स्वचालित &fn : plugin_filenames)
		आरोपय_plugin(fn, {});

	log_suppressed();

	यदि (!vlog_परिभाषते.रिक्तानि()) {
		std::string vdef_cmd = "read -define";
		प्रति (स्वचालित vdef : vlog_परिभाषते)
			vdef_cmd += " " + vdef;
		सततम्_pass(vdef_cmd);
	}

	यदि (scriptfile.रिक्तानि() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		प्रति (स्वचालित special_arg : special_तर्काः)
			frontend_files.प्रक्षिप_पृष्ठभागः(special_arg);
	}

	प्रति (स्वचालित it = frontend_files.begin(); it != frontend_files.अन्तः(); ++it) {
		यदि (सततम्_frontend((*it).c_str(), frontend_command))
			सततम्_shell = असत्य;
	}

	यदि (!topmodule.रिक्तानि())
		सततम्_pass("hierarchy -top " + topmodule);
	यदि (!scriptfile.रिक्तानि()) {
		यदि (scriptfile_tcl) {
#यदि_परिभाषितम् YOSYS_ENABLE_TCL
			पूर्णाङ्क tcl_argc = special_तर्काः.आकार();
			std::सदिशम्<Tcl_Obj*> script_तर्काः;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			प्रति (स्वचालित arg : special_तर्काः)
				script_तर्काः.प्रक्षिप_पृष्ठभागः(Tcl_NewStringObj(arg.c_str(), arg.दीर्घता_2()));

			Tcl_Obj_समुच्चयः_Var2(interp, Tcl_NewStringObj("argc", 4), रिक्त, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_Obj_समुच्चयः_Var2(interp, Tcl_NewStringObj("argv", 4), रिक्त, Tcl_NewListObj(tcl_argc, script_तर्काः.दत्त()), 0);
			Tcl_Obj_समुच्चयः_Var2(interp, Tcl_NewStringObj("argv0", 5), रिक्त, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.दीर्घता_2()), 0);

			यदि (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_परिणाम(yosys_get_tcl_interp()));
#अन्यथा
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#यदि_अन्तः
		} अन्यथा यदि (scriptfile_python) {
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
			py::list new_argv;
			पूर्णाङ्क py_argc = special_तर्काः.आकार() + 1;
			new_argv.append(scriptfile);
			प्रति (पूर्णाङ्क i = 1; i < py_argc; ++i)
				new_argv.append(special_तर्काः[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object मार्गः = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_मार्गः = मार्गः(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_मार्गः));

			FILE *scriptfp = fopen(scriptfile.c_str(), "r");
			यदि (scriptfp == शून्यसूचकः) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			यदि (Py_सततम्_Simple_File(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#अन्यथा
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#यदि_अन्तः
		} अन्यथा
			सततम्_frontend(scriptfile, "script");
	}

	प्रति (स्वचालित it = passes_commands.begin(); it != passes_commands.अन्तः(); it++)
		सततम्_pass(*it);

	यदि (सततम्_tcl_shell) {
#यदि_परिभाषितम् YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#अन्यथा
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#यदि_अन्तः
	} अन्यथा {
		यदि (सततम्_shell)
			shell(yosys_design);
		अन्यथा
			सततम्_backend(output_filename, backend_command);
	}

	yosys_design->check();
	प्रति (स्वचालित it : saved_designs)
		it.second->check();
	प्रति (स्वचालित it : pushed_designs)
		it->check();

	यदि (!depsfile.रिक्तानि())
	{
		FILE *f = fopen(depsfile.c_str(), "wt");
		यदि (f == शून्यसूचकः)
			log_error("Can't open dependencies file for writing: %s\n", strerror(errno));
		तार्किक first = सत्य;
		प्रति (स्वचालित fn : yosys_output_files) {
			fprintf(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = असत्य;
		}
		fprintf(f, ":");
		प्रति (स्वचालित fn : yosys_निवेशः_files) {
			यदि (yosys_output_files.गणनम्(fn) == 0)
				fprintf(f, " %s", escape_filename_spaces(fn).c_str());
		}
		fprintf(f, "\n");
	}

	logger().report_unexpected_error();

	यदि (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().समुच्चयः_log_time(असत्य);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = बहुलकम्_v && !बहुलकम्_q;
		यदि (logger().get_warnings_योग())
			लघुगणकः("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_योग());

		यदि (Get_आकार(logger().get_experimental()) != 0)
			लघुगणकः("Warnings: %d experimental features used (not excluded with -x).\n", Get_आकार(logger().get_experimental()));

		यदि (Get_आकार(logger().get_deprecated()) != 0) {
			लघुगणकः("Deprecated features used:\n");
			प्रति (स्वचालित &[feature, earliest_removal] : logger().get_deprecated())
				यदि (earliest_removal.रिक्तानि())
					लघुगणकः("    %s\n", feature);
				अन्यथा
					लघुगणकः("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#यदि_परिभाषितम् _WIN32
		लघुगणकः("End of script. Logfile hash: %s\n", hash);
		(शून्य)wall_clock_start;
#अन्यथा
		std::string meminfo;
		std::string stats_divider = ", ";

		संरचना rusage ru_बफर;
		getrusage(RUSAGE_SELF, &ru_बफर);
		यदि (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			संरचना rusage ru_बफर_children;
			getrusage(RUSAGE_CHILDREN, &ru_बफर_children);
			ru_बफर.ru_utime.tv_sec += ru_बफर_children.ru_utime.tv_sec;
			ru_बफर.ru_utime.tv_usec += ru_बफर_children.ru_utime.tv_usec;
			ru_बफर.ru_stime.tv_sec += ru_बफर_children.ru_stime.tv_sec;
			ru_बफर.ru_stime.tv_usec += ru_बफर_children.ru_stime.tv_usec;
#यदि defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_बफर.ru_maxrss = std::max(ru_बफर.ru_maxrss, ru_बफर_children.ru_maxrss);
#यदि_अन्तः
		}
#यदि defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_बफर.ru_maxrss / 1024.0);
#नोचेद्यदि defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_बफर.ru_maxrss / (1024.0 * 1024.0));
#यदि_अन्तः
		द्विगुण wall_seconds = std::chrono::duration<द्विगुण>(
				std::chrono::steady_clock::now() - wall_clock_start).गणनम्();

		लघुगणकः("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_बफर.ru_utime.tv_sec + 1e-6 * ru_बफर.ru_utime.tv_usec,
				ru_बफर.ru_stime.tv_sec + 1e-6 * ru_बफर.ru_stime.tv_usec, meminfo.c_str());
#यदि_अन्तः
		लघुगणकः("%s\n", yosys_maybe_संस्करणम्());

		int64_t योग_ns = 0;
		std::समुच्चयः<tuple<int64_t, पूर्णाङ्क, std::string>> timedat;

		प्रति (स्वचालित &it : pass_पञ्जिकाकोशः)
			यदि (it.second->call_गणक) {
				स्वचालित pass_ns = it.second->runtime_ns + 1;
				योग_ns += pass_ns;
				timedat.निवेशय(make_tuple(pass_ns, it.second->call_गणक, it.first));
			}
		यदि (twine_gc_गणनम्) {
			योग_ns += twine_gc_ns;
			timedat.निवेशय(make_tuple(twine_gc_ns, twine_gc_गणनम्, "twine_gc"));
		}

		यदि (timing_details)
		{
			लघुगणकः("Time spent:\n");
			प्रति (स्वचालित it = timedat.rbegin(); it != timedat.rend(); it++) {
				लघुगणकः("%5d%% %5d calls %8.3f sec %s\n", पूर्णाङ्क(100*std::get<0>(*it) / योग_ns),
						std::get<1>(*it), std::get<0>(*it) / 1000000000.0, std::get<2>(*it).c_str());
			}
		}
		अन्यथा
		{
			पूर्णाङ्क बहिर्गमनम्_गणनम् = 0;
			std::string सन्देशः_2 = "Time spent:";
			प्रति (स्वचालित it = timedat.rbegin(); it != timedat.rend() && बहिर्गमनम्_गणनम् < 4; it++, बहिर्गमनम्_गणनम्++) {
				यदि (बहिर्गमनम्_गणनम् >= 2 && (std::get<0>(*it) < 1000000000 || पूर्णाङ्क(100*std::get<0>(*it) / योग_ns) < 20)) {
					सन्देशः_2 += ", ...";
					भङ्ग;
				}
				सन्देशः_2 += stringf("%s %d%% %dx %s (%d sec)", बहिर्गमनम्_गणनम् ? "," : "", पूर्णाङ्क(100*std::get<0>(*it) / योग_ns),
						std::get<1>(*it), std::get<2>(*it).c_str(), पूर्णाङ्क(std::get<0>(*it) / 1000000000));
			}
			सन्देशः_2 += बहिर्गमनम्_गणनम् ? "" : " no commands executed";
			लघुगणकः("%s\n", सन्देशः_2);
		}
		यदि(!perffile.रिक्तानि())
		{
			FILE *f = fopen(perffile.c_str(), "wt");
			यदि (f == शून्यसूचकः)
				log_error("Can't open performance log file for writing: %s\n", strerror(errno));

			fprintf(f, "{\n");
			fprintf(f, "  \"generator\": \"%s\",\n", yosys_maybe_संस्करणम्());
			fprintf(f, "  \"total_ns\": %" PRIu64 ",\n", योग_ns);
			fprintf(f, "  \"passes\": {");

			तार्किक first = सत्य;
			प्रति (स्वचालित it = timedat.rbegin(); it != timedat.rend(); it++) {
				यदि (!first)
					fprintf(f, ",");
				fprintf(f, "\n	\"%s\": {\n", std::get<2>(*it).c_str());
				fprintf(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::get<0>(*it));
				fprintf(f, "	  \"num_calls\": %u\n", std::get<1>(*it));
				fprintf(f, "	}");
				first = असत्य;
			}
			fprintf(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	यदि (call_abort)
		abort();

	log_flush();

	yosys_shutdown();

#यदि defined(_MSC_VER)
	_exit(0);
#नोचेद्यदि defined(_WIN32)
	_Exit(0);
#यदि_अन्तः
	प्रत्यागम 0;
}

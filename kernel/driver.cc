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

#समावेश "kernel/yosys.h"
#समावेश "kernel/hashlib.h"
#घोषणा CXXOPTS_VECTOR_DELIMITER '\0'
#समावेश "libs/cxxopts/include/cxxopts.hpp"
#समावेश <iostream>
#समावेश <chrono>

#अगर_घोषित YOSYS_ENABLE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_EDITLINE
#  include <editline/readline.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_TCL
#  include <tcl.h>
#पूर्ण_अगर

#अगर_घोषित YOSYS_ENABLE_PYTHON
#  include <Python.h>
#  include <pybind11/pybind11.h>
namespace py = pybind11;
#पूर्ण_अगर

#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <limits.h>
#समावेश <errno.h>
#समावेश <fcntl.h>
#अगर_अघोषित __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#पूर्ण_अगर
#समावेश <inttypes.h>

#अगर defined (__linux__) || defined(__FreeBSD__)
#  include <sys/resource.h>
#  include <sys/types.h>
#  include <unistd.h>
#पूर्ण_अगर

#अगर_घोषित __FreeBSD__
#  include <sys/sysctl.h>
#  include <sys/user.h>
#पूर्ण_अगर

#अगर !defined(_WIN32) || defined(__MINGW32__)
#  include <unistd.h>
#पूर्ण_अगर

#समावेश <fmt/color.h>
#समावेश <fmt/format.h>

USING_YOSYS_NAMESPACE

#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
पूर्णांक yosys_history_offset = 0;
std::string yosys_history_file;
#पूर्ण_अगर

व्योम yosys_atexit()
{
#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	अगर (!yosys_history_file.empty()) {
#अगर defined(YOSYS_ENABLE_READLINE)
		अगर (yosys_history_offset > 0) {
			history_truncate_file(yosys_history_file.c_str(), 100);
			append_history(where_history() - yosys_history_offset, yosys_history_file.c_str());
		} अन्यथा
			write_history(yosys_history_file.c_str());
#अन्यथा
		write_history(yosys_history_file.c_str());
#पूर्ण_अगर
	}

	clear_history();
#अगर defined(YOSYS_ENABLE_READLINE)
	HIST_ENTRY **hist_list = history_list();
	अगर (hist_list != शून्य)
		मुक्त(hist_list);
#पूर्ण_अगर
#पूर्ण_अगर
}

#अगर defined(__OpenBSD__)
namespace Yosys {
बाह्य अक्षर *yosys_argv0;
बाह्य अक्षर yosys_path[PATH_MAX];
};
#पूर्ण_अगर
#अगर_घोषित YOSYS_ENABLE_TCL
namespace Yosys {
	बाह्य पूर्णांक yosys_tcl_interp_init(Tcl_Interp *interp);
	बाह्य व्योम yosys_tcl_activate_repl();
};
#पूर्ण_अगर

namespace Yosys {
	बाह्य bool log_stderr_sink_forced;
};

श्रेणी ColorConsoleLogSink : खुला LogSink
{
खुला:
	explicit ColorConsoleLogSink(bool err, bool quiet) : error_output(err), quiet_warnings(quiet) {}
	व्योम log(स्थिर LogMessage &संदेश) override;
	bool should_log(स्थिर LogMessage &संदेश) स्थिर override;
	व्योम सफाई() override;
	खाता *file_handle() override { वापस error_output ? nullptr : मानक_निकास; }
निजी:
	bool error_output;
	bool quiet_warnings;
};

bool no_color = false;

bool is_colored(खाता *file)
{
	अगर (no_color)
		वापस false;

#अगर defined(_WIN32)
	HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file)));
	DWORD mode;
	वापस handle != INVALID_HANDLE_मूल्य &&
			GetConsoleMode(handle, &mode);
#अन्यथा
	वापस isatty(fileno(file));
#पूर्ण_अगर
}

व्योम ColorConsoleLogSink::सफाई()
{
	ख_साफ(मानक_निकास);
	ख_साफ(मानक_त्रुटि);
}

bool अग्र_print_log = true;

व्योम ColorConsoleLogSink::log(स्थिर LogMessage &संदेश)
{
	खाता *f = (error_output || संदेश.severity == LogSeverity::Error) ? मानक_त्रुटि : मानक_निकास;

	अगर (logger().get_log_time() && अग्र_print_log) {
		स्वतः elapsed = संदेश.timestamp - logger().get_initial_time();
		स्वतः us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
		std::string time_str = stringf("[%05d.%06d] ", पूर्णांक(us.गिनती() / 1'000'000),पूर्णांक(us.गिनती() % 1'000'000));
		fmt::print(f, fg(fmt::terminal_color::blue), "{}", time_str);
		अग्र_print_log = false;
	}

	अगर (!संदेश.src.filename.empty())
		fmt::print(f, fg(fmt::terminal_color::bright_cyan), "{}:{}: ", संदेश.src.filename, संदेश.src.start_पंक्ति);

	चयन (संदेश.severity) {
		हाल LogSeverity::Warning:
			fmt::print(f, fg(fmt::terminal_color::bright_yellow), "{}", संदेश.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", संदेश.message);
			अवरोध;

		हाल LogSeverity::Error:
		हाल LogSeverity::NonFatalError:
			fmt::print(f, fg(fmt::terminal_color::bright_red), "{}", संदेश.prefix);
			fmt::print(f, fg(fmt::terminal_color::blue) | fmt::emphasis::bold, "{}", संदेश.message);
			अवरोध;

		हाल LogSeverity::Header:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}", संदेश.prefix);
			fmt::print(f, fg(fmt::terminal_color::white) | fmt::emphasis::bold, "{}", संदेश.message);
			अवरोध;

		हाल LogSeverity::Highlight:
			fmt::print(f, fg(fmt::terminal_color::cyan) | fmt::emphasis::bold, "{}{}", संदेश.prefix, संदेश.message);
			अवरोध;

		हाल LogSeverity::Comment:
		हाल LogSeverity::Debug:
			fmt::print(f, fg(fmt::terminal_color::bright_black), "{}{}", संदेश.prefix, संदेश.message);
			अवरोध;

		हाल LogSeverity::Info:
		शेष:
			अगर (!संदेश.prefix.empty())
				fmt::print(f, fg(fmt::terminal_color::bright_black), "{}", संदेश.prefix);
			fmt::print(f, "{}", संदेश.message);
			अवरोध;
	}
	अगर (!संदेश.message.empty())
		अग्र_print_log = संदेश.message.back() == '\n';
}

bool ColorConsoleLogSink::should_log(स्थिर LogMessage &संदेश) स्थिर
{
	वापस (!error_output) || (संदेश.severity == LogSeverity::Error || संदेश.severity == LogSeverity::NonFatalError ||
			(संदेश.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced);
}

#अगर_घोषित _WIN32
पूर्णांक wmain(पूर्णांक argc, ब_अक्षर_प्रकार **wargv)
{
	// Configure ASCII functions like fopen() to use UTF-8 filenames.
	// See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-170#utf-8-support
#अगर_घोषित _UCRT
	रखो_क्षेत्र(LC_ALL, ".UTF-8");
#पूर्ण_अगर
	// Configure the Win32 console to use UTF-8.
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::vector<अक्षर*> uargs;
	क्रम (पूर्णांक i = 0; i < argc; i++) {
		पूर्णांक usize = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, शून्य, 0, शून्य, शून्य);
		अक्षर *uarg = (अक्षर *)दो_स्मृति(usize);
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uarg, usize, शून्य, शून्य);
		uargs.धकेलो_back(uarg);
	}
	uargs.धकेलो_back(शून्य);
	अक्षर **argv = uargs.तथ्य();
#अन्यथा
पूर्णांक मुख्य(पूर्णांक argc, अक्षर **argv)
{
#पूर्ण_अगर
	स्वतः wall_clock_start = std::chrono::steady_clock::now();
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
	bool scriptfile_tcl = false;
	bool scriptfile_python = false;
	bool print_banner = true;
	bool print_stats = true;
	bool call_abort = false;
	bool timing_details = false;
	bool run_shell = true;
	bool run_tcl_shell = false;
	bool mode_v = false;
	bool mode_q = false;
	bool quiet_warnings = false;
	bool log_stderr = false;
	no_color = std::दो_पर्या("NO_COLOR") != nullptr;

	cxxopts::Options options(argv[0], "Yosys Open SYnthesis Suite");
	options.set_width(SIZE_MAX);

	options.add_options("operation")
		("b,backend", "use <backend> for the output file specified on the command line",
			cxxopts::मूल्य<std::string>(), "<backend>")
		("f,frontend", "use <frontend> for the input files on the command line",
			cxxopts::मूल्य<std::string>(), "<frontend>")
		("s,scriptfile", "execute the commands in <scriptfile>",
			cxxopts::मूल्य<std::string>(), "<scriptfile>")
#अगर_घोषित YOSYS_ENABLE_TCL
		("c,tcl-scriptfile", "execute the commands in the TCL <tcl_scriptfile> (see 'help tcl' for details)",
			cxxopts::मूल्य<std::string>(),"<tcl_scriptfile>")
		("C,tcl-interactive", "enters TCL interactive shell mode")
#पूर्ण_अगर // YOSYS_ENABLE_TCL
#अगर_घोषित YOSYS_ENABLE_PYTHON
		("y,py-scriptfile", "execute the Python <script>",
			cxxopts::मूल्य<std::string>(), "<script>")
#पूर्ण_अगर // YOSYS_ENABLE_PYTHON
		("p,commands", "execute <commands> (to chain commands, separate them with semicolon + whitespace: 'cmd1; cmd2')",
			cxxopts::मूल्य<std::vector<std::string>>(), "<commands>")
		("r,top", "elaborate the specified HDL <top> module",
			cxxopts::मूल्य<std::string>(), "<top>")
		("m,plugin", "load the specified <plugin> module",
			cxxopts::मूल्य<std::vector<std::string>>(), "<plugin>")
		("D,define", "set the specified Verilog define to <value> if supplied via command \"read -define\"",
			cxxopts::मूल्य<std::vector<std::string>>(), "<define>[=<value>]")
		("S,synth", "shortcut for calling the \"synth\" command, a default script for transforming " \
					"the Verilog input to a gate-level netlist. For example: " \
					"yosys -o output.blif -S input.v " \
					"For more complex synthesis jobs it is recommended to use the read_* and write_* " \
					"commands in a script file instead of specifying input and output files on the " \
					"command line.")
		("H", "print the command list")
		("h,help", "print this help message. If given, print help for <command>.",
			cxxopts::मूल्य<std::string>(), "[<command>]")
		("V,version", "print version information and exit")
		("git-hash", "print git commit hash and exit")
		("infile", "input files", cxxopts::मूल्य<std::vector<std::string>>())
	;
	options.add_options("logging")
		("Q", "suppress printing of banner (copyright, disclaimer, version)")
		("T", "suppress printing of footer (log hash, version, timing statistics)")
		("no-version", "suppress writing out Yosys version anywhere excluding -V, --version")
		("q,quiet", "quiet operation. Only write warnings and error messages to console. " \
					"Use this option twice to also quiet warning messages")
		("v,verbose", "print log headers up to <level> to the console. " \
                      "Implies -q for everything except the 'End of script.' message.",
			cxxopts::मूल्य<पूर्णांक>(), "<level>")
		("t,timestamp", "annotate all log messages with a time stamp")
		("d,detailed-timing", "print more detailed timing stats at exit")
		("l,logfile", "write log messages to <logfile>",
			cxxopts::मूल्य<std::vector<std::string>>(), "<logfile>")
		("L,line-buffered-logfile", "like -l but open <logfile> in line buffered mode",
			cxxopts::मूल्य<std::vector<std::string>>(), "<logfile>")
		("o,outfile", "write the design to <outfile> on exit",
			cxxopts::मूल्य<std::string>(), "<outfile>")
		("P,dump-design", "dump the design when printing the specified log header to a file. " \
						   "yosys_dump_<header_id>.il is used as filename if none is specified. " \
						   "Use 'ALL' as <header_id> to dump at every header.",
						   cxxopts::मूल्य<std::vector<std::string>>(), "<header_id>[:<filename>]")
		("W,warning-as-warning", "print a warning for all log messages matching <regex>",
			cxxopts::मूल्य<std::vector<std::string>>(), "<regex>")
		("w,warning-as-message", "if a warning message matches <regex>, it is printed as regular message instead",
			cxxopts::मूल्य<std::vector<std::string>>(), "<regex>")
		("e,warning-as-error", "if a warning message matches <regex>, it is printed as error message instead",
			cxxopts::मूल्य<std::vector<std::string>>(), "<regex>")
		("E,deps-file", "write a Makefile dependencies file <depsfile> with input and output file names",
			cxxopts::मूल्य<std::string>(), "<depsfile>")
		("no-color", "disable colored output");
	;
	options.add_options("developer")
		("X,trace", "enable tracing of core data structure changes. for debugging")
		("M,randomize-pointers", "will slightly randomize allocated pointer addresses. for debugging")
		("autoidx", "start counting autoidx up from <seed>",
			cxxopts::मूल्य<uint64_t>(), "<idx>")
		("A,abort", "will call abort() at the end of the script. for debugging")
		("x,experimental", "do not print warnings for the experimental <feature>",
			cxxopts::मूल्य<std::vector<std::string>>(), "<feature>")
		("g,debug", "globally enable debug log messages")
		("perffile", "write a JSON performance log to <perffile>", cxxopts::मूल्य<std::string>(), "<perffile>")
	;

	options.parse_positional({"infile"});
	options.positional_help("[<infile> [..]]");

	// We can't have -h optionally require an argument
	// cxxopts does have an implit argument concept but that doesn't work for us
	// cxxopts is therefore instructed allowed to only handle the command help case
	अगर (argc == 2 && (!म_भेद(argv[1], "-h") || !म_भेद(argv[1], "-help") || !म_भेद(argv[1], "--help"))) {
		std::अ_बाहर << options.help() << std::प_पूर्ण;
		निकास(0);
	}
	प्रयास {
		// Check for "--" in arguments
		स्वतः it = std::find(argv, argv + argc, std::string("--"));
		अगर (it != argv + argc) {
			special_args.assign(it + 1, argv + argc);
			// Remove these arguments from cxxopts parsing
			argc = std::distance(argv, it);
		}

		स्वतः परिणाम = options.parse(argc, argv);

		अगर (परिणाम.गिनती("M")) memhasher_on();
		अगर (परिणाम.गिनती("X")) yosys_xtrace += परिणाम.गिनती("X");
		अगर (परिणाम.गिनती("A")) call_abort = true;
		अगर (परिणाम.गिनती("Q")) print_banner = false;
		अगर (परिणाम.गिनती("T")) print_stats = false;
		अगर (परिणाम.गिनती("no-version")) yosys_write_versions = false;
		अगर (परिणाम.गिनती("V")) {
			std::अ_बाहर << yosys_version_str << std::प_पूर्ण;
			निकास(0);
		}
		अगर (परिणाम.गिनती("git-hash")) {
			std::अ_बाहर << yosys_git_hash_str << std::प_पूर्ण;
			निकास(0);
		}
		अगर (परिणाम.गिनती("S")) {
			passes_commands.धकेलो_back("synth");
			run_shell = false;
		}
		अगर (परिणाम.गिनती("C")) run_tcl_shell = true;
		अगर (परिणाम.गिनती("g")) logger().force_debug_on();
		अगर (परिणाम.गिनती("m")) plugin_filenames = परिणाम["m"].as<std::vector<std::string>>();
		अगर (परिणाम.गिनती("f")) frontend_command = परिणाम["f"].as<std::string>();
		अगर (परिणाम.गिनती("H")) {
			passes_commands.धकेलो_back("help");
			run_shell = false;
		}
		अगर (परिणाम.गिनती("h")) {
			std::string res = परिणाम["h"].as<std::string>();
			passes_commands.धकेलो_back("help " + res);
			run_shell = false;
		}
		अगर (परिणाम.गिनती("b")) {
			backend_command = परिणाम["b"].as<std::string>();
			run_shell = false;
		}
		अगर (परिणाम.गिनती("p")) {
			स्वतः cmds = परिणाम["p"].as<std::vector<std::string>>();
			passes_commands.जोड़ो(passes_commands.end(), cmds.begin(), cmds.end());
			run_shell = false;
		}
		अगर (परिणाम.गिनती("o")) {
			output_filename = परिणाम["o"].as<std::string>();
			run_shell = false;
		}
		क्रम (स्थिर स्वतः& कुंजी : {"l", "L"}) {
			अगर (परिणाम.गिनती(कुंजी)) {
				क्रम (स्थिर स्वतः& filename : परिणाम[कुंजी].as<std::vector<std::string>>()) {
					प्रयास {
						logger().add_sink<FileLogSink>(filename, कुंजी[0] == 'L', false);
					} पकड़ो (स्थिर std::runtime_error &e) {
 					   std::अ_त्रुटि << e.what();
					   निकास(1);
					}
				}
			}
		}
		अगर (परिणाम.गिनती("q")) {
			mode_q = true;
			अगर (log_stderr) quiet_warnings = true;
			log_stderr = true;
		}
		अगर (परिणाम.गिनती("v")) {
			mode_v = true;
			log_stderr = true;
			logger().set_verbose_level(परिणाम["v"].as<पूर्णांक>());
		}
		अगर (परिणाम.गिनती("t")) logger().set_log_time(true);
		अगर (परिणाम.गिनती("no-color")) no_color = true;
		अगर (परिणाम.गिनती("d")) timing_details = true;
		क्रम (स्थिर स्वतः& कुंजी : {"s", "c"}) {
			अगर (परिणाम.गिनती(कुंजी)) {
				scriptfile = परिणाम[कुंजी].as<std::string>();
				scriptfile_tcl = std::string(कुंजी) == "c";
				run_shell = false;
			}
		}
		अगर (परिणाम.गिनती("y")) {
			scriptfile = परिणाम["y"].as<std::string>();
			scriptfile_python = true;
			run_shell = false;
		}
		क्रम (स्थिर स्वतः& कुंजी : {"W", "w", "e"}) {
			अगर (परिणाम.गिनती(कुंजी)) {
				स्वतः regexes = परिणाम[कुंजी].as<std::vector<std::string>>();
				क्रम (स्थिर स्वतः& regex : regexes) {
					अगर (std::string(कुंजी) == "W")
						logger().add_warn(regex);
					अगर (std::string(कुंजी) == "w")
						logger().add_nowarn(regex);
					अगर (std::string(कुंजी) == "e")
						logger().add_werror(regex);
				}
			}
		}
		अगर (परिणाम.गिनती("r")) topmodule = परिणाम["r"].as<std::string>();
		अगर (परिणाम.गिनती("D")) vlog_defines = परिणाम["D"].as<std::vector<std::string>>();
		अगर (परिणाम.गिनती("P")) {
			स्वतः dump_args = परिणाम["P"].as<std::vector<std::string>>();
			क्रम (स्थिर स्वतः& arg : dump_args) {
				स्वतः tokens = split_tokens(arg, ":");
				अगर (!tokens.empty() && tokens[0] == "ALL") {
					अगर (tokens.आकार() != 1) {
						std::अ_त्रुटि << "Invalid number of tokens in -P ALL." << std::प_पूर्ण;
						निकास(1);
					}
					logger().set_hdump_all(true);
				} अन्यथा {
					अगर (!tokens.empty() && !tokens[0].empty() && tokens[0].back() == '.')
						tokens[0].निकालो_back();
					अगर (tokens.आकार() == 1)
						tokens.धकेलो_back("yosys_dump_" + tokens[0] + ".il");
					अगर (tokens.आकार() != 2) {
						std::अ_त्रुटि << "Invalid number of tokens in -P." << std::प_पूर्ण;
						निकास(1);
					}
					logger().add_hdump(tokens[0],tokens[1]);
				}
			}
		}
		अगर (परिणाम.गिनती("E")) depsfile = परिणाम["E"].as<std::string>();
		अगर (परिणाम.गिनती("x")) {
			क्रम (स्थिर स्वतः &ignore : परिणाम["x"].as<std::vector<std::string>>())
    			logger().add_experimental_ignore(ignore);
		}
		अगर (परिणाम.गिनती("perffile")) perffile = परिणाम["perffile"].as<std::string>();
		अगर (परिणाम.गिनती("infile")) {
			frontend_files = परिणाम["infile"].as<std::vector<std::string>>();
		}
		अगर (परिणाम.गिनती("autoidx")) {
			पूर्णांक idx = परिणाम["autoidx"].as<uint64_t>();
			autoidx = idx;
		}

		अगर (!is_colored(मानक_निकास) || !is_colored(मानक_त्रुटि)) {
			अगर (!log_stderr) {
				logger().add_sink<ConsoleLogSink>();
			} अन्यथा {
				logger().add_sink<StderrLogSink>(quiet_warnings);
			}
		} अन्यथा
			logger().add_sink<ColorConsoleLogSink>(log_stderr, quiet_warnings);

		अगर (print_banner)
			yosys_banner();

	}
	पकड़ो (स्थिर cxxopts::exceptions::parsing& e) {
		std::अ_त्रुटि << "Error parsing options: " << e.what() << std::प_पूर्ण;
		std::अ_त्रुटि << "Run '" << argv[0] << " --help' for help." << std::प_पूर्ण;
		निकास(1);
	}

#अगर defined(YOSYS_ENABLE_READLINE) || defined(YOSYS_ENABLE_EDITLINE)
	std::string state_dir;
	#अगर defined(_WIN32)
		अगर (दो_पर्या("HOMEDRIVE") != शून्य && दो_पर्या("HOMEPATH") != शून्य) {
			state_dir = stringf("%s%s/.local/state", दो_पर्या("HOMEDRIVE"), दो_पर्या("HOMEPATH"));
		} अन्यथा {
			log_debug("$HOMEDRIVE and/or $HOMEPATH is empty. No history file will be created.\n");
		}
	#अन्यथा
		अगर (दो_पर्या("XDG_STATE_HOME") == शून्य || दो_पर्या("XDG_STATE_HOME")[0] == '\0') {
			अगर (दो_पर्या("HOME") != शून्य) {
				state_dir = stringf("%s/.local/state", दो_पर्या("HOME"));
			} अन्यथा {
				log_debug("$HOME is empty. No history file will be created.\n");
			}
		} अन्यथा {
			state_dir = stringf("%s", दो_पर्या("XDG_STATE_HOME"));
		}
	#पूर्ण_अगर

	अगर (!state_dir.empty()) {
		std::string yosys_dir = state_dir + "/yosys";
		create_directory(yosys_dir);

		yosys_history_file = yosys_dir + "/history";
		read_history(yosys_history_file.c_str());
		yosys_history_offset = where_history();
	}
#पूर्ण_अगर

	अगर (print_stats)
		logger().start_hasher();

#अगर defined(__OpenBSD__)
	// save the executable origin for proc_self_dirname()
	yosys_argv0 = argv[0];
	realpath(yosys_argv0, yosys_path);
#पूर्ण_अगर

#अगर defined(__linux__)
	// set stack size to >= 128 MB
	{
		काष्ठा rlimit rl;
		स्थिर rlim_t ढेर_आकार = 128L * 1024L * 1024L;
		अगर (getrlimit(RLIMIT_ढेर, &rl) == 0 && rl.rlim_cur < ढेर_आकार) {
			rl.rlim_cur = ढेर_आकार;
			setrlimit(RLIMIT_ढेर, &rl);
		}
	}
#पूर्ण_अगर

	yosys_setup();
#अगर_घोषित YOSYS_ENABLE_PYTHON
	py::object sys = py::module_::import("sys");
	sys.attr("path").attr("append")(proc_self_dirname());
	sys.attr("path").attr("append")(proc_share_dirname());
#पूर्ण_अगर
	log_error_atexit = yosys_atexit;

	क्रम (स्वतः &fn : plugin_filenames)
		load_plugin(fn, {});

	log_suppressed();

	अगर (!vlog_defines.empty()) {
		std::string vdef_cmd = "read -define";
		क्रम (स्वतः vdef : vlog_defines)
			vdef_cmd += " " + vdef;
		run_pass(vdef_cmd);
	}

	अगर (scriptfile.empty() || (!scriptfile_tcl && !scriptfile_python)) {
		// Without a TCL or Python script, arguments following '--'
		// are also treated as frontend files
		क्रम (स्वतः special_arg : special_args)
			frontend_files.धकेलो_back(special_arg);
	}

	क्रम (स्वतः it = frontend_files.begin(); it != frontend_files.end(); ++it) {
		अगर (run_frontend((*it).c_str(), frontend_command))
			run_shell = false;
	}

	अगर (!topmodule.empty())
		run_pass("hierarchy -top " + topmodule);
	अगर (!scriptfile.empty()) {
		अगर (scriptfile_tcl) {
#अगर_घोषित YOSYS_ENABLE_TCL
			पूर्णांक tcl_argc = special_args.आकार();
			std::vector<Tcl_Obj*> script_args;
			Tcl_Interp *interp = yosys_get_tcl_interp();
			क्रम (स्वतः arg : special_args)
				script_args.धकेलो_back(Tcl_NewStringObj(arg.c_str(), arg.लंबाई()));

			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argc", 4), शून्य, Tcl_NewIntObj(tcl_argc), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv", 4), शून्य, Tcl_NewListObj(tcl_argc, script_args.तथ्य()), 0);
			Tcl_ObjSetVar2(interp, Tcl_NewStringObj("argv0", 5), शून्य, Tcl_NewStringObj(scriptfile.c_str(), scriptfile.लंबाई()), 0);

			अगर (Tcl_EvalFile(interp, scriptfile.c_str()) != TCL_OK)
				log_error("TCL interpreter returned an error: %s\n", Tcl_Get_String_परिणाम(yosys_get_tcl_interp()));
#अन्यथा
			log_error("Can't execute TCL script: this version of yosys is not built with TCL support enabled.\n");
#पूर्ण_अगर
		} अन्यथा अगर (scriptfile_python) {
#अगर_घोषित YOSYS_ENABLE_PYTHON
			py::list new_argv;
			पूर्णांक py_argc = special_args.आकार() + 1;
			new_argv.append(scriptfile);
			क्रम (पूर्णांक i = 1; i < py_argc; ++i)
				new_argv.append(special_args[i - 1]);

			py::setattr(sys, "argv", new_argv);

			py::object Path = py::module_::import("pathlib").attr("Path");
			py::object scriptfile_python_path = Path(scriptfile).attr("parent");

			sys.attr("path").attr("insert")(0, py::str(scriptfile_python_path));

			खाता *scriptfp = ख_खोलो(scriptfile.c_str(), "r");
			अगर (scriptfp == nullptr) {
				log_error("Failed to open file '%s' for reading.\n", scriptfile);
			}
			अगर (PyRun_SimpleFile(scriptfp, scriptfile.c_str()) != 0) {
				log_flush();
				PyErr_Print();
				log_error("Python interpreter encountered an exception.\n");
			}
#अन्यथा
			log_error("Can't execute Python script: this version of yosys is not built with Python support enabled.\n");
#पूर्ण_अगर
		} अन्यथा
			run_frontend(scriptfile, "script");
	}

	क्रम (स्वतः it = passes_commands.begin(); it != passes_commands.end(); it++)
		run_pass(*it);

	अगर (run_tcl_shell) {
#अगर_घोषित YOSYS_ENABLE_TCL
		yosys_tcl_activate_repl();
		Tcl_Main(argc, argv, yosys_tcl_interp_init);
#अन्यथा
		log_error("Can't exectue TCL shell: this version of yosys is not built with TCL support enabled.\n");
#पूर्ण_अगर
	} अन्यथा {
		अगर (run_shell)
			shell(yosys_design);
		अन्यथा
			run_backend(output_filename, backend_command);
	}

	yosys_design->check();
	क्रम (स्वतः it : saved_designs)
		it.second->check();
	क्रम (स्वतः it : pushed_designs)
		it->check();

	अगर (!depsfile.empty())
	{
		खाता *f = ख_खोलो(depsfile.c_str(), "wt");
		अगर (f == nullptr)
			log_error("Can't open dependencies file for writing: %s\n", म_त्रुटि(त्रुटि_सं));
		bool first = true;
		क्रम (स्वतः fn : yosys_output_files) {
			ख_लिखो(f, "%s%s", first ? "" : " ", escape_filename_spaces(fn).c_str());
			first = false;
		}
		ख_लिखो(f, ":");
		क्रम (स्वतः fn : yosys_निवेश_files) {
			अगर (yosys_output_files.गिनती(fn) == 0)
				ख_लिखो(f, " %s", escape_filename_spaces(fn).c_str());
		}
		ख_लिखो(f, "\n");
	}

	logger().report_unexpected_error();

	अगर (print_stats)
	{
		std::string hash = logger().finish_hasher();
		logger().set_log_time(false);
		yosys_xtrace = 0;
		log_spacer();

		log_stderr_sink_forced = mode_v && !mode_q;
		अगर (logger().get_warnings_योग())
			log("Warnings: %d unique messages, %d total\n", logger().get_warnings_unique(), logger().get_warnings_योग());

		अगर (Get_आकार(logger().get_experimental()) != 0)
			log("Warnings: %d experimental features used (not excluded with -x).\n", Get_आकार(logger().get_experimental()));

		अगर (Get_आकार(logger().get_deprecated()) != 0) {
			log("Deprecated features used:\n");
			क्रम (स्वतः &[feature, earliest_removal] : logger().get_deprecated())
				अगर (earliest_removal.empty())
					log("    %s\n", feature);
				अन्यथा
					log("    %s (may be removed as soon as Yosys %s)\n", feature, earliest_removal);
		}
#अगर_घोषित _WIN32
		log("End of script. Logfile hash: %s\n", hash);
		(व्योम)wall_clock_start;
#अन्यथा
		std::string meminfo;
		std::string stats_divider = ", ";

		काष्ठा rusage ru_बफर;
		getrusage(RUSAGE_SELF, &ru_बफर);
		अगर (yosys_design->scratchpad_get_bool("print_stats.include_children")) {
			काष्ठा rusage ru_बफर_children;
			getrusage(RUSAGE_CHILDREN, &ru_बफर_children);
			ru_बफर.ru_utime.tv_sec += ru_बफर_children.ru_utime.tv_sec;
			ru_बफर.ru_utime.tv_usec += ru_बफर_children.ru_utime.tv_usec;
			ru_बफर.ru_stime.tv_sec += ru_बफर_children.ru_stime.tv_sec;
			ru_बफर.ru_stime.tv_usec += ru_बफर_children.ru_stime.tv_usec;
#अगर defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
			ru_बफर.ru_maxrss = std::max(ru_बफर.ru_maxrss, ru_बफर_children.ru_maxrss);
#पूर्ण_अगर
		}
#अगर defined(__linux__) || defined(__FreeBSD__)
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_बफर.ru_maxrss / 1024.0);
#या_अगर defined(__APPLE__)
// https://stackoverflow.com/questions/59913657/strange-values-of-get-rusage-maxrss-on-macos-and-linux
		meminfo = stringf(", MEM: %.2f MB peak",
				ru_बफर.ru_maxrss / (1024.0 * 1024.0));
#पूर्ण_अगर
		द्विगुन wall_seconds = std::chrono::duration<द्विगुन>(
				std::chrono::steady_clock::now() - wall_clock_start).गिनती();

		log("End of script. Logfile hash: %s%stime: %.2fs, user: %.2fs, system: %.2fs%s\n", hash,
				stats_divider.c_str(), wall_seconds, ru_बफर.ru_utime.tv_sec + 1e-6 * ru_बफर.ru_utime.tv_usec,
				ru_बफर.ru_stime.tv_sec + 1e-6 * ru_बफर.ru_stime.tv_usec, meminfo.c_str());
#पूर्ण_अगर
		log("%s\n", yosys_maybe_version());

		int64_t योग_ns = 0;
		std::set<tuple<int64_t, पूर्णांक, std::string>> timedat;

		क्रम (स्वतः &it : pass_register)
			अगर (it.second->call_गणक) {
				स्वतः pass_ns = it.second->runtime_ns + 1;
				योग_ns += pass_ns;
				timedat.जोड़ो(make_tuple(pass_ns, it.second->call_गणक, it.first));
			}
		अगर (twine_gc_गिनती) {
			योग_ns += twine_gc_ns;
			timedat.जोड़ो(make_tuple(twine_gc_ns, twine_gc_गिनती, "twine_gc"));
		}

		अगर (timing_details)
		{
			log("Time spent:\n");
			क्रम (स्वतः it = timedat.rbegin(); it != timedat.rend(); it++) {
				log("%5d%% %5d calls %8.3f sec %s\n", पूर्णांक(100*std::लो<0>(*it) / योग_ns),
						std::लो<1>(*it), std::लो<0>(*it) / 1000000000.0, std::लो<2>(*it).c_str());
			}
		}
		अन्यथा
		{
			पूर्णांक out_गिनती = 0;
			std::string message = "Time spent:";
			क्रम (स्वतः it = timedat.rbegin(); it != timedat.rend() && out_गिनती < 4; it++, out_गिनती++) {
				अगर (out_गिनती >= 2 && (std::लो<0>(*it) < 1000000000 || पूर्णांक(100*std::लो<0>(*it) / योग_ns) < 20)) {
					message += ", ...";
					अवरोध;
				}
				message += stringf("%s %d%% %dx %s (%d sec)", out_गिनती ? "," : "", पूर्णांक(100*std::लो<0>(*it) / योग_ns),
						std::लो<1>(*it), std::लो<2>(*it).c_str(), पूर्णांक(std::लो<0>(*it) / 1000000000));
			}
			message += out_गिनती ? "" : " no commands executed";
			log("%s\n", message);
		}
		अगर(!perffile.empty())
		{
			खाता *f = ख_खोलो(perffile.c_str(), "wt");
			अगर (f == nullptr)
				log_error("Can't open performance log file for writing: %s\n", म_त्रुटि(त्रुटि_सं));

			ख_लिखो(f, "{\n");
			ख_लिखो(f, "  \"generator\": \"%s\",\n", yosys_maybe_version());
			ख_लिखो(f, "  \"total_ns\": %" PRIu64 ",\n", योग_ns);
			ख_लिखो(f, "  \"passes\": {");

			bool first = true;
			क्रम (स्वतः it = timedat.rbegin(); it != timedat.rend(); it++) {
				अगर (!first)
					ख_लिखो(f, ",");
				ख_लिखो(f, "\n	\"%s\": {\n", std::लो<2>(*it).c_str());
				ख_लिखो(f, "	  \"runtime_ns\": %" PRIu64 ",\n", std::लो<0>(*it));
				ख_लिखो(f, "	  \"num_calls\": %u\n", std::लो<1>(*it));
				ख_लिखो(f, "	}");
				first = false;
			}
			ख_लिखो(f, "\n  }\n}\n");
		}
	}

	logger().check_expected();

	yosys_atexit();

	memhasher_off();
	अगर (call_abort)
		पात();

	log_flush();

	yosys_shutdown();

#अगर defined(_MSC_VER)
	_निकास(0);
#या_अगर defined(_WIN32)
	_Exit(0);
#पूर्ण_अगर
	वापस 0;
}

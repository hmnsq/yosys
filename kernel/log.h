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

#ifndef LOG_H
#define LOG_H

#അന്തര്ഭാവ "kernel/yosys_common.h"

#അന്തര്ഭാവ <chrono>
#അന്തര്ഭാവ <time.h>

#അന്തര്ഭാവ <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#എങ്കിൽ defined(_WIN32)
#  include <intrin.h>
#അല്ലെങ്കിൽ
#  include <sys/time.h>
#  include <sys/resource.h>
#  എങ്കിൽ defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <signal.h>
#  endif
#endif

// from libs/sha1/sha1.h
class SHA1;

YOSYS_NAMESPACE_BEGIN

// YS_DEBUGTRAP is a macro that is functionally equivalent to a breakpoint
// if the platform provides such functionality, and does nothing otherwise.
// If no debugger is attached, it starts a just-in-time debugger if available,
// and crashes the process otherwise.
#എങ്കിൽ defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#അല്ലെങ്കിൽ
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# എങ്കിൽ __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP ഉയർത്തുക(SIGTRAP)
# അല്ലെങ്കിൽ
#  define YS_DEBUGTRAP ചെയ്യുക {} ആകുമ്പോൾ(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#എങ്കിൽ defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING ചെയ്യുക { എങ്കിൽ (IsDebuggerPresent()) DebugBreak(); } ആകുമ്പോൾ(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING ചെയ്യുക { \
		auto old = സൂചന(SIGTRAP, SIG_IGN); ഉയർത്തുക(SIGTRAP); സൂചന(SIGTRAP, old); \
	} ആകുമ്പോൾ(0)
#അല്ലെങ്കിൽ
# define YS_DEBUGTRAP_IF_DEBUGGING ചെയ്യുക {} ആകുമ്പോൾ(0)
#endif

ഘടന log_cmd_error_exception { };

എണ്ണൽ class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

ഘടന LogSourceLocation {
	LogSourceLocation() : filename({}),start_വരി(-1),start_col(-1),end_വരി(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_വരി(-1),start_col(-1),end_വരി(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,പൂർണ്ണസംഖ്യ start_വരി) : filename(filename),start_വരി(start_വരി),start_col(-1),end_വരി(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,പൂർണ്ണസംഖ്യ start_വരി,പൂർണ്ണസംഖ്യ start_col,പൂർണ്ണസംഖ്യ end_വരി,പൂർണ്ണസംഖ്യ end_col) :
		filename(filename),start_വരി(start_വരി),start_col(start_col),end_വരി(end_വരി),end_col(end_col) { }
	std::string filename;
	പൂർണ്ണസംഖ്യ start_വരി;
	പൂർണ്ണസംഖ്യ start_col;
	പൂർണ്ണസംഖ്യ end_വരി;
	പൂർണ്ണസംഖ്യ end_col;
};

ഘടന LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_സന്ദേശം;
};

class LogSink
{
പൊതു:
	മായാ ~LogSink() = സ്ഥിരസ്ഥിതി;

	മായാ bool should_log(സ്ഥിരം LogMessage &) സ്ഥിരം { തിരികെ true; }
	മായാ ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) = 0;
	മായാ ശൂന്യം flush() {}
	// TODO: Remove when AST/read_verilog removed
	മായാ FILE *file_handle() { തിരികെ nullptr; }
};

class LogSinkRef : പൊതു LogSink
{
പൊതു:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(സ്ഥിരം LogMessage &സന്ദേശം) സ്ഥിരം override { തിരികെ sink->should_log(സന്ദേശം); }
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override { sink->log(സന്ദേശം); }
	ശൂന്യം flush() override { sink->flush(); }
	FILE *file_handle() override { തിരികെ sink->file_handle(); }
സ്വകാര്യ:
	LogSink *sink;
};

class FileLogSink : പൊതു LogSink
{
പൊതു:
	explicit FileLogSink(സ്ഥിരം std::string &filename, bool വരി_buffered, bool append);
	~FileLogSink() override;
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override;
	ശൂന്യം flush() override;
	FILE *file_handle() override { തിരികെ file; }

സ്വകാര്യ:
	FILE *file;
};

class ConsoleLogSink : പൊതു LogSink
{
പൊതു:
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override;
	ശൂന്യം flush() override;
	FILE *file_handle() override { തിരികെ stdout; }
};

class StderrLogSink : പൊതു LogSink
{
പൊതു:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(സ്ഥിരം LogMessage &സന്ദേശം) സ്ഥിരം override;
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override;
	ശൂന്യം flush() override;
സ്വകാര്യ:
	bool quiet_warnings;
};

class StreamLogSink : പൊതു LogSink
{
പൊതു:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override { stream << സന്ദേശം.cached_സന്ദേശം; }
	ശൂന്യം flush() override { stream.flush(); }

സ്വകാര്യ:
	std::ostream &stream;
};

class ScratchPadLogSink : പൊതു LogSink
{
പൊതു:
	explicit ScratchPadLogSink(std::string scratchpad);
	ശൂന്യം log(സ്ഥിരം LogMessage &സന്ദേശം) override;

സ്വകാര്യ:
	std::string scratchpad;
};

class LogManager
{
സ്വകാര്യ:
	ഘടന LogExpectedItem
	{
		LogExpectedItem(സ്ഥിരം std::regex &pat, പൂർണ്ണസംഖ്യ expected) :
				pattern(pat), expected_എണ്ണം(expected), current_എണ്ണം(0) {}
		LogExpectedItem() : expected_എണ്ണം(0), current_എണ്ണം(0) {}

		std::regex pattern;
		പൂർണ്ണസംഖ്യ expected_എണ്ണം;
		പൂർണ്ണസംഖ്യ current_എണ്ണം;
	};

പൊതു:
	LogManager() = സ്ഥിരസ്ഥിതി;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.തള്ളുക_back(std::move(sink));
		തിരികെ ref;
	}

	template<typename F>
	ശൂന്യം for_each_sink(F &&func)
	{
		വേണ്ടി (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { തിരികെ sinks.empty(); }
	ശൂന്യം clear() { sinks.clear(); }
	ശൂന്യം clear_original();
	ശൂന്യം flush() { വേണ്ടി (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	പൊതു:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			വേണ്ടി (സ്ഥിരം auto &sink : backup_sinks)
				manager.sinks.തള്ളുക_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(സ്ഥിരം Scoped &) = delete;
		Scoped &operator=(സ്ഥിരം Scoped &) = delete;

	സ്വകാര്യ:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		പൂർണ്ണസംഖ്യ backup_verbose_level;
	};

	Scoped sink_scope()
	{
		തിരികെ Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	പൊതു:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	സ്വകാര്യ:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		തിരികെ ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	പൊതു:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			എങ്കിൽ (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		ശൂന്യം on()
		{
			എങ്കിൽ (status)
				തിരികെ;
			status = true;
			manager.make_debug++;
		}

		ശൂന്യം off_silent()
		{
			എങ്കിൽ (!status)
				തിരികെ;
			status = false;
			manager.make_debug--;
		}

		ശൂന്യം off()
		{
			off_silent();
		}
	സ്വകാര്യ:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		തിരികെ LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	പൊതു:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			എങ്കിൽ (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		ശൂന്യം on()
		{
			എങ്കിൽ (active)
				തിരികെ;

			active = true;
			manager.force_debug++;
		}

		ശൂന്യം off()
		{
			എങ്കിൽ (!active)
				തിരികെ;

			active = false;
			manager.force_debug--;
		}
	സ്വകാര്യ:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		തിരികെ ForceDebug(*this, start_on);
	}

	ശൂന്യം force_debug_on() { force_debug++; }
	ശൂന്യം force_debug_off() { എങ്കിൽ (force_debug > 0) force_debug--; }
	ശൂന്യം set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	ശൂന്യം report_unexpected_error();

	ശൂന്യം add_experimental_ignore(std::string name) { experimental_ignored.തിരുകുക(name); }
	ശൂന്യം add_warn(std::string pattern) { warn_regexes.തള്ളുക_back(YS_REGEX_COMPILE(pattern)); }
	ശൂന്യം add_nowarn(std::string pattern) { nowarn_regexes.തള്ളുക_back(YS_REGEX_COMPILE(pattern)); }
	ശൂന്യം add_werror(std::string pattern) { werror_regexes.തള്ളുക_back(YS_REGEX_COMPILE(pattern)); }
	ശൂന്യം add_expect(std::string type, std::string pattern, പൂർണ്ണസംഖ്യ എണ്ണം);

	ശൂന്യം set_verbose_level(പൂർണ്ണസംഖ്യ level) { verbose_level = level; }
	ശൂന്യം add_verbose_level(പൂർണ്ണസംഖ്യ level) { verbose_level += level; }
	ശൂന്യം set_expect_no_warnings(bool മൂല്യം) { expect_no_warnings = മൂല്യം; }
	ശൂന്യം set_log_time(bool മൂല്യം) { log_time = മൂല്യം; }
	ശൂന്യം set_cmd_error_throw(bool മൂല്യം) { cmd_error_throw = മൂല്യം; }
	ശൂന്യം set_hdump_all(bool മൂല്യം) { hdump_all = മൂല്യം; }
	പൂർണ്ണസംഖ്യ get_verbose_level() സ്ഥിരം { തിരികെ verbose_level; }
	bool get_log_time() സ്ഥിരം { തിരികെ log_time; }
	പൂർണ്ണസംഖ്യ get_warnings_unique() സ്ഥിരം { തിരികെ GetSize(warnings); }
	പൂർണ്ണസംഖ്യ get_warnings_total() സ്ഥിരം { തിരികെ warnings_എണ്ണം; }
	പൂർണ്ണസംഖ്യ get_errors_total() സ്ഥിരം { തിരികെ errors_എണ്ണം; }
	സ്ഥിരം std::set<std::string> &get_experimental() സ്ഥിരം { തിരികെ experimental; }
	സ്ഥിരം std::map<std::string, std::string> &get_deprecated() സ്ഥിരം { തിരികെ deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() സ്ഥിരം;

	ശൂന്യം add_hdump(std::string name, std::string മൂല്യം) { hdump[name].തിരുകുക(മൂല്യം); }

	ശൂന്യം formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	ശൂന്യം formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	ശൂന്യം formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] ശൂന്യം formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	ശൂന്യം formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] ശൂന്യം formatted_cmd_error(std::string_view format, std::string message);
	ശൂന്യം suppressed();
	ശൂന്യം add_experimental(സ്ഥിരം std::string &str);
	ശൂന്യം add_deprecated(സ്ഥിരം std::string &feature, സ്ഥിരം std::string &earliest_removal);
	ശൂന്യം spacer();
	ശൂന്യം തള്ളുക();
	ശൂന്യം എടുക്കുക();

	ശൂന്യം reset_അട്ടി();

	ശൂന്യം check_expected();
	bool expects_error() { തിരികെ (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(പൂർണ്ണസംഖ്യ n = 0) { എങ്കിൽ (force_debug) തിരികെ true; debug_suppressed += n; തിരികെ false; }
#അല്ലെങ്കിൽ
	bool is_debug(പൂർണ്ണസംഖ്യ = 0) { തിരികെ false; }
#endif
	ശൂന്യം start_hasher();
	std::string finish_hasher();

സ്വകാര്യ:
	ശൂന്യം logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	ശൂന്യം error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	പൂർണ്ണസംഖ്യ verbose_level = 0;
	പൂർണ്ണസംഖ്യ newline_എണ്ണം = 0;
	vector<പൂർണ്ണസംഖ്യ> header_എണ്ണം;
	പൂർണ്ണസംഖ്യ errors_എണ്ണം = 0;
	പൂർണ്ണസംഖ്യ warnings_എണ്ണം = 0;
	പൂർണ്ണസംഖ്യ warnings_എണ്ണം_noexpect = 0;
	std::set<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::vector<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, LogExpectedItem> expect_log, expect_warning, expect_error;
	dict<std::string, LogExpectedItem> expect_prefix_log, expect_prefix_warning, expect_prefix_error;
	bool expect_no_warnings = false;
	bool log_time = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::set<std::string>> hdump;
	bool hdump_all = false;

	പൂർണ്ണസംഖ്യ debug_suppressed = 0;
	പൂർണ്ണസംഖ്യ make_debug = 0;
	പൂർണ്ണസംഖ്യ force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

ബാഹ്യ ശൂന്യം (*log_error_atexit)();

ശൂന്യം set_verific_logging(ശൂന്യം (*cb)(പൂർണ്ണസംഖ്യ സന്ദേശം_type, സ്ഥിരം അക്ഷരം *message_id, LogSourceLocation src, സ്ഥിരം അക്ഷരം *സന്ദേശം));
ബാഹ്യ ശൂന്യം (*log_verific_callback)(പൂർണ്ണസംഖ്യ സന്ദേശം_type, സ്ഥിരം അക്ഷരം *message_id, LogSourceLocation src, സ്ഥിരം അക്ഷരം *സന്ദേശം);

#ifndef NDEBUG
സ്ഥിതം വരിയിൽ bool ys_debug(പൂർണ്ണസംഖ്യ n = 0) { തിരികെ logger().is_debug(n); }
#അല്ലെങ്കിൽ
സ്ഥിതം വരിയിൽ bool ys_debug(പൂർണ്ണസംഖ്യ = 0) { തിരികെ false; }
#endif

template <typename... Args>
വരിയിൽ ശൂന്യം log(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
വരിയിൽ ശൂന്യം log_comment(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
വരിയിൽ ശൂന്യം log_highlight(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
വരിയിൽ ശൂന്യം log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) ചെയ്യുക { എങ്കിൽ (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } ആകുമ്പോൾ (0)

template <typename... Args>
വരിയിൽ ശൂന്യം log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
വരിയിൽ ശൂന്യം log_warning(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
വരിയിൽ ശൂന്യം log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

വരിയിൽ ശൂന്യം log_experimental(സ്ഥിരം std::string &str)
{
	logger().add_experimental(str);
}

വരിയിൽ ശൂന്യം log_deprecated(സ്ഥിരം std::string &feature, സ്ഥിരം std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
ശൂന്യം log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ശൂന്യം log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] ശൂന്യം log_error(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] ശൂന്യം log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ശൂന്യം log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] ശൂന്യം log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, സ്ഥിരം Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

വരിയിൽ ശൂന്യം log_suppressed()
{
	logger().suppressed();
}

വരിയിൽ ശൂന്യം log_spacer() { logger().spacer(); }
വരിയിൽ ശൂന്യം log_തള്ളുക() { logger().തള്ളുക(); }
വരിയിൽ ശൂന്യം log_എടുക്കുക() { logger().എടുക്കുക(); }

വരിയിൽ ശൂന്യം log_reset_അട്ടി() { logger().reset_അട്ടി(); }
വരിയിൽ ശൂന്യം log_flush() { logger().flush(); }

ശൂന്യം log_backtrace(സ്ഥിരം അക്ഷരം *prefix, പൂർണ്ണസംഖ്യ levels);


std::string log_signal(സ്ഥിരം RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(സ്ഥിരം RTLIL::Const &മൂല്യം, bool autoint = true);

ശൂന്യം log_module(RTLIL::Module *module, std::string indent = "");
ശൂന്യം log_cell(RTLIL::Cell *cell, std::string indent = "");
ശൂന്യം log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
ശൂന്യം log_abort_internal(സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
ഘടന PerformanceTimer
{
#എങ്കിൽ 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	സ്ഥിതം int64_t query() {
#  ifdef _WIN32
		തിരികെ 0;
#  elif defined(RUSAGE_SELF)
		ഘടന rusage rusage;
		int64_t t = 0;
		വേണ്ടി (പൂർണ്ണസംഖ്യ who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			എങ്കിൽ (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		തിരികെ t;
#  അല്ലെങ്കിൽ
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	ശൂന്യം reset() {
		total_ns = 0;
	}

	ശൂന്യം begin() {
		total_ns -= query();
	}

	ശൂന്യം end() {
		total_ns += query();
	}

	ദശാംശം sec() സ്ഥിരം {
		തിരികെ total_ns * 1e-9f;
	}
#അല്ലെങ്കിൽ
	സ്ഥിതം int64_t query() { തിരികെ 0; }
	ശൂന്യം reset() { }
	ശൂന്യം begin() { }
	ശൂന്യം end() { }
	ദശാംശം sec() സ്ഥിരം { തിരികെ 0; }
#endif
};

// simple API for quickly dumping values when debugging

സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(short v) { log("%d", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(unsigned short v) { log("%u", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(പൂർണ്ണസംഖ്യ v) { log("%d", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(unsigned പൂർണ്ണസംഖ്യ v) { log("%u", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(long പൂർണ്ണസംഖ്യ v) { log("%ld", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(unsigned long പൂർണ്ണസംഖ്യ v) { log("%lu", v); }
#ifndef _WIN32
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(long long പൂർണ്ണസംഖ്യ v) { log("%lld", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(unsigned long long പൂർണ്ണസംഖ്യ v) { log("%lld", v); }
#endif
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(അക്ഷരം c)
{
	എങ്കിൽ (c >= 32 && c < 127) {
		log("'%c'", c);
	} അല്ലെങ്കിൽ {
		log("'\\x%02x'", c);
	}
}
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(unsigned അക്ഷരം c)
{
	എങ്കിൽ (c >= 32 && c < 127) {
		log("'%c'", c);
	} അല്ലെങ്കിൽ {
		log("'\\x%02x'", c);
	}
}
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(ഇരട്ട v) { log("%f", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(അക്ഷരം *v) { log("%s", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(സ്ഥിരം അക്ഷരം *v) { log("%s", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(std::string v) { log("%s", v); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_args_worker(സ്ഥിരം അക്ഷരം *p) { log_assert(*p == 0); }
ശൂന്യം log_dump_val_worker(IdString v);
ശൂന്യം log_dump_val_worker(RTLIL::SigSpec v);
ശൂന്യം log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(dict<K, T> &v);
template<typename K> സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(pool<K> &v);
template<typename K> സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(std::vector<K> &v);
template<typename T> സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(T *ptr);

template<typename K, typename T>
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	വേണ്ടി (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	വേണ്ടി (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	വേണ്ടി (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
സ്ഥിതം വരിയിൽ ശൂന്യം log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
ശൂന്യം log_dump_args_worker(സ്ഥിരം അക്ഷരം *p, T first, Args ... args)
{
	പൂർണ്ണസംഖ്യ next_p_state = 0;
	സ്ഥിരം അക്ഷരം *next_p = p;
	ആകുമ്പോൾ (*next_p && (next_p_state != 0 || *next_p != ',')) {
		എങ്കിൽ (*next_p == '"')
			ചെയ്യുക {
				next_p++;
				ആകുമ്പോൾ (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} ആകുമ്പോൾ (*next_p && *next_p != '"');
		എങ്കിൽ (*next_p == '\'') {
			next_p++;
			എങ്കിൽ (*next_p == '\\')
				next_p++;
			എങ്കിൽ (*next_p)
				next_p++;
		}
		എങ്കിൽ (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		എങ്കിൽ ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", പൂർണ്ണസംഖ്യ(next_p - p), p);
	എങ്കിൽ (*next_p == ',')
		next_p++;
	ആകുമ്പോൾ (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) ചെയ്യുക { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} ആകുമ്പോൾ (0)

YOSYS_NAMESPACE_END

#അന്തര്ഭാവ "kernel/yosys.h"

#endif

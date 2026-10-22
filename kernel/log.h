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

#অন্তর্ভাব "kernel/yosys_common.h"

#অন্তর্ভাব <chrono>
#অন্তর্ভাব <time.h>

#অন্তর্ভাব <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#যদি defined(_WIN32)
#  include <intrin.h>
#অন্যথা
#  include <sys/time.h>
#  include <sys/resource.h>
#  যদি defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#যদি defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#অন্যথা
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# যদি __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP তোলো(SIGTRAP)
# অন্যথা
#  define YS_DEBUGTRAP করো {} যখন(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#যদি defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING করো { যদি (IsDebuggerPresent()) DebugBreak(); } যখন(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING করো { \
		auto old = সংকেত(SIGTRAP, SIG_IGN); তোলো(SIGTRAP); সংকেত(SIGTRAP, old); \
	} যখন(0)
#অন্যথা
# define YS_DEBUGTRAP_IF_DEBUGGING করো {} যখন(0)
#endif

কাঠামো log_cmd_error_exception { };

গণনা class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

কাঠামো LogSourceLocation {
	LogSourceLocation() : filename({}),start_সারি(-1),start_col(-1),end_সারি(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_সারি(-1),start_col(-1),end_সারি(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,পূর্ণসংখ্যা start_সারি) : filename(filename),start_সারি(start_সারি),start_col(-1),end_সারি(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,পূর্ণসংখ্যা start_সারি,পূর্ণসংখ্যা start_col,পূর্ণসংখ্যা end_সারি,পূর্ণসংখ্যা end_col) :
		filename(filename),start_সারি(start_সারি),start_col(start_col),end_সারি(end_সারি),end_col(end_col) { }
	std::string filename;
	পূর্ণসংখ্যা start_সারি;
	পূর্ণসংখ্যা start_col;
	পূর্ণসংখ্যা end_সারি;
	পূর্ণসংখ্যা end_col;
};

কাঠামো LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_বার্তা;
};

class LogSink
{
সর্বজনীন:
	আভাসী ~LogSink() = পূর্বনির্ধারিত;

	আভাসী bool should_log(ধ্রুবক LogMessage &) ধ্রুবক { ফেরত true; }
	আভাসী শূন্য log(ধ্রুবক LogMessage &বার্তা) = 0;
	আভাসী শূন্য flush() {}
	// TODO: Remove when AST/read_verilog removed
	আভাসী FILE *file_handle() { ফেরত nullptr; }
};

class LogSinkRef : সর্বজনীন LogSink
{
সর্বজনীন:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ধ্রুবক LogMessage &বার্তা) ধ্রুবক override { ফেরত sink->should_log(বার্তা); }
	শূন্য log(ধ্রুবক LogMessage &বার্তা) override { sink->log(বার্তা); }
	শূন্য flush() override { sink->flush(); }
	FILE *file_handle() override { ফেরত sink->file_handle(); }
ব্যক্তিগত:
	LogSink *sink;
};

class FileLogSink : সর্বজনীন LogSink
{
সর্বজনীন:
	explicit FileLogSink(ধ্রুবক std::string &filename, bool সারি_buffered, bool append);
	~FileLogSink() override;
	শূন্য log(ধ্রুবক LogMessage &বার্তা) override;
	শূন্য flush() override;
	FILE *file_handle() override { ফেরত file; }

ব্যক্তিগত:
	FILE *file;
};

class ConsoleLogSink : সর্বজনীন LogSink
{
সর্বজনীন:
	শূন্য log(ধ্রুবক LogMessage &বার্তা) override;
	শূন্য flush() override;
	FILE *file_handle() override { ফেরত stdout; }
};

class StderrLogSink : সর্বজনীন LogSink
{
সর্বজনীন:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ধ্রুবক LogMessage &বার্তা) ধ্রুবক override;
	শূন্য log(ধ্রুবক LogMessage &বার্তা) override;
	শূন্য flush() override;
ব্যক্তিগত:
	bool quiet_warnings;
};

class StreamLogSink : সর্বজনীন LogSink
{
সর্বজনীন:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	শূন্য log(ধ্রুবক LogMessage &বার্তা) override { stream << বার্তা.cached_বার্তা; }
	শূন্য flush() override { stream.flush(); }

ব্যক্তিগত:
	std::ostream &stream;
};

class ScratchPadLogSink : সর্বজনীন LogSink
{
সর্বজনীন:
	explicit ScratchPadLogSink(std::string scratchpad);
	শূন্য log(ধ্রুবক LogMessage &বার্তা) override;

ব্যক্তিগত:
	std::string scratchpad;
};

class LogManager
{
ব্যক্তিগত:
	কাঠামো LogExpectedItem
	{
		LogExpectedItem(ধ্রুবক std::regex &pat, পূর্ণসংখ্যা expected) :
				pattern(pat), expected_গুনতি(expected), current_গুনতি(0) {}
		LogExpectedItem() : expected_গুনতি(0), current_গুনতি(0) {}

		std::regex pattern;
		পূর্ণসংখ্যা expected_গুনতি;
		পূর্ণসংখ্যা current_গুনতি;
	};

সর্বজনীন:
	LogManager() = পূর্বনির্ধারিত;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.ঠেলো_back(std::move(sink));
		ফেরত ref;
	}

	template<typename F>
	শূন্য for_each_sink(F &&func)
	{
		জন্য (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { ফেরত sinks.empty(); }
	শূন্য clear() { sinks.clear(); }
	শূন্য clear_original();
	শূন্য flush() { জন্য (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	সর্বজনীন:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.আকার_2());

			জন্য (ধ্রুবক auto &sink : backup_sinks)
				manager.sinks.ঠেলো_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ধ্রুবক Scoped &) = delete;
		Scoped &operator=(ধ্রুবক Scoped &) = delete;

	ব্যক্তিগত:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		পূর্ণসংখ্যা backup_verbose_level;
	};

	Scoped sink_scope()
	{
		ফেরত Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	সর্বজনীন:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	ব্যক্তিগত:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		ফেরত ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	সর্বজনীন:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			যদি (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		শূন্য on()
		{
			যদি (status)
				ফেরত;
			status = true;
			manager.make_debug++;
		}

		শূন্য off_silent()
		{
			যদি (!status)
				ফেরত;
			status = false;
			manager.make_debug--;
		}

		শূন্য off()
		{
			off_silent();
		}
	ব্যক্তিগত:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		ফেরত LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	সর্বজনীন:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			যদি (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		শূন্য on()
		{
			যদি (active)
				ফেরত;

			active = true;
			manager.force_debug++;
		}

		শূন্য off()
		{
			যদি (!active)
				ফেরত;

			active = false;
			manager.force_debug--;
		}
	ব্যক্তিগত:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		ফেরত ForceDebug(*this, start_on);
	}

	শূন্য force_debug_on() { force_debug++; }
	শূন্য force_debug_off() { যদি (force_debug > 0) force_debug--; }
	শূন্য set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	শূন্য report_unexpected_error();

	শূন্য add_experimental_ignore(std::string name) { experimental_ignored.ঢোকাও(name); }
	শূন্য add_warn(std::string pattern) { warn_regexes.ঠেলো_back(YS_REGEX_COMPILE(pattern)); }
	শূন্য add_nowarn(std::string pattern) { nowarn_regexes.ঠেলো_back(YS_REGEX_COMPILE(pattern)); }
	শূন্য add_werror(std::string pattern) { werror_regexes.ঠেলো_back(YS_REGEX_COMPILE(pattern)); }
	শূন্য add_expect(std::string type, std::string pattern, পূর্ণসংখ্যা গুনতি);

	শূন্য set_verbose_level(পূর্ণসংখ্যা level) { verbose_level = level; }
	শূন্য add_verbose_level(পূর্ণসংখ্যা level) { verbose_level += level; }
	শূন্য set_expect_no_warnings(bool মান) { expect_no_warnings = মান; }
	শূন্য set_log_time(bool মান) { log_time = মান; }
	শূন্য set_cmd_error_throw(bool মান) { cmd_error_throw = মান; }
	শূন্য set_hdump_all(bool মান) { hdump_all = মান; }
	পূর্ণসংখ্যা get_verbose_level() ধ্রুবক { ফেরত verbose_level; }
	bool get_log_time() ধ্রুবক { ফেরত log_time; }
	পূর্ণসংখ্যা get_warnings_unique() ধ্রুবক { ফেরত Get_আকার(warnings); }
	পূর্ণসংখ্যা get_warnings_যোগ() ধ্রুবক { ফেরত warnings_গুনতি; }
	পূর্ণসংখ্যা get_errors_যোগ() ধ্রুবক { ফেরত errors_গুনতি; }
	ধ্রুবক std::set<std::string> &get_experimental() ধ্রুবক { ফেরত experimental; }
	ধ্রুবক std::map<std::string, std::string> &get_deprecated() ধ্রুবক { ফেরত deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ধ্রুবক;

	শূন্য add_hdump(std::string name, std::string মান) { hdump[name].ঢোকাও(মান); }

	শূন্য formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	শূন্য formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	শূন্য formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] শূন্য formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	শূন্য formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] শূন্য formatted_cmd_error(std::string_view format, std::string message);
	শূন্য suppressed();
	শূন্য add_experimental(ধ্রুবক std::string &str);
	শূন্য add_deprecated(ধ্রুবক std::string &feature, ধ্রুবক std::string &earliest_removal);
	শূন্য spacer();
	শূন্য ঠেলো();
	শূন্য বের_করো();

	শূন্য reset_স্তূপ();

	শূন্য check_expected();
	bool expects_error() { ফেরত (expect_error.আকার_2() + expect_prefix_error.আকার_2())>0; }
#ifndef NDEBUG
	bool is_debug(পূর্ণসংখ্যা n = 0) { যদি (force_debug) ফেরত true; debug_suppressed += n; ফেরত false; }
#অন্যথা
	bool is_debug(পূর্ণসংখ্যা = 0) { ফেরত false; }
#endif
	শূন্য start_hasher();
	std::string finish_hasher();

ব্যক্তিগত:
	শূন্য logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	শূন্য error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	পূর্ণসংখ্যা verbose_level = 0;
	পূর্ণসংখ্যা newline_গুনতি = 0;
	vector<পূর্ণসংখ্যা> header_গুনতি;
	পূর্ণসংখ্যা errors_গুনতি = 0;
	পূর্ণসংখ্যা warnings_গুনতি = 0;
	পূর্ণসংখ্যা warnings_গুনতি_noexpect = 0;
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

	পূর্ণসংখ্যা debug_suppressed = 0;
	পূর্ণসংখ্যা make_debug = 0;
	পূর্ণসংখ্যা force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

বাহ্যিক শূন্য (*log_error_atexit)();

শূন্য set_verific_logging(শূন্য (*cb)(পূর্ণসংখ্যা বার্তা_type, ধ্রুবক অক্ষর *message_id, LogSourceLocation src, ধ্রুবক অক্ষর *বার্তা));
বাহ্যিক শূন্য (*log_verific_callback)(পূর্ণসংখ্যা বার্তা_type, ধ্রুবক অক্ষর *message_id, LogSourceLocation src, ধ্রুবক অক্ষর *বার্তা);

#ifndef NDEBUG
স্থির অন্তঃরেখ bool ys_debug(পূর্ণসংখ্যা n = 0) { ফেরত logger().is_debug(n); }
#অন্যথা
স্থির অন্তঃরেখ bool ys_debug(পূর্ণসংখ্যা = 0) { ফেরত false; }
#endif

template <typename... Args>
অন্তঃরেখ শূন্য log(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃরেখ শূন্য log_comment(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃরেখ শূন্য log_highlight(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃরেখ শূন্য log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) করো { যদি (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } যখন (0)

template <typename... Args>
অন্তঃরেখ শূন্য log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃরেখ শূন্য log_warning(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃরেখ শূন্য log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

অন্তঃরেখ শূন্য log_experimental(ধ্রুবক std::string &str)
{
	logger().add_experimental(str);
}

অন্তঃরেখ শূন্য log_deprecated(ধ্রুবক std::string &feature, ধ্রুবক std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
শূন্য log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
শূন্য log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] শূন্য log_error(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] শূন্য log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
শূন্য log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] শূন্য log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, ধ্রুবক Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

অন্তঃরেখ শূন্য log_suppressed()
{
	logger().suppressed();
}

অন্তঃরেখ শূন্য log_spacer() { logger().spacer(); }
অন্তঃরেখ শূন্য log_ঠেলো() { logger().ঠেলো(); }
অন্তঃরেখ শূন্য log_বের_করো() { logger().বের_করো(); }

অন্তঃরেখ শূন্য log_reset_স্তূপ() { logger().reset_স্তূপ(); }
অন্তঃরেখ শূন্য log_flush() { logger().flush(); }

শূন্য log_backtrace(ধ্রুবক অক্ষর *prefix, পূর্ণসংখ্যা levels);


std::string log_signal(ধ্রুবক RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ধ্রুবক RTLIL::Const &মান, bool autoint = true);

শূন্য log_module(RTLIL::Module *module, std::string indent = "");
শূন্য log_cell(RTLIL::Cell *cell, std::string indent = "");
শূন্য log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
শূন্য log_abort_internal(ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
কাঠামো PerformanceTimer
{
#যদি 1
	int64_t যোগ_ns;

	PerformanceTimer() {
		যোগ_ns = 0;
	}

	স্থির int64_t query() {
#  ifdef _WIN32
		ফেরত 0;
#  elif defined(RUSAGE_SELF)
		কাঠামো rusage rusage;
		int64_t t = 0;
		জন্য (পূর্ণসংখ্যা who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			যদি (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		ফেরত t;
#  অন্যথা
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	শূন্য reset() {
		যোগ_ns = 0;
	}

	শূন্য begin() {
		যোগ_ns -= query();
	}

	শূন্য end() {
		যোগ_ns += query();
	}

	দশমিক sec() ধ্রুবক {
		ফেরত যোগ_ns * 1e-9f;
	}
#অন্যথা
	স্থির int64_t query() { ফেরত 0; }
	শূন্য reset() { }
	শূন্য begin() { }
	শূন্য end() { }
	দশমিক sec() ধ্রুবক { ফেরত 0; }
#endif
};

// simple API for quickly dumping values when debugging

স্থির অন্তঃরেখ শূন্য log_dump_val_worker(short v) { log("%d", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(unsigned short v) { log("%u", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(পূর্ণসংখ্যা v) { log("%d", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(unsigned পূর্ণসংখ্যা v) { log("%u", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(long পূর্ণসংখ্যা v) { log("%ld", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(unsigned long পূর্ণসংখ্যা v) { log("%lu", v); }
#ifndef _WIN32
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(long long পূর্ণসংখ্যা v) { log("%lld", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(unsigned long long পূর্ণসংখ্যা v) { log("%lld", v); }
#endif
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(অক্ষর c)
{
	যদি (c >= 32 && c < 127) {
		log("'%c'", c);
	} অন্যথা {
		log("'\\x%02x'", c);
	}
}
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(unsigned অক্ষর c)
{
	যদি (c >= 32 && c < 127) {
		log("'%c'", c);
	} অন্যথা {
		log("'\\x%02x'", c);
	}
}
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(দ্বিগুণ v) { log("%f", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(অক্ষর *v) { log("%s", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(ধ্রুবক অক্ষর *v) { log("%s", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(std::string v) { log("%s", v); }
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
স্থির অন্তঃরেখ শূন্য log_dump_args_worker(ধ্রুবক অক্ষর *p) { log_assert(*p == 0); }
শূন্য log_dump_val_worker(IdString v);
শূন্য log_dump_val_worker(RTLIL::SigSpec v);
শূন্য log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> স্থির অন্তঃরেখ শূন্য log_dump_val_worker(dict<K, T> &v);
template<typename K> স্থির অন্তঃরেখ শূন্য log_dump_val_worker(pool<K> &v);
template<typename K> স্থির অন্তঃরেখ শূন্য log_dump_val_worker(std::vector<K> &v);
template<typename T> স্থির অন্তঃরেখ শূন্য log_dump_val_worker(T *ptr);

template<typename K, typename T>
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	জন্য (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	জন্য (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	জন্য (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
স্থির অন্তঃরেখ শূন্য log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
শূন্য log_dump_args_worker(ধ্রুবক অক্ষর *p, T first, Args ... args)
{
	পূর্ণসংখ্যা পরবর্তী_p_state = 0;
	ধ্রুবক অক্ষর *পরবর্তী_p = p;
	যখন (*পরবর্তী_p && (পরবর্তী_p_state != 0 || *পরবর্তী_p != ',')) {
		যদি (*পরবর্তী_p == '"')
			করো {
				পরবর্তী_p++;
				যখন (*পরবর্তী_p == '\\' && *(পরবর্তী_p + 1))
					পরবর্তী_p += 2;
			} যখন (*পরবর্তী_p && *পরবর্তী_p != '"');
		যদি (*পরবর্তী_p == '\'') {
			পরবর্তী_p++;
			যদি (*পরবর্তী_p == '\\')
				পরবর্তী_p++;
			যদি (*পরবর্তী_p)
				পরবর্তী_p++;
		}
		যদি (*পরবর্তী_p == '(' || *পরবর্তী_p == '[' || *পরবর্তী_p == '{')
			পরবর্তী_p_state++;
		যদি ((*পরবর্তী_p == ')' || *পরবর্তী_p == ']' || *পরবর্তী_p == '}') && পরবর্তী_p_state > 0)
			পরবর্তী_p_state--;
		পরবর্তী_p++;
	}
	log("\n\t%.*s => ", পূর্ণসংখ্যা(পরবর্তী_p - p), p);
	যদি (*পরবর্তী_p == ',')
		পরবর্তী_p++;
	যখন (*পরবর্তী_p == ' ' || *পরবর্তী_p == '\t' || *পরবর্তী_p == '\r' || *পরবর্তী_p == '\n')
		পরবর্তী_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(পরবর্তী_p, args ...);
}

#define log_dump(...) করো { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} যখন (0)

YOSYS_NAMESPACE_END

#অন্তর্ভাব "kernel/yosys.h"

#endif

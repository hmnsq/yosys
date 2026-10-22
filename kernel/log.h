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
#  define YS_DEBUGTRAP উত্থাপয(SIGTRAP)
# অন্যথা
#  define YS_DEBUGTRAP কুরু {} যাবত্(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#যদি defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING কুরু { যদি (IsDebuggerPresent()) DebugBreak(); } যাবত্(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING কুরু { \
		auto old = সঙ্কেত(SIGTRAP, SIG_IGN); উত্থাপয(SIGTRAP); সঙ্কেত(SIGTRAP, old); \
	} যাবত্(0)
#অন্যথা
# define YS_DEBUGTRAP_IF_DEBUGGING কুরু {} যাবত্(0)
#endif

সংরচনা log_cmd_error_exception { };

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

সংরচনা LogSourceLocation {
	LogSourceLocation() : filename({}),start_সারি(-1),start_col(-1),end_সারি(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_সারি(-1),start_col(-1),end_সারি(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,পূর্ণাঙ্ক start_সারি) : filename(filename),start_সারি(start_সারি),start_col(-1),end_সারি(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,পূর্ণাঙ্ক start_সারি,পূর্ণাঙ্ক start_col,পূর্ণাঙ্ক end_সারি,পূর্ণাঙ্ক end_col) :
		filename(filename),start_সারি(start_সারি),start_col(start_col),end_সারি(end_সারি),end_col(end_col) { }
	std::string filename;
	পূর্ণাঙ্ক start_সারি;
	পূর্ণাঙ্ক start_col;
	পূর্ণাঙ্ক end_সারি;
	পূর্ণাঙ্ক end_col;
};

সংরচনা LogMessage {
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
সার্বজনিক:
	আভাস ~LogSink() = স্বভাব;

	আভাস bool should_log(ধ্রুব LogMessage &) ধ্রুব { প্রত্যাগম true; }
	আভাস শূন্য log(ধ্রুব LogMessage &বার্তা) = 0;
	আভাস শূন্য flush() {}
	// TODO: Remove when AST/read_verilog removed
	আভাস FILE *file_handle() { প্রত্যাগম nullptr; }
};

class LogSinkRef : সার্বজনিক LogSink
{
সার্বজনিক:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ধ্রুব LogMessage &বার্তা) ধ্রুব override { প্রত্যাগম sink->should_log(বার্তা); }
	শূন্য log(ধ্রুব LogMessage &বার্তা) override { sink->log(বার্তা); }
	শূন্য flush() override { sink->flush(); }
	FILE *file_handle() override { প্রত্যাগম sink->file_handle(); }
গুপ্ত:
	LogSink *sink;
};

class FileLogSink : সার্বজনিক LogSink
{
সার্বজনিক:
	explicit FileLogSink(ধ্রুব std::string &filename, bool সারি_buffered, bool append);
	~FileLogSink() override;
	শূন্য log(ধ্রুব LogMessage &বার্তা) override;
	শূন্য flush() override;
	FILE *file_handle() override { প্রত্যাগম file; }

গুপ্ত:
	FILE *file;
};

class ConsoleLogSink : সার্বজনিক LogSink
{
সার্বজনিক:
	শূন্য log(ধ্রুব LogMessage &বার্তা) override;
	শূন্য flush() override;
	FILE *file_handle() override { প্রত্যাগম stdout; }
};

class StderrLogSink : সার্বজনিক LogSink
{
সার্বজনিক:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ধ্রুব LogMessage &বার্তা) ধ্রুব override;
	শূন্য log(ধ্রুব LogMessage &বার্তা) override;
	শূন্য flush() override;
গুপ্ত:
	bool quiet_warnings;
};

class StreamLogSink : সার্বজনিক LogSink
{
সার্বজনিক:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	শূন্য log(ধ্রুব LogMessage &বার্তা) override { stream << বার্তা.cached_বার্তা; }
	শূন্য flush() override { stream.flush(); }

গুপ্ত:
	std::ostream &stream;
};

class ScratchPadLogSink : সার্বজনিক LogSink
{
সার্বজনিক:
	explicit ScratchPadLogSink(std::string scratchpad);
	শূন্য log(ধ্রুব LogMessage &বার্তা) override;

গুপ্ত:
	std::string scratchpad;
};

class LogManager
{
গুপ্ত:
	সংরচনা LogExpectedItem
	{
		LogExpectedItem(ধ্রুব std::regex &pat, পূর্ণাঙ্ক expected) :
				pattern(pat), expected_গুনতি(expected), current_গুনতি(0) {}
		LogExpectedItem() : expected_গুনতি(0), current_গুনতি(0) {}

		std::regex pattern;
		পূর্ণাঙ্ক expected_গুনতি;
		পূর্ণাঙ্ক current_গুনতি;
	};

সার্বজনিক:
	LogManager() = স্বভাব;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.ঠেলো_back(std::move(sink));
		প্রত্যাগম ref;
	}

	template<typename F>
	শূন্য for_each_sink(F &&func)
	{
		প্রতি (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { প্রত্যাগম sinks.empty(); }
	শূন্য clear() { sinks.clear(); }
	শূন্য clear_original();
	শূন্য flush() { প্রতি (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	সার্বজনিক:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			প্রতি (ধ্রুব auto &sink : backup_sinks)
				manager.sinks.ঠেলো_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ধ্রুব Scoped &) = delete;
		Scoped &operator=(ধ্রুব Scoped &) = delete;

	গুপ্ত:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		পূর্ণাঙ্ক backup_verbose_level;
	};

	Scoped sink_scope()
	{
		প্রত্যাগম Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	সার্বজনিক:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	গুপ্ত:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		প্রত্যাগম ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	সার্বজনিক:
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
				প্রত্যাগম;
			status = true;
			manager.make_debug++;
		}

		শূন্য off_silent()
		{
			যদি (!status)
				প্রত্যাগম;
			status = false;
			manager.make_debug--;
		}

		শূন্য off()
		{
			off_silent();
		}
	গুপ্ত:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		প্রত্যাগম LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	সার্বজনিক:
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
				প্রত্যাগম;

			active = true;
			manager.force_debug++;
		}

		শূন্য off()
		{
			যদি (!active)
				প্রত্যাগম;

			active = false;
			manager.force_debug--;
		}
	গুপ্ত:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		প্রত্যাগম ForceDebug(*this, start_on);
	}

	শূন্য force_debug_on() { force_debug++; }
	শূন্য force_debug_off() { যদি (force_debug > 0) force_debug--; }
	শূন্য set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	শূন্য report_unexpected_error();

	শূন্য add_experimental_ignore(std::string name) { experimental_ignored.ঢোকাও(name); }
	শূন্য add_warn(std::string pattern) { warn_regexes.ঠেলো_back(YS_REGEX_COMPILE(pattern)); }
	শূন্য add_nowarn(std::string pattern) { nowarn_regexes.ঠেলো_back(YS_REGEX_COMPILE(pattern)); }
	শূন্য add_werror(std::string pattern) { werror_regexes.ঠেলো_back(YS_REGEX_COMPILE(pattern)); }
	শূন্য add_expect(std::string type, std::string pattern, পূর্ণাঙ্ক গুনতি);

	শূন্য set_verbose_level(পূর্ণাঙ্ক level) { verbose_level = level; }
	শূন্য add_verbose_level(পূর্ণাঙ্ক level) { verbose_level += level; }
	শূন্য set_expect_no_warnings(bool মান) { expect_no_warnings = মান; }
	শূন্য set_log_time(bool মান) { log_time = মান; }
	শূন্য set_cmd_error_throw(bool মান) { cmd_error_throw = মান; }
	শূন্য set_hdump_all(bool মান) { hdump_all = মান; }
	পূর্ণাঙ্ক get_verbose_level() ধ্রুব { প্রত্যাগম verbose_level; }
	bool get_log_time() ধ্রুব { প্রত্যাগম log_time; }
	পূর্ণাঙ্ক get_warnings_unique() ধ্রুব { প্রত্যাগম GetSize(warnings); }
	পূর্ণাঙ্ক get_warnings_total() ধ্রুব { প্রত্যাগম warnings_গুনতি; }
	পূর্ণাঙ্ক get_errors_total() ধ্রুব { প্রত্যাগম errors_গুনতি; }
	ধ্রুব std::set<std::string> &get_experimental() ধ্রুব { প্রত্যাগম experimental; }
	ধ্রুব std::map<std::string, std::string> &get_deprecated() ধ্রুব { প্রত্যাগম deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ধ্রুব;

	শূন্য add_hdump(std::string name, std::string মান) { hdump[name].ঢোকাও(মান); }

	শূন্য formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	শূন্য formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	শূন্য formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] শূন্য formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	শূন্য formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] শূন্য formatted_cmd_error(std::string_view format, std::string message);
	শূন্য suppressed();
	শূন্য add_experimental(ধ্রুব std::string &str);
	শূন্য add_deprecated(ধ্রুব std::string &feature, ধ্রুব std::string &earliest_removal);
	শূন্য spacer();
	শূন্য ঠেলো();
	শূন্য বের_করো();

	শূন্য reset_স্তূপ();

	শূন্য check_expected();
	bool expects_error() { প্রত্যাগম (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(পূর্ণাঙ্ক n = 0) { যদি (force_debug) প্রত্যাগম true; debug_suppressed += n; প্রত্যাগম false; }
#অন্যথা
	bool is_debug(পূর্ণাঙ্ক = 0) { প্রত্যাগম false; }
#endif
	শূন্য start_hasher();
	std::string finish_hasher();

গুপ্ত:
	শূন্য logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	শূন্য error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	পূর্ণাঙ্ক verbose_level = 0;
	পূর্ণাঙ্ক newline_গুনতি = 0;
	vector<পূর্ণাঙ্ক> header_গুনতি;
	পূর্ণাঙ্ক errors_গুনতি = 0;
	পূর্ণাঙ্ক warnings_গুনতি = 0;
	পূর্ণাঙ্ক warnings_গুনতি_noexpect = 0;
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

	পূর্ণাঙ্ক debug_suppressed = 0;
	পূর্ণাঙ্ক make_debug = 0;
	পূর্ণাঙ্ক force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

বাহ্য শূন্য (*log_error_atexit)();

শূন্য set_verific_logging(শূন্য (*cb)(পূর্ণাঙ্ক বার্তা_type, ধ্রুব বর্ণ *message_id, LogSourceLocation src, ধ্রুব বর্ণ *বার্তা));
বাহ্য শূন্য (*log_verific_callback)(পূর্ণাঙ্ক বার্তা_type, ধ্রুব বর্ণ *message_id, LogSourceLocation src, ধ্রুব বর্ণ *বার্তা);

#ifndef NDEBUG
স্থির অন্তঃস্থ bool ys_debug(পূর্ণাঙ্ক n = 0) { প্রত্যাগম logger().is_debug(n); }
#অন্যথা
স্থির অন্তঃস্থ bool ys_debug(পূর্ণাঙ্ক = 0) { প্রত্যাগম false; }
#endif

template <typename... Args>
অন্তঃস্থ শূন্য log(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃস্থ শূন্য log_comment(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃস্থ শূন্য log_highlight(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃস্থ শূন্য log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) কুরু { যদি (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } যাবত্ (0)

template <typename... Args>
অন্তঃস্থ শূন্য log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃস্থ শূন্য log_warning(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
অন্তঃস্থ শূন্য log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

অন্তঃস্থ শূন্য log_experimental(ধ্রুব std::string &str)
{
	logger().add_experimental(str);
}

অন্তঃস্থ শূন্য log_deprecated(ধ্রুব std::string &feature, ধ্রুব std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
শূন্য log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
শূন্য log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] শূন্য log_error(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] শূন্য log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
শূন্য log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] শূন্য log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, ধ্রুব Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

অন্তঃস্থ শূন্য log_suppressed()
{
	logger().suppressed();
}

অন্তঃস্থ শূন্য log_spacer() { logger().spacer(); }
অন্তঃস্থ শূন্য log_ঠেলো() { logger().ঠেলো(); }
অন্তঃস্থ শূন্য log_বের_করো() { logger().বের_করো(); }

অন্তঃস্থ শূন্য log_reset_স্তূপ() { logger().reset_স্তূপ(); }
অন্তঃস্থ শূন্য log_flush() { logger().flush(); }

শূন্য log_backtrace(ধ্রুব বর্ণ *prefix, পূর্ণাঙ্ক levels);


std::string log_signal(ধ্রুব RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ধ্রুব RTLIL::Const &মান, bool autoint = true);

শূন্য log_module(RTLIL::Module *module, std::string indent = "");
শূন্য log_cell(RTLIL::Cell *cell, std::string indent = "");
শূন্য log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
শূন্য log_abort_internal(ধ্রুব বর্ণ *file, পূর্ণাঙ্ক সারি);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
সংরচনা PerformanceTimer
{
#যদি 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	স্থির int64_t query() {
#  ifdef _WIN32
		প্রত্যাগম 0;
#  elif defined(RUSAGE_SELF)
		সংরচনা rusage rusage;
		int64_t t = 0;
		প্রতি (পূর্ণাঙ্ক who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			যদি (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		প্রত্যাগম t;
#  অন্যথা
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	শূন্য reset() {
		total_ns = 0;
	}

	শূন্য begin() {
		total_ns -= query();
	}

	শূন্য end() {
		total_ns += query();
	}

	ভিন্ন sec() ধ্রুব {
		প্রত্যাগম total_ns * 1e-9f;
	}
#অন্যথা
	স্থির int64_t query() { প্রত্যাগম 0; }
	শূন্য reset() { }
	শূন্য begin() { }
	শূন্য end() { }
	ভিন্ন sec() ধ্রুব { প্রত্যাগম 0; }
#endif
};

// simple API for quickly dumping values when debugging

স্থির অন্তঃস্থ শূন্য log_dump_val_worker(short v) { log("%d", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(unsigned short v) { log("%u", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(পূর্ণাঙ্ক v) { log("%d", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(unsigned পূর্ণাঙ্ক v) { log("%u", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(long পূর্ণাঙ্ক v) { log("%ld", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(unsigned long পূর্ণাঙ্ক v) { log("%lu", v); }
#ifndef _WIN32
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(long long পূর্ণাঙ্ক v) { log("%lld", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(unsigned long long পূর্ণাঙ্ক v) { log("%lld", v); }
#endif
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(বর্ণ c)
{
	যদি (c >= 32 && c < 127) {
		log("'%c'", c);
	} অন্যথা {
		log("'\\x%02x'", c);
	}
}
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(unsigned বর্ণ c)
{
	যদি (c >= 32 && c < 127) {
		log("'%c'", c);
	} অন্যথা {
		log("'\\x%02x'", c);
	}
}
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(দ্বিগুণ v) { log("%f", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(বর্ণ *v) { log("%s", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(ধ্রুব বর্ণ *v) { log("%s", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(std::string v) { log("%s", v); }
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
স্থির অন্তঃস্থ শূন্য log_dump_args_worker(ধ্রুব বর্ণ *p) { log_assert(*p == 0); }
শূন্য log_dump_val_worker(IdString v);
শূন্য log_dump_val_worker(RTLIL::SigSpec v);
শূন্য log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> স্থির অন্তঃস্থ শূন্য log_dump_val_worker(dict<K, T> &v);
template<typename K> স্থির অন্তঃস্থ শূন্য log_dump_val_worker(pool<K> &v);
template<typename K> স্থির অন্তঃস্থ শূন্য log_dump_val_worker(std::vector<K> &v);
template<typename T> স্থির অন্তঃস্থ শূন্য log_dump_val_worker(T *ptr);

template<typename K, typename T>
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	প্রতি (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	প্রতি (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	প্রতি (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
স্থির অন্তঃস্থ শূন্য log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
শূন্য log_dump_args_worker(ধ্রুব বর্ণ *p, T first, Args ... args)
{
	পূর্ণাঙ্ক next_p_state = 0;
	ধ্রুব বর্ণ *next_p = p;
	যাবত্ (*next_p && (next_p_state != 0 || *next_p != ',')) {
		যদি (*next_p == '"')
			কুরু {
				next_p++;
				যাবত্ (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} যাবত্ (*next_p && *next_p != '"');
		যদি (*next_p == '\'') {
			next_p++;
			যদি (*next_p == '\\')
				next_p++;
			যদি (*next_p)
				next_p++;
		}
		যদি (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		যদি ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", পূর্ণাঙ্ক(next_p - p), p);
	যদি (*next_p == ',')
		next_p++;
	যাবত্ (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) কুরু { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} যাবত্ (0)

YOSYS_NAMESPACE_END

#অন্তর্ভাব "kernel/yosys.h"

#endif

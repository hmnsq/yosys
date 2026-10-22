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

#شامل "kernel/yosys_common.h"

#شامل <chrono>
#شامل <time.h>

#شامل <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#اگر defined(_WIN32)
#  include <intrin.h>
#وگرنه
#  include <sys/time.h>
#  include <sys/resource.h>
#  اگر defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#اگر defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#وگرنه
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# اگر __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP بالاببر(SIGTRAP)
# وگرنه
#  define YS_DEBUGTRAP انجامبده {} وقتی(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#اگر defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING انجامبده { اگر (IsDebuggerPresent()) DebugBreak(); } وقتی(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING انجامبده { \
		auto old = سیگنال(SIGTRAP, SIG_IGN); بالاببر(SIGTRAP); سیگنال(SIGTRAP, old); \
	} وقتی(0)
#وگرنه
# define YS_DEBUGTRAP_IF_DEBUGGING انجامبده {} وقتی(0)
#endif

ساختار log_cmd_error_exception { };

شمارش class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

ساختار LogSourceLocation {
	LogSourceLocation() : filename({}),start_سطر(-1),start_col(-1),end_سطر(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_سطر(-1),start_col(-1),end_سطر(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,صحیح start_سطر) : filename(filename),start_سطر(start_سطر),start_col(-1),end_سطر(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,صحیح start_سطر,صحیح start_col,صحیح end_سطر,صحیح end_col) :
		filename(filename),start_سطر(start_سطر),start_col(start_col),end_سطر(end_سطر),end_col(end_col) { }
	std::string filename;
	صحیح start_سطر;
	صحیح start_col;
	صحیح end_سطر;
	صحیح end_col;
};

ساختار LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_پیام;
};

class LogSink
{
عمومی:
	مجازی ~LogSink() = پیشفرض;

	مجازی bool should_log(ثابت LogMessage &) ثابت { بازگشت true; }
	مجازی تهی log(ثابت LogMessage &پیام) = 0;
	مجازی تهی flush() {}
	// TODO: Remove when AST/read_verilog removed
	مجازی FILE *file_handle() { بازگشت nullptr; }
};

class LogSinkRef : عمومی LogSink
{
عمومی:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ثابت LogMessage &پیام) ثابت override { بازگشت sink->should_log(پیام); }
	تهی log(ثابت LogMessage &پیام) override { sink->log(پیام); }
	تهی flush() override { sink->flush(); }
	FILE *file_handle() override { بازگشت sink->file_handle(); }
خصوصی:
	LogSink *sink;
};

class FileLogSink : عمومی LogSink
{
عمومی:
	explicit FileLogSink(ثابت std::string &filename, bool سطر_buffered, bool append);
	~FileLogSink() override;
	تهی log(ثابت LogMessage &پیام) override;
	تهی flush() override;
	FILE *file_handle() override { بازگشت file; }

خصوصی:
	FILE *file;
};

class ConsoleLogSink : عمومی LogSink
{
عمومی:
	تهی log(ثابت LogMessage &پیام) override;
	تهی flush() override;
	FILE *file_handle() override { بازگشت stdout; }
};

class StderrLogSink : عمومی LogSink
{
عمومی:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ثابت LogMessage &پیام) ثابت override;
	تهی log(ثابت LogMessage &پیام) override;
	تهی flush() override;
خصوصی:
	bool quiet_warnings;
};

class StreamLogSink : عمومی LogSink
{
عمومی:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	تهی log(ثابت LogMessage &پیام) override { stream << پیام.cached_پیام; }
	تهی flush() override { stream.flush(); }

خصوصی:
	std::ostream &stream;
};

class ScratchPadLogSink : عمومی LogSink
{
عمومی:
	explicit ScratchPadLogSink(std::string scratchpad);
	تهی log(ثابت LogMessage &پیام) override;

خصوصی:
	std::string scratchpad;
};

class LogManager
{
خصوصی:
	ساختار LogExpectedItem
	{
		LogExpectedItem(ثابت std::regex &pat, صحیح expected) :
				pattern(pat), expected_شمار(expected), current_شمار(0) {}
		LogExpectedItem() : expected_شمار(0), current_شمار(0) {}

		std::regex pattern;
		صحیح expected_شمار;
		صحیح current_شمار;
	};

عمومی:
	LogManager() = پیشفرض;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.افزودن_back(std::move(sink));
		بازگشت ref;
	}

	template<typename F>
	تهی for_each_sink(F &&func)
	{
		برای (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { بازگشت sinks.empty(); }
	تهی clear() { sinks.clear(); }
	تهی clear_original();
	تهی flush() { برای (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	عمومی:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			برای (ثابت auto &sink : backup_sinks)
				manager.sinks.افزودن_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ثابت Scoped &) = delete;
		Scoped &operator=(ثابت Scoped &) = delete;

	خصوصی:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		صحیح backup_verbose_level;
	};

	Scoped sink_scope()
	{
		بازگشت Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	عمومی:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	خصوصی:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		بازگشت ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	عمومی:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			اگر (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		تهی on()
		{
			اگر (status)
				بازگشت;
			status = true;
			manager.make_debug++;
		}

		تهی off_silent()
		{
			اگر (!status)
				بازگشت;
			status = false;
			manager.make_debug--;
		}

		تهی off()
		{
			off_silent();
		}
	خصوصی:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		بازگشت LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	عمومی:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			اگر (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		تهی on()
		{
			اگر (active)
				بازگشت;

			active = true;
			manager.force_debug++;
		}

		تهی off()
		{
			اگر (!active)
				بازگشت;

			active = false;
			manager.force_debug--;
		}
	خصوصی:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		بازگشت ForceDebug(*this, start_on);
	}

	تهی force_debug_on() { force_debug++; }
	تهی force_debug_off() { اگر (force_debug > 0) force_debug--; }
	تهی set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	تهی report_unexpected_error();

	تهی add_experimental_ignore(std::string name) { experimental_ignored.درج(name); }
	تهی add_warn(std::string pattern) { warn_regexes.افزودن_back(YS_REGEX_COMPILE(pattern)); }
	تهی add_nowarn(std::string pattern) { nowarn_regexes.افزودن_back(YS_REGEX_COMPILE(pattern)); }
	تهی add_werror(std::string pattern) { werror_regexes.افزودن_back(YS_REGEX_COMPILE(pattern)); }
	تهی add_expect(std::string type, std::string pattern, صحیح شمار);

	تهی set_verbose_level(صحیح level) { verbose_level = level; }
	تهی add_verbose_level(صحیح level) { verbose_level += level; }
	تهی set_expect_no_warnings(bool مقدار) { expect_no_warnings = مقدار; }
	تهی set_log_time(bool مقدار) { log_time = مقدار; }
	تهی set_cmd_error_throw(bool مقدار) { cmd_error_throw = مقدار; }
	تهی set_hdump_all(bool مقدار) { hdump_all = مقدار; }
	صحیح get_verbose_level() ثابت { بازگشت verbose_level; }
	bool get_log_time() ثابت { بازگشت log_time; }
	صحیح get_warnings_unique() ثابت { بازگشت GetSize(warnings); }
	صحیح get_warnings_total() ثابت { بازگشت warnings_شمار; }
	صحیح get_errors_total() ثابت { بازگشت errors_شمار; }
	ثابت std::set<std::string> &get_experimental() ثابت { بازگشت experimental; }
	ثابت std::map<std::string, std::string> &get_deprecated() ثابت { بازگشت deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ثابت;

	تهی add_hdump(std::string name, std::string مقدار) { hdump[name].درج(مقدار); }

	تهی formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	تهی formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	تهی formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] تهی formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	تهی formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] تهی formatted_cmd_error(std::string_view format, std::string message);
	تهی suppressed();
	تهی add_experimental(ثابت std::string &str);
	تهی add_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal);
	تهی spacer();
	تهی افزودن();
	تهی برداشت();

	تهی reset_پشته();

	تهی check_expected();
	bool expects_error() { بازگشت (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(صحیح n = 0) { اگر (force_debug) بازگشت true; debug_suppressed += n; بازگشت false; }
#وگرنه
	bool is_debug(صحیح = 0) { بازگشت false; }
#endif
	تهی start_hasher();
	std::string finish_hasher();

خصوصی:
	تهی logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	تهی error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	صحیح verbose_level = 0;
	صحیح newline_شمار = 0;
	vector<صحیح> header_شمار;
	صحیح errors_شمار = 0;
	صحیح warnings_شمار = 0;
	صحیح warnings_شمار_noexpect = 0;
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

	صحیح debug_suppressed = 0;
	صحیح make_debug = 0;
	صحیح force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

بیرونی تهی (*log_error_atexit)();

تهی set_verific_logging(تهی (*cb)(صحیح پیام_type, ثابت نویسه *message_id, LogSourceLocation src, ثابت نویسه *پیام));
بیرونی تهی (*log_verific_callback)(صحیح پیام_type, ثابت نویسه *message_id, LogSourceLocation src, ثابت نویسه *پیام);

#ifndef NDEBUG
ایستا درونخطی bool ys_debug(صحیح n = 0) { بازگشت logger().is_debug(n); }
#وگرنه
ایستا درونخطی bool ys_debug(صحیح = 0) { بازگشت false; }
#endif

template <typename... Args>
درونخطی تهی log(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
درونخطی تهی log_comment(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
درونخطی تهی log_highlight(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
درونخطی تهی log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) انجامبده { اگر (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } وقتی (0)

template <typename... Args>
درونخطی تهی log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
درونخطی تهی log_warning(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
درونخطی تهی log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

درونخطی تهی log_experimental(ثابت std::string &str)
{
	logger().add_experimental(str);
}

درونخطی تهی log_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
تهی log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
تهی log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] تهی log_error(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] تهی log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
تهی log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] تهی log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, ثابت Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

درونخطی تهی log_suppressed()
{
	logger().suppressed();
}

درونخطی تهی log_spacer() { logger().spacer(); }
درونخطی تهی log_افزودن() { logger().افزودن(); }
درونخطی تهی log_برداشت() { logger().برداشت(); }

درونخطی تهی log_reset_پشته() { logger().reset_پشته(); }
درونخطی تهی log_flush() { logger().flush(); }

تهی log_backtrace(ثابت نویسه *prefix, صحیح levels);


std::string log_signal(ثابت RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ثابت RTLIL::Const &مقدار, bool autoint = true);

تهی log_module(RTLIL::Module *module, std::string indent = "");
تهی log_cell(RTLIL::Cell *cell, std::string indent = "");
تهی log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
تهی log_abort_internal(ثابت نویسه *file, صحیح سطر);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
ساختار PerformanceTimer
{
#اگر 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	ایستا int64_t query() {
#  ifdef _WIN32
		بازگشت 0;
#  elif defined(RUSAGE_SELF)
		ساختار rusage rusage;
		int64_t t = 0;
		برای (صحیح who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			اگر (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		بازگشت t;
#  وگرنه
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	تهی reset() {
		total_ns = 0;
	}

	تهی begin() {
		total_ns -= query();
	}

	تهی end() {
		total_ns += query();
	}

	اعشاری sec() ثابت {
		بازگشت total_ns * 1e-9f;
	}
#وگرنه
	ایستا int64_t query() { بازگشت 0; }
	تهی reset() { }
	تهی begin() { }
	تهی end() { }
	اعشاری sec() ثابت { بازگشت 0; }
#endif
};

// simple API for quickly dumping values when debugging

ایستا درونخطی تهی log_dump_val_worker(short v) { log("%d", v); }
ایستا درونخطی تهی log_dump_val_worker(unsigned short v) { log("%u", v); }
ایستا درونخطی تهی log_dump_val_worker(صحیح v) { log("%d", v); }
ایستا درونخطی تهی log_dump_val_worker(unsigned صحیح v) { log("%u", v); }
ایستا درونخطی تهی log_dump_val_worker(long صحیح v) { log("%ld", v); }
ایستا درونخطی تهی log_dump_val_worker(unsigned long صحیح v) { log("%lu", v); }
#ifndef _WIN32
ایستا درونخطی تهی log_dump_val_worker(long long صحیح v) { log("%lld", v); }
ایستا درونخطی تهی log_dump_val_worker(unsigned long long صحیح v) { log("%lld", v); }
#endif
ایستا درونخطی تهی log_dump_val_worker(نویسه c)
{
	اگر (c >= 32 && c < 127) {
		log("'%c'", c);
	} وگرنه {
		log("'\\x%02x'", c);
	}
}
ایستا درونخطی تهی log_dump_val_worker(unsigned نویسه c)
{
	اگر (c >= 32 && c < 127) {
		log("'%c'", c);
	} وگرنه {
		log("'\\x%02x'", c);
	}
}
ایستا درونخطی تهی log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
ایستا درونخطی تهی log_dump_val_worker(دوگانه v) { log("%f", v); }
ایستا درونخطی تهی log_dump_val_worker(نویسه *v) { log("%s", v); }
ایستا درونخطی تهی log_dump_val_worker(ثابت نویسه *v) { log("%s", v); }
ایستا درونخطی تهی log_dump_val_worker(std::string v) { log("%s", v); }
ایستا درونخطی تهی log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
ایستا درونخطی تهی log_dump_args_worker(ثابت نویسه *p) { log_assert(*p == 0); }
تهی log_dump_val_worker(IdString v);
تهی log_dump_val_worker(RTLIL::SigSpec v);
تهی log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> ایستا درونخطی تهی log_dump_val_worker(dict<K, T> &v);
template<typename K> ایستا درونخطی تهی log_dump_val_worker(pool<K> &v);
template<typename K> ایستا درونخطی تهی log_dump_val_worker(std::vector<K> &v);
template<typename T> ایستا درونخطی تهی log_dump_val_worker(T *ptr);

template<typename K, typename T>
ایستا درونخطی تهی log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	برای (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
ایستا درونخطی تهی log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	برای (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
ایستا درونخطی تهی log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	برای (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
ایستا درونخطی تهی log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
تهی log_dump_args_worker(ثابت نویسه *p, T first, Args ... args)
{
	صحیح next_p_state = 0;
	ثابت نویسه *next_p = p;
	وقتی (*next_p && (next_p_state != 0 || *next_p != ',')) {
		اگر (*next_p == '"')
			انجامبده {
				next_p++;
				وقتی (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} وقتی (*next_p && *next_p != '"');
		اگر (*next_p == '\'') {
			next_p++;
			اگر (*next_p == '\\')
				next_p++;
			اگر (*next_p)
				next_p++;
		}
		اگر (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		اگر ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", صحیح(next_p - p), p);
	اگر (*next_p == ',')
		next_p++;
	وقتی (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) انجامبده { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} وقتی (0)

YOSYS_NAMESPACE_END

#شامل "kernel/yosys.h"

#endif

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

#ضمن "kernel/yosys_common.h"

#ضمن <chrono>
#ضمن <time.h>

#ضمن <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#إذا defined(_WIN32)
#  include <intrin.h>
#وإلا
#  include <sys/time.h>
#  include <sys/resource.h>
#  إذا defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#إذا defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#وإلا
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# إذا __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP ارفع(SIGTRAP)
# وإلا
#  define YS_DEBUGTRAP افعل {} طالما(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#إذا defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { إذا (IsDebuggerPresent()) DebugBreak(); } طالما(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { \
		auto old = إشارة(SIGTRAP, SIG_IGN); ارفع(SIGTRAP); إشارة(SIGTRAP, old); \
	} طالما(0)
#وإلا
# define YS_DEBUGTRAP_IF_DEBUGGING افعل {} طالما(0)
#endif

بنية log_cmd_error_exception { };

تعداد class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

بنية LogSourceLocation {
	LogSourceLocation() : filename({}),start_لي(-1),start_col(-1),end_لي(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_لي(-1),start_col(-1),end_لي(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,صحيح start_لي) : filename(filename),start_لي(start_لي),start_col(-1),end_لي(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,صحيح start_لي,صحيح start_col,صحيح end_لي,صحيح end_col) :
		filename(filename),start_لي(start_لي),start_col(start_col),end_لي(end_لي),end_col(end_col) { }
	std::string filename;
	صحيح start_لي;
	صحيح start_col;
	صحيح end_لي;
	صحيح end_col;
};

بنية LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_سكو;
};

class LogSink
{
عام:
	virtual ~LogSink() = افتراضي;

	virtual bool should_log(ثابت LogMessage &) ثابت { أرجع true; }
	virtual فراغ log(ثابت LogMessage &سكو) = 0;
	virtual فراغ flush() {}
	// TODO: Remove when AST/read_verilog removed
	virtual FILE *file_handle() { أرجع nullptr; }
};

class LogSinkRef : عام LogSink
{
عام:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ثابت LogMessage &سكو) ثابت override { أرجع sink->should_log(سكو); }
	فراغ log(ثابت LogMessage &سكو) override { sink->log(سكو); }
	فراغ flush() override { sink->flush(); }
	FILE *file_handle() override { أرجع sink->file_handle(); }
خاص:
	LogSink *sink;
};

class FileLogSink : عام LogSink
{
عام:
	explicit FileLogSink(ثابت std::string &filename, bool لي_buffered, bool append);
	~FileLogSink() override;
	فراغ log(ثابت LogMessage &سكو) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع file; }

خاص:
	FILE *file;
};

class ConsoleLogSink : عام LogSink
{
عام:
	فراغ log(ثابت LogMessage &سكو) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع stdout; }
};

class StderrLogSink : عام LogSink
{
عام:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ثابت LogMessage &سكو) ثابت override;
	فراغ log(ثابت LogMessage &سكو) override;
	فراغ flush() override;
خاص:
	bool quiet_warnings;
};

class StreamLogSink : عام LogSink
{
عام:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	فراغ log(ثابت LogMessage &سكو) override { stream << سكو.cached_سكو; }
	فراغ flush() override { stream.flush(); }

خاص:
	std::ostream &stream;
};

class ScratchPadLogSink : عام LogSink
{
عام:
	explicit ScratchPadLogSink(std::string scratchpad);
	فراغ log(ثابت LogMessage &سكو) override;

خاص:
	std::string scratchpad;
};

class LogManager
{
خاص:
	بنية LogExpectedItem
	{
		LogExpectedItem(ثابت std::regex &pat, صحيح expected) :
				pattern(pat), expected_كدي(expected), current_كدي(0) {}
		LogExpectedItem() : expected_كدي(0), current_كدي(0) {}

		std::regex pattern;
		صحيح expected_كدي;
		صحيح current_كدي;
	};

عام:
	LogManager() = افتراضي;

	template<typename T, typename... حجوجي_3>
	T &add_sink(حجوجي_3&&... حجوجي_2)
	{
		auto sink = std::make_unique<T>(std::forward<حجوجي_3>(حجوجي_2)...);
		T &ref = *sink;
		sinks.تور_back(std::move(sink));
		أرجع ref;
	}

	template<typename F>
	فراغ for_each_sink(F &&func)
	{
		لكل (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { أرجع sinks.empty(); }
	فراغ clear() { sinks.clear(); }
	فراغ clear_original();
	فراغ flush() { لكل (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	عام:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			لكل (ثابت auto &sink : backup_sinks)
				manager.sinks.تور_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ثابت Scoped &) = delete;
		Scoped &operator=(ثابت Scoped &) = delete;

	خاص:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		صحيح backup_verbose_level;
	};

	Scoped sink_scope()
	{
		أرجع Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	عام:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	خاص:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		أرجع ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	عام:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			إذا (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		فراغ on()
		{
			إذا (status)
				أرجع;
			status = true;
			manager.make_debug++;
		}

		فراغ off_silent()
		{
			إذا (!status)
				أرجع;
			status = false;
			manager.make_debug--;
		}

		فراغ off()
		{
			off_silent();
		}
	خاص:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		أرجع LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	عام:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			إذا (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		فراغ on()
		{
			إذا (active)
				أرجع;

			active = true;
			manager.force_debug++;
		}

		فراغ off()
		{
			إذا (!active)
				أرجع;

			active = false;
			manager.force_debug--;
		}
	خاص:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		أرجع ForceDebug(*this, start_on);
	}

	فراغ force_debug_on() { force_debug++; }
	فراغ force_debug_off() { إذا (force_debug > 0) force_debug--; }
	فراغ set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	فراغ report_unexpected_error();

	فراغ add_experimental_ignore(std::string name) { experimental_ignored.سك(name); }
	فراغ add_warn(std::string pattern) { warn_regexes.تور_back(YS_REGEX_COMPILE(pattern)); }
	فراغ add_nowarn(std::string pattern) { nowarn_regexes.تور_back(YS_REGEX_COMPILE(pattern)); }
	فراغ add_werror(std::string pattern) { werror_regexes.تور_back(YS_REGEX_COMPILE(pattern)); }
	فراغ add_expect(std::string type, std::string pattern, صحيح كدي);

	فراغ set_verbose_level(صحيح level) { verbose_level = level; }
	فراغ add_verbose_level(صحيح level) { verbose_level += level; }
	فراغ set_expect_no_warnings(bool درج) { expect_no_warnings = درج; }
	فراغ set_log_time(bool درج) { log_time = درج; }
	فراغ set_cmd_error_throw(bool درج) { cmd_error_throw = درج; }
	فراغ set_hdump_all(bool درج) { hdump_all = درج; }
	صحيح get_verbose_level() ثابت { أرجع verbose_level; }
	bool get_log_time() ثابت { أرجع log_time; }
	صحيح get_warnings_unique() ثابت { أرجع GetSize(warnings); }
	صحيح get_warnings_total() ثابت { أرجع warnings_كدي; }
	صحيح get_errors_total() ثابت { أرجع errors_كدي; }
	ثابت std::set<std::string> &get_experimental() ثابت { أرجع experimental; }
	ثابت std::map<std::string, std::string> &get_deprecated() ثابت { أرجع deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ثابت;

	فراغ add_hdump(std::string name, std::string درج) { hdump[name].سك(درج); }

	فراغ formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	فراغ formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	فراغ formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] فراغ formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	فراغ formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] فراغ formatted_cmd_error(std::string_view format, std::string message);
	فراغ suppressed();
	فراغ add_experimental(ثابت std::string &str);
	فراغ add_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal);
	فراغ spacer();
	فراغ تور();
	فراغ چرو();

	فراغ reset_تر();

	فراغ check_expected();
	bool expects_error() { أرجع (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(صحيح n = 0) { إذا (force_debug) أرجع true; debug_suppressed += n; أرجع false; }
#وإلا
	bool is_debug(صحيح = 0) { أرجع false; }
#endif
	فراغ start_hasher();
	std::string finish_hasher();

خاص:
	فراغ logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	فراغ error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	صحيح verbose_level = 0;
	صحيح newline_كدي = 0;
	vector<صحيح> header_كدي;
	صحيح errors_كدي = 0;
	صحيح warnings_كدي = 0;
	صحيح warnings_كدي_noexpect = 0;
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

	صحيح debug_suppressed = 0;
	صحيح make_debug = 0;
	صحيح force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

خارجي فراغ (*log_error_atexit)();

فراغ set_verific_logging(فراغ (*cb)(صحيح سكو_type, ثابت حرف *message_id, LogSourceLocation src, ثابت حرف *سكو));
خارجي فراغ (*log_verific_callback)(صحيح سكو_type, ثابت حرف *message_id, LogSourceLocation src, ثابت حرف *سكو);

#ifndef NDEBUG
ساكن مضمن bool ys_debug(صحيح n = 0) { أرجع logger().is_debug(n); }
#وإلا
ساكن مضمن bool ys_debug(صحيح = 0) { أرجع false; }
#endif

template <typename... حجوجي_3>
مضمن فراغ log(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
مضمن فراغ log_comment(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
مضمن فراغ log_highlight(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
مضمن فراغ log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(حجوجي_2...));
}

#define log_debug(...) افعل { إذا (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } طالما (0)

template <typename... حجوجي_3>
مضمن فراغ log_header(RTLIL::Design *design, FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
مضمن فراغ log_warning(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
مضمن فراغ log_warning_noprefix(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(حجوجي_2...));
}

مضمن فراغ log_experimental(ثابت std::string &str)
{
	logger().add_experimental(str);
}

مضمن فراغ log_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... حجوجي_3>
فراغ log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
فراغ log_file_info(LogSourceLocation src, FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
[[noreturn]] فراغ log_error(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
[[noreturn]] فراغ log_file_error(LogSourceLocation src, FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
فراغ log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(حجوجي_2...));
}

template <typename... حجوجي_3>
[[noreturn]] فراغ log_cmd_error(FmtString<TypeIdentity<حجوجي_3>...> fmt, ثابت حجوجي_3 &... حجوجي_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(حجوجي_2...));
}

مضمن فراغ log_suppressed()
{
	logger().suppressed();
}

مضمن فراغ log_spacer() { logger().spacer(); }
مضمن فراغ log_تور() { logger().تور(); }
مضمن فراغ log_چرو() { logger().چرو(); }

مضمن فراغ log_reset_تر() { logger().reset_تر(); }
مضمن فراغ log_flush() { logger().flush(); }

فراغ log_backtrace(ثابت حرف *prefix, صحيح levels);


std::string log_signal(ثابت RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ثابت RTLIL::Const &درج, bool autoint = true);

فراغ log_module(RTLIL::Module *module, std::string indent = "");
فراغ log_cell(RTLIL::Cell *cell, std::string indent = "");
فراغ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
فراغ log_abort_internal(ثابت حرف *file, صحيح لي);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
بنية PerformanceTimer
{
#إذا 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	ساكن int64_t query() {
#  ifdef _WIN32
		أرجع 0;
#  elif defined(RUSAGE_SELF)
		بنية rusage rusage;
		int64_t t = 0;
		لكل (صحيح who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			إذا (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		أرجع t;
#  وإلا
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	فراغ reset() {
		total_ns = 0;
	}

	فراغ begin() {
		total_ns -= query();
	}

	فراغ end() {
		total_ns += query();
	}

	عائم sec() ثابت {
		أرجع total_ns * 1e-9f;
	}
#وإلا
	ساكن int64_t query() { أرجع 0; }
	فراغ reset() { }
	فراغ begin() { }
	فراغ end() { }
	عائم sec() ثابت { أرجع 0; }
#endif
};

// simple API for quickly dumping values when debugging

ساكن مضمن فراغ log_dump_val_worker(short v) { log("%d", v); }
ساكن مضمن فراغ log_dump_val_worker(unsigned short v) { log("%u", v); }
ساكن مضمن فراغ log_dump_val_worker(صحيح v) { log("%d", v); }
ساكن مضمن فراغ log_dump_val_worker(unsigned صحيح v) { log("%u", v); }
ساكن مضمن فراغ log_dump_val_worker(long صحيح v) { log("%ld", v); }
ساكن مضمن فراغ log_dump_val_worker(unsigned long صحيح v) { log("%lu", v); }
#ifndef _WIN32
ساكن مضمن فراغ log_dump_val_worker(long long صحيح v) { log("%lld", v); }
ساكن مضمن فراغ log_dump_val_worker(unsigned long long صحيح v) { log("%lld", v); }
#endif
ساكن مضمن فراغ log_dump_val_worker(حرف c)
{
	إذا (c >= 32 && c < 127) {
		log("'%c'", c);
	} وإلا {
		log("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_worker(unsigned حرف c)
{
	إذا (c >= 32 && c < 127) {
		log("'%c'", c);
	} وإلا {
		log("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
ساكن مضمن فراغ log_dump_val_worker(مزدوج v) { log("%f", v); }
ساكن مضمن فراغ log_dump_val_worker(حرف *v) { log("%s", v); }
ساكن مضمن فراغ log_dump_val_worker(ثابت حرف *v) { log("%s", v); }
ساكن مضمن فراغ log_dump_val_worker(std::string v) { log("%s", v); }
ساكن مضمن فراغ log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
ساكن مضمن فراغ log_dump_حجوجي_worker(ثابت حرف *p) { log_assert(*p == 0); }
فراغ log_dump_val_worker(IdString v);
فراغ log_dump_val_worker(RTLIL::SigSpec v);
فراغ log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> ساكن مضمن فراغ log_dump_val_worker(dict<K, T> &v);
template<typename K> ساكن مضمن فراغ log_dump_val_worker(pool<K> &v);
template<typename K> ساكن مضمن فراغ log_dump_val_worker(std::vector<K> &v);
template<typename T> ساكن مضمن فراغ log_dump_val_worker(T *ptr);

template<typename K, typename T>
ساكن مضمن فراغ log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	لكل (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
ساكن مضمن فراغ log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	لكل (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
ساكن مضمن فراغ log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	لكل (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
ساكن مضمن فراغ log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... حجوجي_3>
فراغ log_dump_حجوجي_worker(ثابت حرف *p, T first, حجوجي_3 ... حجوجي_2)
{
	صحيح next_p_state = 0;
	ثابت حرف *next_p = p;
	طالما (*next_p && (next_p_state != 0 || *next_p != ',')) {
		إذا (*next_p == '"')
			افعل {
				next_p++;
				طالما (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} طالما (*next_p && *next_p != '"');
		إذا (*next_p == '\'') {
			next_p++;
			إذا (*next_p == '\\')
				next_p++;
			إذا (*next_p)
				next_p++;
		}
		إذا (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		إذا ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", صحيح(next_p - p), p);
	إذا (*next_p == ',')
		next_p++;
	طالما (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_حجوجي_worker(next_p, حجوجي_2 ...);
}

#define log_dump(...) افعل { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_حجوجي_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} طالما (0)

YOSYS_NAMESPACE_END

#ضمن "kernel/yosys.h"

#endif

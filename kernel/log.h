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

#અન્તર્ભાવ "kernel/yosys_common.h"

#અન્તર્ભાવ <chrono>
#અન્તર્ભાવ <time.h>

#અન્તર્ભાવ <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#જો defined(_WIN32)
#  include <intrin.h>
#નહીંતર
#  include <sys/time.h>
#  include <sys/resource.h>
#  જો defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#જો defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#નહીંતર
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# જો __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP ઉઠાવો(SIGTRAP)
# નહીંતર
#  define YS_DEBUGTRAP કરો {} જ્યારે(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#જો defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING કરો { જો (IsDebuggerPresent()) DebugBreak(); } જ્યારે(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING કરો { \
		auto old = સંકેત(SIGTRAP, SIG_IGN); ઉઠાવો(SIGTRAP); સંકેત(SIGTRAP, old); \
	} જ્યારે(0)
#નહીંતર
# define YS_DEBUGTRAP_IF_DEBUGGING કરો {} જ્યારે(0)
#endif

સંરચના log_cmd_error_exception { };

ગણના class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

સંરચના LogSourceLocation {
	LogSourceLocation() : filename({}),start_લીટી(-1),start_col(-1),end_લીટી(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_લીટી(-1),start_col(-1),end_લીટી(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,પૂર્ણાંક start_લીટી) : filename(filename),start_લીટી(start_લીટી),start_col(-1),end_લીટી(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,પૂર્ણાંક start_લીટી,પૂર્ણાંક start_col,પૂર્ણાંક end_લીટી,પૂર્ણાંક end_col) :
		filename(filename),start_લીટી(start_લીટી),start_col(start_col),end_લીટી(end_લીટી),end_col(end_col) { }
	std::string filename;
	પૂર્ણાંક start_લીટી;
	પૂર્ણાંક start_col;
	પૂર્ણાંક end_લીટી;
	પૂર્ણાંક end_col;
};

સંરચના LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_સંદેશ;
};

class LogSink
{
જાહેર:
	આભાસી ~LogSink() = મૂળભૂત;

	આભાસી bool should_log(અચલ LogMessage &) અચલ { પાછા true; }
	આભાસી શૂન્ય log(અચલ LogMessage &સંદેશ) = 0;
	આભાસી શૂન્ય flush() {}
	// TODO: Remove when AST/read_verilog removed
	આભાસી FILE *file_handle() { પાછા nullptr; }
};

class LogSinkRef : જાહેર LogSink
{
જાહેર:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(અચલ LogMessage &સંદેશ) અચલ override { પાછા sink->should_log(સંદેશ); }
	શૂન્ય log(અચલ LogMessage &સંદેશ) override { sink->log(સંદેશ); }
	શૂન્ય flush() override { sink->flush(); }
	FILE *file_handle() override { પાછા sink->file_handle(); }
ખાનગી:
	LogSink *sink;
};

class FileLogSink : જાહેર LogSink
{
જાહેર:
	explicit FileLogSink(અચલ std::string &filename, bool લીટી_buffered, bool append);
	~FileLogSink() override;
	શૂન્ય log(અચલ LogMessage &સંદેશ) override;
	શૂન્ય flush() override;
	FILE *file_handle() override { પાછા file; }

ખાનગી:
	FILE *file;
};

class ConsoleLogSink : જાહેર LogSink
{
જાહેર:
	શૂન્ય log(અચલ LogMessage &સંદેશ) override;
	શૂન્ય flush() override;
	FILE *file_handle() override { પાછા stdout; }
};

class StderrLogSink : જાહેર LogSink
{
જાહેર:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(અચલ LogMessage &સંદેશ) અચલ override;
	શૂન્ય log(અચલ LogMessage &સંદેશ) override;
	શૂન્ય flush() override;
ખાનગી:
	bool quiet_warnings;
};

class StreamLogSink : જાહેર LogSink
{
જાહેર:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	શૂન્ય log(અચલ LogMessage &સંદેશ) override { stream << સંદેશ.cached_સંદેશ; }
	શૂન્ય flush() override { stream.flush(); }

ખાનગી:
	std::ostream &stream;
};

class ScratchPadLogSink : જાહેર LogSink
{
જાહેર:
	explicit ScratchPadLogSink(std::string scratchpad);
	શૂન્ય log(અચલ LogMessage &સંદેશ) override;

ખાનગી:
	std::string scratchpad;
};

class LogManager
{
ખાનગી:
	સંરચના LogExpectedItem
	{
		LogExpectedItem(અચલ std::regex &pat, પૂર્ણાંક expected) :
				pattern(pat), expected_ગણતરી(expected), current_ગણતરી(0) {}
		LogExpectedItem() : expected_ગણતરી(0), current_ગણતરી(0) {}

		std::regex pattern;
		પૂર્ણાંક expected_ગણતરી;
		પૂર્ણાંક current_ગણતરી;
	};

જાહેર:
	LogManager() = મૂળભૂત;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.ધકેલો_back(std::move(sink));
		પાછા ref;
	}

	template<typename F>
	શૂન્ય for_each_sink(F &&func)
	{
		માટે (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { પાછા sinks.empty(); }
	શૂન્ય clear() { sinks.clear(); }
	શૂન્ય clear_original();
	શૂન્ય flush() { માટે (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	જાહેર:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			માટે (અચલ auto &sink : backup_sinks)
				manager.sinks.ધકેલો_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(અચલ Scoped &) = delete;
		Scoped &operator=(અચલ Scoped &) = delete;

	ખાનગી:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		પૂર્ણાંક backup_verbose_level;
	};

	Scoped sink_scope()
	{
		પાછા Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	જાહેર:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	ખાનગી:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		પાછા ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	જાહેર:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			જો (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		શૂન્ય on()
		{
			જો (status)
				પાછા;
			status = true;
			manager.make_debug++;
		}

		શૂન્ય off_silent()
		{
			જો (!status)
				પાછા;
			status = false;
			manager.make_debug--;
		}

		શૂન્ય off()
		{
			off_silent();
		}
	ખાનગી:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		પાછા LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	જાહેર:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			જો (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		શૂન્ય on()
		{
			જો (active)
				પાછા;

			active = true;
			manager.force_debug++;
		}

		શૂન્ય off()
		{
			જો (!active)
				પાછા;

			active = false;
			manager.force_debug--;
		}
	ખાનગી:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		પાછા ForceDebug(*this, start_on);
	}

	શૂન્ય force_debug_on() { force_debug++; }
	શૂન્ય force_debug_off() { જો (force_debug > 0) force_debug--; }
	શૂન્ય set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	શૂન્ય report_unexpected_error();

	શૂન્ય add_experimental_ignore(std::string name) { experimental_ignored.ઉમેરો(name); }
	શૂન્ય add_warn(std::string pattern) { warn_regexes.ધકેલો_back(YS_REGEX_COMPILE(pattern)); }
	શૂન્ય add_nowarn(std::string pattern) { nowarn_regexes.ધકેલો_back(YS_REGEX_COMPILE(pattern)); }
	શૂન્ય add_werror(std::string pattern) { werror_regexes.ધકેલો_back(YS_REGEX_COMPILE(pattern)); }
	શૂન્ય add_expect(std::string type, std::string pattern, પૂર્ણાંક ગણતરી);

	શૂન્ય set_verbose_level(પૂર્ણાંક level) { verbose_level = level; }
	શૂન્ય add_verbose_level(પૂર્ણાંક level) { verbose_level += level; }
	શૂન્ય set_expect_no_warnings(bool કિંમત) { expect_no_warnings = કિંમત; }
	શૂન્ય set_log_time(bool કિંમત) { log_time = કિંમત; }
	શૂન્ય set_cmd_error_throw(bool કિંમત) { cmd_error_throw = કિંમત; }
	શૂન્ય set_hdump_all(bool કિંમત) { hdump_all = કિંમત; }
	પૂર્ણાંક get_verbose_level() અચલ { પાછા verbose_level; }
	bool get_log_time() અચલ { પાછા log_time; }
	પૂર્ણાંક get_warnings_unique() અચલ { પાછા GetSize(warnings); }
	પૂર્ણાંક get_warnings_total() અચલ { પાછા warnings_ગણતરી; }
	પૂર્ણાંક get_errors_total() અચલ { પાછા errors_ગણતરી; }
	અચલ std::set<std::string> &get_experimental() અચલ { પાછા experimental; }
	અચલ std::map<std::string, std::string> &get_deprecated() અચલ { પાછા deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() અચલ;

	શૂન્ય add_hdump(std::string name, std::string કિંમત) { hdump[name].ઉમેરો(કિંમત); }

	શૂન્ય formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	શૂન્ય formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	શૂન્ય formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] શૂન્ય formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	શૂન્ય formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] શૂન્ય formatted_cmd_error(std::string_view format, std::string message);
	શૂન્ય suppressed();
	શૂન્ય add_experimental(અચલ std::string &str);
	શૂન્ય add_deprecated(અચલ std::string &feature, અચલ std::string &earliest_removal);
	શૂન્ય spacer();
	શૂન્ય ધકેલો();
	શૂન્ય કાઢો();

	શૂન્ય reset_ઢગલો();

	શૂન્ય check_expected();
	bool expects_error() { પાછા (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(પૂર્ણાંક n = 0) { જો (force_debug) પાછા true; debug_suppressed += n; પાછા false; }
#નહીંતર
	bool is_debug(પૂર્ણાંક = 0) { પાછા false; }
#endif
	શૂન્ય start_hasher();
	std::string finish_hasher();

ખાનગી:
	શૂન્ય logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	શૂન્ય error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	પૂર્ણાંક verbose_level = 0;
	પૂર્ણાંક newline_ગણતરી = 0;
	vector<પૂર્ણાંક> header_ગણતરી;
	પૂર્ણાંક errors_ગણતરી = 0;
	પૂર્ણાંક warnings_ગણતરી = 0;
	પૂર્ણાંક warnings_ગણતરી_noexpect = 0;
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

	પૂર્ણાંક debug_suppressed = 0;
	પૂર્ણાંક make_debug = 0;
	પૂર્ણાંક force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

બાહ્ય શૂન્ય (*log_error_atexit)();

શૂન્ય set_verific_logging(શૂન્ય (*cb)(પૂર્ણાંક સંદેશ_type, અચલ અક્ષર *message_id, LogSourceLocation src, અચલ અક્ષર *સંદેશ));
બાહ્ય શૂન્ય (*log_verific_callback)(પૂર્ણાંક સંદેશ_type, અચલ અક્ષર *message_id, LogSourceLocation src, અચલ અક્ષર *સંદેશ);

#ifndef NDEBUG
સ્થિર અંતઃરેખ bool ys_debug(પૂર્ણાંક n = 0) { પાછા logger().is_debug(n); }
#નહીંતર
સ્થિર અંતઃરેખ bool ys_debug(પૂર્ણાંક = 0) { પાછા false; }
#endif

template <typename... Args>
અંતઃરેખ શૂન્ય log(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
અંતઃરેખ શૂન્ય log_comment(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
અંતઃરેખ શૂન્ય log_highlight(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
અંતઃરેખ શૂન્ય log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) કરો { જો (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } જ્યારે (0)

template <typename... Args>
અંતઃરેખ શૂન્ય log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
અંતઃરેખ શૂન્ય log_warning(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
અંતઃરેખ શૂન્ય log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

અંતઃરેખ શૂન્ય log_experimental(અચલ std::string &str)
{
	logger().add_experimental(str);
}

અંતઃરેખ શૂન્ય log_deprecated(અચલ std::string &feature, અચલ std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
શૂન્ય log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
શૂન્ય log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] શૂન્ય log_error(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] શૂન્ય log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
શૂન્ય log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] શૂન્ય log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, અચલ Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

અંતઃરેખ શૂન્ય log_suppressed()
{
	logger().suppressed();
}

અંતઃરેખ શૂન્ય log_spacer() { logger().spacer(); }
અંતઃરેખ શૂન્ય log_ધકેલો() { logger().ધકેલો(); }
અંતઃરેખ શૂન્ય log_કાઢો() { logger().કાઢો(); }

અંતઃરેખ શૂન્ય log_reset_ઢગલો() { logger().reset_ઢગલો(); }
અંતઃરેખ શૂન્ય log_flush() { logger().flush(); }

શૂન્ય log_backtrace(અચલ અક્ષર *prefix, પૂર્ણાંક levels);


std::string log_signal(અચલ RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(અચલ RTLIL::Const &કિંમત, bool autoint = true);

શૂન્ય log_module(RTLIL::Module *module, std::string indent = "");
શૂન્ય log_cell(RTLIL::Cell *cell, std::string indent = "");
શૂન્ય log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
શૂન્ય log_abort_internal(અચલ અક્ષર *file, પૂર્ણાંક લીટી);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
સંરચના PerformanceTimer
{
#જો 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	સ્થિર int64_t query() {
#  ifdef _WIN32
		પાછા 0;
#  elif defined(RUSAGE_SELF)
		સંરચના rusage rusage;
		int64_t t = 0;
		માટે (પૂર્ણાંક who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			જો (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		પાછા t;
#  નહીંતર
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	શૂન્ય reset() {
		total_ns = 0;
	}

	શૂન્ય begin() {
		total_ns -= query();
	}

	શૂન્ય end() {
		total_ns += query();
	}

	દશાંશ sec() અચલ {
		પાછા total_ns * 1e-9f;
	}
#નહીંતર
	સ્થિર int64_t query() { પાછા 0; }
	શૂન્ય reset() { }
	શૂન્ય begin() { }
	શૂન્ય end() { }
	દશાંશ sec() અચલ { પાછા 0; }
#endif
};

// simple API for quickly dumping values when debugging

સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(short v) { log("%d", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(unsigned short v) { log("%u", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(પૂર્ણાંક v) { log("%d", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(unsigned પૂર્ણાંક v) { log("%u", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(long પૂર્ણાંક v) { log("%ld", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(unsigned long પૂર્ણાંક v) { log("%lu", v); }
#ifndef _WIN32
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(long long પૂર્ણાંક v) { log("%lld", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(unsigned long long પૂર્ણાંક v) { log("%lld", v); }
#endif
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(અક્ષર c)
{
	જો (c >= 32 && c < 127) {
		log("'%c'", c);
	} નહીંતર {
		log("'\\x%02x'", c);
	}
}
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(unsigned અક્ષર c)
{
	જો (c >= 32 && c < 127) {
		log("'%c'", c);
	} નહીંતર {
		log("'\\x%02x'", c);
	}
}
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(દ્વિગુણ v) { log("%f", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(અક્ષર *v) { log("%s", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(અચલ અક્ષર *v) { log("%s", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(std::string v) { log("%s", v); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
સ્થિર અંતઃરેખ શૂન્ય log_dump_args_worker(અચલ અક્ષર *p) { log_assert(*p == 0); }
શૂન્ય log_dump_val_worker(IdString v);
શૂન્ય log_dump_val_worker(RTLIL::SigSpec v);
શૂન્ય log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(dict<K, T> &v);
template<typename K> સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(pool<K> &v);
template<typename K> સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(std::vector<K> &v);
template<typename T> સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(T *ptr);

template<typename K, typename T>
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	માટે (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	માટે (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	માટે (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
સ્થિર અંતઃરેખ શૂન્ય log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
શૂન્ય log_dump_args_worker(અચલ અક્ષર *p, T first, Args ... args)
{
	પૂર્ણાંક next_p_state = 0;
	અચલ અક્ષર *next_p = p;
	જ્યારે (*next_p && (next_p_state != 0 || *next_p != ',')) {
		જો (*next_p == '"')
			કરો {
				next_p++;
				જ્યારે (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} જ્યારે (*next_p && *next_p != '"');
		જો (*next_p == '\'') {
			next_p++;
			જો (*next_p == '\\')
				next_p++;
			જો (*next_p)
				next_p++;
		}
		જો (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		જો ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", પૂર્ણાંક(next_p - p), p);
	જો (*next_p == ',')
		next_p++;
	જ્યારે (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) કરો { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} જ્યારે (0)

YOSYS_NAMESPACE_END

#અન્તર્ભાવ "kernel/yosys.h"

#endif

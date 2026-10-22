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

#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव <chrono>
#अन्तर्भाव <time.h>

#अन्तर्भाव <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#जर defined(_WIN32)
#  include <intrin.h>
#अन्यथा
#  include <sys/time.h>
#  include <sys/resource.h>
#  जर defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#जर defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#अन्यथा
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# जर __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP उठवा(SIGTRAP)
# अन्यथा
#  define YS_DEBUGTRAP करा {} पर्यंत(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#जर defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING करा { जर (IsDebuggerPresent()) DebugBreak(); } पर्यंत(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING करा { \
		स्वतः old = संकेत(SIGTRAP, संक_छोड़ो); उठवा(SIGTRAP); संकेत(SIGTRAP, old); \
	} पर्यंत(0)
#अन्यथा
# define YS_DEBUGTRAP_IF_DEBUGGING करा {} पर्यंत(0)
#endif

संरचना log_cmd_error_exception { };

गणना class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

संरचना LogSourceLocation {
	LogSourceLocation() : filename({}),start_ओळ(-1),start_col(-1),end_ओळ(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_ओळ(-1),start_col(-1),end_ओळ(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णांक start_ओळ) : filename(filename),start_ओळ(start_ओळ),start_col(-1),end_ओळ(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णांक start_ओळ,पूर्णांक start_col,पूर्णांक end_ओळ,पूर्णांक end_col) :
		filename(filename),start_ओळ(start_ओळ),start_col(start_col),end_ओळ(end_ओळ),end_col(end_col) { }
	std::string filename;
	पूर्णांक start_ओळ;
	पूर्णांक start_col;
	पूर्णांक end_ओळ;
	पूर्णांक end_col;
};

संरचना LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_संदेश;
};

class LogSink
{
सार्वजनिक:
	आभासी ~LogSink() = पूर्वनिर्धारित;

	आभासी bool should_log(स्थिरांक LogMessage &) स्थिरांक { परत true; }
	आभासी शून्य log(स्थिरांक LogMessage &संदेश) = 0;
	आभासी शून्य flush() {}
	// TODO: Remove when AST/read_verilog removed
	आभासी खाता *file_handle() { परत nullptr; }
};

class LogSinkRef : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(स्थिरांक LogMessage &संदेश) स्थिरांक override { परत sink->should_log(संदेश); }
	शून्य log(स्थिरांक LogMessage &संदेश) override { sink->log(संदेश); }
	शून्य flush() override { sink->flush(); }
	खाता *file_handle() override { परत sink->file_handle(); }
खाजगी:
	LogSink *sink;
};

class FileLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit FileLogSink(स्थिरांक std::string &filename, bool ओळ_buffered, bool append);
	~FileLogSink() override;
	शून्य log(स्थिरांक LogMessage &संदेश) override;
	शून्य flush() override;
	खाता *file_handle() override { परत file; }

खाजगी:
	खाता *file;
};

class ConsoleLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	शून्य log(स्थिरांक LogMessage &संदेश) override;
	शून्य flush() override;
	खाता *file_handle() override { परत मानक_निकास; }
};

class StderrLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(स्थिरांक LogMessage &संदेश) स्थिरांक override;
	शून्य log(स्थिरांक LogMessage &संदेश) override;
	शून्य flush() override;
खाजगी:
	bool quiet_warnings;
};

class StreamLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	शून्य log(स्थिरांक LogMessage &संदेश) override { stream << संदेश.cached_संदेश; }
	शून्य flush() override { stream.flush(); }

खाजगी:
	std::ostream &stream;
};

class ScratchPadLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit ScratchPadLogSink(std::string scratchpad);
	शून्य log(स्थिरांक LogMessage &संदेश) override;

खाजगी:
	std::string scratchpad;
};

class LogManager
{
खाजगी:
	संरचना LogExpectedItem
	{
		LogExpectedItem(स्थिरांक std::regex &pat, पूर्णांक expected) :
				pattern(pat), expected_मोजणी(expected), current_मोजणी(0) {}
		LogExpectedItem() : expected_मोजणी(0), current_मोजणी(0) {}

		std::regex pattern;
		पूर्णांक expected_मोजणी;
		पूर्णांक current_मोजणी;
	};

सार्वजनिक:
	LogManager() = पूर्वनिर्धारित;

	ढाँचा<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		स्वतः sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.ढकला_back(std::move(sink));
		परत ref;
	}

	ढाँचा<typename F>
	शून्य for_each_sink(F &&func)
	{
		साठी (स्वतः &sink : sinks)
			func(*sink);
	}

	bool empty() { परत sinks.empty(); }
	शून्य clear() { sinks.clear(); }
	शून्य clear_original();
	शून्य flush() { साठी (स्वतः &sink : sinks) sink->flush(); }

	class Scoped
	{
	सार्वजनिक:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			साठी (स्थिरांक स्वतः &sink : backup_sinks)
				manager.sinks.ढकला_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(स्थिरांक Scoped &) = delete;
		Scoped &चालक=(स्थिरांक Scoped &) = delete;

	खाजगी:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		पूर्णांक backup_verbose_level;
	};

	Scoped sink_scope()
	{
		परत Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	सार्वजनिक:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	खाजगी:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		परत ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	सार्वजनिक:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			जर (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		शून्य on()
		{
			जर (status)
				परत;
			status = true;
			manager.make_debug++;
		}

		शून्य off_silent()
		{
			जर (!status)
				परत;
			status = false;
			manager.make_debug--;
		}

		शून्य off()
		{
			off_silent();
		}
	खाजगी:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		परत LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	सार्वजनिक:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			जर (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		शून्य on()
		{
			जर (active)
				परत;

			active = true;
			manager.force_debug++;
		}

		शून्य off()
		{
			जर (!active)
				परत;

			active = false;
			manager.force_debug--;
		}
	खाजगी:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		परत ForceDebug(*this, start_on);
	}

	शून्य force_debug_on() { force_debug++; }
	शून्य force_debug_off() { जर (force_debug > 0) force_debug--; }
	शून्य set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	शून्य report_unexpected_error();

	शून्य add_experimental_ignore(std::string name) { experimental_ignored.घाला(name); }
	शून्य add_warn(std::string pattern) { warn_regexes.ढकला_back(YS_REGEX_COMPILE(pattern)); }
	शून्य add_nowarn(std::string pattern) { nowarn_regexes.ढकला_back(YS_REGEX_COMPILE(pattern)); }
	शून्य add_werror(std::string pattern) { werror_regexes.ढकला_back(YS_REGEX_COMPILE(pattern)); }
	शून्य add_expect(std::string type, std::string pattern, पूर्णांक मोजणी);

	शून्य set_verbose_level(पूर्णांक level) { verbose_level = level; }
	शून्य add_verbose_level(पूर्णांक level) { verbose_level += level; }
	शून्य set_expect_no_warnings(bool मूल्य) { expect_no_warnings = मूल्य; }
	शून्य set_log_time(bool मूल्य) { log_time = मूल्य; }
	शून्य set_cmd_error_throw(bool मूल्य) { cmd_error_throw = मूल्य; }
	शून्य set_hdump_all(bool मूल्य) { hdump_all = मूल्य; }
	पूर्णांक get_verbose_level() स्थिरांक { परत verbose_level; }
	bool get_log_time() स्थिरांक { परत log_time; }
	पूर्णांक get_warnings_unique() स्थिरांक { परत GetSize(warnings); }
	पूर्णांक get_warnings_total() स्थिरांक { परत warnings_मोजणी; }
	पूर्णांक get_errors_total() स्थिरांक { परत errors_मोजणी; }
	स्थिरांक std::set<std::string> &get_experimental() स्थिरांक { परत experimental; }
	स्थिरांक std::map<std::string, std::string> &get_deprecated() स्थिरांक { परत deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() स्थिरांक;

	शून्य add_hdump(std::string name, std::string मूल्य) { hdump[name].घाला(मूल्य); }

	शून्य formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	शून्य formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	शून्य formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] शून्य formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	शून्य formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] शून्य formatted_cmd_error(std::string_view format, std::string message);
	शून्य suppressed();
	शून्य add_experimental(स्थिरांक std::string &str);
	शून्य add_deprecated(स्थिरांक std::string &feature, स्थिरांक std::string &earliest_removal);
	शून्य spacer();
	शून्य ढकला();
	शून्य काढा();

	शून्य reset_चळत();

	शून्य check_expected();
	bool expects_error() { परत (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef न_संशोधन
	bool is_debug(पूर्णांक n = 0) { जर (force_debug) परत true; debug_suppressed += n; परत false; }
#अन्यथा
	bool is_debug(पूर्णांक = 0) { परत false; }
#endif
	शून्य start_hasher();
	std::string finish_hasher();

खाजगी:
	शून्य logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	शून्य error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	पूर्णांक verbose_level = 0;
	पूर्णांक newline_मोजणी = 0;
	vector<पूर्णांक> header_मोजणी;
	पूर्णांक errors_मोजणी = 0;
	पूर्णांक warnings_मोजणी = 0;
	पूर्णांक warnings_मोजणी_noexpect = 0;
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

	पूर्णांक debug_suppressed = 0;
	पूर्णांक make_debug = 0;
	पूर्णांक force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

बाह्य शून्य (*log_error_atexit)();

शून्य set_verific_logging(शून्य (*cb)(पूर्णांक संदेश_type, स्थिरांक अक्षर *message_id, LogSourceLocation src, स्थिरांक अक्षर *संदेश));
बाह्य शून्य (*log_verific_callback)(पूर्णांक संदेश_type, स्थिरांक अक्षर *message_id, LogSourceLocation src, स्थिरांक अक्षर *संदेश);

#ifndef न_संशोधन
स्थिर ओळीत bool ys_debug(पूर्णांक n = 0) { परत logger().is_debug(n); }
#अन्यथा
स्थिर ओळीत bool ys_debug(पूर्णांक = 0) { परत false; }
#endif

ढाँचा <typename... Args>
ओळीत शून्य log(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
ओळीत शून्य log_comment(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
ओळीत शून्य log_highlight(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
ओळीत शून्य log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) करा { जर (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } पर्यंत (0)

ढाँचा <typename... Args>
ओळीत शून्य log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
ओळीत शून्य log_warning(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
ओळीत शून्य log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ओळीत शून्य log_experimental(स्थिरांक std::string &str)
{
	logger().add_experimental(str);
}

ओळीत शून्य log_deprecated(स्थिरांक std::string &feature, स्थिरांक std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
ढाँचा <typename... Args>
शून्य log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
शून्य log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
[[noreturn]] शून्य log_error(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
[[noreturn]] शून्य log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
शून्य log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
[[noreturn]] शून्य log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, स्थिरांक Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

ओळीत शून्य log_suppressed()
{
	logger().suppressed();
}

ओळीत शून्य log_spacer() { logger().spacer(); }
ओळीत शून्य log_ढकला() { logger().ढकला(); }
ओळीत शून्य log_काढा() { logger().काढा(); }

ओळीत शून्य log_reset_चळत() { logger().reset_चळत(); }
ओळीत शून्य log_flush() { logger().flush(); }

शून्य log_backtrace(स्थिरांक अक्षर *prefix, पूर्णांक levels);


std::string log_signal(स्थिरांक RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(स्थिरांक RTLIL::Const &मूल्य, bool autoint = true);

शून्य log_module(RTLIL::Module *module, std::string indent = "");
शून्य log_cell(RTLIL::Cell *cell, std::string indent = "");
शून्य log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
शून्य log_abort_internal(स्थिरांक अक्षर *file, पूर्णांक ओळ);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
संरचना PerformanceTimer
{
#जर 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	स्थिर int64_t query() {
#  ifdef _WIN32
		परत 0;
#  elif defined(RUSAGE_SELF)
		संरचना rusage rusage;
		int64_t t = 0;
		साठी (पूर्णांक who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			जर (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		परत t;
#  अन्यथा
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	शून्य reset() {
		total_ns = 0;
	}

	शून्य begin() {
		total_ns -= query();
	}

	शून्य end() {
		total_ns += query();
	}

	दशांश sec() स्थिरांक {
		परत total_ns * 1e-9f;
	}
#अन्यथा
	स्थिर int64_t query() { परत 0; }
	शून्य reset() { }
	शून्य begin() { }
	शून्य end() { }
	दशांश sec() स्थिरांक { परत 0; }
#endif
};

// simple API for quickly dumping values when debugging

स्थिर ओळीत शून्य log_dump_val_worker(लघु v) { log("%d", v); }
स्थिर ओळीत शून्य log_dump_val_worker(अचिन्हित लघु v) { log("%u", v); }
स्थिर ओळीत शून्य log_dump_val_worker(पूर्णांक v) { log("%d", v); }
स्थिर ओळीत शून्य log_dump_val_worker(अचिन्हित पूर्णांक v) { log("%u", v); }
स्थिर ओळीत शून्य log_dump_val_worker(दीर्घ पूर्णांक v) { log("%ld", v); }
स्थिर ओळीत शून्य log_dump_val_worker(अचिन्हित दीर्घ पूर्णांक v) { log("%lu", v); }
#ifndef _WIN32
स्थिर ओळीत शून्य log_dump_val_worker(दीर्घ दीर्घ पूर्णांक v) { log("%lld", v); }
स्थिर ओळीत शून्य log_dump_val_worker(अचिन्हित दीर्घ दीर्घ पूर्णांक v) { log("%lld", v); }
#endif
स्थिर ओळीत शून्य log_dump_val_worker(अक्षर c)
{
	जर (c >= 32 && c < 127) {
		log("'%c'", c);
	} अन्यथा {
		log("'\\x%02x'", c);
	}
}
स्थिर ओळीत शून्य log_dump_val_worker(अचिन्हित अक्षर c)
{
	जर (c >= 32 && c < 127) {
		log("'%c'", c);
	} अन्यथा {
		log("'\\x%02x'", c);
	}
}
स्थिर ओळीत शून्य log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
स्थिर ओळीत शून्य log_dump_val_worker(द्विगुण v) { log("%f", v); }
स्थिर ओळीत शून्य log_dump_val_worker(अक्षर *v) { log("%s", v); }
स्थिर ओळीत शून्य log_dump_val_worker(स्थिरांक अक्षर *v) { log("%s", v); }
स्थिर ओळीत शून्य log_dump_val_worker(std::string v) { log("%s", v); }
स्थिर ओळीत शून्य log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
स्थिर ओळीत शून्य log_dump_args_worker(स्थिरांक अक्षर *p) { log_assert(*p == 0); }
शून्य log_dump_val_worker(IdString v);
शून्य log_dump_val_worker(RTLIL::SigSpec v);
शून्य log_dump_val_worker(RTLIL::State v);

ढाँचा<typename K, typename T> स्थिर ओळीत शून्य log_dump_val_worker(dict<K, T> &v);
ढाँचा<typename K> स्थिर ओळीत शून्य log_dump_val_worker(pool<K> &v);
ढाँचा<typename K> स्थिर ओळीत शून्य log_dump_val_worker(std::vector<K> &v);
ढाँचा<typename T> स्थिर ओळीत शून्य log_dump_val_worker(T *ptr);

ढाँचा<typename K, typename T>
स्थिर ओळीत शून्य log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	साठी (स्वतः &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

ढाँचा<typename K>
स्थिर ओळीत शून्य log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	साठी (स्वतः &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

ढाँचा<typename K>
स्थिर ओळीत शून्य log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	साठी (स्वतः &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

ढाँचा<typename T>
स्थिर ओळीत शून्य log_dump_val_worker(T *ptr) { log("%p", ptr); }

ढाँचा<typename T, typename ... Args>
शून्य log_dump_args_worker(स्थिरांक अक्षर *p, T first, Args ... args)
{
	पूर्णांक next_p_state = 0;
	स्थिरांक अक्षर *next_p = p;
	पर्यंत (*next_p && (next_p_state != 0 || *next_p != ',')) {
		जर (*next_p == '"')
			करा {
				next_p++;
				पर्यंत (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} पर्यंत (*next_p && *next_p != '"');
		जर (*next_p == '\'') {
			next_p++;
			जर (*next_p == '\\')
				next_p++;
			जर (*next_p)
				next_p++;
		}
		जर (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		जर ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", पूर्णांक(next_p - p), p);
	जर (*next_p == ',')
		next_p++;
	पर्यंत (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) करा { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} पर्यंत (0)

YOSYS_NAMESPACE_END

#अन्तर्भाव "kernel/yosys.h"

#endif

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

#अगर_अघोषित LOG_H
#घोषणा LOG_H

#समावेश "kernel/yosys_common.h"

#समावेश <chrono>
#समावेश <time.h>

#समावेश <regex>
#घोषणा YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#घोषणा YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#अगर defined(_WIN32)
#  include <intrin.h>
#अन्यथा
#  include <sys/time.h>
#  include <sys/resource.h>
#  अगर defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <signal.h>
#  endif
#पूर्ण_अगर

// from libs/sha1/sha1.h
class SHA1;

YOSYS_NAMESPACE_BEGIN

// YS_DEBUGTRAP is a macro that is functionally equivalent to a breakpoint
// if the platform provides such functionality, and does nothing otherwise.
// If no debugger is attached, it starts a just-in-time debugger if available,
// and crashes the process otherwise.
#अगर defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#अन्यथा
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# अगर __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP उठाओ(SIGTRAP)
# अन्यथा
#  define YS_DEBUGTRAP करो {} जबतक(0)
# endif
#पूर्ण_अगर

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#अगर defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING करो { अगर (IsDebuggerPresent()) DebugBreak(); } जबतक(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING करो { \
		स्वतः old = संकेत(SIGTRAP, संक_छोड़ो); उठाओ(SIGTRAP); संकेत(SIGTRAP, old); \
	} जबतक(0)
#अन्यथा
# define YS_DEBUGTRAP_IF_DEBUGGING करो {} जबतक(0)
#पूर्ण_अगर

काष्ठा log_cmd_error_exception { };

क्रमागत class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

काष्ठा LogSourceLocation {
	LogSourceLocation() : filename({}),start_पंक्ति(-1),start_col(-1),end_पंक्ति(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_पंक्ति(-1),start_col(-1),end_पंक्ति(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णांक start_पंक्ति) : filename(filename),start_पंक्ति(start_पंक्ति),start_col(-1),end_पंक्ति(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णांक start_पंक्ति,पूर्णांक start_col,पूर्णांक end_पंक्ति,पूर्णांक end_col) :
		filename(filename),start_पंक्ति(start_पंक्ति),start_col(start_col),end_पंक्ति(end_पंक्ति),end_col(end_col) { }
	std::string filename;
	पूर्णांक start_पंक्ति;
	पूर्णांक start_col;
	पूर्णांक end_पंक्ति;
	पूर्णांक end_col;
};

काष्ठा LogMessage {
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
खुला:
	भव ~LogSink() = शेष;

	भव bool should_log(स्थिर LogMessage &) स्थिर { वापस true; }
	भव व्योम log(स्थिर LogMessage &संदेश) = 0;
	भव व्योम flush() {}
	// TODO: Remove when AST/read_verilog removed
	भव खाता *file_handle() { वापस nullptr; }
};

class LogSinkRef : खुला LogSink
{
खुला:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(स्थिर LogMessage &संदेश) स्थिर override { वापस sink->should_log(संदेश); }
	व्योम log(स्थिर LogMessage &संदेश) override { sink->log(संदेश); }
	व्योम flush() override { sink->flush(); }
	खाता *file_handle() override { वापस sink->file_handle(); }
निजी:
	LogSink *sink;
};

class FileLogSink : खुला LogSink
{
खुला:
	explicit FileLogSink(स्थिर std::string &filename, bool पंक्ति_buffered, bool append);
	~FileLogSink() override;
	व्योम log(स्थिर LogMessage &संदेश) override;
	व्योम flush() override;
	खाता *file_handle() override { वापस file; }

निजी:
	खाता *file;
};

class ConsoleLogSink : खुला LogSink
{
खुला:
	व्योम log(स्थिर LogMessage &संदेश) override;
	व्योम flush() override;
	खाता *file_handle() override { वापस मानक_निकास; }
};

class StderrLogSink : खुला LogSink
{
खुला:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(स्थिर LogMessage &संदेश) स्थिर override;
	व्योम log(स्थिर LogMessage &संदेश) override;
	व्योम flush() override;
निजी:
	bool quiet_warnings;
};

class StreamLogSink : खुला LogSink
{
खुला:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	व्योम log(स्थिर LogMessage &संदेश) override { stream << संदेश.cached_संदेश; }
	व्योम flush() override { stream.flush(); }

निजी:
	std::ostream &stream;
};

class ScratchPadLogSink : खुला LogSink
{
खुला:
	explicit ScratchPadLogSink(std::string scratchpad);
	व्योम log(स्थिर LogMessage &संदेश) override;

निजी:
	std::string scratchpad;
};

class LogManager
{
निजी:
	काष्ठा LogExpectedItem
	{
		LogExpectedItem(स्थिर std::regex &pat, पूर्णांक expected) :
				pattern(pat), expected_गिनती(expected), current_गिनती(0) {}
		LogExpectedItem() : expected_गिनती(0), current_गिनती(0) {}

		std::regex pattern;
		पूर्णांक expected_गिनती;
		पूर्णांक current_गिनती;
	};

खुला:
	LogManager() = शेष;

	ढाँचा<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		स्वतः sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.धकेलो_back(std::move(sink));
		वापस ref;
	}

	ढाँचा<typename F>
	व्योम for_each_sink(F &&func)
	{
		क्रम (स्वतः &sink : sinks)
			func(*sink);
	}

	bool empty() { वापस sinks.empty(); }
	व्योम clear() { sinks.clear(); }
	व्योम clear_original();
	व्योम flush() { क्रम (स्वतः &sink : sinks) sink->flush(); }

	class Scoped
	{
	खुला:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.आकार());

			क्रम (स्थिर स्वतः &sink : backup_sinks)
				manager.sinks.धकेलो_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(स्थिर Scoped &) = delete;
		Scoped &चालक=(स्थिर Scoped &) = delete;

	निजी:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		पूर्णांक backup_verbose_level;
	};

	Scoped sink_scope()
	{
		वापस Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	खुला:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	निजी:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		वापस ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	खुला:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			अगर (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		व्योम on()
		{
			अगर (status)
				वापस;
			status = true;
			manager.make_debug++;
		}

		व्योम off_silent()
		{
			अगर (!status)
				वापस;
			status = false;
			manager.make_debug--;
		}

		व्योम off()
		{
			off_silent();
		}
	निजी:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		वापस LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	खुला:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			अगर (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		व्योम on()
		{
			अगर (active)
				वापस;

			active = true;
			manager.force_debug++;
		}

		व्योम off()
		{
			अगर (!active)
				वापस;

			active = false;
			manager.force_debug--;
		}
	निजी:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		वापस ForceDebug(*this, start_on);
	}

	व्योम force_debug_on() { force_debug++; }
	व्योम force_debug_off() { अगर (force_debug > 0) force_debug--; }
	व्योम set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	व्योम report_unexpected_error();

	व्योम add_experimental_ignore(std::string name) { experimental_ignored.जोड़ो(name); }
	व्योम add_warn(std::string pattern) { warn_regexes.धकेलो_back(YS_REGEX_COMPILE(pattern)); }
	व्योम add_nowarn(std::string pattern) { nowarn_regexes.धकेलो_back(YS_REGEX_COMPILE(pattern)); }
	व्योम add_werror(std::string pattern) { werror_regexes.धकेलो_back(YS_REGEX_COMPILE(pattern)); }
	व्योम add_expect(std::string type, std::string pattern, पूर्णांक गिनती);

	व्योम set_verbose_level(पूर्णांक level) { verbose_level = level; }
	व्योम add_verbose_level(पूर्णांक level) { verbose_level += level; }
	व्योम set_expect_no_warnings(bool मूल्य) { expect_no_warnings = मूल्य; }
	व्योम set_log_time(bool मूल्य) { log_time = मूल्य; }
	व्योम set_cmd_error_throw(bool मूल्य) { cmd_error_throw = मूल्य; }
	व्योम set_hdump_all(bool मूल्य) { hdump_all = मूल्य; }
	पूर्णांक get_verbose_level() स्थिर { वापस verbose_level; }
	bool get_log_time() स्थिर { वापस log_time; }
	पूर्णांक get_warnings_unique() स्थिर { वापस Get_आकार(warnings); }
	पूर्णांक get_warnings_योग() स्थिर { वापस warnings_गिनती; }
	पूर्णांक get_errors_योग() स्थिर { वापस errors_गिनती; }
	स्थिर std::set<std::string> &get_experimental() स्थिर { वापस experimental; }
	स्थिर std::map<std::string, std::string> &get_deprecated() स्थिर { वापस deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() स्थिर;

	व्योम add_hdump(std::string name, std::string मूल्य) { hdump[name].जोड़ो(मूल्य); }

	व्योम formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	व्योम formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	व्योम formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] व्योम formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	व्योम formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] व्योम formatted_cmd_error(std::string_view format, std::string message);
	व्योम suppressed();
	व्योम add_experimental(स्थिर std::string &str);
	व्योम add_deprecated(स्थिर std::string &feature, स्थिर std::string &earliest_removal);
	व्योम spacer();
	व्योम धकेलो();
	व्योम निकालो();

	व्योम reset_ढेर();

	व्योम check_expected();
	bool expects_error() { वापस (expect_error.आकार() + expect_prefix_error.आकार())>0; }
#अगर_अघोषित न_संशोधन
	bool is_debug(पूर्णांक n = 0) { अगर (force_debug) वापस true; debug_suppressed += n; वापस false; }
#अन्यथा
	bool is_debug(पूर्णांक = 0) { वापस false; }
#पूर्ण_अगर
	व्योम start_hasher();
	std::string finish_hasher();

निजी:
	व्योम logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	व्योम error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	पूर्णांक verbose_level = 0;
	पूर्णांक newline_गिनती = 0;
	vector<पूर्णांक> header_गिनती;
	पूर्णांक errors_गिनती = 0;
	पूर्णांक warnings_गिनती = 0;
	पूर्णांक warnings_गिनती_noexpect = 0;
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

बाह्य व्योम (*log_error_atexit)();

व्योम set_verific_logging(व्योम (*cb)(पूर्णांक संदेश_type, स्थिर अक्षर *message_id, LogSourceLocation src, स्थिर अक्षर *संदेश));
बाह्य व्योम (*log_verific_callback)(पूर्णांक संदेश_type, स्थिर अक्षर *message_id, LogSourceLocation src, स्थिर अक्षर *संदेश);

#अगर_अघोषित न_संशोधन
अटल अंतरभूत bool ys_debug(पूर्णांक n = 0) { वापस logger().is_debug(n); }
#अन्यथा
अटल अंतरभूत bool ys_debug(पूर्णांक = 0) { वापस false; }
#पूर्ण_अगर

ढाँचा <typename... Args>
अंतरभूत व्योम log(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
अंतरभूत व्योम log_comment(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
अंतरभूत व्योम log_highlight(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
अंतरभूत व्योम log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#घोषणा log_debug(...) करो { अगर (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } जबतक (0)

ढाँचा <typename... Args>
अंतरभूत व्योम log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
अंतरभूत व्योम log_warning(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
अंतरभूत व्योम log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

अंतरभूत व्योम log_experimental(स्थिर std::string &str)
{
	logger().add_experimental(str);
}

अंतरभूत व्योम log_deprecated(स्थिर std::string &feature, स्थिर std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
ढाँचा <typename... Args>
व्योम log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
व्योम log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
[[noreturn]] व्योम log_error(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
[[noreturn]] व्योम log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
व्योम log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

ढाँचा <typename... Args>
[[noreturn]] व्योम log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, स्थिर Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

अंतरभूत व्योम log_suppressed()
{
	logger().suppressed();
}

अंतरभूत व्योम log_spacer() { logger().spacer(); }
अंतरभूत व्योम log_धकेलो() { logger().धकेलो(); }
अंतरभूत व्योम log_निकालो() { logger().निकालो(); }

अंतरभूत व्योम log_reset_ढेर() { logger().reset_ढेर(); }
अंतरभूत व्योम log_flush() { logger().flush(); }

व्योम log_backtrace(स्थिर अक्षर *prefix, पूर्णांक levels);


std::string log_signal(स्थिर RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(स्थिर RTLIL::Const &मूल्य, bool autoint = true);

व्योम log_module(RTLIL::Module *module, std::string indent = "");
व्योम log_cell(RTLIL::Cell *cell, std::string indent = "");
व्योम log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
व्योम log_abort_internal(स्थिर अक्षर *file, पूर्णांक पंक्ति);
#घोषणा log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__खाता__, __LINE__)
#घोषणा log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __खाता__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
काष्ठा PerformanceTimer
{
#अगर 1
	int64_t योग_ns;

	PerformanceTimer() {
		योग_ns = 0;
	}

	अटल int64_t query() {
#  ifdef _WIN32
		वापस 0;
#  elif defined(RUSAGE_SELF)
		काष्ठा rusage rusage;
		int64_t t = 0;
		क्रम (पूर्णांक who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			अगर (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		वापस t;
#  अन्यथा
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	व्योम reset() {
		योग_ns = 0;
	}

	व्योम begin() {
		योग_ns -= query();
	}

	व्योम end() {
		योग_ns += query();
	}

	भग्न sec() स्थिर {
		वापस योग_ns * 1e-9f;
	}
#अन्यथा
	अटल int64_t query() { वापस 0; }
	व्योम reset() { }
	व्योम begin() { }
	व्योम end() { }
	भग्न sec() स्थिर { वापस 0; }
#पूर्ण_अगर
};

// simple API for quickly dumping values when debugging

अटल अंतरभूत व्योम log_dump_val_worker(लघु v) { log("%d", v); }
अटल अंतरभूत व्योम log_dump_val_worker(अचिन्हित लघु v) { log("%u", v); }
अटल अंतरभूत व्योम log_dump_val_worker(पूर्णांक v) { log("%d", v); }
अटल अंतरभूत व्योम log_dump_val_worker(अचिन्हित पूर्णांक v) { log("%u", v); }
अटल अंतरभूत व्योम log_dump_val_worker(दीर्घ पूर्णांक v) { log("%ld", v); }
अटल अंतरभूत व्योम log_dump_val_worker(अचिन्हित दीर्घ पूर्णांक v) { log("%lu", v); }
#अगर_अघोषित _WIN32
अटल अंतरभूत व्योम log_dump_val_worker(दीर्घ दीर्घ पूर्णांक v) { log("%lld", v); }
अटल अंतरभूत व्योम log_dump_val_worker(अचिन्हित दीर्घ दीर्घ पूर्णांक v) { log("%lld", v); }
#पूर्ण_अगर
अटल अंतरभूत व्योम log_dump_val_worker(अक्षर c)
{
	अगर (c >= 32 && c < 127) {
		log("'%c'", c);
	} अन्यथा {
		log("'\\x%02x'", c);
	}
}
अटल अंतरभूत व्योम log_dump_val_worker(अचिन्हित अक्षर c)
{
	अगर (c >= 32 && c < 127) {
		log("'%c'", c);
	} अन्यथा {
		log("'\\x%02x'", c);
	}
}
अटल अंतरभूत व्योम log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
अटल अंतरभूत व्योम log_dump_val_worker(द्विगुन v) { log("%f", v); }
अटल अंतरभूत व्योम log_dump_val_worker(अक्षर *v) { log("%s", v); }
अटल अंतरभूत व्योम log_dump_val_worker(स्थिर अक्षर *v) { log("%s", v); }
अटल अंतरभूत व्योम log_dump_val_worker(std::string v) { log("%s", v); }
अटल अंतरभूत व्योम log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
अटल अंतरभूत व्योम log_dump_args_worker(स्थिर अक्षर *p) { log_assert(*p == 0); }
व्योम log_dump_val_worker(IdString v);
व्योम log_dump_val_worker(RTLIL::SigSpec v);
व्योम log_dump_val_worker(RTLIL::State v);

ढाँचा<typename K, typename T> अटल अंतरभूत व्योम log_dump_val_worker(dict<K, T> &v);
ढाँचा<typename K> अटल अंतरभूत व्योम log_dump_val_worker(pool<K> &v);
ढाँचा<typename K> अटल अंतरभूत व्योम log_dump_val_worker(std::vector<K> &v);
ढाँचा<typename T> अटल अंतरभूत व्योम log_dump_val_worker(T *ptr);

ढाँचा<typename K, typename T>
अटल अंतरभूत व्योम log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	क्रम (स्वतः &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

ढाँचा<typename K>
अटल अंतरभूत व्योम log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	क्रम (स्वतः &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

ढाँचा<typename K>
अटल अंतरभूत व्योम log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	क्रम (स्वतः &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

ढाँचा<typename T>
अटल अंतरभूत व्योम log_dump_val_worker(T *ptr) { log("%p", ptr); }

ढाँचा<typename T, typename ... Args>
व्योम log_dump_args_worker(स्थिर अक्षर *p, T first, Args ... args)
{
	पूर्णांक अग्र_p_state = 0;
	स्थिर अक्षर *अग्र_p = p;
	जबतक (*अग्र_p && (अग्र_p_state != 0 || *अग्र_p != ',')) {
		अगर (*अग्र_p == '"')
			करो {
				अग्र_p++;
				जबतक (*अग्र_p == '\\' && *(अग्र_p + 1))
					अग्र_p += 2;
			} जबतक (*अग्र_p && *अग्र_p != '"');
		अगर (*अग्र_p == '\'') {
			अग्र_p++;
			अगर (*अग्र_p == '\\')
				अग्र_p++;
			अगर (*अग्र_p)
				अग्र_p++;
		}
		अगर (*अग्र_p == '(' || *अग्र_p == '[' || *अग्र_p == '{')
			अग्र_p_state++;
		अगर ((*अग्र_p == ')' || *अग्र_p == ']' || *अग्र_p == '}') && अग्र_p_state > 0)
			अग्र_p_state--;
		अग्र_p++;
	}
	log("\n\t%.*s => ", पूर्णांक(अग्र_p - p), p);
	अगर (*अग्र_p == ',')
		अग्र_p++;
	जबतक (*अग्र_p == ' ' || *अग्र_p == '\t' || *अग्र_p == '\r' || *अग्र_p == '\n')
		अग्र_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(अग्र_p, args ...);
}

#घोषणा log_dump(...) करो { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __खाता__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} जबतक (0)

YOSYS_NAMESPACE_END

#समावेश "kernel/yosys.h"

#पूर्ण_अगर

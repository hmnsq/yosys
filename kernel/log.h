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

#यदि defined(_WIN32)
#  include <intrin.h>
#अन्यथा
#  include <sys/time.h>
#  include <sys/resource.h>
#  यदि defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#यदि defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#अन्यथा
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# यदि __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP उत्थापय(SIGTRAP)
# अन्यथा
#  define YS_DEBUGTRAP कुरु {} यावत्(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#यदि defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING कुरु { यदि (IsDebuggerPresent()) DebugBreak(); } यावत्(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING कुरु { \
		auto old = सङ्केत(SIGTRAP, SIG_IGN); उत्थापय(SIGTRAP); सङ्केत(SIGTRAP, old); \
	} यावत्(0)
#अन्यथा
# define YS_DEBUGTRAP_IF_DEBUGGING कुरु {} यावत्(0)
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
	LogSourceLocation(std::string filename,पूर्णाङ्क start_ओळ) : filename(filename),start_ओळ(start_ओळ),start_col(-1),end_ओळ(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णाङ्क start_ओळ,पूर्णाङ्क start_col,पूर्णाङ्क end_ओळ,पूर्णाङ्क end_col) :
		filename(filename),start_ओळ(start_ओळ),start_col(start_col),end_ओळ(end_ओळ),end_col(end_col) { }
	std::string filename;
	पूर्णाङ्क start_ओळ;
	पूर्णाङ्क start_col;
	पूर्णाङ्क end_ओळ;
	पूर्णाङ्क end_col;
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
	आभास ~LogSink() = स्वभाव;

	आभास bool should_log(ध्रुव LogMessage &) ध्रुव { प्रत्यागम true; }
	आभास शून्य log(ध्रुव LogMessage &संदेश) = 0;
	आभास शून्य flush() {}
	// TODO: Remove when AST/read_verilog removed
	आभास FILE *file_handle() { प्रत्यागम nullptr; }
};

class LogSinkRef : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ध्रुव LogMessage &संदेश) ध्रुव override { प्रत्यागम sink->should_log(संदेश); }
	शून्य log(ध्रुव LogMessage &संदेश) override { sink->log(संदेश); }
	शून्य flush() override { sink->flush(); }
	FILE *file_handle() override { प्रत्यागम sink->file_handle(); }
गुप्त:
	LogSink *sink;
};

class FileLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit FileLogSink(ध्रुव std::string &filename, bool ओळ_buffered, bool append);
	~FileLogSink() override;
	शून्य log(ध्रुव LogMessage &संदेश) override;
	शून्य flush() override;
	FILE *file_handle() override { प्रत्यागम file; }

गुप्त:
	FILE *file;
};

class ConsoleLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	शून्य log(ध्रुव LogMessage &संदेश) override;
	शून्य flush() override;
	FILE *file_handle() override { प्रत्यागम stdout; }
};

class StderrLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ध्रुव LogMessage &संदेश) ध्रुव override;
	शून्य log(ध्रुव LogMessage &संदेश) override;
	शून्य flush() override;
गुप्त:
	bool quiet_warnings;
};

class StreamLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	शून्य log(ध्रुव LogMessage &संदेश) override { stream << संदेश.cached_संदेश; }
	शून्य flush() override { stream.flush(); }

गुप्त:
	std::ostream &stream;
};

class ScratchPadLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit ScratchPadLogSink(std::string scratchpad);
	शून्य log(ध्रुव LogMessage &संदेश) override;

गुप्त:
	std::string scratchpad;
};

class LogManager
{
गुप्त:
	संरचना LogExpectedItem
	{
		LogExpectedItem(ध्रुव std::regex &pat, पूर्णाङ्क expected) :
				pattern(pat), expected_मोजणी(expected), current_मोजणी(0) {}
		LogExpectedItem() : expected_मोजणी(0), current_मोजणी(0) {}

		std::regex pattern;
		पूर्णाङ्क expected_मोजणी;
		पूर्णाङ्क current_मोजणी;
	};

सार्वजनिक:
	LogManager() = स्वभाव;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.ढकला_back(std::move(sink));
		प्रत्यागम ref;
	}

	template<typename F>
	शून्य for_each_sink(F &&func)
	{
		प्रति (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { प्रत्यागम sinks.empty(); }
	शून्य clear() { sinks.clear(); }
	शून्य clear_original();
	शून्य flush() { प्रति (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	सार्वजनिक:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			प्रति (ध्रुव auto &sink : backup_sinks)
				manager.sinks.ढकला_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ध्रुव Scoped &) = delete;
		Scoped &operator=(ध्रुव Scoped &) = delete;

	गुप्त:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		पूर्णाङ्क backup_verbose_level;
	};

	Scoped sink_scope()
	{
		प्रत्यागम Scoped(*this);
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

	गुप्त:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		प्रत्यागम ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	सार्वजनिक:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			यदि (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		शून्य on()
		{
			यदि (status)
				प्रत्यागम;
			status = true;
			manager.make_debug++;
		}

		शून्य off_silent()
		{
			यदि (!status)
				प्रत्यागम;
			status = false;
			manager.make_debug--;
		}

		शून्य off()
		{
			off_silent();
		}
	गुप्त:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		प्रत्यागम LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	सार्वजनिक:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			यदि (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		शून्य on()
		{
			यदि (active)
				प्रत्यागम;

			active = true;
			manager.force_debug++;
		}

		शून्य off()
		{
			यदि (!active)
				प्रत्यागम;

			active = false;
			manager.force_debug--;
		}
	गुप्त:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		प्रत्यागम ForceDebug(*this, start_on);
	}

	शून्य force_debug_on() { force_debug++; }
	शून्य force_debug_off() { यदि (force_debug > 0) force_debug--; }
	शून्य set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	शून्य report_unexpected_error();

	शून्य add_experimental_ignore(std::string name) { experimental_ignored.घाला(name); }
	शून्य add_warn(std::string pattern) { warn_regexes.ढकला_back(YS_REGEX_COMPILE(pattern)); }
	शून्य add_nowarn(std::string pattern) { nowarn_regexes.ढकला_back(YS_REGEX_COMPILE(pattern)); }
	शून्य add_werror(std::string pattern) { werror_regexes.ढकला_back(YS_REGEX_COMPILE(pattern)); }
	शून्य add_expect(std::string type, std::string pattern, पूर्णाङ्क मोजणी);

	शून्य set_verbose_level(पूर्णाङ्क level) { verbose_level = level; }
	शून्य add_verbose_level(पूर्णाङ्क level) { verbose_level += level; }
	शून्य set_expect_no_warnings(bool मूल्य) { expect_no_warnings = मूल्य; }
	शून्य set_log_time(bool मूल्य) { log_time = मूल्य; }
	शून्य set_cmd_error_throw(bool मूल्य) { cmd_error_throw = मूल्य; }
	शून्य set_hdump_all(bool मूल्य) { hdump_all = मूल्य; }
	पूर्णाङ्क get_verbose_level() ध्रुव { प्रत्यागम verbose_level; }
	bool get_log_time() ध्रुव { प्रत्यागम log_time; }
	पूर्णाङ्क get_warnings_unique() ध्रुव { प्रत्यागम GetSize(warnings); }
	पूर्णाङ्क get_warnings_total() ध्रुव { प्रत्यागम warnings_मोजणी; }
	पूर्णाङ्क get_errors_total() ध्रुव { प्रत्यागम errors_मोजणी; }
	ध्रुव std::set<std::string> &get_experimental() ध्रुव { प्रत्यागम experimental; }
	ध्रुव std::map<std::string, std::string> &get_deprecated() ध्रुव { प्रत्यागम deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ध्रुव;

	शून्य add_hdump(std::string name, std::string मूल्य) { hdump[name].घाला(मूल्य); }

	शून्य formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	शून्य formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	शून्य formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] शून्य formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	शून्य formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] शून्य formatted_cmd_error(std::string_view format, std::string message);
	शून्य suppressed();
	शून्य add_experimental(ध्रुव std::string &str);
	शून्य add_deprecated(ध्रुव std::string &feature, ध्रुव std::string &earliest_removal);
	शून्य spacer();
	शून्य ढकला();
	शून्य काढा();

	शून्य reset_चळत();

	शून्य check_expected();
	bool expects_error() { प्रत्यागम (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(पूर्णाङ्क n = 0) { यदि (force_debug) प्रत्यागम true; debug_suppressed += n; प्रत्यागम false; }
#अन्यथा
	bool is_debug(पूर्णाङ्क = 0) { प्रत्यागम false; }
#endif
	शून्य start_hasher();
	std::string finish_hasher();

गुप्त:
	शून्य logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	शून्य error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	पूर्णाङ्क verbose_level = 0;
	पूर्णाङ्क newline_मोजणी = 0;
	vector<पूर्णाङ्क> header_मोजणी;
	पूर्णाङ्क errors_मोजणी = 0;
	पूर्णाङ्क warnings_मोजणी = 0;
	पूर्णाङ्क warnings_मोजणी_noexpect = 0;
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

	पूर्णाङ्क debug_suppressed = 0;
	पूर्णाङ्क make_debug = 0;
	पूर्णाङ्क force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

बाह्य शून्य (*log_error_atexit)();

शून्य set_verific_logging(शून्य (*cb)(पूर्णाङ्क संदेश_type, ध्रुव वर्ण *message_id, LogSourceLocation src, ध्रुव वर्ण *संदेश));
बाह्य शून्य (*log_verific_callback)(पूर्णाङ्क संदेश_type, ध्रुव वर्ण *message_id, LogSourceLocation src, ध्रुव वर्ण *संदेश);

#ifndef NDEBUG
स्थिर अन्तःस्थ bool ys_debug(पूर्णाङ्क n = 0) { प्रत्यागम logger().is_debug(n); }
#अन्यथा
स्थिर अन्तःस्थ bool ys_debug(पूर्णाङ्क = 0) { प्रत्यागम false; }
#endif

template <typename... Args>
अन्तःस्थ शून्य log(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
अन्तःस्थ शून्य log_comment(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
अन्तःस्थ शून्य log_highlight(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
अन्तःस्थ शून्य log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) कुरु { यदि (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } यावत् (0)

template <typename... Args>
अन्तःस्थ शून्य log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
अन्तःस्थ शून्य log_warning(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
अन्तःस्थ शून्य log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

अन्तःस्थ शून्य log_experimental(ध्रुव std::string &str)
{
	logger().add_experimental(str);
}

अन्तःस्थ शून्य log_deprecated(ध्रुव std::string &feature, ध्रुव std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
शून्य log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
शून्य log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] शून्य log_error(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] शून्य log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
शून्य log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] शून्य log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, ध्रुव Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

अन्तःस्थ शून्य log_suppressed()
{
	logger().suppressed();
}

अन्तःस्थ शून्य log_spacer() { logger().spacer(); }
अन्तःस्थ शून्य log_ढकला() { logger().ढकला(); }
अन्तःस्थ शून्य log_काढा() { logger().काढा(); }

अन्तःस्थ शून्य log_reset_चळत() { logger().reset_चळत(); }
अन्तःस्थ शून्य log_flush() { logger().flush(); }

शून्य log_backtrace(ध्रुव वर्ण *prefix, पूर्णाङ्क levels);


std::string log_signal(ध्रुव RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ध्रुव RTLIL::Const &मूल्य, bool autoint = true);

शून्य log_module(RTLIL::Module *module, std::string indent = "");
शून्य log_cell(RTLIL::Cell *cell, std::string indent = "");
शून्य log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
शून्य log_abort_internal(ध्रुव वर्ण *file, पूर्णाङ्क ओळ);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
संरचना PerformanceTimer
{
#यदि 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	स्थिर int64_t query() {
#  ifdef _WIN32
		प्रत्यागम 0;
#  elif defined(RUSAGE_SELF)
		संरचना rusage rusage;
		int64_t t = 0;
		प्रति (पूर्णाङ्क who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			यदि (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		प्रत्यागम t;
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

	भिन्न sec() ध्रुव {
		प्रत्यागम total_ns * 1e-9f;
	}
#अन्यथा
	स्थिर int64_t query() { प्रत्यागम 0; }
	शून्य reset() { }
	शून्य begin() { }
	शून्य end() { }
	भिन्न sec() ध्रुव { प्रत्यागम 0; }
#endif
};

// simple API for quickly dumping values when debugging

स्थिर अन्तःस्थ शून्य log_dump_val_worker(short v) { log("%d", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(unsigned short v) { log("%u", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(पूर्णाङ्क v) { log("%d", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(unsigned पूर्णाङ्क v) { log("%u", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(long पूर्णाङ्क v) { log("%ld", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(unsigned long पूर्णाङ्क v) { log("%lu", v); }
#ifndef _WIN32
स्थिर अन्तःस्थ शून्य log_dump_val_worker(long long पूर्णाङ्क v) { log("%lld", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(unsigned long long पूर्णाङ्क v) { log("%lld", v); }
#endif
स्थिर अन्तःस्थ शून्य log_dump_val_worker(वर्ण c)
{
	यदि (c >= 32 && c < 127) {
		log("'%c'", c);
	} अन्यथा {
		log("'\\x%02x'", c);
	}
}
स्थिर अन्तःस्थ शून्य log_dump_val_worker(unsigned वर्ण c)
{
	यदि (c >= 32 && c < 127) {
		log("'%c'", c);
	} अन्यथा {
		log("'\\x%02x'", c);
	}
}
स्थिर अन्तःस्थ शून्य log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(द्विगुण v) { log("%f", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(वर्ण *v) { log("%s", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(ध्रुव वर्ण *v) { log("%s", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(std::string v) { log("%s", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
स्थिर अन्तःस्थ शून्य log_dump_args_worker(ध्रुव वर्ण *p) { log_assert(*p == 0); }
शून्य log_dump_val_worker(IdString v);
शून्य log_dump_val_worker(RTLIL::SigSpec v);
शून्य log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> स्थिर अन्तःस्थ शून्य log_dump_val_worker(dict<K, T> &v);
template<typename K> स्थिर अन्तःस्थ शून्य log_dump_val_worker(pool<K> &v);
template<typename K> स्थिर अन्तःस्थ शून्य log_dump_val_worker(std::vector<K> &v);
template<typename T> स्थिर अन्तःस्थ शून्य log_dump_val_worker(T *ptr);

template<typename K, typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	प्रति (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	प्रति (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	प्रति (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
शून्य log_dump_args_worker(ध्रुव वर्ण *p, T first, Args ... args)
{
	पूर्णाङ्क next_p_state = 0;
	ध्रुव वर्ण *next_p = p;
	यावत् (*next_p && (next_p_state != 0 || *next_p != ',')) {
		यदि (*next_p == '"')
			कुरु {
				next_p++;
				यावत् (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} यावत् (*next_p && *next_p != '"');
		यदि (*next_p == '\'') {
			next_p++;
			यदि (*next_p == '\\')
				next_p++;
			यदि (*next_p)
				next_p++;
		}
		यदि (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		यदि ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", पूर्णाङ्क(next_p - p), p);
	यदि (*next_p == ',')
		next_p++;
	यावत् (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) कुरु { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} यावत् (0)

YOSYS_NAMESPACE_END

#अन्तर्भाव "kernel/yosys.h"

#endif

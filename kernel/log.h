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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"

#ଅନ୍ତର୍ଭାଵ <chrono>
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଅନ୍ତର୍ଭାଵ <regex>
#define YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#define YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#ଯଦି defined(_WIN32)
#  include <intrin.h>
#ଅନ୍ୟଥା
#  include <sys/time.h>
#  include <sys/resource.h>
#  ଯଦି defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
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
#ଯଦି defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#ଅନ୍ୟଥା
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# ଯଦି __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP ଉଠାଅ(SIGTRAP)
# ଅନ୍ୟଥା
#  define YS_DEBUGTRAP କର {} ଯେତେବେଳେ(0)
# endif
#endif

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#ଯଦି defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING କର { ଯଦି (IsDebuggerPresent()) DebugBreak(); } ଯେତେବେଳେ(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING କର { \
		auto old = ସଙ୍କେତ(SIGTRAP, SIG_IGN); ଉଠାଅ(SIGTRAP); ସଙ୍କେତ(SIGTRAP, old); \
	} ଯେତେବେଳେ(0)
#ଅନ୍ୟଥା
# define YS_DEBUGTRAP_IF_DEBUGGING କର {} ଯେତେବେଳେ(0)
#endif

ସଂରଚନା log_cmd_error_exception { };

ଗଣନା class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	Header,
	Warning,
	NonFatalError,
	Error
};

ସଂରଚନା LogSourceLocation {
	LogSourceLocation() : filename({}),start_ଧାଡ଼ି(-1),start_col(-1),end_ଧାଡ଼ି(-1),end_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_ଧାଡ଼ି(-1),start_col(-1),end_ଧାଡ଼ି(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_ଧାଡ଼ି) : filename(filename),start_ଧାଡ଼ି(start_ଧାଡ଼ି),start_col(-1),end_ଧାଡ଼ି(-1),end_col(-1) { }
	LogSourceLocation(std::string filename,ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_ଧାଡ଼ି,ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_col,ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_ଧାଡ଼ି,ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col) :
		filename(filename),start_ଧାଡ଼ି(start_ଧାଡ଼ି),start_col(start_col),end_ଧାଡ଼ି(end_ଧାଡ଼ି),end_col(end_col) { }
	std::string filename;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_ଧାଡ଼ି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_ଧାଡ଼ି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col;
};

ସଂରଚନା LogMessage {
	LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message);
	LogSeverity severity;
	std::string prefix;
	std::string format;
	std::string message;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_ସନ୍ଦେଶ;
};

class LogSink
{
ସର୍ବସାଧାରଣ:
	ଆଭାସୀ ~LogSink() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଆଭାସୀ bool should_log(ଅଚଳ LogMessage &) ଅଚଳ { ଫେରନ୍ତୁ true; }
	ଆଭାସୀ ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) = 0;
	ଆଭାସୀ ଶୂନ୍ୟ flush() {}
	// TODO: Remove when AST/read_verilog removed
	ଆଭାସୀ FILE *file_handle() { ଫେରନ୍ତୁ nullptr; }
};

class LogSinkRef : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) ଅଚଳ override { ଫେରନ୍ତୁ sink->should_log(ସନ୍ଦେଶ); }
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override { sink->log(ସନ୍ଦେଶ); }
	ଶୂନ୍ୟ flush() override { sink->flush(); }
	FILE *file_handle() override { ଫେରନ୍ତୁ sink->file_handle(); }
ବ୍ୟକ୍ତିଗତ:
	LogSink *sink;
};

class FileLogSink : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	explicit FileLogSink(ଅଚଳ std::string &filename, bool ଧାଡ଼ି_buffered, bool append);
	~FileLogSink() override;
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override;
	ଶୂନ୍ୟ flush() override;
	FILE *file_handle() override { ଫେରନ୍ତୁ file; }

ବ୍ୟକ୍ତିଗତ:
	FILE *file;
};

class ConsoleLogSink : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override;
	ଶୂନ୍ୟ flush() override;
	FILE *file_handle() override { ଫେରନ୍ତୁ stdout; }
};

class StderrLogSink : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) ଅଚଳ override;
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override;
	ଶୂନ୍ୟ flush() override;
ବ୍ୟକ୍ତିଗତ:
	bool quiet_warnings;
};

class StreamLogSink : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	explicit StreamLogSink(std::ostream &stream) : stream(stream) {}
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override { stream << ସନ୍ଦେଶ.cached_ସନ୍ଦେଶ; }
	ଶୂନ୍ୟ flush() override { stream.flush(); }

ବ୍ୟକ୍ତିଗତ:
	std::ostream &stream;
};

class ScratchPadLogSink : ସର୍ବସାଧାରଣ LogSink
{
ସର୍ବସାଧାରଣ:
	explicit ScratchPadLogSink(std::string scratchpad);
	ଶୂନ୍ୟ log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) override;

ବ୍ୟକ୍ତିଗତ:
	std::string scratchpad;
};

class LogManager
{
ବ୍ୟକ୍ତିଗତ:
	ସଂରଚନା LogExpectedItem
	{
		LogExpectedItem(ଅଚଳ std::regex &pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା expected) :
				pattern(pat), expected_ଗଣନା(expected), current_ଗଣନା(0) {}
		LogExpectedItem() : expected_ଗଣନା(0), current_ଗଣନା(0) {}

		std::regex pattern;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା expected_ଗଣନା;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_ଗଣନା;
	};

ସର୍ବସାଧାରଣ:
	LogManager() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	template<typename T, typename... Args>
	T &add_sink(Args&&... args)
	{
		auto sink = std::make_unique<T>(std::forward<Args>(args)...);
		T &ref = *sink;
		sinks.ଠେଲ_back(std::move(sink));
		ଫେରନ୍ତୁ ref;
	}

	template<typename F>
	ଶୂନ୍ୟ for_each_sink(F &&func)
	{
		ପାଇଁ (auto &sink : sinks)
			func(*sink);
	}

	bool empty() { ଫେରନ୍ତୁ sinks.empty(); }
	ଶୂନ୍ୟ clear() { sinks.clear(); }
	ଶୂନ୍ୟ clear_original();
	ଶୂନ୍ୟ flush() { ପାଇଁ (auto &sink : sinks) sink->flush(); }

	class Scoped
	{
	ସର୍ବସାଧାରଣ:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.size());

			ପାଇଁ (ଅଚଳ auto &sink : backup_sinks)
				manager.sinks.ଠେଲ_back(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ଅଚଳ Scoped &) = delete;
		Scoped &operator=(ଅଚଳ Scoped &) = delete;

	ବ୍ୟକ୍ତିଗତ:
		LogManager &manager;
		std::vector<std::unique_ptr<LogSink>> backup_sinks;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା backup_verbose_level;
	};

	Scoped sink_scope()
	{
		ଫେରନ୍ତୁ Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	ସର୍ବସାଧାରଣ:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), previous(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = previous;
		}

	ବ୍ୟକ୍ତିଗତ:
		LogManager &manager;
		bool previous;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		ଫେରନ୍ତୁ ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	ସର୍ବସାଧାରଣ:
		explicit LogMakeDebugHdl(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			ଯଦି (start_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		ଶୂନ୍ୟ on()
		{
			ଯଦି (status)
				ଫେରନ୍ତୁ;
			status = true;
			manager.make_debug++;
		}

		ଶୂନ୍ୟ off_silent()
		{
			ଯଦି (!status)
				ଫେରନ୍ତୁ;
			status = false;
			manager.make_debug--;
		}

		ଶୂନ୍ୟ off()
		{
			off_silent();
		}
	ବ୍ୟକ୍ତିଗତ:
		LogManager &manager;
		bool status = false;
	};

	LogMakeDebugHdl make_debug_scope(bool start_on = false)
	{
		ଫେରନ୍ତୁ LogMakeDebugHdl(*this, start_on);
	}

	class ForceDebug
	{
	ସର୍ବସାଧାରଣ:
		explicit ForceDebug(LogManager &manager, bool start_on = false)
			: manager(manager)
		{
			ଯଦି (start_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		ଶୂନ୍ୟ on()
		{
			ଯଦି (active)
				ଫେରନ୍ତୁ;

			active = true;
			manager.force_debug++;
		}

		ଶୂନ୍ୟ off()
		{
			ଯଦି (!active)
				ଫେରନ୍ତୁ;

			active = false;
			manager.force_debug--;
		}
	ବ୍ୟକ୍ତିଗତ:
		LogManager &manager;
		bool active = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		ଫେରନ୍ତୁ ForceDebug(*this, start_on);
	}

	ଶୂନ୍ୟ force_debug_on() { force_debug++; }
	ଶୂନ୍ୟ force_debug_off() { ଯଦି (force_debug > 0) force_debug--; }
	ଶୂନ୍ୟ set_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	ଶୂନ୍ୟ report_unexpected_error();

	ଶୂନ୍ୟ add_experimental_ignore(std::string name) { experimental_ignored.ଭର୍ତ୍ତି(name); }
	ଶୂନ୍ୟ add_warn(std::string pattern) { warn_regexes.ଠେଲ_back(YS_REGEX_COMPILE(pattern)); }
	ଶୂନ୍ୟ add_nowarn(std::string pattern) { nowarn_regexes.ଠେଲ_back(YS_REGEX_COMPILE(pattern)); }
	ଶୂନ୍ୟ add_werror(std::string pattern) { werror_regexes.ଠେଲ_back(YS_REGEX_COMPILE(pattern)); }
	ଶୂନ୍ୟ add_expect(std::string type, std::string pattern, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2);

	ଶୂନ୍ୟ set_verbose_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) { verbose_level = level; }
	ଶୂନ୍ୟ add_verbose_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) { verbose_level += level; }
	ଶୂନ୍ୟ set_expect_no_warnings(bool ମୂଲ୍ୟ) { expect_no_warnings = ମୂଲ୍ୟ; }
	ଶୂନ୍ୟ set_log_time(bool ମୂଲ୍ୟ) { log_time = ମୂଲ୍ୟ; }
	ଶୂନ୍ୟ set_cmd_error_throw(bool ମୂଲ୍ୟ) { cmd_error_throw = ମୂଲ୍ୟ; }
	ଶୂନ୍ୟ set_hdump_all(bool ମୂଲ୍ୟ) { hdump_all = ମୂଲ୍ୟ; }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_verbose_level() ଅଚଳ { ଫେରନ୍ତୁ verbose_level; }
	bool get_log_time() ଅଚଳ { ଫେରନ୍ତୁ log_time; }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_warnings_unique() ଅଚଳ { ଫେରନ୍ତୁ GetSize(warnings); }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_warnings_total() ଅଚଳ { ଫେରନ୍ତୁ warnings_ଗଣନା; }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_errors_total() ଅଚଳ { ଫେରନ୍ତୁ errors_ଗଣନା; }
	ଅଚଳ std::set<std::string> &get_experimental() ଅଚଳ { ଫେରନ୍ତୁ experimental; }
	ଅଚଳ std::map<std::string, std::string> &get_deprecated() ଅଚଳ { ଫେରନ୍ତୁ deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ଅଚଳ;

	ଶୂନ୍ୟ add_hdump(std::string name, std::string ମୂଲ୍ୟ) { hdump[name].ଭର୍ତ୍ତି(ମୂଲ୍ୟ); }

	ଶୂନ୍ୟ formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str);
	ଶୂନ୍ୟ formatted_header(RTLIL::Design *design, std::string_view format, std::string str);
	ଶୂନ୍ୟ formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] ଶୂନ୍ୟ formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	ଶୂନ୍ୟ formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);
	[[noreturn]] ଶୂନ୍ୟ formatted_cmd_error(std::string_view format, std::string message);
	ଶୂନ୍ୟ suppressed();
	ଶୂନ୍ୟ add_experimental(ଅଚଳ std::string &str);
	ଶୂନ୍ୟ add_deprecated(ଅଚଳ std::string &feature, ଅଚଳ std::string &earliest_removal);
	ଶୂନ୍ୟ spacer();
	ଶୂନ୍ୟ ଠେଲ();
	ଶୂନ୍ୟ କାଢ଼();

	ଶୂନ୍ୟ reset_ଗଦା();

	ଶୂନ୍ୟ check_expected();
	bool expects_error() { ଫେରନ୍ତୁ (expect_error.size() + expect_prefix_error.size())>0; }
#ifndef NDEBUG
	bool is_debug(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0) { ଯଦି (force_debug) ଫେରନ୍ତୁ true; debug_suppressed += n; ଫେରନ୍ତୁ false; }
#ଅନ୍ୟଥା
	bool is_debug(ପୂର୍ଣ୍ଣସଂଖ୍ୟା = 0) { ଫେରନ୍ତୁ false; }
#endif
	ଶୂନ୍ୟ start_hasher();
	std::string finish_hasher();

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in);
	ଶୂନ୍ୟ error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message);

	std::vector<std::unique_ptr<LogSink>> sinks;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose_level = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା newline_ଗଣନା = 0;
	vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> header_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା errors_ଗଣନା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା warnings_ଗଣନା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା warnings_ଗଣନା_noexpect = 0;
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

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug_suppressed = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_debug = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

ବାହ୍ୟ ଶୂନ୍ୟ (*log_error_atexit)();

ଶୂନ୍ୟ set_verific_logging(ଶୂନ୍ୟ (*cb)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_type, ଅଚଳ ଅକ୍ଷର *message_id, LogSourceLocation src, ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ));
ବାହ୍ୟ ଶୂନ୍ୟ (*log_verific_callback)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_type, ଅଚଳ ଅକ୍ଷର *message_id, LogSourceLocation src, ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ);

#ifndef NDEBUG
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ys_debug(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0) { ଫେରନ୍ତୁ logger().is_debug(n); }
#ଅନ୍ୟଥା
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ys_debug(ପୂର୍ଣ୍ଣସଂଖ୍ୟା = 0) { ଫେରନ୍ତୁ false; }
#endif

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_comment(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_highlight(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix,
		FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_string(severity, src, prefix, fmt.format_string(), fmt.format(args...));
}

#define log_debug(...) କର { ଯଦି (ys_debug(1)) YOSYS_NAMESPACE_PREFIX log_formatted_string(YOSYS_NAMESPACE_PREFIX LogSeverity::Debug, {}, {}, __VA_ARGS__); } ଯେତେବେଳେ (0)

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_header(RTLIL::Design *design, FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_header(design, fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_warning(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_warning_noprefix(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(args...));
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_experimental(ଅଚଳ std::string &str)
{
	logger().add_experimental(str);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_deprecated(ଅଚଳ std::string &feature, ଅଚଳ std::string &earliest_removal = "")
{
	logger().add_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
template <typename... Args>
ଶୂନ୍ୟ log_file_warning(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଶୂନ୍ୟ log_file_info(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] ଶୂନ୍ୟ log_error(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] ଶୂନ୍ୟ log_file_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
ଶୂନ୍ୟ log_file_nonfatal_error(LogSourceLocation src, FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(args...));
}

template <typename... Args>
[[noreturn]] ଶୂନ୍ୟ log_cmd_error(FmtString<TypeIdentity<Args>...> fmt, ଅଚଳ Args &... args)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(args...));
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_suppressed()
{
	logger().suppressed();
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_spacer() { logger().spacer(); }
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_ଠେଲ() { logger().ଠେଲ(); }
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_କାଢ଼() { logger().କାଢ଼(); }

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_reset_ଗଦା() { logger().reset_ଗଦା(); }
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_flush() { logger().flush(); }

ଶୂନ୍ୟ log_backtrace(ଅଚଳ ଅକ୍ଷର *prefix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା levels);


std::string log_signal(ଅଚଳ RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ଅଚଳ RTLIL::Const &ମୂଲ୍ୟ, bool autoint = true);

ଶୂନ୍ୟ log_module(RTLIL::Module *module, std::string indent = "");
ଶୂନ୍ୟ log_cell(RTLIL::Cell *cell, std::string indent = "");
ଶୂନ୍ୟ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
ଶୂନ୍ୟ log_abort_internal(ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି);
#define log_abort() YOSYS_NAMESPACE_PREFIX log_abort_internal(__FILE__, __LINE__)
#define log_ping() YOSYS_NAMESPACE_PREFIX log("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
ସଂରଚନା PerformanceTimer
{
#ଯଦି 1
	int64_t total_ns;

	PerformanceTimer() {
		total_ns = 0;
	}

	ସ୍ଥିର int64_t query() {
#  ifdef _WIN32
		ଫେରନ୍ତୁ 0;
#  elif defined(RUSAGE_SELF)
		ସଂରଚନା rusage rusage;
		int64_t t = 0;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			ଯଦି (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		ଫେରନ୍ତୁ t;
#  ଅନ୍ୟଥା
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	ଶୂନ୍ୟ reset() {
		total_ns = 0;
	}

	ଶୂନ୍ୟ begin() {
		total_ns -= query();
	}

	ଶୂନ୍ୟ end() {
		total_ns += query();
	}

	ଦଶମିକ sec() ଅଚଳ {
		ଫେରନ୍ତୁ total_ns * 1e-9f;
	}
#ଅନ୍ୟଥା
	ସ୍ଥିର int64_t query() { ଫେରନ୍ତୁ 0; }
	ଶୂନ୍ୟ reset() { }
	ଶୂନ୍ୟ begin() { }
	ଶୂନ୍ୟ end() { }
	ଦଶମିକ sec() ଅଚଳ { ଫେରନ୍ତୁ 0; }
#endif
};

// simple API for quickly dumping values when debugging

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(short v) { log("%d", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(unsigned short v) { log("%u", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) { log("%d", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) { log("%u", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(long ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) { log("%ld", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) { log("%lu", v); }
#ifndef _WIN32
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(long long ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) { log("%lld", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(unsigned long long ପୂର୍ଣ୍ଣସଂଖ୍ୟା v) { log("%lld", v); }
#endif
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ଅକ୍ଷର c)
{
	ଯଦି (c >= 32 && c < 127) {
		log("'%c'", c);
	} ଅନ୍ୟଥା {
		log("'\\x%02x'", c);
	}
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(unsigned ଅକ୍ଷର c)
{
	ଯଦି (c >= 32 && c < 127) {
		log("'%c'", c);
	} ଅନ୍ୟଥା {
		log("'\\x%02x'", c);
	}
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(bool v) { log("%s", v ? "true" : "false"); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ଦ୍ୱିଗୁଣ v) { log("%f", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ଅକ୍ଷର *v) { log("%s", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(ଅଚଳ ଅକ୍ଷର *v) { log("%s", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(std::string v) { log("%s", v); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(PerformanceTimer p) { log("%f seconds", p.sec()); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_args_worker(ଅଚଳ ଅକ୍ଷର *p) { log_assert(*p == 0); }
ଶୂନ୍ୟ log_dump_val_worker(IdString v);
ଶୂନ୍ୟ log_dump_val_worker(RTLIL::SigSpec v);
ଶୂନ୍ୟ log_dump_val_worker(RTLIL::State v);

template<typename K, typename T> ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(dict<K, T> &v);
template<typename K> ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(pool<K> &v);
template<typename K> ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(std::vector<K> &v);
template<typename T> ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(T *ptr);

template<typename K, typename T>
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(dict<K, T> &v) {
	log("{");
	bool first = true;
	ପାଇଁ (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it.first);
		log(": ");
		log_dump_val_worker(it.second);
		first = false;
	}
	log(" }");
}

template<typename K>
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(pool<K> &v) {
	log("{");
	bool first = true;
	ପାଇଁ (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename K>
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(std::vector<K> &v) {
	log("{");
	bool first = true;
	ପାଇଁ (auto &it : v) {
		log("%s ", first ? "" : ",");
		log_dump_val_worker(it);
		first = false;
	}
	log(" }");
}

template<typename T>
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ log_dump_val_worker(T *ptr) { log("%p", ptr); }

template<typename T, typename ... Args>
ଶୂନ୍ୟ log_dump_args_worker(ଅଚଳ ଅକ୍ଷର *p, T first, Args ... args)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_p_state = 0;
	ଅଚଳ ଅକ୍ଷର *next_p = p;
	ଯେତେବେଳେ (*next_p && (next_p_state != 0 || *next_p != ',')) {
		ଯଦି (*next_p == '"')
			କର {
				next_p++;
				ଯେତେବେଳେ (*next_p == '\\' && *(next_p + 1))
					next_p += 2;
			} ଯେତେବେଳେ (*next_p && *next_p != '"');
		ଯଦି (*next_p == '\'') {
			next_p++;
			ଯଦି (*next_p == '\\')
				next_p++;
			ଯଦି (*next_p)
				next_p++;
		}
		ଯଦି (*next_p == '(' || *next_p == '[' || *next_p == '{')
			next_p_state++;
		ଯଦି ((*next_p == ')' || *next_p == ']' || *next_p == '}') && next_p_state > 0)
			next_p_state--;
		next_p++;
	}
	log("\n\t%.*s => ", ପୂର୍ଣ୍ଣସଂଖ୍ୟା(next_p - p), p);
	ଯଦି (*next_p == ',')
		next_p++;
	ଯେତେବେଳେ (*next_p == ' ' || *next_p == '\t' || *next_p == '\r' || *next_p == '\n')
		next_p++;
	log_dump_val_worker(first);
	log_dump_args_worker(next_p, args ...);
}

#define log_dump(...) କର { \
	log("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_args_worker(#__VA_ARGS__, __VA_ARGS__); \
	log("\n"); \
} ଯେତେବେଳେ (0)

YOSYS_NAMESPACE_END

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

#endif

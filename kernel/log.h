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

#నిర్వచితం_కాకపోతే LOG_H
#నిర్వచించు LOG_H

#అన్తర్భావ "kernel/yosys_common.h"

#అన్తర్భావ <chrono>
#అన్తర్భావ <time.h>

#అన్తర్భావ <regex>
#నిర్వచించు YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#నిర్వచించు YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#ఒకవేళ defined(_WIN32)
#  include <intrin.h>
#లేకపోతే
#  include <sys/time.h>
#  include <sys/resource.h>
#  ఒకవేళ defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <signal.h>
#  endif
#అయితే_ముగింపు

// from libs/sha1/sha1.h
class SHA1;

YOSYS_NAMESPACE_BEGIN

// YS_DEBUGTRAP is a macro that is functionally equivalent to a breakpoint
// if the platform provides such functionality, and does nothing otherwise.
// If no debugger is attached, it starts a just-in-time debugger if available,
// and crashes the process otherwise.
#ఒకవేళ defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#లేకపోతే
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# ఒకవేళ __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP లేపు(SIGTRAP)
# లేకపోతే
#  define YS_DEBUGTRAP చేయి {} అయితే(0)
# endif
#అయితే_ముగింపు

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#ఒకవేళ defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING చేయి { ఒకవేళ (IsDebuggerPresent()) DebugBreak(); } అయితే(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING చేయి { \
		స్వయంచాలక old = సంకేతం(SIGTRAP, SIG_IGN); లేపు(SIGTRAP); సంకేతం(SIGTRAP, old); \
	} అయితే(0)
#లేకపోతే
# define YS_DEBUGTRAP_IF_DEBUGGING చేయి {} అయితే(0)
#అయితే_ముగింపు

నిర్మాణం log_cmd_error_exception { };

గణన class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	శీర్షిక,
	Warning,
	NonFatalError,
	Error
};

నిర్మాణం Log_ఆధారం_Location {
	Log_ఆధారం_Location() : filename({}),ప్రారంభం_పంక్తి(-1),ప్రారంభం_నిలువు(-1),చివర_పంక్తి(-1),చివర_నిలువు(-1) { }
	Log_ఆధారం_Location(std::string filename) : filename(filename),ప్రారంభం_పంక్తి(-1),ప్రారంభం_నిలువు(-1),చివర_పంక్తి(-1),చివర_నిలువు(-1) { }
	Log_ఆధారం_Location(std::string filename,పూర్ణాంకం ప్రారంభం_పంక్తి) : filename(filename),ప్రారంభం_పంక్తి(ప్రారంభం_పంక్తి),ప్రారంభం_నిలువు(-1),చివర_పంక్తి(-1),చివర_నిలువు(-1) { }
	Log_ఆధారం_Location(std::string filename,పూర్ణాంకం ప్రారంభం_పంక్తి,పూర్ణాంకం ప్రారంభం_నిలువు,పూర్ణాంకం చివర_పంక్తి,పూర్ణాంకం చివర_నిలువు) :
		filename(filename),ప్రారంభం_పంక్తి(ప్రారంభం_పంక్తి),ప్రారంభం_నిలువు(ప్రారంభం_నిలువు),చివర_పంక్తి(చివర_పంక్తి),చివర_నిలువు(చివర_నిలువు) { }
	std::string filename;
	పూర్ణాంకం ప్రారంభం_పంక్తి;
	పూర్ణాంకం ప్రారంభం_నిలువు;
	పూర్ణాంకం చివర_పంక్తి;
	పూర్ణాంకం చివర_నిలువు;
};

నిర్మాణం Log_సందేశం {
	Log_సందేశం(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string_view సందేశం_2);
	LogSeverity severity;
	std::string ఉపసర్గ_2;
	std::string format;
	std::string సందేశం_2;
	std::chrono::steady_clock::సమయం_బిందువు timestamp;
	Log_ఆధారం_Location src;

	std::string cached_సందేశం;
};

class LogSink
{
బహిరంగ:
	మాయా ~LogSink() = అప్రమేయ;

	మాయా bool should_log(స్థిరరాశి Log_సందేశం &) స్థిరరాశి { తిరిగి true; }
	మాయా శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) = 0;
	మాయా శూన్య flush() {}
	// TODO: Remove when AST/read_verilog removed
	మాయా FILE *file_handle() { తిరిగి nullptr; }
};

class LogSinkRef : బహిరంగ LogSink
{
బహిరంగ:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(స్థిరరాశి Log_సందేశం &సందేశం) స్థిరరాశి override { తిరిగి sink->should_log(సందేశం); }
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override { sink->సంవర్గమానం(సందేశం); }
	శూన్య flush() override { sink->flush(); }
	FILE *file_handle() override { తిరిగి sink->file_handle(); }
అంతరంగ:
	LogSink *sink;
};

class FileLogSink : బహిరంగ LogSink
{
బహిరంగ:
	explicit FileLogSink(స్థిరరాశి std::string &filename, bool పంక్తి_buffered, bool append);
	~FileLogSink() override;
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override;
	శూన్య flush() override;
	FILE *file_handle() override { తిరిగి file; }

అంతరంగ:
	FILE *file;
};

class ConsoleLogSink : బహిరంగ LogSink
{
బహిరంగ:
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override;
	శూన్య flush() override;
	FILE *file_handle() override { తిరిగి stdout; }
};

class StderrLogSink : బహిరంగ LogSink
{
బహిరంగ:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(స్థిరరాశి Log_సందేశం &సందేశం) స్థిరరాశి override;
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override;
	శూన్య flush() override;
అంతరంగ:
	bool quiet_warnings;
};

class ప్రవాహం_Log_Sink : బహిరంగ LogSink
{
బహిరంగ:
	explicit ప్రవాహం_Log_Sink(std::ostream &ప్రవాహం) : ప్రవాహం(ప్రవాహం) {}
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override { ప్రవాహం << సందేశం.cached_సందేశం; }
	శూన్య flush() override { ప్రవాహం.flush(); }

అంతరంగ:
	std::ostream &ప్రవాహం;
};

class ScratchPadLogSink : బహిరంగ LogSink
{
బహిరంగ:
	explicit ScratchPadLogSink(std::string scratchpad);
	శూన్య సంవర్గమానం(స్థిరరాశి Log_సందేశం &సందేశం) override;

అంతరంగ:
	std::string scratchpad;
};

class LogManager
{
అంతరంగ:
	నిర్మాణం Log_Expected_అంశం
	{
		Log_Expected_అంశం(స్థిరరాశి std::regex &pat, పూర్ణాంకం expected) :
				నమూనా(pat), expected_లెక్క(expected), current_లెక్క(0) {}
		Log_Expected_అంశం() : expected_లెక్క(0), current_లెక్క(0) {}

		std::regex నమూనా;
		పూర్ణాంకం expected_లెక్క;
		పూర్ణాంకం current_లెక్క;
	};

బహిరంగ:
	LogManager() = అప్రమేయ;

	మూస<typename T, typename... వాదనలు_3>
	T &కూడు_sink(వాదనలు_3&&... వాదనలు_2)
	{
		స్వయంచాలక sink = std::make_unique<T>(std::forward<వాదనలు_3>(వాదనలు_2)...);
		T &ref = *sink;
		sinks.నెట్టు_వెనుక(std::move(sink));
		తిరిగి ref;
	}

	మూస<typename F>
	శూన్య for_each_sink(F &&func)
	{
		కొరకు (స్వయంచాలక &sink : sinks)
			func(*sink);
	}

	bool ఖాళీలు() { తిరిగి sinks.ఖాళీలు(); }
	శూన్య clear() { sinks.clear(); }
	శూన్య clear_original();
	శూన్య flush() { కొరకు (స్వయంచాలక &sink : sinks) sink->flush(); }

	class Scoped
	{
	బహిరంగ:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.కొలత());

			కొరకు (స్థిరరాశి స్వయంచాలక &sink : backup_sinks)
				manager.sinks.నెట్టు_వెనుక(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(స్థిరరాశి Scoped &) = delete;
		Scoped &క్రియాకారకం=(స్థిరరాశి Scoped &) = delete;

	అంతరంగ:
		LogManager &manager;
		std::సదిశ<std::unique_ptr<LogSink>> backup_sinks;
		పూర్ణాంకం backup_verbose_level;
	};

	Scoped sink_scope()
	{
		తిరిగి Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	బహిరంగ:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), మునుపటి(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = మునుపటి;
		}

	అంతరంగ:
		LogManager &manager;
		bool మునుపటి;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		తిరిగి ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	బహిరంగ:
		explicit LogMakeDebugHdl(LogManager &manager, bool ప్రారంభం_on = false)
			: manager(manager)
		{
			ఒకవేళ (ప్రారంభం_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		శూన్య on()
		{
			ఒకవేళ (స్థితి_2)
				తిరిగి;
			స్థితి_2 = true;
			manager.make_debug++;
		}

		శూన్య off_silent()
		{
			ఒకవేళ (!స్థితి_2)
				తిరిగి;
			స్థితి_2 = false;
			manager.make_debug--;
		}

		శూన్య off()
		{
			off_silent();
		}
	అంతరంగ:
		LogManager &manager;
		bool స్థితి_2 = false;
	};

	LogMakeDebugHdl make_debug_scope(bool ప్రారంభం_on = false)
	{
		తిరిగి LogMakeDebugHdl(*this, ప్రారంభం_on);
	}

	class ForceDebug
	{
	బహిరంగ:
		explicit ForceDebug(LogManager &manager, bool ప్రారంభం_on = false)
			: manager(manager)
		{
			ఒకవేళ (ప్రారంభం_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		శూన్య on()
		{
			ఒకవేళ (చురుకైన)
				తిరిగి;

			చురుకైన = true;
			manager.force_debug++;
		}

		శూన్య off()
		{
			ఒకవేళ (!చురుకైన)
				తిరిగి;

			చురుకైన = false;
			manager.force_debug--;
		}
	అంతరంగ:
		LogManager &manager;
		bool చురుకైన = false;
	};

	ForceDebug force_debug_scope(bool ప్రారంభం_on = false)
	{
		తిరిగి ForceDebug(*this, ప్రారంభం_on);
	}

	శూన్య force_debug_on() { force_debug++; }
	శూన్య force_debug_off() { ఒకవేళ (force_debug > 0) force_debug--; }
	శూన్య సమితి_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	శూన్య report_unexpected_error();

	శూన్య కూడు_experimental_ignore(std::string name) { experimental_ignored.చొప్పించు(name); }
	శూన్య కూడు_warn(std::string నమూనా) { warn_regexes.నెట్టు_వెనుక(YS_REGEX_COMPILE(నమూనా)); }
	శూన్య కూడు_nowarn(std::string నమూనా) { nowarn_regexes.నెట్టు_వెనుక(YS_REGEX_COMPILE(నమూనా)); }
	శూన్య కూడు_werror(std::string నమూనా) { werror_regexes.నెట్టు_వెనుక(YS_REGEX_COMPILE(నమూనా)); }
	శూన్య కూడు_expect(std::string type, std::string నమూనా, పూర్ణాంకం లెక్క);

	శూన్య సమితి_verbose_level(పూర్ణాంకం level) { verbose_level = level; }
	శూన్య కూడు_verbose_level(పూర్ణాంకం level) { verbose_level += level; }
	శూన్య సమితి_expect_no_warnings(bool విలువ) { expect_no_warnings = విలువ; }
	శూన్య సమితి_log_సమయం(bool విలువ) { log_సమయం = విలువ; }
	శూన్య సమితి_cmd_error_throw(bool విలువ) { cmd_error_throw = విలువ; }
	శూన్య సమితి_hdump_all(bool విలువ) { hdump_all = విలువ; }
	పూర్ణాంకం get_verbose_level() స్థిరరాశి { తిరిగి verbose_level; }
	bool get_log_సమయం() స్థిరరాశి { తిరిగి log_సమయం; }
	పూర్ణాంకం get_warnings_unique() స్థిరరాశి { తిరిగి Get_కొలత(warnings); }
	పూర్ణాంకం get_warnings_మొత్తం() స్థిరరాశి { తిరిగి warnings_లెక్క; }
	పూర్ణాంకం get_errors_మొత్తం() స్థిరరాశి { తిరిగి errors_లెక్క; }
	స్థిరరాశి std::సమితి<std::string> &get_experimental() స్థిరరాశి { తిరిగి experimental; }
	స్థిరరాశి std::map<std::string, std::string> &get_deprecated() స్థిరరాశి { తిరిగి deprecated; }
	std::chrono::steady_clock::సమయం_బిందువు get_initial_సమయం() స్థిరరాశి;

	శూన్య కూడు_hdump(std::string name, std::string విలువ) { hdump[name].చొప్పించు(విలువ); }

	శూన్య formatted_string(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string str);
	శూన్య formatted_శీర్షిక(RTLIL::Design *design, std::string_view format, std::string str);
	శూన్య formatted_warning(Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2);
	[[noreturn]] శూన్య formatted_error(Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2);
	శూన్య formatted_nonfatal_error(Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2);
	[[noreturn]] శూన్య formatted_cmd_error(std::string_view format, std::string సందేశం_2);
	శూన్య suppressed();
	శూన్య కూడు_experimental(స్థిరరాశి std::string &str);
	శూన్య కూడు_deprecated(స్థిరరాశి std::string &feature, స్థిరరాశి std::string &earliest_removal);
	శూన్య spacer();
	శూన్య నెట్టు();
	శూన్య తీయి();

	శూన్య reset_దొంతర();

	శూన్య check_expected();
	bool expects_error() { తిరిగి (expect_error.కొలత() + expect_ఉపసర్గ_error.కొలత())>0; }
#నిర్వచితం_కాకపోతే NDEBUG
	bool is_debug(పూర్ణాంకం n = 0) { ఒకవేళ (force_debug) తిరిగి true; debug_suppressed += n; తిరిగి false; }
#లేకపోతే
	bool is_debug(పూర్ణాంకం = 0) { తిరిగి false; }
#అయితే_ముగింపు
	శూన్య ప్రారంభం_hasher();
	std::string ముగింపు_hasher();

అంతరంగ:
	శూన్య logv_string(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string str_లోపల);
	శూన్య error_with_ఉపసర్గ(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2, std::string_view format, std::string సందేశం_2);

	std::సదిశ<std::unique_ptr<LogSink>> sinks;
	పూర్ణాంకం verbose_level = 0;
	పూర్ణాంకం newline_లెక్క = 0;
	సదిశ<పూర్ణాంకం> శీర్షిక_లెక్క;
	పూర్ణాంకం errors_లెక్క = 0;
	పూర్ణాంకం warnings_లెక్క = 0;
	పూర్ణాంకం warnings_లెక్క_noexpect = 0;
	std::సమితి<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::సదిశ<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_అంశం> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_అంశం> expect_ఉపసర్గ_log, expect_ఉపసర్గ_warning, expect_ఉపసర్గ_error;
	bool expect_no_warnings = false;
	bool log_సమయం = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::సమితి<std::string>> hdump;
	bool hdump_all = false;

	పూర్ణాంకం debug_suppressed = 0;
	పూర్ణాంకం make_debug = 0;
	పూర్ణాంకం force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

బాహ్య శూన్య (*log_error_atexit)();

శూన్య సమితి_verific_logging(శూన్య (*cb)(పూర్ణాంకం సందేశం_type, స్థిరరాశి అక్షరం *సందేశం_id, Log_ఆధారం_Location src, స్థిరరాశి అక్షరం *సందేశం));
బాహ్య శూన్య (*log_verific_callback)(పూర్ణాంకం సందేశం_type, స్థిరరాశి అక్షరం *సందేశం_id, Log_ఆధారం_Location src, స్థిరరాశి అక్షరం *సందేశం);

#నిర్వచితం_కాకపోతే NDEBUG
స్థిర అంతఃవరుస bool ys_debug(పూర్ణాంకం n = 0) { తిరిగి logger().is_debug(n); }
#లేకపోతే
స్థిర అంతఃవరుస bool ys_debug(పూర్ణాంకం = 0) { తిరిగి false; }
#అయితే_ముగింపు

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య సంవర్గమానం(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_string(LogSeverity::Info, Log_ఆధారం_Location{}, {}, fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య log_comment(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_ఆధారం_Location{}, {}, fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య log_highlight(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_ఆధారం_Location{}, {}, fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య log_formatted_string(LogSeverity severity, Log_ఆధారం_Location src, std::string_view ఉపసర్గ_2,
		FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_string(severity, src, ఉపసర్గ_2, fmt.format_string(), fmt.format(వాదనలు_2...));
}

#నిర్వచించు log_debug(...) చేయి { ఒకవేళ (ys_debug(1)) YOSYS_NAMESPACE_ఉపసర్గ log_formatted_string(YOSYS_NAMESPACE_ఉపసర్గ LogSeverity::Debug, {}, {}, __VA_ARGS__); } అయితే (0)

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య log_శీర్షిక(RTLIL::Design *design, FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_శీర్షిక(design, fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య log_warning(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_warning(Log_ఆధారం_Location{}, "Warning: ", fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
అంతఃవరుస శూన్య log_warning_noprefix(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_warning(Log_ఆధారం_Location{}, {}, fmt.format_string(), fmt.format(వాదనలు_2...));
}

అంతఃవరుస శూన్య log_experimental(స్థిరరాశి std::string &str)
{
	logger().కూడు_experimental(str);
}

అంతఃవరుస శూన్య log_deprecated(స్థిరరాశి std::string &feature, స్థిరరాశి std::string &earliest_removal = "")
{
	logger().కూడు_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
మూస <typename... వాదనలు_3>
శూన్య log_file_warning(Log_ఆధారం_Location src, FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
శూన్య log_file_info(Log_ఆధారం_Location src, FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
[[noreturn]] శూన్య log_error(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_error(Log_ఆధారం_Location{}, "ERROR: ", fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
[[noreturn]] శూన్య log_file_error(Log_ఆధారం_Location src, FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
శూన్య log_file_nonfatal_error(Log_ఆధారం_Location src, FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(వాదనలు_2...));
}

మూస <typename... వాదనలు_3>
[[noreturn]] శూన్య log_cmd_error(FmtString<Type_తటస్థం<వాదనలు_3>...> fmt, స్థిరరాశి వాదనలు_3 &... వాదనలు_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(వాదనలు_2...));
}

అంతఃవరుస శూన్య log_suppressed()
{
	logger().suppressed();
}

అంతఃవరుస శూన్య log_spacer() { logger().spacer(); }
అంతఃవరుస శూన్య log_నెట్టు() { logger().నెట్టు(); }
అంతఃవరుస శూన్య log_తీయి() { logger().తీయి(); }

అంతఃవరుస శూన్య log_reset_దొంతర() { logger().reset_దొంతర(); }
అంతఃవరుస శూన్య log_flush() { logger().flush(); }

శూన్య log_backtrace(స్థిరరాశి అక్షరం *ఉపసర్గ_2, పూర్ణాంకం స్థాయిలు);


std::string log_signal(స్థిరరాశి RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(స్థిరరాశి RTLIL::Const &విలువ, bool autoint = true);

శూన్య log_module(RTLIL::Module *module, std::string indent = "");
శూన్య log_cell(RTLIL::Cell *cell, std::string indent = "");
శూన్య log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
శూన్య log_abort_అంతర్గత(స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి);
#నిర్వచించు log_abort() YOSYS_NAMESPACE_ఉపసర్గ log_abort_అంతర్గత(__FILE__, __LINE__)
#నిర్వచించు log_ping() YOSYS_NAMESPACE_ఉపసర్గ సంవర్గమానం("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
నిర్మాణం PerformanceTimer
{
#ఒకవేళ 1
	int64_t మొత్తం_ns;

	PerformanceTimer() {
		మొత్తం_ns = 0;
	}

	స్థిర int64_t query() {
#  ifdef _WIN32
		తిరిగి 0;
#  elif defined(RUSAGE_SELF)
		నిర్మాణం rusage rusage;
		int64_t t = 0;
		కొరకు (పూర్ణాంకం who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			ఒకవేళ (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		తిరిగి t;
#  లేకపోతే
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	శూన్య reset() {
		మొత్తం_ns = 0;
	}

	శూన్య begin() {
		మొత్తం_ns -= query();
	}

	శూన్య చివర() {
		మొత్తం_ns += query();
	}

	దశాంశం sec() స్థిరరాశి {
		తిరిగి మొత్తం_ns * 1e-9f;
	}
#లేకపోతే
	స్థిర int64_t query() { తిరిగి 0; }
	శూన్య reset() { }
	శూన్య begin() { }
	శూన్య చివర() { }
	దశాంశం sec() స్థిరరాశి { తిరిగి 0; }
#అయితే_ముగింపు
};

// simple API for quickly dumping values when debugging

స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(పొట్టి v) { సంవర్గమానం("%d", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(గుర్తులేని పొట్టి v) { సంవర్గమానం("%u", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(పూర్ణాంకం v) { సంవర్గమానం("%d", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(గుర్తులేని పూర్ణాంకం v) { సంవర్గమానం("%u", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(దీర్ఘ పూర్ణాంకం v) { సంవర్గమానం("%ld", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(గుర్తులేని దీర్ఘ పూర్ణాంకం v) { సంవర్గమానం("%lu", v); }
#నిర్వచితం_కాకపోతే _WIN32
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(దీర్ఘ దీర్ఘ పూర్ణాంకం v) { సంవర్గమానం("%lld", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(గుర్తులేని దీర్ఘ దీర్ఘ పూర్ణాంకం v) { సంవర్గమానం("%lld", v); }
#అయితే_ముగింపు
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(అక్షరం c)
{
	ఒకవేళ (c >= 32 && c < 127) {
		సంవర్గమానం("'%c'", c);
	} లేకపోతే {
		సంవర్గమానం("'\\x%02x'", c);
	}
}
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(గుర్తులేని అక్షరం c)
{
	ఒకవేళ (c >= 32 && c < 127) {
		సంవర్గమానం("'%c'", c);
	} లేకపోతే {
		సంవర్గమానం("'\\x%02x'", c);
	}
}
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(bool v) { సంవర్గమానం("%s", v ? "true" : "false"); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(ద్విగుణ v) { సంవర్గమానం("%f", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(అక్షరం *v) { సంవర్గమానం("%s", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(స్థిరరాశి అక్షరం *v) { సంవర్గమానం("%s", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(std::string v) { సంవర్గమానం("%s", v); }
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(PerformanceTimer p) { సంవర్గమానం("%f seconds", p.sec()); }
స్థిర అంతఃవరుస శూన్య log_dump_వాదనలు_కార్మికుడు(స్థిరరాశి అక్షరం *p) { log_assert(*p == 0); }
శూన్య log_dump_val_కార్మికుడు(IdString v);
శూన్య log_dump_val_కార్మికుడు(RTLIL::SigSpec v);
శూన్య log_dump_val_కార్మికుడు(RTLIL::State v);

మూస<typename K, typename T> స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(dict<K, T> &v);
మూస<typename K> స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(pool<K> &v);
మూస<typename K> స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(std::సదిశ<K> &v);
మూస<typename T> స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(T *ptr);

మూస<typename K, typename T>
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(dict<K, T> &v) {
	సంవర్గమానం("{");
	bool మొదటి = true;
	కొరకు (స్వయంచాలక &it : v) {
		సంవర్గమానం("%s ", మొదటి ? "" : ",");
		log_dump_val_కార్మికుడు(it.మొదటి);
		సంవర్గమానం(": ");
		log_dump_val_కార్మికుడు(it.రెండవ);
		మొదటి = false;
	}
	సంవర్గమానం(" }");
}

మూస<typename K>
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(pool<K> &v) {
	సంవర్గమానం("{");
	bool మొదటి = true;
	కొరకు (స్వయంచాలక &it : v) {
		సంవర్గమానం("%s ", మొదటి ? "" : ",");
		log_dump_val_కార్మికుడు(it);
		మొదటి = false;
	}
	సంవర్గమానం(" }");
}

మూస<typename K>
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(std::సదిశ<K> &v) {
	సంవర్గమానం("{");
	bool మొదటి = true;
	కొరకు (స్వయంచాలక &it : v) {
		సంవర్గమానం("%s ", మొదటి ? "" : ",");
		log_dump_val_కార్మికుడు(it);
		మొదటి = false;
	}
	సంవర్గమానం(" }");
}

మూస<typename T>
స్థిర అంతఃవరుస శూన్య log_dump_val_కార్మికుడు(T *ptr) { సంవర్గమానం("%p", ptr); }

మూస<typename T, typename ... వాదనలు_3>
శూన్య log_dump_వాదనలు_కార్మికుడు(స్థిరరాశి అక్షరం *p, T మొదటి, వాదనలు_3 ... వాదనలు_2)
{
	పూర్ణాంకం తదుపరి_p_state = 0;
	స్థిరరాశి అక్షరం *తదుపరి_p = p;
	అయితే (*తదుపరి_p && (తదుపరి_p_state != 0 || *తదుపరి_p != ',')) {
		ఒకవేళ (*తదుపరి_p == '"')
			చేయి {
				తదుపరి_p++;
				అయితే (*తదుపరి_p == '\\' && *(తదుపరి_p + 1))
					తదుపరి_p += 2;
			} అయితే (*తదుపరి_p && *తదుపరి_p != '"');
		ఒకవేళ (*తదుపరి_p == '\'') {
			తదుపరి_p++;
			ఒకవేళ (*తదుపరి_p == '\\')
				తదుపరి_p++;
			ఒకవేళ (*తదుపరి_p)
				తదుపరి_p++;
		}
		ఒకవేళ (*తదుపరి_p == '(' || *తదుపరి_p == '[' || *తదుపరి_p == '{')
			తదుపరి_p_state++;
		ఒకవేళ ((*తదుపరి_p == ')' || *తదుపరి_p == ']' || *తదుపరి_p == '}') && తదుపరి_p_state > 0)
			తదుపరి_p_state--;
		తదుపరి_p++;
	}
	సంవర్గమానం("\n\t%.*s => ", పూర్ణాంకం(తదుపరి_p - p), p);
	ఒకవేళ (*తదుపరి_p == ',')
		తదుపరి_p++;
	అయితే (*తదుపరి_p == ' ' || *తదుపరి_p == '\t' || *తదుపరి_p == '\r' || *తదుపరి_p == '\n')
		తదుపరి_p++;
	log_dump_val_కార్మికుడు(మొదటి);
	log_dump_వాదనలు_కార్మికుడు(తదుపరి_p, వాదనలు_2 ...);
}

#నిర్వచించు log_dump(...) చేయి { \
	సంవర్గమానం("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_వాదనలు_కార్మికుడు(#__VA_ARGS__, __VA_ARGS__); \
	సంవర్గమానం("\n"); \
} అయితే (0)

YOSYS_NAMESPACE_చివర

#అన్తర్భావ "kernel/yosys.h"

#అయితే_ముగింపు

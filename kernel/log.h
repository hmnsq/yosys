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

#اگر_غیر_تعریف_شدہ LOG_H
#تعریف_کرو LOG_H

#شامل "kernel/yosys_common.h"

#شامل <chrono>
#شامل <time.h>

#شامل <regex>
#تعریف_کرو YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#تعریف_کرو YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#اگر defined(_WIN32)
#  include <intrin.h>
#ورنہ
#  include <sys/time.h>
#  include <sys/resource.h>
#  اگر defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <signal.h>
#  endif
#اگر_ختم

// from libs/sha1/sha1.h
class SHA1;

YOSYS_NAMESPACE_BEGIN

// YS_DEBUGTRAP is a macro that is functionally equivalent to a breakpoint
// if the platform provides such functionality, and does nothing otherwise.
// If no debugger is attached, it starts a just-in-time debugger if available,
// and crashes the process otherwise.
#اگر defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#ورنہ
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# اگر __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP اٹھاؤ(SIGTRAP)
# ورنہ
#  define YS_DEBUGTRAP کرو {} جب(0)
# endif
#اگر_ختم

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#اگر defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING کرو { اگر (IsDebuggerPresent()) DebugBreak(); } جب(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING کرو { \
		خودکار old = اشارہ(SIGTRAP, SIG_IGN); اٹھاؤ(SIGTRAP); اشارہ(SIGTRAP, old); \
	} جب(0)
#ورنہ
# define YS_DEBUGTRAP_IF_DEBUGGING کرو {} جب(0)
#اگر_ختم

ساخت log_cmd_error_exception { };

شمار class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	سرخی,
	Warning,
	NonFatalError,
	Error
};

ساخت Log_ماخذ_Location {
	Log_ماخذ_Location() : filename({}),آغاز_سطر(-1),آغاز_ستون(-1),انتہا_سطر(-1),انتہا_ستون(-1) { }
	Log_ماخذ_Location(std::string filename) : filename(filename),آغاز_سطر(-1),آغاز_ستون(-1),انتہا_سطر(-1),انتہا_ستون(-1) { }
	Log_ماخذ_Location(std::string filename,صحیح آغاز_سطر) : filename(filename),آغاز_سطر(آغاز_سطر),آغاز_ستون(-1),انتہا_سطر(-1),انتہا_ستون(-1) { }
	Log_ماخذ_Location(std::string filename,صحیح آغاز_سطر,صحیح آغاز_ستون,صحیح انتہا_سطر,صحیح انتہا_ستون) :
		filename(filename),آغاز_سطر(آغاز_سطر),آغاز_ستون(آغاز_ستون),انتہا_سطر(انتہا_سطر),انتہا_ستون(انتہا_ستون) { }
	std::string filename;
	صحیح آغاز_سطر;
	صحیح آغاز_ستون;
	صحیح انتہا_سطر;
	صحیح انتہا_ستون;
};

ساخت Log_پیغام {
	Log_پیغام(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string_view پیغام_2);
	LogSeverity severity;
	std::string سابقہ_2;
	std::string format;
	std::string پیغام_2;
	std::chrono::steady_clock::وقت_نقطہ timestamp;
	Log_ماخذ_Location src;

	std::string cached_پیغام;
};

class LogSink
{
عوامی:
	مجازی ~LogSink() = طےشدہ;

	مجازی bool should_log(ثابت Log_پیغام &) ثابت { واپس true; }
	مجازی خالی لوگارتھم(ثابت Log_پیغام &پیغام) = 0;
	مجازی خالی flush() {}
	// TODO: Remove when AST/read_verilog removed
	مجازی FILE *file_handle() { واپس nullptr; }
};

class LogSinkRef : عوامی LogSink
{
عوامی:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ثابت Log_پیغام &پیغام) ثابت override { واپس sink->should_log(پیغام); }
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override { sink->لوگارتھم(پیغام); }
	خالی flush() override { sink->flush(); }
	FILE *file_handle() override { واپس sink->file_handle(); }
نجی:
	LogSink *sink;
};

class FileLogSink : عوامی LogSink
{
عوامی:
	explicit FileLogSink(ثابت std::string &filename, bool سطر_buffered, bool append);
	~FileLogSink() override;
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override;
	خالی flush() override;
	FILE *file_handle() override { واپس file; }

نجی:
	FILE *file;
};

class ConsoleLogSink : عوامی LogSink
{
عوامی:
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override;
	خالی flush() override;
	FILE *file_handle() override { واپس stdout; }
};

class StderrLogSink : عوامی LogSink
{
عوامی:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ثابت Log_پیغام &پیغام) ثابت override;
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override;
	خالی flush() override;
نجی:
	bool quiet_warnings;
};

class دھارا_Log_Sink : عوامی LogSink
{
عوامی:
	explicit دھارا_Log_Sink(std::ostream &دھارا) : دھارا(دھارا) {}
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override { دھارا << پیغام.cached_پیغام; }
	خالی flush() override { دھارا.flush(); }

نجی:
	std::ostream &دھارا;
};

class ScratchPadLogSink : عوامی LogSink
{
عوامی:
	explicit ScratchPadLogSink(std::string scratchpad);
	خالی لوگارتھم(ثابت Log_پیغام &پیغام) override;

نجی:
	std::string scratchpad;
};

class LogManager
{
نجی:
	ساخت Log_Expected_شے
	{
		Log_Expected_شے(ثابت std::regex &pat, صحیح expected) :
				نمونہ(pat), expected_گنتی(expected), current_گنتی(0) {}
		Log_Expected_شے() : expected_گنتی(0), current_گنتی(0) {}

		std::regex نمونہ;
		صحیح expected_گنتی;
		صحیح current_گنتی;
	};

عوامی:
	LogManager() = طےشدہ;

	سانچہ<typename T, typename... دلائل_3>
	T &جوڑو_sink(دلائل_3&&... دلائل_2)
	{
		خودکار sink = std::make_unique<T>(std::forward<دلائل_3>(دلائل_2)...);
		T &ref = *sink;
		sinks.ڈالو_عقب(std::move(sink));
		واپس ref;
	}

	سانچہ<typename F>
	خالی for_each_sink(F &&func)
	{
		برائے (خودکار &sink : sinks)
			func(*sink);
	}

	bool خالی_خانے() { واپس sinks.خالی_خانے(); }
	خالی clear() { sinks.clear(); }
	خالی clear_original();
	خالی flush() { برائے (خودکار &sink : sinks) sink->flush(); }

	class Scoped
	{
	عوامی:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.سائز());

			برائے (ثابت خودکار &sink : backup_sinks)
				manager.sinks.ڈالو_عقب(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ثابت Scoped &) = delete;
		Scoped &عامل=(ثابت Scoped &) = delete;

	نجی:
		LogManager &manager;
		std::سمتیہ<std::unique_ptr<LogSink>> backup_sinks;
		صحیح backup_verbose_level;
	};

	Scoped sink_scope()
	{
		واپس Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	عوامی:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), پچھلا_2(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = پچھلا_2;
		}

	نجی:
		LogManager &manager;
		bool پچھلا_2;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		واپس ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	عوامی:
		explicit LogMakeDebugHdl(LogManager &manager, bool آغاز_on = false)
			: manager(manager)
		{
			اگر (آغاز_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		خالی on()
		{
			اگر (حالت_2)
				واپس;
			حالت_2 = true;
			manager.make_debug++;
		}

		خالی off_silent()
		{
			اگر (!حالت_2)
				واپس;
			حالت_2 = false;
			manager.make_debug--;
		}

		خالی off()
		{
			off_silent();
		}
	نجی:
		LogManager &manager;
		bool حالت_2 = false;
	};

	LogMakeDebugHdl make_debug_scope(bool آغاز_on = false)
	{
		واپس LogMakeDebugHdl(*this, آغاز_on);
	}

	class ForceDebug
	{
	عوامی:
		explicit ForceDebug(LogManager &manager, bool آغاز_on = false)
			: manager(manager)
		{
			اگر (آغاز_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		خالی on()
		{
			اگر (فعال)
				واپس;

			فعال = true;
			manager.force_debug++;
		}

		خالی off()
		{
			اگر (!فعال)
				واپس;

			فعال = false;
			manager.force_debug--;
		}
	نجی:
		LogManager &manager;
		bool فعال = false;
	};

	ForceDebug force_debug_scope(bool آغاز_on = false)
	{
		واپس ForceDebug(*this, آغاز_on);
	}

	خالی force_debug_on() { force_debug++; }
	خالی force_debug_off() { اگر (force_debug > 0) force_debug--; }
	خالی سیٹ_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	خالی report_unexpected_error();

	خالی جوڑو_experimental_ignore(std::string name) { experimental_ignored.داخل_کرو(name); }
	خالی جوڑو_warn(std::string نمونہ) { warn_regexes.ڈالو_عقب(YS_REGEX_COMPILE(نمونہ)); }
	خالی جوڑو_nowarn(std::string نمونہ) { nowarn_regexes.ڈالو_عقب(YS_REGEX_COMPILE(نمونہ)); }
	خالی جوڑو_werror(std::string نمونہ) { werror_regexes.ڈالو_عقب(YS_REGEX_COMPILE(نمونہ)); }
	خالی جوڑو_expect(std::string type, std::string نمونہ, صحیح گنتی);

	خالی سیٹ_verbose_level(صحیح level) { verbose_level = level; }
	خالی جوڑو_verbose_level(صحیح level) { verbose_level += level; }
	خالی سیٹ_expect_no_warnings(bool قدر) { expect_no_warnings = قدر; }
	خالی سیٹ_log_وقت(bool قدر) { log_وقت = قدر; }
	خالی سیٹ_cmd_error_throw(bool قدر) { cmd_error_throw = قدر; }
	خالی سیٹ_hdump_all(bool قدر) { hdump_all = قدر; }
	صحیح get_verbose_level() ثابت { واپس verbose_level; }
	bool get_log_وقت() ثابت { واپس log_وقت; }
	صحیح get_warnings_unique() ثابت { واپس Get_سائز(warnings); }
	صحیح get_warnings_کل() ثابت { واپس warnings_گنتی; }
	صحیح get_errors_کل() ثابت { واپس errors_گنتی; }
	ثابت std::سیٹ<std::string> &get_experimental() ثابت { واپس experimental; }
	ثابت std::map<std::string, std::string> &get_deprecated() ثابت { واپس deprecated; }
	std::chrono::steady_clock::وقت_نقطہ get_initial_وقت() ثابت;

	خالی جوڑو_hdump(std::string name, std::string قدر) { hdump[name].داخل_کرو(قدر); }

	خالی formatted_string(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string str);
	خالی formatted_سرخی(RTLIL::Design *design, std::string_view format, std::string str);
	خالی formatted_warning(Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2);
	[[noreturn]] خالی formatted_error(Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2);
	خالی formatted_nonfatal_error(Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2);
	[[noreturn]] خالی formatted_cmd_error(std::string_view format, std::string پیغام_2);
	خالی suppressed();
	خالی جوڑو_experimental(ثابت std::string &str);
	خالی جوڑو_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal);
	خالی spacer();
	خالی ڈالو();
	خالی نکالو();

	خالی reset_ڈھیر();

	خالی check_expected();
	bool expects_error() { واپس (expect_error.سائز() + expect_سابقہ_error.سائز())>0; }
#اگر_غیر_تعریف_شدہ NDEBUG
	bool is_debug(صحیح n = 0) { اگر (force_debug) واپس true; debug_suppressed += n; واپس false; }
#ورنہ
	bool is_debug(صحیح = 0) { واپس false; }
#اگر_ختم
	خالی آغاز_hasher();
	std::string اختتام_hasher();

نجی:
	خالی logv_string(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string str_اندر);
	خالی error_with_سابقہ(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2, std::string_view format, std::string پیغام_2);

	std::سمتیہ<std::unique_ptr<LogSink>> sinks;
	صحیح verbose_level = 0;
	صحیح newline_گنتی = 0;
	سمتیہ<صحیح> سرخی_گنتی;
	صحیح errors_گنتی = 0;
	صحیح warnings_گنتی = 0;
	صحیح warnings_گنتی_noexpect = 0;
	std::سیٹ<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::سمتیہ<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_شے> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_شے> expect_سابقہ_log, expect_سابقہ_warning, expect_سابقہ_error;
	bool expect_no_warnings = false;
	bool log_وقت = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::سیٹ<std::string>> hdump;
	bool hdump_all = false;

	صحیح debug_suppressed = 0;
	صحیح make_debug = 0;
	صحیح force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

خارجی خالی (*log_error_atexit)();

خالی سیٹ_verific_logging(خالی (*cb)(صحیح پیغام_type, ثابت حرف *پیغام_id, Log_ماخذ_Location src, ثابت حرف *پیغام));
خارجی خالی (*log_verific_callback)(صحیح پیغام_type, ثابت حرف *پیغام_id, Log_ماخذ_Location src, ثابت حرف *پیغام);

#اگر_غیر_تعریف_شدہ NDEBUG
ساکن اندرونی bool ys_debug(صحیح n = 0) { واپس logger().is_debug(n); }
#ورنہ
ساکن اندرونی bool ys_debug(صحیح = 0) { واپس false; }
#اگر_ختم

سانچہ <typename... دلائل_3>
اندرونی خالی لوگارتھم(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_string(LogSeverity::Info, Log_ماخذ_Location{}, {}, fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
اندرونی خالی log_comment(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_ماخذ_Location{}, {}, fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
اندرونی خالی log_highlight(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_ماخذ_Location{}, {}, fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
اندرونی خالی log_formatted_string(LogSeverity severity, Log_ماخذ_Location src, std::string_view سابقہ_2,
		FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_string(severity, src, سابقہ_2, fmt.format_string(), fmt.format(دلائل_2...));
}

#تعریف_کرو log_debug(...) کرو { اگر (ys_debug(1)) YOSYS_NAMESPACE_سابقہ log_formatted_string(YOSYS_NAMESPACE_سابقہ LogSeverity::Debug, {}, {}, __VA_ARGS__); } جب (0)

سانچہ <typename... دلائل_3>
اندرونی خالی log_سرخی(RTLIL::Design *design, FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_سرخی(design, fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
اندرونی خالی log_warning(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_warning(Log_ماخذ_Location{}, "Warning: ", fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
اندرونی خالی log_warning_noprefix(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_warning(Log_ماخذ_Location{}, {}, fmt.format_string(), fmt.format(دلائل_2...));
}

اندرونی خالی log_experimental(ثابت std::string &str)
{
	logger().جوڑو_experimental(str);
}

اندرونی خالی log_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal = "")
{
	logger().جوڑو_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
سانچہ <typename... دلائل_3>
خالی log_file_warning(Log_ماخذ_Location src, FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
خالی log_file_info(Log_ماخذ_Location src, FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
[[noreturn]] خالی log_error(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_error(Log_ماخذ_Location{}, "ERROR: ", fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
[[noreturn]] خالی log_file_error(Log_ماخذ_Location src, FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
خالی log_file_nonfatal_error(Log_ماخذ_Location src, FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(دلائل_2...));
}

سانچہ <typename... دلائل_3>
[[noreturn]] خالی log_cmd_error(FmtString<Type_شناختی_عنصر<دلائل_3>...> fmt, ثابت دلائل_3 &... دلائل_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(دلائل_2...));
}

اندرونی خالی log_suppressed()
{
	logger().suppressed();
}

اندرونی خالی log_spacer() { logger().spacer(); }
اندرونی خالی log_ڈالو() { logger().ڈالو(); }
اندرونی خالی log_نکالو() { logger().نکالو(); }

اندرونی خالی log_reset_ڈھیر() { logger().reset_ڈھیر(); }
اندرونی خالی log_flush() { logger().flush(); }

خالی log_backtrace(ثابت حرف *سابقہ_2, صحیح سطحیں);


std::string log_signal(ثابت RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ثابت RTLIL::Const &قدر, bool autoint = true);

خالی log_module(RTLIL::Module *module, std::string indent = "");
خالی log_cell(RTLIL::Cell *cell, std::string indent = "");
خالی log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
خالی log_abort_اندرونی(ثابت حرف *file, صحیح سطر);
#تعریف_کرو log_abort() YOSYS_NAMESPACE_سابقہ log_abort_اندرونی(__FILE__, __LINE__)
#تعریف_کرو log_ping() YOSYS_NAMESPACE_سابقہ لوگارتھم("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
ساخت PerformanceTimer
{
#اگر 1
	int64_t کل_ns;

	PerformanceTimer() {
		کل_ns = 0;
	}

	ساکن int64_t query() {
#  ifdef _WIN32
		واپس 0;
#  elif defined(RUSAGE_SELF)
		ساخت rusage rusage;
		int64_t t = 0;
		برائے (صحیح who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			اگر (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		واپس t;
#  ورنہ
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	خالی reset() {
		کل_ns = 0;
	}

	خالی begin() {
		کل_ns -= query();
	}

	خالی انتہا() {
		کل_ns += query();
	}

	اعشاری sec() ثابت {
		واپس کل_ns * 1e-9f;
	}
#ورنہ
	ساکن int64_t query() { واپس 0; }
	خالی reset() { }
	خالی begin() { }
	خالی انتہا() { }
	اعشاری sec() ثابت { واپس 0; }
#اگر_ختم
};

// simple API for quickly dumping values when debugging

ساکن اندرونی خالی log_dump_val_کارکن(مختصر v) { لوگارتھم("%d", v); }
ساکن اندرونی خالی log_dump_val_کارکن(غیرعلامتی مختصر v) { لوگارتھم("%u", v); }
ساکن اندرونی خالی log_dump_val_کارکن(صحیح v) { لوگارتھم("%d", v); }
ساکن اندرونی خالی log_dump_val_کارکن(غیرعلامتی صحیح v) { لوگارتھم("%u", v); }
ساکن اندرونی خالی log_dump_val_کارکن(طویل صحیح v) { لوگارتھم("%ld", v); }
ساکن اندرونی خالی log_dump_val_کارکن(غیرعلامتی طویل صحیح v) { لوگارتھم("%lu", v); }
#اگر_غیر_تعریف_شدہ _WIN32
ساکن اندرونی خالی log_dump_val_کارکن(طویل طویل صحیح v) { لوگارتھم("%lld", v); }
ساکن اندرونی خالی log_dump_val_کارکن(غیرعلامتی طویل طویل صحیح v) { لوگارتھم("%lld", v); }
#اگر_ختم
ساکن اندرونی خالی log_dump_val_کارکن(حرف c)
{
	اگر (c >= 32 && c < 127) {
		لوگارتھم("'%c'", c);
	} ورنہ {
		لوگارتھم("'\\x%02x'", c);
	}
}
ساکن اندرونی خالی log_dump_val_کارکن(غیرعلامتی حرف c)
{
	اگر (c >= 32 && c < 127) {
		لوگارتھم("'%c'", c);
	} ورنہ {
		لوگارتھم("'\\x%02x'", c);
	}
}
ساکن اندرونی خالی log_dump_val_کارکن(bool v) { لوگارتھم("%s", v ? "true" : "false"); }
ساکن اندرونی خالی log_dump_val_کارکن(دوہرا v) { لوگارتھم("%f", v); }
ساکن اندرونی خالی log_dump_val_کارکن(حرف *v) { لوگارتھم("%s", v); }
ساکن اندرونی خالی log_dump_val_کارکن(ثابت حرف *v) { لوگارتھم("%s", v); }
ساکن اندرونی خالی log_dump_val_کارکن(std::string v) { لوگارتھم("%s", v); }
ساکن اندرونی خالی log_dump_val_کارکن(PerformanceTimer p) { لوگارتھم("%f seconds", p.sec()); }
ساکن اندرونی خالی log_dump_دلائل_کارکن(ثابت حرف *p) { log_assert(*p == 0); }
خالی log_dump_val_کارکن(IdString v);
خالی log_dump_val_کارکن(RTLIL::SigSpec v);
خالی log_dump_val_کارکن(RTLIL::State v);

سانچہ<typename K, typename T> ساکن اندرونی خالی log_dump_val_کارکن(dict<K, T> &v);
سانچہ<typename K> ساکن اندرونی خالی log_dump_val_کارکن(pool<K> &v);
سانچہ<typename K> ساکن اندرونی خالی log_dump_val_کارکن(std::سمتیہ<K> &v);
سانچہ<typename T> ساکن اندرونی خالی log_dump_val_کارکن(T *ptr);

سانچہ<typename K, typename T>
ساکن اندرونی خالی log_dump_val_کارکن(dict<K, T> &v) {
	لوگارتھم("{");
	bool پہلا = true;
	برائے (خودکار &it : v) {
		لوگارتھم("%s ", پہلا ? "" : ",");
		log_dump_val_کارکن(it.پہلا);
		لوگارتھم(": ");
		log_dump_val_کارکن(it.دوسرا);
		پہلا = false;
	}
	لوگارتھم(" }");
}

سانچہ<typename K>
ساکن اندرونی خالی log_dump_val_کارکن(pool<K> &v) {
	لوگارتھم("{");
	bool پہلا = true;
	برائے (خودکار &it : v) {
		لوگارتھم("%s ", پہلا ? "" : ",");
		log_dump_val_کارکن(it);
		پہلا = false;
	}
	لوگارتھم(" }");
}

سانچہ<typename K>
ساکن اندرونی خالی log_dump_val_کارکن(std::سمتیہ<K> &v) {
	لوگارتھم("{");
	bool پہلا = true;
	برائے (خودکار &it : v) {
		لوگارتھم("%s ", پہلا ? "" : ",");
		log_dump_val_کارکن(it);
		پہلا = false;
	}
	لوگارتھم(" }");
}

سانچہ<typename T>
ساکن اندرونی خالی log_dump_val_کارکن(T *ptr) { لوگارتھم("%p", ptr); }

سانچہ<typename T, typename ... دلائل_3>
خالی log_dump_دلائل_کارکن(ثابت حرف *p, T پہلا, دلائل_3 ... دلائل_2)
{
	صحیح اگلا_p_state = 0;
	ثابت حرف *اگلا_p = p;
	جب (*اگلا_p && (اگلا_p_state != 0 || *اگلا_p != ',')) {
		اگر (*اگلا_p == '"')
			کرو {
				اگلا_p++;
				جب (*اگلا_p == '\\' && *(اگلا_p + 1))
					اگلا_p += 2;
			} جب (*اگلا_p && *اگلا_p != '"');
		اگر (*اگلا_p == '\'') {
			اگلا_p++;
			اگر (*اگلا_p == '\\')
				اگلا_p++;
			اگر (*اگلا_p)
				اگلا_p++;
		}
		اگر (*اگلا_p == '(' || *اگلا_p == '[' || *اگلا_p == '{')
			اگلا_p_state++;
		اگر ((*اگلا_p == ')' || *اگلا_p == ']' || *اگلا_p == '}') && اگلا_p_state > 0)
			اگلا_p_state--;
		اگلا_p++;
	}
	لوگارتھم("\n\t%.*s => ", صحیح(اگلا_p - p), p);
	اگر (*اگلا_p == ',')
		اگلا_p++;
	جب (*اگلا_p == ' ' || *اگلا_p == '\t' || *اگلا_p == '\r' || *اگلا_p == '\n')
		اگلا_p++;
	log_dump_val_کارکن(پہلا);
	log_dump_دلائل_کارکن(اگلا_p, دلائل_2 ...);
}

#تعریف_کرو log_dump(...) کرو { \
	لوگارتھم("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_دلائل_کارکن(#__VA_ARGS__, __VA_ARGS__); \
	لوگارتھم("\n"); \
} جب (0)

YOSYS_NAMESPACE_انتہا

#شامل "kernel/yosys.h"

#اگر_ختم

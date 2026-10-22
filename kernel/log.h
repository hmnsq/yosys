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

#که_نه_تعریف_شوی LOG_H
#تعریف_کړه LOG_H

#ضمن "kernel/yosys_common.h"

#ضمن <chrono>
#ضمن <time.h>

#ضمن <regex>
#تعریف_کړه YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#تعریف_کړه YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
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
#د_که_پای

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
#د_که_پای

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#إذا defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { إذا (IsDebuggerPresent()) DebugBreak(); } طالما(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { \
		خپلکاری old = إشارة(SIGTRAP, SIG_IGN); ارفع(SIGTRAP); إشارة(SIGTRAP, old); \
	} طالما(0)
#وإلا
# define YS_DEBUGTRAP_IF_DEBUGGING افعل {} طالما(0)
#د_که_پای

بنية log_cmd_error_exception { };

تعداد class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	سرلیک,
	Warning,
	NonFatalError,
	Error
};

بنية Log_سرچینه_Location {
	Log_سرچینه_Location() : filename({}),پیل_کرښه(-1),پیل_ستون(-1),پای_ټکی_کرښه(-1),پای_ټکی_ستون(-1) { }
	Log_سرچینه_Location(std::string filename) : filename(filename),پیل_کرښه(-1),پیل_ستون(-1),پای_ټکی_کرښه(-1),پای_ټکی_ستون(-1) { }
	Log_سرچینه_Location(std::string filename,صحيح پیل_کرښه) : filename(filename),پیل_کرښه(پیل_کرښه),پیل_ستون(-1),پای_ټکی_کرښه(-1),پای_ټکی_ستون(-1) { }
	Log_سرچینه_Location(std::string filename,صحيح پیل_کرښه,صحيح پیل_ستون,صحيح پای_ټکی_کرښه,صحيح پای_ټکی_ستون) :
		filename(filename),پیل_کرښه(پیل_کرښه),پیل_ستون(پیل_ستون),پای_ټکی_کرښه(پای_ټکی_کرښه),پای_ټکی_ستون(پای_ټکی_ستون) { }
	std::string filename;
	صحيح پیل_کرښه;
	صحيح پیل_ستون;
	صحيح پای_ټکی_کرښه;
	صحيح پای_ټکی_ستون;
};

بنية Log_پیغام {
	Log_پیغام(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string_view پیغام_2);
	LogSeverity severity;
	std::string مختاړی_2;
	std::string format;
	std::string پیغام_2;
	std::chrono::steady_clock::وخت_ټکی timestamp;
	Log_سرچینه_Location src;

	std::string cached_پیغام;
};

class LogSink
{
عام:
	مجازي ~LogSink() = افتراضي;

	مجازي bool should_log(ثابت Log_پیغام &) ثابت { أرجع true; }
	مجازي فراغ لوګاریتم(ثابت Log_پیغام &پیغام) = 0;
	مجازي فراغ flush() {}
	// TODO: Remove when AST/read_verilog removed
	مجازي FILE *file_handle() { أرجع nullptr; }
};

class LogSinkRef : عام LogSink
{
عام:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ثابت Log_پیغام &پیغام) ثابت override { أرجع sink->should_log(پیغام); }
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override { sink->لوګاریتم(پیغام); }
	فراغ flush() override { sink->flush(); }
	FILE *file_handle() override { أرجع sink->file_handle(); }
خاص:
	LogSink *sink;
};

class FileLogSink : عام LogSink
{
عام:
	explicit FileLogSink(ثابت std::string &filename, bool کرښه_buffered, bool append);
	~FileLogSink() override;
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع file; }

خاص:
	FILE *file;
};

class ConsoleLogSink : عام LogSink
{
عام:
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع stdout; }
};

class StderrLogSink : عام LogSink
{
عام:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ثابت Log_پیغام &پیغام) ثابت override;
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override;
	فراغ flush() override;
خاص:
	bool quiet_warnings;
};

class بهیر_Log_Sink : عام LogSink
{
عام:
	explicit بهیر_Log_Sink(std::ostream &بهیر) : بهیر(بهیر) {}
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override { بهیر << پیغام.cached_پیغام; }
	فراغ flush() override { بهیر.flush(); }

خاص:
	std::ostream &بهیر;
};

class ScratchPadLogSink : عام LogSink
{
عام:
	explicit ScratchPadLogSink(std::string scratchpad);
	فراغ لوګاریتم(ثابت Log_پیغام &پیغام) override;

خاص:
	std::string scratchpad;
};

class LogManager
{
خاص:
	بنية Log_Expected_توکی
	{
		Log_Expected_توکی(ثابت std::regex &pat, صحيح expected) :
				نمونه(pat), expected_شمېر(expected), current_شمېر(0) {}
		Log_Expected_توکی() : expected_شمېر(0), current_شمېر(0) {}

		std::regex نمونه;
		صحيح expected_شمېر;
		صحيح current_شمېر;
	};

عام:
	LogManager() = افتراضي;

	کينډۍ<typename T, typename... دلیلونه_3>
	T &جمع_کړه_sink(دلیلونه_3&&... دلیلونه_2)
	{
		خپلکاری sink = std::make_unique<T>(std::forward<دلیلونه_3>(دلیلونه_2)...);
		T &ref = *sink;
		sinks.ټېل_شا(std::move(sink));
		أرجع ref;
	}

	کينډۍ<typename F>
	فراغ for_each_sink(F &&func)
	{
		لكل (خپلکاری &sink : sinks)
			func(*sink);
	}

	bool تشه() { أرجع sinks.تشه(); }
	فراغ clear() { sinks.clear(); }
	فراغ clear_original();
	فراغ flush() { لكل (خپلکاری &sink : sinks) sink->flush(); }

	class Scoped
	{
	عام:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.اندازه());

			لكل (ثابت خپلکاری &sink : backup_sinks)
				manager.sinks.ټېل_شا(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ثابت Scoped &) = delete;
		Scoped &چلوونکی=(ثابت Scoped &) = delete;

	خاص:
		LogManager &manager;
		std::ویکټور<std::unique_ptr<LogSink>> backup_sinks;
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
			: manager(manager), پخوانی(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = پخوانی;
		}

	خاص:
		LogManager &manager;
		bool پخوانی;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		أرجع ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	عام:
		explicit LogMakeDebugHdl(LogManager &manager, bool پیل_on = false)
			: manager(manager)
		{
			إذا (پیل_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		فراغ on()
		{
			إذا (حالت)
				أرجع;
			حالت = true;
			manager.make_debug++;
		}

		فراغ off_silent()
		{
			إذا (!حالت)
				أرجع;
			حالت = false;
			manager.make_debug--;
		}

		فراغ off()
		{
			off_silent();
		}
	خاص:
		LogManager &manager;
		bool حالت = false;
	};

	LogMakeDebugHdl make_debug_scope(bool پیل_on = false)
	{
		أرجع LogMakeDebugHdl(*this, پیل_on);
	}

	class ForceDebug
	{
	عام:
		explicit ForceDebug(LogManager &manager, bool پیل_on = false)
			: manager(manager)
		{
			إذا (پیل_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		فراغ on()
		{
			إذا (فعال)
				أرجع;

			فعال = true;
			manager.force_debug++;
		}

		فراغ off()
		{
			إذا (!فعال)
				أرجع;

			فعال = false;
			manager.force_debug--;
		}
	خاص:
		LogManager &manager;
		bool فعال = false;
	};

	ForceDebug force_debug_scope(bool پیل_on = false)
	{
		أرجع ForceDebug(*this, پیل_on);
	}

	فراغ force_debug_on() { force_debug++; }
	فراغ force_debug_off() { إذا (force_debug > 0) force_debug--; }
	فراغ سېټ_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	فراغ report_unexpected_error();

	فراغ جمع_کړه_experimental_ignore(std::string name) { experimental_ignored.ورننباسه(name); }
	فراغ جمع_کړه_warn(std::string نمونه) { warn_regexes.ټېل_شا(YS_REGEX_COMPILE(نمونه)); }
	فراغ جمع_کړه_nowarn(std::string نمونه) { nowarn_regexes.ټېل_شا(YS_REGEX_COMPILE(نمونه)); }
	فراغ جمع_کړه_werror(std::string نمونه) { werror_regexes.ټېل_شا(YS_REGEX_COMPILE(نمونه)); }
	فراغ جمع_کړه_expect(std::string type, std::string نمونه, صحيح شمېر);

	فراغ سېټ_verbose_level(صحيح level) { verbose_level = level; }
	فراغ جمع_کړه_verbose_level(صحيح level) { verbose_level += level; }
	فراغ سېټ_expect_no_warnings(bool ارزښت) { expect_no_warnings = ارزښت; }
	فراغ سېټ_log_وخت(bool ارزښت) { log_وخت = ارزښت; }
	فراغ سېټ_cmd_error_throw(bool ارزښت) { cmd_error_throw = ارزښت; }
	فراغ سېټ_hdump_all(bool ارزښت) { hdump_all = ارزښت; }
	صحيح get_verbose_level() ثابت { أرجع verbose_level; }
	bool get_log_وخت() ثابت { أرجع log_وخت; }
	صحيح get_warnings_unique() ثابت { أرجع Get_اندازه(warnings); }
	صحيح get_warnings_ټول() ثابت { أرجع warnings_شمېر; }
	صحيح get_errors_ټول() ثابت { أرجع errors_شمېر; }
	ثابت std::سېټ<std::string> &get_experimental() ثابت { أرجع experimental; }
	ثابت std::map<std::string, std::string> &get_deprecated() ثابت { أرجع deprecated; }
	std::chrono::steady_clock::وخت_ټکی get_initial_وخت() ثابت;

	فراغ جمع_کړه_hdump(std::string name, std::string ارزښت) { hdump[name].ورننباسه(ارزښت); }

	فراغ formatted_string(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string str);
	فراغ formatted_سرلیک(RTLIL::Design *design, std::string_view format, std::string str);
	فراغ formatted_warning(Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2);
	[[noreturn]] فراغ formatted_error(Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2);
	فراغ formatted_nonfatal_error(Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2);
	[[noreturn]] فراغ formatted_cmd_error(std::string_view format, std::string پیغام_2);
	فراغ suppressed();
	فراغ جمع_کړه_experimental(ثابت std::string &str);
	فراغ جمع_کړه_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal);
	فراغ spacer();
	فراغ ټېل();
	فراغ راوباسه();

	فراغ reset_ډېری();

	فراغ check_expected();
	bool expects_error() { أرجع (expect_error.اندازه() + expect_مختاړی_error.اندازه())>0; }
#که_نه_تعریف_شوی NDEBUG
	bool is_debug(صحيح n = 0) { إذا (force_debug) أرجع true; debug_suppressed += n; أرجع false; }
#وإلا
	bool is_debug(صحيح = 0) { أرجع false; }
#د_که_پای
	فراغ پیل_hasher();
	std::string پای_hasher();

خاص:
	فراغ logv_string(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string str_دننه);
	فراغ error_with_مختاړی(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2, std::string_view format, std::string پیغام_2);

	std::ویکټور<std::unique_ptr<LogSink>> sinks;
	صحيح verbose_level = 0;
	صحيح newline_شمېر = 0;
	ویکټور<صحيح> سرلیک_شمېر;
	صحيح errors_شمېر = 0;
	صحيح warnings_شمېر = 0;
	صحيح warnings_شمېر_noexpect = 0;
	std::سېټ<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::ویکټور<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_توکی> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_توکی> expect_مختاړی_log, expect_مختاړی_warning, expect_مختاړی_error;
	bool expect_no_warnings = false;
	bool log_وخت = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::سېټ<std::string>> hdump;
	bool hdump_all = false;

	صحيح debug_suppressed = 0;
	صحيح make_debug = 0;
	صحيح force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

خارجي فراغ (*log_error_atexit)();

فراغ سېټ_verific_logging(فراغ (*cb)(صحيح پیغام_type, ثابت حرف *پیغام_id, Log_سرچینه_Location src, ثابت حرف *پیغام));
خارجي فراغ (*log_verific_callback)(صحيح پیغام_type, ثابت حرف *پیغام_id, Log_سرچینه_Location src, ثابت حرف *پیغام);

#که_نه_تعریف_شوی NDEBUG
ساكن مضمن bool ys_debug(صحيح n = 0) { أرجع logger().is_debug(n); }
#وإلا
ساكن مضمن bool ys_debug(صحيح = 0) { أرجع false; }
#د_که_پای

کينډۍ <typename... دلیلونه_3>
مضمن فراغ لوګاریتم(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_string(LogSeverity::Info, Log_سرچینه_Location{}, {}, fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
مضمن فراغ log_comment(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_سرچینه_Location{}, {}, fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
مضمن فراغ log_highlight(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_سرچینه_Location{}, {}, fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
مضمن فراغ log_formatted_string(LogSeverity severity, Log_سرچینه_Location src, std::string_view مختاړی_2,
		FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_string(severity, src, مختاړی_2, fmt.format_string(), fmt.format(دلیلونه_2...));
}

#تعریف_کړه log_debug(...) افعل { إذا (ys_debug(1)) YOSYS_NAMESPACE_مختاړی log_formatted_string(YOSYS_NAMESPACE_مختاړی LogSeverity::Debug, {}, {}, __VA_ARGS__); } طالما (0)

کينډۍ <typename... دلیلونه_3>
مضمن فراغ log_سرلیک(RTLIL::Design *design, FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_سرلیک(design, fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
مضمن فراغ log_warning(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_warning(Log_سرچینه_Location{}, "Warning: ", fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
مضمن فراغ log_warning_noprefix(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_warning(Log_سرچینه_Location{}, {}, fmt.format_string(), fmt.format(دلیلونه_2...));
}

مضمن فراغ log_experimental(ثابت std::string &str)
{
	logger().جمع_کړه_experimental(str);
}

مضمن فراغ log_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal = "")
{
	logger().جمع_کړه_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
کينډۍ <typename... دلیلونه_3>
فراغ log_file_warning(Log_سرچینه_Location src, FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
فراغ log_file_info(Log_سرچینه_Location src, FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
[[noreturn]] فراغ log_error(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_error(Log_سرچینه_Location{}, "ERROR: ", fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
[[noreturn]] فراغ log_file_error(Log_سرچینه_Location src, FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
فراغ log_file_nonfatal_error(Log_سرچینه_Location src, FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(دلیلونه_2...));
}

کينډۍ <typename... دلیلونه_3>
[[noreturn]] فراغ log_cmd_error(FmtString<Type_بې_اغېزه_عنصر<دلیلونه_3>...> fmt, ثابت دلیلونه_3 &... دلیلونه_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(دلیلونه_2...));
}

مضمن فراغ log_suppressed()
{
	logger().suppressed();
}

مضمن فراغ log_spacer() { logger().spacer(); }
مضمن فراغ log_ټېل() { logger().ټېل(); }
مضمن فراغ log_راوباسه() { logger().راوباسه(); }

مضمن فراغ log_reset_ډېری() { logger().reset_ډېری(); }
مضمن فراغ log_flush() { logger().flush(); }

فراغ log_backtrace(ثابت حرف *مختاړی_2, صحيح پوړونه);


std::string log_signal(ثابت RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ثابت RTLIL::Const &ارزښت, bool autoint = true);

فراغ log_module(RTLIL::Module *module, std::string indent = "");
فراغ log_cell(RTLIL::Cell *cell, std::string indent = "");
فراغ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
فراغ log_abort_دننی(ثابت حرف *file, صحيح کرښه);
#تعریف_کړه log_abort() YOSYS_NAMESPACE_مختاړی log_abort_دننی(__FILE__, __LINE__)
#تعریف_کړه log_ping() YOSYS_NAMESPACE_مختاړی لوګاریتم("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
بنية PerformanceTimer
{
#إذا 1
	int64_t ټول_ns;

	PerformanceTimer() {
		ټول_ns = 0;
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
		ټول_ns = 0;
	}

	فراغ begin() {
		ټول_ns -= query();
	}

	فراغ پای_ټکی() {
		ټول_ns += query();
	}

	عائم sec() ثابت {
		أرجع ټول_ns * 1e-9f;
	}
#وإلا
	ساكن int64_t query() { أرجع 0; }
	فراغ reset() { }
	فراغ begin() { }
	فراغ پای_ټکی() { }
	عائم sec() ثابت { أرجع 0; }
#د_که_پای
};

// simple API for quickly dumping values when debugging

ساكن مضمن فراغ log_dump_val_کارګر(لنډ v) { لوګاریتم("%d", v); }
ساكن مضمن فراغ log_dump_val_کارګر(بینښه لنډ v) { لوګاریتم("%u", v); }
ساكن مضمن فراغ log_dump_val_کارګر(صحيح v) { لوګاریتم("%d", v); }
ساكن مضمن فراغ log_dump_val_کارګر(بینښه صحيح v) { لوګاریتم("%u", v); }
ساكن مضمن فراغ log_dump_val_کارګر(اوږد صحيح v) { لوګاریتم("%ld", v); }
ساكن مضمن فراغ log_dump_val_کارګر(بینښه اوږد صحيح v) { لوګاریتم("%lu", v); }
#که_نه_تعریف_شوی _WIN32
ساكن مضمن فراغ log_dump_val_کارګر(اوږد اوږد صحيح v) { لوګاریتم("%lld", v); }
ساكن مضمن فراغ log_dump_val_کارګر(بینښه اوږد اوږد صحيح v) { لوګاریتم("%lld", v); }
#د_که_پای
ساكن مضمن فراغ log_dump_val_کارګر(حرف c)
{
	إذا (c >= 32 && c < 127) {
		لوګاریتم("'%c'", c);
	} وإلا {
		لوګاریتم("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_کارګر(بینښه حرف c)
{
	إذا (c >= 32 && c < 127) {
		لوګاریتم("'%c'", c);
	} وإلا {
		لوګاریتم("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_کارګر(bool v) { لوګاریتم("%s", v ? "true" : "false"); }
ساكن مضمن فراغ log_dump_val_کارګر(مزدوج v) { لوګاریتم("%f", v); }
ساكن مضمن فراغ log_dump_val_کارګر(حرف *v) { لوګاریتم("%s", v); }
ساكن مضمن فراغ log_dump_val_کارګر(ثابت حرف *v) { لوګاریتم("%s", v); }
ساكن مضمن فراغ log_dump_val_کارګر(std::string v) { لوګاریتم("%s", v); }
ساكن مضمن فراغ log_dump_val_کارګر(PerformanceTimer p) { لوګاریتم("%f seconds", p.sec()); }
ساكن مضمن فراغ log_dump_دلیلونه_کارګر(ثابت حرف *p) { log_assert(*p == 0); }
فراغ log_dump_val_کارګر(IdString v);
فراغ log_dump_val_کارګر(RTLIL::SigSpec v);
فراغ log_dump_val_کارګر(RTLIL::State v);

کينډۍ<typename K, typename T> ساكن مضمن فراغ log_dump_val_کارګر(dict<K, T> &v);
کينډۍ<typename K> ساكن مضمن فراغ log_dump_val_کارګر(pool<K> &v);
کينډۍ<typename K> ساكن مضمن فراغ log_dump_val_کارګر(std::ویکټور<K> &v);
کينډۍ<typename T> ساكن مضمن فراغ log_dump_val_کارګر(T *ptr);

کينډۍ<typename K, typename T>
ساكن مضمن فراغ log_dump_val_کارګر(dict<K, T> &v) {
	لوګاریتم("{");
	bool لومړی = true;
	لكل (خپلکاری &it : v) {
		لوګاریتم("%s ", لومړی ? "" : ",");
		log_dump_val_کارګر(it.لومړی);
		لوګاریتم(": ");
		log_dump_val_کارګر(it.دوهم);
		لومړی = false;
	}
	لوګاریتم(" }");
}

کينډۍ<typename K>
ساكن مضمن فراغ log_dump_val_کارګر(pool<K> &v) {
	لوګاریتم("{");
	bool لومړی = true;
	لكل (خپلکاری &it : v) {
		لوګاریتم("%s ", لومړی ? "" : ",");
		log_dump_val_کارګر(it);
		لومړی = false;
	}
	لوګاریتم(" }");
}

کينډۍ<typename K>
ساكن مضمن فراغ log_dump_val_کارګر(std::ویکټور<K> &v) {
	لوګاریتم("{");
	bool لومړی = true;
	لكل (خپلکاری &it : v) {
		لوګاریتم("%s ", لومړی ? "" : ",");
		log_dump_val_کارګر(it);
		لومړی = false;
	}
	لوګاریتم(" }");
}

کينډۍ<typename T>
ساكن مضمن فراغ log_dump_val_کارګر(T *ptr) { لوګاریتم("%p", ptr); }

کينډۍ<typename T, typename ... دلیلونه_3>
فراغ log_dump_دلیلونه_کارګر(ثابت حرف *p, T لومړی, دلیلونه_3 ... دلیلونه_2)
{
	صحيح بل_p_state = 0;
	ثابت حرف *بل_p = p;
	طالما (*بل_p && (بل_p_state != 0 || *بل_p != ',')) {
		إذا (*بل_p == '"')
			افعل {
				بل_p++;
				طالما (*بل_p == '\\' && *(بل_p + 1))
					بل_p += 2;
			} طالما (*بل_p && *بل_p != '"');
		إذا (*بل_p == '\'') {
			بل_p++;
			إذا (*بل_p == '\\')
				بل_p++;
			إذا (*بل_p)
				بل_p++;
		}
		إذا (*بل_p == '(' || *بل_p == '[' || *بل_p == '{')
			بل_p_state++;
		إذا ((*بل_p == ')' || *بل_p == ']' || *بل_p == '}') && بل_p_state > 0)
			بل_p_state--;
		بل_p++;
	}
	لوګاریتم("\n\t%.*s => ", صحيح(بل_p - p), p);
	إذا (*بل_p == ',')
		بل_p++;
	طالما (*بل_p == ' ' || *بل_p == '\t' || *بل_p == '\r' || *بل_p == '\n')
		بل_p++;
	log_dump_val_کارګر(لومړی);
	log_dump_دلیلونه_کارګر(بل_p, دلیلونه_2 ...);
}

#تعریف_کړه log_dump(...) افعل { \
	لوګاریتم("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_دلیلونه_کارګر(#__VA_ARGS__, __VA_ARGS__); \
	لوګاریتم("\n"); \
} طالما (0)

YOSYS_NAMESPACE_پای_ټکی

#ضمن "kernel/yosys.h"

#د_که_پای

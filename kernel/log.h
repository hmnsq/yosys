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

#جے_بنا_تعریف LOG_H
#تعریف_کرو LOG_H

#ضمن "kernel/yosys_common.h"

#ضمن <chrono>
#ضمن <time.h>

#ضمن <regex>
#تعریف_کرو YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#تعریف_کرو YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
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
#جے_انت

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
#جے_انت

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#إذا defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { إذا (IsDebuggerPresent()) DebugBreak(); } طالما(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { \
		خودکار old = إشارة(SIGTRAP, SIG_IGN); ارفع(SIGTRAP); إشارة(SIGTRAP, old); \
	} طالما(0)
#وإلا
# define YS_DEBUGTRAP_IF_DEBUGGING افعل {} طالما(0)
#جے_انت

بنية log_cmd_error_exception { };

تعداد class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	سرخی,
	Warning,
	NonFatalError,
	Error
};

بنية Log_سروت_Location {
	Log_سروت_Location() : filename({}),شروع_سطر(-1),شروع_خانہ(-1),سرا_سطر(-1),سرا_خانہ(-1) { }
	Log_سروت_Location(std::string filename) : filename(filename),شروع_سطر(-1),شروع_خانہ(-1),سرا_سطر(-1),سرا_خانہ(-1) { }
	Log_سروت_Location(std::string filename,صحيح شروع_سطر) : filename(filename),شروع_سطر(شروع_سطر),شروع_خانہ(-1),سرا_سطر(-1),سرا_خانہ(-1) { }
	Log_سروت_Location(std::string filename,صحيح شروع_سطر,صحيح شروع_خانہ,صحيح سرا_سطر,صحيح سرا_خانہ) :
		filename(filename),شروع_سطر(شروع_سطر),شروع_خانہ(شروع_خانہ),سرا_سطر(سرا_سطر),سرا_خانہ(سرا_خانہ) { }
	std::string filename;
	صحيح شروع_سطر;
	صحيح شروع_خانہ;
	صحيح سرا_سطر;
	صحيح سرا_خانہ;
};

بنية Log_سنیہا {
	Log_سنیہا(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string_view سنیہا);
	LogSeverity severity;
	std::string اگیتر_2;
	std::string format;
	std::string سنیہا;
	std::chrono::steady_clock::ویلا_نقطہ timestamp;
	Log_سروت_Location src;

	std::string cached_پیغام;
};

class LogSink
{
عام:
	مجازی ~LogSink() = افتراضي;

	مجازی bool should_log(ثابت Log_سنیہا &) ثابت { أرجع true; }
	مجازی فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) = 0;
	مجازی فراغ flush() {}
	// TODO: Remove when AST/read_verilog removed
	مجازی FILE *file_handle() { أرجع nullptr; }
};

class LogSinkRef : عام LogSink
{
عام:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ثابت Log_سنیہا &پیغام) ثابت override { أرجع sink->should_log(پیغام); }
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override { sink->لوگارتھم(پیغام); }
	فراغ flush() override { sink->flush(); }
	FILE *file_handle() override { أرجع sink->file_handle(); }
خاص:
	LogSink *sink;
};

class FileLogSink : عام LogSink
{
عام:
	explicit FileLogSink(ثابت std::string &filename, bool سطر_buffered, bool append);
	~FileLogSink() override;
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع file; }

خاص:
	FILE *file;
};

class ConsoleLogSink : عام LogSink
{
عام:
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع stdout; }
};

class StderrLogSink : عام LogSink
{
عام:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ثابت Log_سنیہا &پیغام) ثابت override;
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override;
	فراغ flush() override;
خاص:
	bool quiet_warnings;
};

class دھارا_Log_Sink : عام LogSink
{
عام:
	explicit دھارا_Log_Sink(std::ostream &دھارا) : دھارا(دھارا) {}
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override { دھارا << پیغام.cached_پیغام; }
	فراغ flush() override { دھارا.flush(); }

خاص:
	std::ostream &دھارا;
};

class ScratchPadLogSink : عام LogSink
{
عام:
	explicit ScratchPadLogSink(std::string scratchpad);
	فراغ لوگارتھم(ثابت Log_سنیہا &پیغام) override;

خاص:
	std::string scratchpad;
};

class LogManager
{
خاص:
	بنية Log_Expected_شے
	{
		Log_Expected_شے(ثابت std::regex &pat, صحيح expected) :
				نمونہ(pat), expected_گنتی(expected), current_گنتی(0) {}
		Log_Expected_شے() : expected_گنتی(0), current_گنتی(0) {}

		std::regex نمونہ;
		صحيح expected_گنتی;
		صحيح current_گنتی;
	};

عام:
	LogManager() = افتراضي;

	سانچا<typename T, typename... دلیلاں_3>
	T &جوڑو_sink(دلیلاں_3&&... دلیلاں_2)
	{
		خودکار sink = std::make_unique<T>(std::forward<دلیلاں_3>(دلیلاں_2)...);
		T &ref = *sink;
		sinks.ڈالو_پٹھ(std::move(sink));
		أرجع ref;
	}

	سانچا<typename F>
	فراغ for_each_sink(F &&func)
	{
		لكل (خودکار &sink : sinks)
			func(*sink);
	}

	bool خالی_تھاواں() { أرجع sinks.خالی_تھاواں(); }
	فراغ clear() { sinks.clear(); }
	فراغ clear_original();
	فراغ flush() { لكل (خودکار &sink : sinks) sink->flush(); }

	class Scoped
	{
	عام:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.ناپ());

			لكل (ثابت خودکار &sink : backup_sinks)
				manager.sinks.ڈالو_پٹھ(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ثابت Scoped &) = delete;
		Scoped &عامل=(ثابت Scoped &) = delete;

	خاص:
		LogManager &manager;
		std::سمتیہ<std::unique_ptr<LogSink>> backup_sinks;
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
			: manager(manager), پچھلا(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = پچھلا;
		}

	خاص:
		LogManager &manager;
		bool پچھلا;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		أرجع ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	عام:
		explicit LogMakeDebugHdl(LogManager &manager, bool شروع_on = false)
			: manager(manager)
		{
			إذا (شروع_on)
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

	LogMakeDebugHdl make_debug_scope(bool شروع_on = false)
	{
		أرجع LogMakeDebugHdl(*this, شروع_on);
	}

	class ForceDebug
	{
	عام:
		explicit ForceDebug(LogManager &manager, bool شروع_on = false)
			: manager(manager)
		{
			إذا (شروع_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		فراغ on()
		{
			إذا (سرگرم)
				أرجع;

			سرگرم = true;
			manager.force_debug++;
		}

		فراغ off()
		{
			إذا (!سرگرم)
				أرجع;

			سرگرم = false;
			manager.force_debug--;
		}
	خاص:
		LogManager &manager;
		bool سرگرم = false;
	};

	ForceDebug force_debug_scope(bool شروع_on = false)
	{
		أرجع ForceDebug(*this, شروع_on);
	}

	فراغ force_debug_on() { force_debug++; }
	فراغ force_debug_off() { إذا (force_debug > 0) force_debug--; }
	فراغ سیٹ_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	فراغ report_unexpected_error();

	فراغ جوڑو_experimental_ignore(std::string name) { experimental_ignored.داخل_کرو(name); }
	فراغ جوڑو_warn(std::string نمونہ) { warn_regexes.ڈالو_پٹھ(YS_REGEX_COMPILE(نمونہ)); }
	فراغ جوڑو_nowarn(std::string نمونہ) { nowarn_regexes.ڈالو_پٹھ(YS_REGEX_COMPILE(نمونہ)); }
	فراغ جوڑو_werror(std::string نمونہ) { werror_regexes.ڈالو_پٹھ(YS_REGEX_COMPILE(نمونہ)); }
	فراغ جوڑو_expect(std::string type, std::string نمونہ, صحيح گنتی);

	فراغ سیٹ_verbose_level(صحيح level) { verbose_level = level; }
	فراغ جوڑو_verbose_level(صحيح level) { verbose_level += level; }
	فراغ سیٹ_expect_no_warnings(bool قدر) { expect_no_warnings = قدر; }
	فراغ سیٹ_log_ویلا(bool قدر) { log_ویلا = قدر; }
	فراغ سیٹ_cmd_error_throw(bool قدر) { cmd_error_throw = قدر; }
	فراغ سیٹ_hdump_all(bool قدر) { hdump_all = قدر; }
	صحيح get_verbose_level() ثابت { أرجع verbose_level; }
	bool get_log_ویلا() ثابت { أرجع log_ویلا; }
	صحيح get_warnings_unique() ثابت { أرجع Get_ناپ(warnings); }
	صحيح get_warnings_کل() ثابت { أرجع warnings_گنتی; }
	صحيح get_errors_کل() ثابت { أرجع errors_گنتی; }
	ثابت std::سیٹ<std::string> &get_experimental() ثابت { أرجع experimental; }
	ثابت std::map<std::string, std::string> &get_deprecated() ثابت { أرجع deprecated; }
	std::chrono::steady_clock::ویلا_نقطہ get_initial_ویلا() ثابت;

	فراغ جوڑو_hdump(std::string name, std::string قدر) { hdump[name].داخل_کرو(قدر); }

	فراغ formatted_string(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string str);
	فراغ formatted_سرخی(RTLIL::Design *design, std::string_view format, std::string str);
	فراغ formatted_warning(Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا);
	[[noreturn]] فراغ formatted_error(Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا);
	فراغ formatted_nonfatal_error(Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا);
	[[noreturn]] فراغ formatted_cmd_error(std::string_view format, std::string سنیہا);
	فراغ suppressed();
	فراغ جوڑو_experimental(ثابت std::string &str);
	فراغ جوڑو_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal);
	فراغ spacer();
	فراغ ڈالو();
	فراغ نکالو();

	فراغ reset_ڈھیر();

	فراغ check_expected();
	bool expects_error() { أرجع (expect_error.ناپ() + expect_اگیتر_error.ناپ())>0; }
#جے_بنا_تعریف NDEBUG
	bool is_debug(صحيح n = 0) { إذا (force_debug) أرجع true; debug_suppressed += n; أرجع false; }
#وإلا
	bool is_debug(صحيح = 0) { أرجع false; }
#جے_انت
	فراغ شروع_hasher();
	std::string انت_hasher();

خاص:
	فراغ logv_string(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string str_وچ);
	فراغ error_with_اگیتر(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2, std::string_view format, std::string سنیہا);

	std::سمتیہ<std::unique_ptr<LogSink>> sinks;
	صحيح verbose_level = 0;
	صحيح newline_گنتی = 0;
	سمتیہ<صحيح> سرخی_گنتی;
	صحيح errors_گنتی = 0;
	صحيح warnings_گنتی = 0;
	صحيح warnings_گنتی_noexpect = 0;
	std::سیٹ<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::سمتیہ<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_شے> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_شے> expect_اگیتر_log, expect_اگیتر_warning, expect_اگیتر_error;
	bool expect_no_warnings = false;
	bool log_ویلا = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::سیٹ<std::string>> hdump;
	bool hdump_all = false;

	صحيح debug_suppressed = 0;
	صحيح make_debug = 0;
	صحيح force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

خارجي فراغ (*log_error_atexit)();

فراغ سیٹ_verific_logging(فراغ (*cb)(صحيح پیغام_type, ثابت حرف *سنیہا_id, Log_سروت_Location src, ثابت حرف *پیغام));
خارجي فراغ (*log_verific_callback)(صحيح پیغام_type, ثابت حرف *سنیہا_id, Log_سروت_Location src, ثابت حرف *پیغام);

#جے_بنا_تعریف NDEBUG
ساكن مضمن bool ys_debug(صحيح n = 0) { أرجع logger().is_debug(n); }
#وإلا
ساكن مضمن bool ys_debug(صحيح = 0) { أرجع false; }
#جے_انت

سانچا <typename... دلیلاں_3>
مضمن فراغ لوگارتھم(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_string(LogSeverity::Info, Log_سروت_Location{}, {}, fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
مضمن فراغ log_comment(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_سروت_Location{}, {}, fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
مضمن فراغ log_highlight(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_سروت_Location{}, {}, fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
مضمن فراغ log_formatted_string(LogSeverity severity, Log_سروت_Location src, std::string_view اگیتر_2,
		FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_string(severity, src, اگیتر_2, fmt.format_string(), fmt.format(دلیلاں_2...));
}

#تعریف_کرو log_debug(...) افعل { إذا (ys_debug(1)) YOSYS_NAMESPACE_اگیتر log_formatted_string(YOSYS_NAMESPACE_اگیتر LogSeverity::Debug, {}, {}, __VA_ARGS__); } طالما (0)

سانچا <typename... دلیلاں_3>
مضمن فراغ log_سرخی(RTLIL::Design *design, FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_سرخی(design, fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
مضمن فراغ log_warning(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_warning(Log_سروت_Location{}, "Warning: ", fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
مضمن فراغ log_warning_noprefix(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_warning(Log_سروت_Location{}, {}, fmt.format_string(), fmt.format(دلیلاں_2...));
}

مضمن فراغ log_experimental(ثابت std::string &str)
{
	logger().جوڑو_experimental(str);
}

مضمن فراغ log_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal = "")
{
	logger().جوڑو_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
سانچا <typename... دلیلاں_3>
فراغ log_file_warning(Log_سروت_Location src, FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
فراغ log_file_info(Log_سروت_Location src, FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
[[noreturn]] فراغ log_error(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_error(Log_سروت_Location{}, "ERROR: ", fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
[[noreturn]] فراغ log_file_error(Log_سروت_Location src, FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
فراغ log_file_nonfatal_error(Log_سروت_Location src, FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(دلیلاں_2...));
}

سانچا <typename... دلیلاں_3>
[[noreturn]] فراغ log_cmd_error(FmtString<Type_پچھان_تت<دلیلاں_3>...> fmt, ثابت دلیلاں_3 &... دلیلاں_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(دلیلاں_2...));
}

مضمن فراغ log_suppressed()
{
	logger().suppressed();
}

مضمن فراغ log_spacer() { logger().spacer(); }
مضمن فراغ log_ڈالو() { logger().ڈالو(); }
مضمن فراغ log_نکالو() { logger().نکالو(); }

مضمن فراغ log_reset_ڈھیر() { logger().reset_ڈھیر(); }
مضمن فراغ log_flush() { logger().flush(); }

فراغ log_backtrace(ثابت حرف *اگیتر_2, صحيح پدھراں);


std::string log_signal(ثابت RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ثابت RTLIL::Const &قدر, bool autoint = true);

فراغ log_module(RTLIL::Module *module, std::string indent = "");
فراغ log_cell(RTLIL::Cell *cell, std::string indent = "");
فراغ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
فراغ log_abort_اندرونی(ثابت حرف *file, صحيح سطر);
#تعریف_کرو log_abort() YOSYS_NAMESPACE_اگیتر log_abort_اندرونی(__FILE__, __LINE__)
#تعریف_کرو log_ping() YOSYS_NAMESPACE_اگیتر لوگارتھم("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
بنية PerformanceTimer
{
#إذا 1
	int64_t کل_ns;

	PerformanceTimer() {
		کل_ns = 0;
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
		کل_ns = 0;
	}

	فراغ begin() {
		کل_ns -= query();
	}

	فراغ سرا() {
		کل_ns += query();
	}

	عائم sec() ثابت {
		أرجع کل_ns * 1e-9f;
	}
#وإلا
	ساكن int64_t query() { أرجع 0; }
	فراغ reset() { }
	فراغ begin() { }
	فراغ سرا() { }
	عائم sec() ثابت { أرجع 0; }
#جے_انت
};

// simple API for quickly dumping values when debugging

ساكن مضمن فراغ log_dump_val_کامہ(چھوٹا v) { لوگارتھم("%d", v); }
ساكن مضمن فراغ log_dump_val_کامہ(بنانشانی چھوٹا v) { لوگارتھم("%u", v); }
ساكن مضمن فراغ log_dump_val_کامہ(صحيح v) { لوگارتھم("%d", v); }
ساكن مضمن فراغ log_dump_val_کامہ(بنانشانی صحيح v) { لوگارتھم("%u", v); }
ساكن مضمن فراغ log_dump_val_کامہ(لما صحيح v) { لوگارتھم("%ld", v); }
ساكن مضمن فراغ log_dump_val_کامہ(بنانشانی لما صحيح v) { لوگارتھم("%lu", v); }
#جے_بنا_تعریف _WIN32
ساكن مضمن فراغ log_dump_val_کامہ(لما لما صحيح v) { لوگارتھم("%lld", v); }
ساكن مضمن فراغ log_dump_val_کامہ(بنانشانی لما لما صحيح v) { لوگارتھم("%lld", v); }
#جے_انت
ساكن مضمن فراغ log_dump_val_کامہ(حرف c)
{
	إذا (c >= 32 && c < 127) {
		لوگارتھم("'%c'", c);
	} وإلا {
		لوگارتھم("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_کامہ(بنانشانی حرف c)
{
	إذا (c >= 32 && c < 127) {
		لوگارتھم("'%c'", c);
	} وإلا {
		لوگارتھم("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_کامہ(bool v) { لوگارتھم("%s", v ? "true" : "false"); }
ساكن مضمن فراغ log_dump_val_کامہ(مزدوج v) { لوگارتھم("%f", v); }
ساكن مضمن فراغ log_dump_val_کامہ(حرف *v) { لوگارتھم("%s", v); }
ساكن مضمن فراغ log_dump_val_کامہ(ثابت حرف *v) { لوگارتھم("%s", v); }
ساكن مضمن فراغ log_dump_val_کامہ(std::string v) { لوگارتھم("%s", v); }
ساكن مضمن فراغ log_dump_val_کامہ(PerformanceTimer p) { لوگارتھم("%f seconds", p.sec()); }
ساكن مضمن فراغ log_dump_دلیلاں_کامہ(ثابت حرف *p) { log_assert(*p == 0); }
فراغ log_dump_val_کامہ(IdString v);
فراغ log_dump_val_کامہ(RTLIL::SigSpec v);
فراغ log_dump_val_کامہ(RTLIL::State v);

سانچا<typename K, typename T> ساكن مضمن فراغ log_dump_val_کامہ(dict<K, T> &v);
سانچا<typename K> ساكن مضمن فراغ log_dump_val_کامہ(pool<K> &v);
سانچا<typename K> ساكن مضمن فراغ log_dump_val_کامہ(std::سمتیہ<K> &v);
سانچا<typename T> ساكن مضمن فراغ log_dump_val_کامہ(T *ptr);

سانچا<typename K, typename T>
ساكن مضمن فراغ log_dump_val_کامہ(dict<K, T> &v) {
	لوگارتھم("{");
	bool پہلا = true;
	لكل (خودکار &it : v) {
		لوگارتھم("%s ", پہلا ? "" : ",");
		log_dump_val_کامہ(it.پہلا);
		لوگارتھم(": ");
		log_dump_val_کامہ(it.دوجا);
		پہلا = false;
	}
	لوگارتھم(" }");
}

سانچا<typename K>
ساكن مضمن فراغ log_dump_val_کامہ(pool<K> &v) {
	لوگارتھم("{");
	bool پہلا = true;
	لكل (خودکار &it : v) {
		لوگارتھم("%s ", پہلا ? "" : ",");
		log_dump_val_کامہ(it);
		پہلا = false;
	}
	لوگارتھم(" }");
}

سانچا<typename K>
ساكن مضمن فراغ log_dump_val_کامہ(std::سمتیہ<K> &v) {
	لوگارتھم("{");
	bool پہلا = true;
	لكل (خودکار &it : v) {
		لوگارتھم("%s ", پہلا ? "" : ",");
		log_dump_val_کامہ(it);
		پہلا = false;
	}
	لوگارتھم(" }");
}

سانچا<typename T>
ساكن مضمن فراغ log_dump_val_کامہ(T *ptr) { لوگارتھم("%p", ptr); }

سانچا<typename T, typename ... دلیلاں_3>
فراغ log_dump_دلیلاں_کامہ(ثابت حرف *p, T پہلا, دلیلاں_3 ... دلیلاں_2)
{
	صحيح اگلا_p_state = 0;
	ثابت حرف *اگلا_p = p;
	طالما (*اگلا_p && (اگلا_p_state != 0 || *اگلا_p != ',')) {
		إذا (*اگلا_p == '"')
			افعل {
				اگلا_p++;
				طالما (*اگلا_p == '\\' && *(اگلا_p + 1))
					اگلا_p += 2;
			} طالما (*اگلا_p && *اگلا_p != '"');
		إذا (*اگلا_p == '\'') {
			اگلا_p++;
			إذا (*اگلا_p == '\\')
				اگلا_p++;
			إذا (*اگلا_p)
				اگلا_p++;
		}
		إذا (*اگلا_p == '(' || *اگلا_p == '[' || *اگلا_p == '{')
			اگلا_p_state++;
		إذا ((*اگلا_p == ')' || *اگلا_p == ']' || *اگلا_p == '}') && اگلا_p_state > 0)
			اگلا_p_state--;
		اگلا_p++;
	}
	لوگارتھم("\n\t%.*s => ", صحيح(اگلا_p - p), p);
	إذا (*اگلا_p == ',')
		اگلا_p++;
	طالما (*اگلا_p == ' ' || *اگلا_p == '\t' || *اگلا_p == '\r' || *اگلا_p == '\n')
		اگلا_p++;
	log_dump_val_کامہ(پہلا);
	log_dump_دلیلاں_کامہ(اگلا_p, دلیلاں_2 ...);
}

#تعریف_کرو log_dump(...) افعل { \
	لوگارتھم("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_دلیلاں_کامہ(#__VA_ARGS__, __VA_ARGS__); \
	لوگارتھم("\n"); \
} طالما (0)

YOSYS_NAMESPACE_سرا

#ضمن "kernel/yosys.h"

#جے_انت

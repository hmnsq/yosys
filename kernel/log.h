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

#جيڪڏهن_وصف_نه_ٿيل LOG_H
#وصف_ڪريو LOG_H

#ضمن "kernel/yosys_common.h"

#ضمن <chrono>
#ضمن <time.h>

#ضمن <regex>
#وصف_ڪريو YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#وصف_ڪريو YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
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
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#إذا defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { إذا (IsDebuggerPresent()) DebugBreak(); } طالما(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING افعل { \
		خودڪار old = إشارة(SIGTRAP, SIG_IGN); ارفع(SIGTRAP); إشارة(SIGTRAP, old); \
	} طالما(0)
#وإلا
# define YS_DEBUGTRAP_IF_DEBUGGING افعل {} طالما(0)
#جيڪڏهن_پورو

بنية log_cmd_error_exception { };

تعداد class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	سرو,
	Warning,
	NonFatalError,
	Error
};

بنية Log_ذريعو_Location {
	Log_ذريعو_Location() : filename({}),شروعات_سٽ(-1),شروعات_کانو(-1),پڇاڙي_سٽ(-1),پڇاڙي_کانو(-1) { }
	Log_ذريعو_Location(std::string filename) : filename(filename),شروعات_سٽ(-1),شروعات_کانو(-1),پڇاڙي_سٽ(-1),پڇاڙي_کانو(-1) { }
	Log_ذريعو_Location(std::string filename,صحيح شروعات_سٽ) : filename(filename),شروعات_سٽ(شروعات_سٽ),شروعات_کانو(-1),پڇاڙي_سٽ(-1),پڇاڙي_کانو(-1) { }
	Log_ذريعو_Location(std::string filename,صحيح شروعات_سٽ,صحيح شروعات_کانو,صحيح پڇاڙي_سٽ,صحيح پڇاڙي_کانو) :
		filename(filename),شروعات_سٽ(شروعات_سٽ),شروعات_کانو(شروعات_کانو),پڇاڙي_سٽ(پڇاڙي_سٽ),پڇاڙي_کانو(پڇاڙي_کانو) { }
	std::string filename;
	صحيح شروعات_سٽ;
	صحيح شروعات_کانو;
	صحيح پڇاڙي_سٽ;
	صحيح پڇاڙي_کانو;
};

بنية Log_پيغام {
	Log_پيغام(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string_view پيغام);
	LogSeverity severity;
	std::string اڳياڙي_2;
	std::string format;
	std::string پيغام;
	std::chrono::steady_clock::وقت_نقطو timestamp;
	Log_ذريعو_Location src;

	std::string cached_نياپو;
};

class LogSink
{
عام:
	مجازي ~LogSink() = افتراضي;

	مجازي bool should_log(ثابت Log_پيغام &) ثابت { أرجع true; }
	مجازي فراغ لاگرٿم(ثابت Log_پيغام &نياپو) = 0;
	مجازي فراغ flush() {}
	// TODO: Remove when AST/read_verilog removed
	مجازي FILE *file_handle() { أرجع nullptr; }
};

class LogSinkRef : عام LogSink
{
عام:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ثابت Log_پيغام &نياپو) ثابت override { أرجع sink->should_log(نياپو); }
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override { sink->لاگرٿم(نياپو); }
	فراغ flush() override { sink->flush(); }
	FILE *file_handle() override { أرجع sink->file_handle(); }
خاص:
	LogSink *sink;
};

class FileLogSink : عام LogSink
{
عام:
	explicit FileLogSink(ثابت std::string &filename, bool سٽ_buffered, bool append);
	~FileLogSink() override;
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع file; }

خاص:
	FILE *file;
};

class ConsoleLogSink : عام LogSink
{
عام:
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override;
	فراغ flush() override;
	FILE *file_handle() override { أرجع stdout; }
};

class StderrLogSink : عام LogSink
{
عام:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ثابت Log_پيغام &نياپو) ثابت override;
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override;
	فراغ flush() override;
خاص:
	bool quiet_warnings;
};

class وهڪرو_Log_Sink : عام LogSink
{
عام:
	explicit وهڪرو_Log_Sink(std::ostream &وهڪرو) : وهڪرو(وهڪرو) {}
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override { وهڪرو << نياپو.cached_نياپو; }
	فراغ flush() override { وهڪرو.flush(); }

خاص:
	std::ostream &وهڪرو;
};

class ScratchPadLogSink : عام LogSink
{
عام:
	explicit ScratchPadLogSink(std::string scratchpad);
	فراغ لاگرٿم(ثابت Log_پيغام &نياپو) override;

خاص:
	std::string scratchpad;
};

class LogManager
{
خاص:
	بنية Log_Expected_شي
	{
		Log_Expected_شي(ثابت std::regex &pat, صحيح expected) :
				نمونو(pat), expected_ڳڻپ(expected), current_ڳڻپ(0) {}
		Log_Expected_شي() : expected_ڳڻپ(0), current_ڳڻپ(0) {}

		std::regex نمونو;
		صحيح expected_ڳڻپ;
		صحيح current_ڳڻپ;
	};

عام:
	LogManager() = افتراضي;

	سانچو<typename T, typename... دليلون_3>
	T &جوڙيو_sink(دليلون_3&&... دليلون_2)
	{
		خودڪار sink = std::make_unique<T>(std::forward<دليلون_3>(دليلون_2)...);
		T &ref = *sink;
		sinks.ڌڪو_پٺ(std::move(sink));
		أرجع ref;
	}

	سانچو<typename F>
	فراغ for_each_sink(F &&func)
	{
		لكل (خودڪار &sink : sinks)
			func(*sink);
	}

	bool خالي_جايون() { أرجع sinks.خالي_جايون(); }
	فراغ clear() { sinks.clear(); }
	فراغ clear_original();
	فراغ flush() { لكل (خودڪار &sink : sinks) sink->flush(); }

	class Scoped
	{
	عام:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.سائيز());

			لكل (ثابت خودڪار &sink : backup_sinks)
				manager.sinks.ڌڪو_پٺ(std::make_unique<LogSinkRef>(sink.get()));
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
		std::ويڪٽر<std::unique_ptr<LogSink>> backup_sinks;
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
			: manager(manager), اڳيون(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = اڳيون;
		}

	خاص:
		LogManager &manager;
		bool اڳيون;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		أرجع ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	عام:
		explicit LogMakeDebugHdl(LogManager &manager, bool شروعات_on = false)
			: manager(manager)
		{
			إذا (شروعات_on)
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

	LogMakeDebugHdl make_debug_scope(bool شروعات_on = false)
	{
		أرجع LogMakeDebugHdl(*this, شروعات_on);
	}

	class ForceDebug
	{
	عام:
		explicit ForceDebug(LogManager &manager, bool شروعات_on = false)
			: manager(manager)
		{
			إذا (شروعات_on)
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

	ForceDebug force_debug_scope(bool شروعات_on = false)
	{
		أرجع ForceDebug(*this, شروعات_on);
	}

	فراغ force_debug_on() { force_debug++; }
	فراغ force_debug_off() { إذا (force_debug > 0) force_debug--; }
	فراغ سيٽ_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	فراغ report_unexpected_error();

	فراغ جوڙيو_experimental_ignore(std::string name) { experimental_ignored.وجهو(name); }
	فراغ جوڙيو_warn(std::string نمونو) { warn_regexes.ڌڪو_پٺ(YS_REGEX_COMPILE(نمونو)); }
	فراغ جوڙيو_nowarn(std::string نمونو) { nowarn_regexes.ڌڪو_پٺ(YS_REGEX_COMPILE(نمونو)); }
	فراغ جوڙيو_werror(std::string نمونو) { werror_regexes.ڌڪو_پٺ(YS_REGEX_COMPILE(نمونو)); }
	فراغ جوڙيو_expect(std::string type, std::string نمونو, صحيح ڳڻپ);

	فراغ سيٽ_verbose_level(صحيح level) { verbose_level = level; }
	فراغ جوڙيو_verbose_level(صحيح level) { verbose_level += level; }
	فراغ سيٽ_expect_no_warnings(bool قدر) { expect_no_warnings = قدر; }
	فراغ سيٽ_log_وقت(bool قدر) { log_وقت = قدر; }
	فراغ سيٽ_cmd_error_throw(bool قدر) { cmd_error_throw = قدر; }
	فراغ سيٽ_hdump_all(bool قدر) { hdump_all = قدر; }
	صحيح get_verbose_level() ثابت { أرجع verbose_level; }
	bool get_log_وقت() ثابت { أرجع log_وقت; }
	صحيح get_warnings_unique() ثابت { أرجع Get_سائيز(warnings); }
	صحيح get_warnings_ڪل() ثابت { أرجع warnings_ڳڻپ; }
	صحيح get_errors_ڪل() ثابت { أرجع errors_ڳڻپ; }
	ثابت std::سيٽ<std::string> &get_experimental() ثابت { أرجع experimental; }
	ثابت std::map<std::string, std::string> &get_deprecated() ثابت { أرجع deprecated; }
	std::chrono::steady_clock::وقت_نقطو get_initial_وقت() ثابت;

	فراغ جوڙيو_hdump(std::string name, std::string قدر) { hdump[name].وجهو(قدر); }

	فراغ formatted_string(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string str);
	فراغ formatted_سرو(RTLIL::Design *design, std::string_view format, std::string str);
	فراغ formatted_warning(Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام);
	[[noreturn]] فراغ formatted_error(Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام);
	فراغ formatted_nonfatal_error(Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام);
	[[noreturn]] فراغ formatted_cmd_error(std::string_view format, std::string پيغام);
	فراغ suppressed();
	فراغ جوڙيو_experimental(ثابت std::string &str);
	فراغ جوڙيو_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal);
	فراغ spacer();
	فراغ ڌڪو();
	فراغ ڪڍو();

	فراغ reset_ڍير();

	فراغ check_expected();
	bool expects_error() { أرجع (expect_error.سائيز() + expect_اڳياڙي_error.سائيز())>0; }
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
	bool is_debug(صحيح n = 0) { إذا (force_debug) أرجع true; debug_suppressed += n; أرجع false; }
#وإلا
	bool is_debug(صحيح = 0) { أرجع false; }
#جيڪڏهن_پورو
	فراغ شروعات_hasher();
	std::string پڄاڻي_hasher();

خاص:
	فراغ logv_string(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string str_اندريون);
	فراغ error_with_اڳياڙي(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2, std::string_view format, std::string پيغام);

	std::ويڪٽر<std::unique_ptr<LogSink>> sinks;
	صحيح verbose_level = 0;
	صحيح newline_ڳڻپ = 0;
	ويڪٽر<صحيح> سرو_ڳڻپ;
	صحيح errors_ڳڻپ = 0;
	صحيح warnings_ڳڻپ = 0;
	صحيح warnings_ڳڻپ_noexpect = 0;
	std::سيٽ<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::ويڪٽر<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_شي> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_شي> expect_اڳياڙي_log, expect_اڳياڙي_warning, expect_اڳياڙي_error;
	bool expect_no_warnings = false;
	bool log_وقت = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::سيٽ<std::string>> hdump;
	bool hdump_all = false;

	صحيح debug_suppressed = 0;
	صحيح make_debug = 0;
	صحيح force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

خارجي فراغ (*log_error_atexit)();

فراغ سيٽ_verific_logging(فراغ (*cb)(صحيح نياپو_type, ثابت حرف *پيغام_id, Log_ذريعو_Location src, ثابت حرف *نياپو));
خارجي فراغ (*log_verific_callback)(صحيح نياپو_type, ثابت حرف *پيغام_id, Log_ذريعو_Location src, ثابت حرف *نياپو);

#جيڪڏهن_وصف_نه_ٿيل NDEBUG
ساكن مضمن bool ys_debug(صحيح n = 0) { أرجع logger().is_debug(n); }
#وإلا
ساكن مضمن bool ys_debug(صحيح = 0) { أرجع false; }
#جيڪڏهن_پورو

سانچو <typename... دليلون_3>
مضمن فراغ لاگرٿم(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_string(LogSeverity::Info, Log_ذريعو_Location{}, {}, fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
مضمن فراغ log_comment(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_ذريعو_Location{}, {}, fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
مضمن فراغ log_highlight(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_ذريعو_Location{}, {}, fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
مضمن فراغ log_formatted_string(LogSeverity severity, Log_ذريعو_Location src, std::string_view اڳياڙي_2,
		FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_string(severity, src, اڳياڙي_2, fmt.format_string(), fmt.format(دليلون_2...));
}

#وصف_ڪريو log_debug(...) افعل { إذا (ys_debug(1)) YOSYS_NAMESPACE_اڳياڙي log_formatted_string(YOSYS_NAMESPACE_اڳياڙي LogSeverity::Debug, {}, {}, __VA_ARGS__); } طالما (0)

سانچو <typename... دليلون_3>
مضمن فراغ log_سرو(RTLIL::Design *design, FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_سرو(design, fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
مضمن فراغ log_warning(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_warning(Log_ذريعو_Location{}, "Warning: ", fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
مضمن فراغ log_warning_noprefix(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_warning(Log_ذريعو_Location{}, {}, fmt.format_string(), fmt.format(دليلون_2...));
}

مضمن فراغ log_experimental(ثابت std::string &str)
{
	logger().جوڙيو_experimental(str);
}

مضمن فراغ log_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal = "")
{
	logger().جوڙيو_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
سانچو <typename... دليلون_3>
فراغ log_file_warning(Log_ذريعو_Location src, FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
فراغ log_file_info(Log_ذريعو_Location src, FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
[[noreturn]] فراغ log_error(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_error(Log_ذريعو_Location{}, "ERROR: ", fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
[[noreturn]] فراغ log_file_error(Log_ذريعو_Location src, FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
فراغ log_file_nonfatal_error(Log_ذريعو_Location src, FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(دليلون_2...));
}

سانچو <typename... دليلون_3>
[[noreturn]] فراغ log_cmd_error(FmtString<Type_سڃاڻپ_عنصر<دليلون_3>...> fmt, ثابت دليلون_3 &... دليلون_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(دليلون_2...));
}

مضمن فراغ log_suppressed()
{
	logger().suppressed();
}

مضمن فراغ log_spacer() { logger().spacer(); }
مضمن فراغ log_ڌڪو() { logger().ڌڪو(); }
مضمن فراغ log_ڪڍو() { logger().ڪڍو(); }

مضمن فراغ log_reset_ڍير() { logger().reset_ڍير(); }
مضمن فراغ log_flush() { logger().flush(); }

فراغ log_backtrace(ثابت حرف *اڳياڙي_2, صحيح سطحون);


std::string log_signal(ثابت RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ثابت RTLIL::Const &قدر, bool autoint = true);

فراغ log_module(RTLIL::Module *module, std::string indent = "");
فراغ log_cell(RTLIL::Cell *cell, std::string indent = "");
فراغ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
فراغ log_abort_اندروني(ثابت حرف *file, صحيح سٽ);
#وصف_ڪريو log_abort() YOSYS_NAMESPACE_اڳياڙي log_abort_اندروني(__FILE__, __LINE__)
#وصف_ڪريو log_ping() YOSYS_NAMESPACE_اڳياڙي لاگرٿم("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
بنية PerformanceTimer
{
#إذا 1
	int64_t ڪل_ns;

	PerformanceTimer() {
		ڪل_ns = 0;
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
		ڪل_ns = 0;
	}

	فراغ begin() {
		ڪل_ns -= query();
	}

	فراغ پڇاڙي() {
		ڪل_ns += query();
	}

	عائم sec() ثابت {
		أرجع ڪل_ns * 1e-9f;
	}
#وإلا
	ساكن int64_t query() { أرجع 0; }
	فراغ reset() { }
	فراغ begin() { }
	فراغ پڇاڙي() { }
	عائم sec() ثابت { أرجع 0; }
#جيڪڏهن_پورو
};

// simple API for quickly dumping values when debugging

ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(ننڍو v) { لاگرٿم("%d", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(بنانشان ننڍو v) { لاگرٿم("%u", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(صحيح v) { لاگرٿم("%d", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(بنانشان صحيح v) { لاگرٿم("%u", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(ڊگهو صحيح v) { لاگرٿم("%ld", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(بنانشان ڊگهو صحيح v) { لاگرٿم("%lu", v); }
#جيڪڏهن_وصف_نه_ٿيل _WIN32
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(ڊگهو ڊگهو صحيح v) { لاگرٿم("%lld", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(بنانشان ڊگهو ڊگهو صحيح v) { لاگرٿم("%lld", v); }
#جيڪڏهن_پورو
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(حرف c)
{
	إذا (c >= 32 && c < 127) {
		لاگرٿم("'%c'", c);
	} وإلا {
		لاگرٿم("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(بنانشان حرف c)
{
	إذا (c >= 32 && c < 127) {
		لاگرٿم("'%c'", c);
	} وإلا {
		لاگرٿم("'\\x%02x'", c);
	}
}
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(bool v) { لاگرٿم("%s", v ? "true" : "false"); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(مزدوج v) { لاگرٿم("%f", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(حرف *v) { لاگرٿم("%s", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(ثابت حرف *v) { لاگرٿم("%s", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(std::string v) { لاگرٿم("%s", v); }
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(PerformanceTimer p) { لاگرٿم("%f seconds", p.sec()); }
ساكن مضمن فراغ log_dump_دليلون_ڪم_ڪندڙ(ثابت حرف *p) { log_assert(*p == 0); }
فراغ log_dump_val_ڪم_ڪندڙ(IdString v);
فراغ log_dump_val_ڪم_ڪندڙ(RTLIL::SigSpec v);
فراغ log_dump_val_ڪم_ڪندڙ(RTLIL::State v);

سانچو<typename K, typename T> ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(dict<K, T> &v);
سانچو<typename K> ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(pool<K> &v);
سانچو<typename K> ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(std::ويڪٽر<K> &v);
سانچو<typename T> ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(T *ptr);

سانچو<typename K, typename T>
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(dict<K, T> &v) {
	لاگرٿم("{");
	bool پهريون = true;
	لكل (خودڪار &it : v) {
		لاگرٿم("%s ", پهريون ? "" : ",");
		log_dump_val_ڪم_ڪندڙ(it.پهريون);
		لاگرٿم(": ");
		log_dump_val_ڪم_ڪندڙ(it.ٻيو);
		پهريون = false;
	}
	لاگرٿم(" }");
}

سانچو<typename K>
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(pool<K> &v) {
	لاگرٿم("{");
	bool پهريون = true;
	لكل (خودڪار &it : v) {
		لاگرٿم("%s ", پهريون ? "" : ",");
		log_dump_val_ڪم_ڪندڙ(it);
		پهريون = false;
	}
	لاگرٿم(" }");
}

سانچو<typename K>
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(std::ويڪٽر<K> &v) {
	لاگرٿم("{");
	bool پهريون = true;
	لكل (خودڪار &it : v) {
		لاگرٿم("%s ", پهريون ? "" : ",");
		log_dump_val_ڪم_ڪندڙ(it);
		پهريون = false;
	}
	لاگرٿم(" }");
}

سانچو<typename T>
ساكن مضمن فراغ log_dump_val_ڪم_ڪندڙ(T *ptr) { لاگرٿم("%p", ptr); }

سانچو<typename T, typename ... دليلون_3>
فراغ log_dump_دليلون_ڪم_ڪندڙ(ثابت حرف *p, T پهريون, دليلون_3 ... دليلون_2)
{
	صحيح ايندڙ_p_state = 0;
	ثابت حرف *ايندڙ_p = p;
	طالما (*ايندڙ_p && (ايندڙ_p_state != 0 || *ايندڙ_p != ',')) {
		إذا (*ايندڙ_p == '"')
			افعل {
				ايندڙ_p++;
				طالما (*ايندڙ_p == '\\' && *(ايندڙ_p + 1))
					ايندڙ_p += 2;
			} طالما (*ايندڙ_p && *ايندڙ_p != '"');
		إذا (*ايندڙ_p == '\'') {
			ايندڙ_p++;
			إذا (*ايندڙ_p == '\\')
				ايندڙ_p++;
			إذا (*ايندڙ_p)
				ايندڙ_p++;
		}
		إذا (*ايندڙ_p == '(' || *ايندڙ_p == '[' || *ايندڙ_p == '{')
			ايندڙ_p_state++;
		إذا ((*ايندڙ_p == ')' || *ايندڙ_p == ']' || *ايندڙ_p == '}') && ايندڙ_p_state > 0)
			ايندڙ_p_state--;
		ايندڙ_p++;
	}
	لاگرٿم("\n\t%.*s => ", صحيح(ايندڙ_p - p), p);
	إذا (*ايندڙ_p == ',')
		ايندڙ_p++;
	طالما (*ايندڙ_p == ' ' || *ايندڙ_p == '\t' || *ايندڙ_p == '\r' || *ايندڙ_p == '\n')
		ايندڙ_p++;
	log_dump_val_ڪم_ڪندڙ(پهريون);
	log_dump_دليلون_ڪم_ڪندڙ(ايندڙ_p, دليلون_2 ...);
}

#وصف_ڪريو log_dump(...) افعل { \
	لاگرٿم("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_دليلون_ڪم_ڪندڙ(#__VA_ARGS__, __VA_ARGS__); \
	لاگرٿم("\n"); \
} طالما (0)

YOSYS_NAMESPACE_پڇاڙي

#ضمن "kernel/yosys.h"

#جيڪڏهن_پورو

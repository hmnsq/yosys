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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <chrono>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <regex>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#ਜੇ defined(_WIN32)
#  include <intrin.h>
#ਨਹੀਂਤਾਂ
#  include <sys/time.h>
#  include <sys/resource.h>
#  ਜੇ defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <signal.h>
#  endif
#ਜੇ_ਅੰਤ

// from libs/sha1/sha1.h
class SHA1;

YOSYS_NAMESPACE_BEGIN

// YS_DEBUGTRAP is a macro that is functionally equivalent to a breakpoint
// if the platform provides such functionality, and does nothing otherwise.
// If no debugger is attached, it starts a just-in-time debugger if available,
// and crashes the process otherwise.
#ਜੇ defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#ਨਹੀਂਤਾਂ
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# ਜੇ __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP ਉਠਾਓ(SIGTRAP)
# ਨਹੀਂਤਾਂ
#  define YS_DEBUGTRAP ਕਰੋ {} ਜਦੋਂ(0)
# endif
#ਜੇ_ਅੰਤ

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#ਜੇ defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING ਕਰੋ { ਜੇ (IsDebuggerPresent()) DebugBreak(); } ਜਦੋਂ(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING ਕਰੋ { \
		ਸਵੈਚਾਲਿਤ old = ਸੰਕੇਤ(SIGTRAP, SIG_IGN); ਉਠਾਓ(SIGTRAP); ਸੰਕੇਤ(SIGTRAP, old); \
	} ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
# define YS_DEBUGTRAP_IF_DEBUGGING ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

ਬਣਤਰ log_cmd_error_exception { };

ਗਿਣਤੀ class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	ਸਿਰਲੇਖ,
	Warning,
	NonFatalError,
	Error
};

ਬਣਤਰ Log_ਸਰੋਤ_Location {
	Log_ਸਰੋਤ_Location() : filename({}),ਸ਼ੁਰੂ_ਸਤਰ(-1),ਸ਼ੁਰੂ_ਖਾਨਾ(-1),ਸਿਰਾ_ਸਤਰ(-1),ਸਿਰਾ_ਖਾਨਾ(-1) { }
	Log_ਸਰੋਤ_Location(std::string filename) : filename(filename),ਸ਼ੁਰੂ_ਸਤਰ(-1),ਸ਼ੁਰੂ_ਖਾਨਾ(-1),ਸਿਰਾ_ਸਤਰ(-1),ਸਿਰਾ_ਖਾਨਾ(-1) { }
	Log_ਸਰੋਤ_Location(std::string filename,ਪੂਰਨ ਸ਼ੁਰੂ_ਸਤਰ) : filename(filename),ਸ਼ੁਰੂ_ਸਤਰ(ਸ਼ੁਰੂ_ਸਤਰ),ਸ਼ੁਰੂ_ਖਾਨਾ(-1),ਸਿਰਾ_ਸਤਰ(-1),ਸਿਰਾ_ਖਾਨਾ(-1) { }
	Log_ਸਰੋਤ_Location(std::string filename,ਪੂਰਨ ਸ਼ੁਰੂ_ਸਤਰ,ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ,ਪੂਰਨ ਸਿਰਾ_ਸਤਰ,ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ) :
		filename(filename),ਸ਼ੁਰੂ_ਸਤਰ(ਸ਼ੁਰੂ_ਸਤਰ),ਸ਼ੁਰੂ_ਖਾਨਾ(ਸ਼ੁਰੂ_ਖਾਨਾ),ਸਿਰਾ_ਸਤਰ(ਸਿਰਾ_ਸਤਰ),ਸਿਰਾ_ਖਾਨਾ(ਸਿਰਾ_ਖਾਨਾ) { }
	std::string filename;
	ਪੂਰਨ ਸ਼ੁਰੂ_ਸਤਰ;
	ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ;
	ਪੂਰਨ ਸਿਰਾ_ਸਤਰ;
	ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ;
};

ਬਣਤਰ Log_ਸੁਨੇਹਾ {
	Log_ਸੁਨੇਹਾ(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string_view ਸੁਨੇਹਾ_2);
	LogSeverity severity;
	std::string ਅਗੇਤਰ_2;
	std::string format;
	std::string ਸੁਨੇਹਾ_2;
	std::chrono::steady_clock::ਸਮਾਂ_ਬਿੰਦੂ timestamp;
	Log_ਸਰੋਤ_Location src;

	std::string cached_ਸੁਨੇਹਾ;
};

class LogSink
{
ਜਨਤਕ:
	ਆਭਾਸੀ ~LogSink() = ਮੂਲ;

	ਆਭਾਸੀ bool should_log(ਅਚਲ Log_ਸੁਨੇਹਾ &) ਅਚਲ { ਵਾਪਸ true; }
	ਆਭਾਸੀ ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) = 0;
	ਆਭਾਸੀ ਖਾਲੀ flush() {}
	// TODO: Remove when AST/read_verilog removed
	ਆਭਾਸੀ FILE *file_handle() { ਵਾਪਸ nullptr; }
};

class LogSinkRef : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) ਅਚਲ override { ਵਾਪਸ sink->should_log(ਸੁਨੇਹਾ); }
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override { sink->ਲਘੂਗਣਕ(ਸੁਨੇਹਾ); }
	ਖਾਲੀ flush() override { sink->flush(); }
	FILE *file_handle() override { ਵਾਪਸ sink->file_handle(); }
ਨਿਜੀ:
	LogSink *sink;
};

class FileLogSink : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	explicit FileLogSink(ਅਚਲ std::string &filename, bool ਸਤਰ_buffered, bool append);
	~FileLogSink() override;
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override;
	ਖਾਲੀ flush() override;
	FILE *file_handle() override { ਵਾਪਸ file; }

ਨਿਜੀ:
	FILE *file;
};

class ConsoleLogSink : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override;
	ਖਾਲੀ flush() override;
	FILE *file_handle() override { ਵਾਪਸ stdout; }
};

class StderrLogSink : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) ਅਚਲ override;
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override;
	ਖਾਲੀ flush() override;
ਨਿਜੀ:
	bool quiet_warnings;
};

class ਧਾਰਾ_Log_Sink : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	explicit ਧਾਰਾ_Log_Sink(std::ostream &ਧਾਰਾ) : ਧਾਰਾ(ਧਾਰਾ) {}
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override { ਧਾਰਾ << ਸੁਨੇਹਾ.cached_ਸੁਨੇਹਾ; }
	ਖਾਲੀ flush() override { ਧਾਰਾ.flush(); }

ਨਿਜੀ:
	std::ostream &ਧਾਰਾ;
};

class ScratchPadLogSink : ਜਨਤਕ LogSink
{
ਜਨਤਕ:
	explicit ScratchPadLogSink(std::string scratchpad);
	ਖਾਲੀ ਲਘੂਗਣਕ(ਅਚਲ Log_ਸੁਨੇਹਾ &ਸੁਨੇਹਾ) override;

ਨਿਜੀ:
	std::string scratchpad;
};

class LogManager
{
ਨਿਜੀ:
	ਬਣਤਰ Log_Expected_ਵਸਤੂ
	{
		Log_Expected_ਵਸਤੂ(ਅਚਲ std::regex &pat, ਪੂਰਨ expected) :
				ਨਮੂਨਾ(pat), expected_ਗਿਣਤੀ(expected), current_ਗਿਣਤੀ(0) {}
		Log_Expected_ਵਸਤੂ() : expected_ਗਿਣਤੀ(0), current_ਗਿਣਤੀ(0) {}

		std::regex ਨਮੂਨਾ;
		ਪੂਰਨ expected_ਗਿਣਤੀ;
		ਪੂਰਨ current_ਗਿਣਤੀ;
	};

ਜਨਤਕ:
	LogManager() = ਮੂਲ;

	ਸਾਂਚਾ<typename T, typename... ਦਲੀਲਾਂ_3>
	T &ਜੋੜੋ_sink(ਦਲੀਲਾਂ_3&&... ਦਲੀਲਾਂ_2)
	{
		ਸਵੈਚਾਲਿਤ sink = std::make_unique<T>(std::forward<ਦਲੀਲਾਂ_3>(ਦਲੀਲਾਂ_2)...);
		T &ref = *sink;
		sinks.ਧੱਕੋ_ਪਿੱਠ(std::move(sink));
		ਵਾਪਸ ref;
	}

	ਸਾਂਚਾ<typename F>
	ਖਾਲੀ for_each_sink(F &&func)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &sink : sinks)
			func(*sink);
	}

	bool ਖਾਲੀ_ਥਾਵਾਂ() { ਵਾਪਸ sinks.ਖਾਲੀ_ਥਾਵਾਂ(); }
	ਖਾਲੀ clear() { sinks.clear(); }
	ਖਾਲੀ clear_original();
	ਖਾਲੀ flush() { ਲਈ (ਸਵੈਚਾਲਿਤ &sink : sinks) sink->flush(); }

	class Scoped
	{
	ਜਨਤਕ:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.ਮਾਪ());

			ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &sink : backup_sinks)
				manager.sinks.ਧੱਕੋ_ਪਿੱਠ(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ਅਚਲ Scoped &) = delete;
		Scoped &ਸੰਚਾਲਕ=(ਅਚਲ Scoped &) = delete;

	ਨਿਜੀ:
		LogManager &manager;
		std::ਸਦਿਸ਼<std::unique_ptr<LogSink>> backup_sinks;
		ਪੂਰਨ backup_verbose_level;
	};

	Scoped sink_scope()
	{
		ਵਾਪਸ Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	ਜਨਤਕ:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), ਪਿਛਲਾ(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = ਪਿਛਲਾ;
		}

	ਨਿਜੀ:
		LogManager &manager;
		bool ਪਿਛਲਾ;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		ਵਾਪਸ ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	ਜਨਤਕ:
		explicit LogMakeDebugHdl(LogManager &manager, bool ਸ਼ੁਰੂ_on = false)
			: manager(manager)
		{
			ਜੇ (ਸ਼ੁਰੂ_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		ਖਾਲੀ on()
		{
			ਜੇ (ਸਥਿਤੀ)
				ਵਾਪਸ;
			ਸਥਿਤੀ = true;
			manager.make_debug++;
		}

		ਖਾਲੀ off_silent()
		{
			ਜੇ (!ਸਥਿਤੀ)
				ਵਾਪਸ;
			ਸਥਿਤੀ = false;
			manager.make_debug--;
		}

		ਖਾਲੀ off()
		{
			off_silent();
		}
	ਨਿਜੀ:
		LogManager &manager;
		bool ਸਥਿਤੀ = false;
	};

	LogMakeDebugHdl make_debug_scope(bool ਸ਼ੁਰੂ_on = false)
	{
		ਵਾਪਸ LogMakeDebugHdl(*this, ਸ਼ੁਰੂ_on);
	}

	class ForceDebug
	{
	ਜਨਤਕ:
		explicit ForceDebug(LogManager &manager, bool ਸ਼ੁਰੂ_on = false)
			: manager(manager)
		{
			ਜੇ (ਸ਼ੁਰੂ_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		ਖਾਲੀ on()
		{
			ਜੇ (ਸਰਗਰਮ)
				ਵਾਪਸ;

			ਸਰਗਰਮ = true;
			manager.force_debug++;
		}

		ਖਾਲੀ off()
		{
			ਜੇ (!ਸਰਗਰਮ)
				ਵਾਪਸ;

			ਸਰਗਰਮ = false;
			manager.force_debug--;
		}
	ਨਿਜੀ:
		LogManager &manager;
		bool ਸਰਗਰਮ = false;
	};

	ForceDebug force_debug_scope(bool ਸ਼ੁਰੂ_on = false)
	{
		ਵਾਪਸ ForceDebug(*this, ਸ਼ੁਰੂ_on);
	}

	ਖਾਲੀ force_debug_on() { force_debug++; }
	ਖਾਲੀ force_debug_off() { ਜੇ (force_debug > 0) force_debug--; }
	ਖਾਲੀ ਸੈੱਟ_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	ਖਾਲੀ report_unexpected_error();

	ਖਾਲੀ ਜੋੜੋ_experimental_ignore(std::string name) { experimental_ignored.ਪਾਓ(name); }
	ਖਾਲੀ ਜੋੜੋ_warn(std::string ਨਮੂਨਾ) { warn_regexes.ਧੱਕੋ_ਪਿੱਠ(YS_REGEX_COMPILE(ਨਮੂਨਾ)); }
	ਖਾਲੀ ਜੋੜੋ_nowarn(std::string ਨਮੂਨਾ) { nowarn_regexes.ਧੱਕੋ_ਪਿੱਠ(YS_REGEX_COMPILE(ਨਮੂਨਾ)); }
	ਖਾਲੀ ਜੋੜੋ_werror(std::string ਨਮੂਨਾ) { werror_regexes.ਧੱਕੋ_ਪਿੱਠ(YS_REGEX_COMPILE(ਨਮੂਨਾ)); }
	ਖਾਲੀ ਜੋੜੋ_expect(std::string type, std::string ਨਮੂਨਾ, ਪੂਰਨ ਗਿਣਤੀ_2);

	ਖਾਲੀ ਸੈੱਟ_verbose_level(ਪੂਰਨ level) { verbose_level = level; }
	ਖਾਲੀ ਜੋੜੋ_verbose_level(ਪੂਰਨ level) { verbose_level += level; }
	ਖਾਲੀ ਸੈੱਟ_expect_no_warnings(bool ਮੁੱਲ) { expect_no_warnings = ਮੁੱਲ; }
	ਖਾਲੀ ਸੈੱਟ_log_ਸਮਾਂ(bool ਮੁੱਲ) { log_ਸਮਾਂ = ਮੁੱਲ; }
	ਖਾਲੀ ਸੈੱਟ_cmd_error_throw(bool ਮੁੱਲ) { cmd_error_throw = ਮੁੱਲ; }
	ਖਾਲੀ ਸੈੱਟ_hdump_all(bool ਮੁੱਲ) { hdump_all = ਮੁੱਲ; }
	ਪੂਰਨ get_verbose_level() ਅਚਲ { ਵਾਪਸ verbose_level; }
	bool get_log_ਸਮਾਂ() ਅਚਲ { ਵਾਪਸ log_ਸਮਾਂ; }
	ਪੂਰਨ get_warnings_unique() ਅਚਲ { ਵਾਪਸ Get_ਮਾਪ(warnings); }
	ਪੂਰਨ get_warnings_ਕੁੱਲ() ਅਚਲ { ਵਾਪਸ warnings_ਗਿਣਤੀ; }
	ਪੂਰਨ get_errors_ਕੁੱਲ() ਅਚਲ { ਵਾਪਸ errors_ਗਿਣਤੀ; }
	ਅਚਲ std::ਸੈੱਟ<std::string> &get_experimental() ਅਚਲ { ਵਾਪਸ experimental; }
	ਅਚਲ std::map<std::string, std::string> &get_deprecated() ਅਚਲ { ਵਾਪਸ deprecated; }
	std::chrono::steady_clock::ਸਮਾਂ_ਬਿੰਦੂ get_initial_ਸਮਾਂ() ਅਚਲ;

	ਖਾਲੀ ਜੋੜੋ_hdump(std::string name, std::string ਮੁੱਲ) { hdump[name].ਪਾਓ(ਮੁੱਲ); }

	ਖਾਲੀ formatted_string(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string str);
	ਖਾਲੀ formatted_ਸਿਰਲੇਖ(RTLIL::Design *design, std::string_view format, std::string str);
	ਖਾਲੀ formatted_warning(Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2);
	[[noreturn]] ਖਾਲੀ formatted_error(Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2);
	ਖਾਲੀ formatted_nonfatal_error(Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2);
	[[noreturn]] ਖਾਲੀ formatted_cmd_error(std::string_view format, std::string ਸੁਨੇਹਾ_2);
	ਖਾਲੀ suppressed();
	ਖਾਲੀ ਜੋੜੋ_experimental(ਅਚਲ std::string &str);
	ਖਾਲੀ ਜੋੜੋ_deprecated(ਅਚਲ std::string &feature, ਅਚਲ std::string &earliest_removal);
	ਖਾਲੀ spacer();
	ਖਾਲੀ ਧੱਕੋ();
	ਖਾਲੀ ਕੱਢੋ();

	ਖਾਲੀ reset_ਢੇਰ();

	ਖਾਲੀ check_expected();
	bool expects_error() { ਵਾਪਸ (expect_error.ਮਾਪ() + expect_ਅਗੇਤਰ_error.ਮਾਪ())>0; }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
	bool is_debug(ਪੂਰਨ n = 0) { ਜੇ (force_debug) ਵਾਪਸ true; debug_suppressed += n; ਵਾਪਸ false; }
#ਨਹੀਂਤਾਂ
	bool is_debug(ਪੂਰਨ = 0) { ਵਾਪਸ false; }
#ਜੇ_ਅੰਤ
	ਖਾਲੀ ਸ਼ੁਰੂ_hasher();
	std::string ਅੰਤ_hasher();

ਨਿਜੀ:
	ਖਾਲੀ logv_string(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string str_ਵਿੱਚ);
	ਖਾਲੀ error_with_ਅਗੇਤਰ(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2, std::string_view format, std::string ਸੁਨੇਹਾ_2);

	std::ਸਦਿਸ਼<std::unique_ptr<LogSink>> sinks;
	ਪੂਰਨ verbose_level = 0;
	ਪੂਰਨ newline_ਗਿਣਤੀ = 0;
	ਸਦਿਸ਼<ਪੂਰਨ> ਸਿਰਲੇਖ_ਗਿਣਤੀ;
	ਪੂਰਨ errors_ਗਿਣਤੀ = 0;
	ਪੂਰਨ warnings_ਗਿਣਤੀ = 0;
	ਪੂਰਨ warnings_ਗਿਣਤੀ_noexpect = 0;
	std::ਸੈੱਟ<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::ਸਦਿਸ਼<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_ਵਸਤੂ> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_ਵਸਤੂ> expect_ਅਗੇਤਰ_log, expect_ਅਗੇਤਰ_warning, expect_ਅਗੇਤਰ_error;
	bool expect_no_warnings = false;
	bool log_ਸਮਾਂ = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::ਸੈੱਟ<std::string>> hdump;
	bool hdump_all = false;

	ਪੂਰਨ debug_suppressed = 0;
	ਪੂਰਨ make_debug = 0;
	ਪੂਰਨ force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

ਬਾਹਰੀ ਖਾਲੀ (*log_error_atexit)();

ਖਾਲੀ ਸੈੱਟ_verific_logging(ਖਾਲੀ (*cb)(ਪੂਰਨ ਸੁਨੇਹਾ_type, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_id, Log_ਸਰੋਤ_Location src, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ));
ਬਾਹਰੀ ਖਾਲੀ (*log_verific_callback)(ਪੂਰਨ ਸੁਨੇਹਾ_type, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_id, Log_ਸਰੋਤ_Location src, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ys_debug(ਪੂਰਨ n = 0) { ਵਾਪਸ logger().is_debug(n); }
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ys_debug(ਪੂਰਨ = 0) { ਵਾਪਸ false; }
#ਜੇ_ਅੰਤ

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ ਲਘੂਗਣਕ(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_string(LogSeverity::Info, Log_ਸਰੋਤ_Location{}, {}, fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_comment(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_ਸਰੋਤ_Location{}, {}, fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_highlight(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_ਸਰੋਤ_Location{}, {}, fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_formatted_string(LogSeverity severity, Log_ਸਰੋਤ_Location src, std::string_view ਅਗੇਤਰ_2,
		FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_string(severity, src, ਅਗੇਤਰ_2, fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_debug(...) ਕਰੋ { ਜੇ (ys_debug(1)) YOSYS_NAMESPACE_ਅਗੇਤਰ log_formatted_string(YOSYS_NAMESPACE_ਅਗੇਤਰ LogSeverity::Debug, {}, {}, __VA_ARGS__); } ਜਦੋਂ (0)

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_ਸਿਰਲੇਖ(RTLIL::Design *design, FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_ਸਿਰਲੇਖ(design, fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_warning(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_warning(Log_ਸਰੋਤ_Location{}, "Warning: ", fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਸਤਰਅੰਦਰ ਖਾਲੀ log_warning_noprefix(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_warning(Log_ਸਰੋਤ_Location{}, {}, fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਤਰਅੰਦਰ ਖਾਲੀ log_experimental(ਅਚਲ std::string &str)
{
	logger().ਜੋੜੋ_experimental(str);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ log_deprecated(ਅਚਲ std::string &feature, ਅਚਲ std::string &earliest_removal = "")
{
	logger().ਜੋੜੋ_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਖਾਲੀ log_file_warning(Log_ਸਰੋਤ_Location src, FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਖਾਲੀ log_file_info(Log_ਸਰੋਤ_Location src, FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
[[noreturn]] ਖਾਲੀ log_error(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_error(Log_ਸਰੋਤ_Location{}, "ERROR: ", fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
[[noreturn]] ਖਾਲੀ log_file_error(Log_ਸਰੋਤ_Location src, FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
ਖਾਲੀ log_file_nonfatal_error(Log_ਸਰੋਤ_Location src, FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ_3>
[[noreturn]] ਖਾਲੀ log_cmd_error(FmtString<Type_ਪਛਾਣ_ਤੱਤ<ਦਲੀਲਾਂ_3>...> fmt, ਅਚਲ ਦਲੀਲਾਂ_3 &... ਦਲੀਲਾਂ_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(ਦਲੀਲਾਂ_2...));
}

ਸਤਰਅੰਦਰ ਖਾਲੀ log_suppressed()
{
	logger().suppressed();
}

ਸਤਰਅੰਦਰ ਖਾਲੀ log_spacer() { logger().spacer(); }
ਸਤਰਅੰਦਰ ਖਾਲੀ log_ਧੱਕੋ() { logger().ਧੱਕੋ(); }
ਸਤਰਅੰਦਰ ਖਾਲੀ log_ਕੱਢੋ() { logger().ਕੱਢੋ(); }

ਸਤਰਅੰਦਰ ਖਾਲੀ log_reset_ਢੇਰ() { logger().reset_ਢੇਰ(); }
ਸਤਰਅੰਦਰ ਖਾਲੀ log_flush() { logger().flush(); }

ਖਾਲੀ log_backtrace(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ_2, ਪੂਰਨ ਪੱਧਰ);


std::string log_signal(ਅਚਲ RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ਅਚਲ RTLIL::Const &ਮੁੱਲ, bool autoint = true);

ਖਾਲੀ log_module(RTLIL::Module *module, std::string indent = "");
ਖਾਲੀ log_cell(RTLIL::Cell *cell, std::string indent = "");
ਖਾਲੀ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
ਖਾਲੀ log_abort_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_abort() YOSYS_NAMESPACE_ਅਗੇਤਰ log_abort_ਅੰਦਰੂਨੀ(__FILE__, __LINE__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ping() YOSYS_NAMESPACE_ਅਗੇਤਰ ਲਘੂਗਣਕ("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
ਬਣਤਰ PerformanceTimer
{
#ਜੇ 1
	int64_t ਕੁੱਲ_ns;

	PerformanceTimer() {
		ਕੁੱਲ_ns = 0;
	}

	ਸਥਿਰ int64_t query() {
#  ifdef _WIN32
		ਵਾਪਸ 0;
#  elif defined(RUSAGE_SELF)
		ਬਣਤਰ rusage rusage;
		int64_t t = 0;
		ਲਈ (ਪੂਰਨ who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			ਜੇ (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		ਵਾਪਸ t;
#  ਨਹੀਂਤਾਂ
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	ਖਾਲੀ reset() {
		ਕੁੱਲ_ns = 0;
	}

	ਖਾਲੀ begin() {
		ਕੁੱਲ_ns -= query();
	}

	ਖਾਲੀ ਸਿਰਾ() {
		ਕੁੱਲ_ns += query();
	}

	ਦਸ਼ਮਲਵ sec() ਅਚਲ {
		ਵਾਪਸ ਕੁੱਲ_ns * 1e-9f;
	}
#ਨਹੀਂਤਾਂ
	ਸਥਿਰ int64_t query() { ਵਾਪਸ 0; }
	ਖਾਲੀ reset() { }
	ਖਾਲੀ begin() { }
	ਖਾਲੀ ਸਿਰਾ() { }
	ਦਸ਼ਮਲਵ sec() ਅਚਲ { ਵਾਪਸ 0; }
#ਜੇ_ਅੰਤ
};

// simple API for quickly dumping values when debugging

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਛੋਟਾ v) { ਲਘੂਗਣਕ("%d", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ v) { ਲਘੂਗਣਕ("%u", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਪੂਰਨ v) { ਲਘੂਗਣਕ("%d", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ v) { ਲਘੂਗਣਕ("%u", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਲੰਮਾ ਪੂਰਨ v) { ਲਘੂਗਣਕ("%ld", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਪੂਰਨ v) { ਲਘੂਗਣਕ("%lu", v); }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਲੰਮਾ ਲੰਮਾ ਪੂਰਨ v) { ਲਘੂਗਣਕ("%lld", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ਪੂਰਨ v) { ਲਘੂਗਣਕ("%lld", v); }
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਅੱਖਰ c)
{
	ਜੇ (c >= 32 && c < 127) {
		ਲਘੂਗਣਕ("'%c'", c);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("'\\x%02x'", c);
	}
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
	ਜੇ (c >= 32 && c < 127) {
		ਲਘੂਗਣਕ("'%c'", c);
	} ਨਹੀਂਤਾਂ {
		ਲਘੂਗਣਕ("'\\x%02x'", c);
	}
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(bool v) { ਲਘੂਗਣਕ("%s", v ? "true" : "false"); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਦੁੱਗਣਾ v) { ਲਘੂਗਣਕ("%f", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਅੱਖਰ *v) { ਲਘੂਗਣਕ("%s", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(ਅਚਲ ਅੱਖਰ *v) { ਲਘੂਗਣਕ("%s", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(std::string v) { ਲਘੂਗਣਕ("%s", v); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(PerformanceTimer p) { ਲਘੂਗਣਕ("%f seconds", p.sec()); }
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_ਦਲੀਲਾਂ_ਕਾਮਾ(ਅਚਲ ਅੱਖਰ *p) { log_assert(*p == 0); }
ਖਾਲੀ log_dump_val_ਕਾਮਾ(IdString v);
ਖਾਲੀ log_dump_val_ਕਾਮਾ(RTLIL::SigSpec v);
ਖਾਲੀ log_dump_val_ਕਾਮਾ(RTLIL::State v);

ਸਾਂਚਾ<typename K, typename T> ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(dict<K, T> &v);
ਸਾਂਚਾ<typename K> ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(pool<K> &v);
ਸਾਂਚਾ<typename K> ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(std::ਸਦਿਸ਼<K> &v);
ਸਾਂਚਾ<typename T> ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(T *ptr);

ਸਾਂਚਾ<typename K, typename T>
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(dict<K, T> &v) {
	ਲਘੂਗਣਕ("{");
	bool ਪਹਿਲਾ = true;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : v) {
		ਲਘੂਗਣਕ("%s ", ਪਹਿਲਾ ? "" : ",");
		log_dump_val_ਕਾਮਾ(it.ਪਹਿਲਾ);
		ਲਘੂਗਣਕ(": ");
		log_dump_val_ਕਾਮਾ(it.ਦੂਜਾ);
		ਪਹਿਲਾ = false;
	}
	ਲਘੂਗਣਕ(" }");
}

ਸਾਂਚਾ<typename K>
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(pool<K> &v) {
	ਲਘੂਗਣਕ("{");
	bool ਪਹਿਲਾ = true;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : v) {
		ਲਘੂਗਣਕ("%s ", ਪਹਿਲਾ ? "" : ",");
		log_dump_val_ਕਾਮਾ(it);
		ਪਹਿਲਾ = false;
	}
	ਲਘੂਗਣਕ(" }");
}

ਸਾਂਚਾ<typename K>
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(std::ਸਦਿਸ਼<K> &v) {
	ਲਘੂਗਣਕ("{");
	bool ਪਹਿਲਾ = true;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : v) {
		ਲਘੂਗਣਕ("%s ", ਪਹਿਲਾ ? "" : ",");
		log_dump_val_ਕਾਮਾ(it);
		ਪਹਿਲਾ = false;
	}
	ਲਘੂਗਣਕ(" }");
}

ਸਾਂਚਾ<typename T>
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ log_dump_val_ਕਾਮਾ(T *ptr) { ਲਘੂਗਣਕ("%p", ptr); }

ਸਾਂਚਾ<typename T, typename ... ਦਲੀਲਾਂ_3>
ਖਾਲੀ log_dump_ਦਲੀਲਾਂ_ਕਾਮਾ(ਅਚਲ ਅੱਖਰ *p, T ਪਹਿਲਾ, ਦਲੀਲਾਂ_3 ... ਦਲੀਲਾਂ_2)
{
	ਪੂਰਨ ਅਗਲਾ_p_state = 0;
	ਅਚਲ ਅੱਖਰ *ਅਗਲਾ_p = p;
	ਜਦੋਂ (*ਅਗਲਾ_p && (ਅਗਲਾ_p_state != 0 || *ਅਗਲਾ_p != ',')) {
		ਜੇ (*ਅਗਲਾ_p == '"')
			ਕਰੋ {
				ਅਗਲਾ_p++;
				ਜਦੋਂ (*ਅਗਲਾ_p == '\\' && *(ਅਗਲਾ_p + 1))
					ਅਗਲਾ_p += 2;
			} ਜਦੋਂ (*ਅਗਲਾ_p && *ਅਗਲਾ_p != '"');
		ਜੇ (*ਅਗਲਾ_p == '\'') {
			ਅਗਲਾ_p++;
			ਜੇ (*ਅਗਲਾ_p == '\\')
				ਅਗਲਾ_p++;
			ਜੇ (*ਅਗਲਾ_p)
				ਅਗਲਾ_p++;
		}
		ਜੇ (*ਅਗਲਾ_p == '(' || *ਅਗਲਾ_p == '[' || *ਅਗਲਾ_p == '{')
			ਅਗਲਾ_p_state++;
		ਜੇ ((*ਅਗਲਾ_p == ')' || *ਅਗਲਾ_p == ']' || *ਅਗਲਾ_p == '}') && ਅਗਲਾ_p_state > 0)
			ਅਗਲਾ_p_state--;
		ਅਗਲਾ_p++;
	}
	ਲਘੂਗਣਕ("\n\t%.*s => ", ਪੂਰਨ(ਅਗਲਾ_p - p), p);
	ਜੇ (*ਅਗਲਾ_p == ',')
		ਅਗਲਾ_p++;
	ਜਦੋਂ (*ਅਗਲਾ_p == ' ' || *ਅਗਲਾ_p == '\t' || *ਅਗਲਾ_p == '\r' || *ਅਗਲਾ_p == '\n')
		ਅਗਲਾ_p++;
	log_dump_val_ਕਾਮਾ(ਪਹਿਲਾ);
	log_dump_ਦਲੀਲਾਂ_ਕਾਮਾ(ਅਗਲਾ_p, ਦਲੀਲਾਂ_2 ...);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_dump(...) ਕਰੋ { \
	ਲਘੂਗਣਕ("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_ਦਲੀਲਾਂ_ਕਾਮਾ(#__VA_ARGS__, __VA_ARGS__); \
	ਲਘੂਗਣਕ("\n"); \
} ਜਦੋਂ (0)

YOSYS_NAMESPACE_ਸਿਰਾ

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

#ਜੇ_ਅੰਤ

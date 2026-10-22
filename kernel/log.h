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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOG_H
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ಅನ್ತರ್ಭಾವ <chrono>
#ಅನ್ತರ್ಭಾವ <time.h>

#ಅನ್ತರ್ಭಾವ <regex>
#ವ್ಯಾಖ್ಯಾನಿಸು YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#ವ್ಯಾಖ್ಯಾನಿಸು YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)

#ಆದರೆ defined(_WIN32)
#  include <intrin.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  include <sys/time.h>
#  include <sys/resource.h>
#  ಆದರೆ defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#    include <signal.h>
#  endif
#ಆದರೆ_ಅಂತ್ಯ

// from libs/sha1/sha1.h
class SHA1;

YOSYS_NAMESPACE_BEGIN

// YS_DEBUGTRAP is a macro that is functionally equivalent to a breakpoint
// if the platform provides such functionality, and does nothing otherwise.
// If no debugger is attached, it starts a just-in-time debugger if available,
// and crashes the process otherwise.
#ಆದರೆ defined(_WIN32)
# define YS_DEBUGTRAP __debugbreak()
#ಇಲ್ಲದಿದ್ದರೆ
# ifndef __has_builtin
// __has_builtin is a GCC/Clang extension; on a different compiler (or old enough GCC/Clang)
// that does not have it, using __has_builtin(...) is a syntax error.
#  define __has_builtin(x) 0
# endif
# ಆದರೆ __has_builtin(__builtin_debugtrap)
#  define YS_DEBUGTRAP __builtin_debugtrap()
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#  define YS_DEBUGTRAP ಎತ್ತು(SIGTRAP)
# ಇಲ್ಲದಿದ್ದರೆ
#  define YS_DEBUGTRAP ಮಾಡು {} ಆಗ(0)
# endif
#ಆದರೆ_ಅಂತ್ಯ

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#ಆದರೆ defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING ಮಾಡು { ಆದರೆ (IsDebuggerPresent()) DebugBreak(); } ಆಗ(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING ಮಾಡು { \
		ಸ್ವಯಂಚಾಲಿತ old = ಸಂಕೇತ(SIGTRAP, SIG_IGN); ಎತ್ತು(SIGTRAP); ಸಂಕೇತ(SIGTRAP, old); \
	} ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
# define YS_DEBUGTRAP_IF_DEBUGGING ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ log_cmd_error_exception { };

ಎಣಿಕೆ class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	ಶೀರ್ಷಿಕೆ,
	Warning,
	NonFatalError,
	Error
};

ರಚನೆ Log_ಮೂಲ_Location {
	Log_ಮೂಲ_Location() : filename({}),ಆರಂಭ_ಸಾಲು(-1),ಆರಂಭ_ಲಂಬಸಾಲು(-1),ಕೊನೆ_ಸಾಲು(-1),ಕೊನೆ_ಲಂಬಸಾಲು(-1) { }
	Log_ಮೂಲ_Location(std::string filename) : filename(filename),ಆರಂಭ_ಸಾಲು(-1),ಆರಂಭ_ಲಂಬಸಾಲು(-1),ಕೊನೆ_ಸಾಲು(-1),ಕೊನೆ_ಲಂಬಸಾಲು(-1) { }
	Log_ಮೂಲ_Location(std::string filename,ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಸಾಲು) : filename(filename),ಆರಂಭ_ಸಾಲು(ಆರಂಭ_ಸಾಲು),ಆರಂಭ_ಲಂಬಸಾಲು(-1),ಕೊನೆ_ಸಾಲು(-1),ಕೊನೆ_ಲಂಬಸಾಲು(-1) { }
	Log_ಮೂಲ_Location(std::string filename,ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಸಾಲು,ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು,ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಸಾಲು,ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು) :
		filename(filename),ಆರಂಭ_ಸಾಲು(ಆರಂಭ_ಸಾಲು),ಆರಂಭ_ಲಂಬಸಾಲು(ಆರಂಭ_ಲಂಬಸಾಲು),ಕೊನೆ_ಸಾಲು(ಕೊನೆ_ಸಾಲು),ಕೊನೆ_ಲಂಬಸಾಲು(ಕೊನೆ_ಲಂಬಸಾಲು) { }
	std::string filename;
	ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು;
};

ರಚನೆ Log_ಸಂದೇಶ {
	Log_ಸಂದೇಶ(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string_view ಸಂದೇಶ_2);
	LogSeverity severity;
	std::string ಪೂರ್ವಪ್ರತ್ಯಯ_2;
	std::string format;
	std::string ಸಂದೇಶ_2;
	std::chrono::steady_clock::ಸಮಯ_ಬಿಂದು timestamp;
	Log_ಮೂಲ_Location src;

	std::string cached_ಸಂದೇಶ;
};

class LogSink
{
ಸಾರ್ವಜನಿಕ:
	ವಾಸ್ತವಿಕ ~LogSink() = ಪೂರ್ವನಿಯೋಜಿತ;

	ವಾಸ್ತವಿಕ bool should_log(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ true; }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) = 0;
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ flush() {}
	// TODO: Remove when AST/read_verilog removed
	ವಾಸ್ತವಿಕ FILE *file_handle() { ಹಿಂತಿರುಗಿ nullptr; }
};

class LogSinkRef : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ sink->should_log(ಸಂದೇಶ); }
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override { sink->ಲಘುಗಣಕ(ಸಂದೇಶ); }
	ಶೂನ್ಯ flush() override { sink->flush(); }
	FILE *file_handle() override { ಹಿಂತಿರುಗಿ sink->file_handle(); }
ಖಾಸಗಿ:
	LogSink *sink;
};

class FileLogSink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	explicit FileLogSink(ಸ್ಥಿರಾಂಕ std::string &filename, bool ಸಾಲು_buffered, bool append);
	~FileLogSink() override;
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override;
	ಶೂನ್ಯ flush() override;
	FILE *file_handle() override { ಹಿಂತಿರುಗಿ file; }

ಖಾಸಗಿ:
	FILE *file;
};

class ConsoleLogSink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override;
	ಶೂನ್ಯ flush() override;
	FILE *file_handle() override { ಹಿಂತಿರುಗಿ stdout; }
};

class StderrLogSink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) ಸ್ಥಿರಾಂಕ override;
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override;
	ಶೂನ್ಯ flush() override;
ಖಾಸಗಿ:
	bool quiet_warnings;
};

class ಹರಿವು_Log_Sink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	explicit ಹರಿವು_Log_Sink(std::ostream &ಹರಿವು) : ಹರಿವು(ಹರಿವು) {}
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override { ಹರಿವು << ಸಂದೇಶ.cached_ಸಂದೇಶ; }
	ಶೂನ್ಯ flush() override { ಹರಿವು.flush(); }

ಖಾಸಗಿ:
	std::ostream &ಹರಿವು;
};

class ScratchPadLogSink : ಸಾರ್ವಜನಿಕ LogSink
{
ಸಾರ್ವಜನಿಕ:
	explicit ScratchPadLogSink(std::string scratchpad);
	ಶೂನ್ಯ ಲಘುಗಣಕ(ಸ್ಥಿರಾಂಕ Log_ಸಂದೇಶ &ಸಂದೇಶ) override;

ಖಾಸಗಿ:
	std::string scratchpad;
};

class LogManager
{
ಖಾಸಗಿ:
	ರಚನೆ Log_Expected_ಅಂಶ
	{
		Log_Expected_ಅಂಶ(ಸ್ಥಿರಾಂಕ std::regex &pat, ಪೂರ್ಣಾಂಕ expected) :
				ನಮೂನೆ(pat), expected_ಎಣಿಕೆ(expected), current_ಎಣಿಕೆ(0) {}
		Log_Expected_ಅಂಶ() : expected_ಎಣಿಕೆ(0), current_ಎಣಿಕೆ(0) {}

		std::regex ನಮೂನೆ;
		ಪೂರ್ಣಾಂಕ expected_ಎಣಿಕೆ;
		ಪೂರ್ಣಾಂಕ current_ಎಣಿಕೆ;
	};

ಸಾರ್ವಜನಿಕ:
	LogManager() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಮಾದರಿ<typename T, typename... ವಾದಗಳು_3>
	T &ಕೂಡಿಸು_sink(ವಾದಗಳು_3&&... ವಾದಗಳು_2)
	{
		ಸ್ವಯಂಚಾಲಿತ sink = std::make_unique<T>(std::forward<ವಾದಗಳು_3>(ವಾದಗಳು_2)...);
		T &ref = *sink;
		sinks.ತಳ್ಳು_ಹಿಂಭಾಗ(std::move(sink));
		ಹಿಂತಿರುಗಿ ref;
	}

	ಮಾದರಿ<typename F>
	ಶೂನ್ಯ for_each_sink(F &&func)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sink : sinks)
			func(*sink);
	}

	bool ಖಾಲಿಗಳು() { ಹಿಂತಿರುಗಿ sinks.ಖಾಲಿಗಳು(); }
	ಶೂನ್ಯ clear() { sinks.clear(); }
	ಶೂನ್ಯ clear_original();
	ಶೂನ್ಯ flush() { ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sink : sinks) sink->flush(); }

	class Scoped
	{
	ಸಾರ್ವಜನಿಕ:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.ಗಾತ್ರ_2());

			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &sink : backup_sinks)
				manager.sinks.ತಳ್ಳು_ಹಿಂಭಾಗ(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ಸ್ಥಿರಾಂಕ Scoped &) = delete;
		Scoped &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Scoped &) = delete;

	ಖಾಸಗಿ:
		LogManager &manager;
		std::ಸದಿಶ<std::unique_ptr<LogSink>> backup_sinks;
		ಪೂರ್ಣಾಂಕ backup_verbose_level;
	};

	Scoped sink_scope()
	{
		ಹಿಂತಿರುಗಿ Scoped(*this);
	}

	class ScopedCmdErrorThrow
	{
	ಸಾರ್ವಜನಿಕ:
		explicit ScopedCmdErrorThrow(LogManager &manager)
			: manager(manager), ಹಿಂದಿನ(manager.cmd_error_throw)
		{
			manager.cmd_error_throw = true;
		}

		~ScopedCmdErrorThrow()
		{
			manager.cmd_error_throw = ಹಿಂದಿನ;
		}

	ಖಾಸಗಿ:
		LogManager &manager;
		bool ಹಿಂದಿನ;
	};

	ScopedCmdErrorThrow error_throw_scope()
	{
		ಹಿಂತಿರುಗಿ ScopedCmdErrorThrow(*this);
	}

	class LogMakeDebugHdl
	{
	ಸಾರ್ವಜನಿಕ:
		explicit LogMakeDebugHdl(LogManager &manager, bool ಆರಂಭ_on = false)
			: manager(manager)
		{
			ಆದರೆ (ಆರಂಭ_on)
				on();
		}

		~LogMakeDebugHdl()
		{
			off();
		}

		ಶೂನ್ಯ on()
		{
			ಆದರೆ (ಸ್ಥಿತಿ_2)
				ಹಿಂತಿರುಗಿ;
			ಸ್ಥಿತಿ_2 = true;
			manager.make_debug++;
		}

		ಶೂನ್ಯ off_silent()
		{
			ಆದರೆ (!ಸ್ಥಿತಿ_2)
				ಹಿಂತಿರುಗಿ;
			ಸ್ಥಿತಿ_2 = false;
			manager.make_debug--;
		}

		ಶೂನ್ಯ off()
		{
			off_silent();
		}
	ಖಾಸಗಿ:
		LogManager &manager;
		bool ಸ್ಥಿತಿ_2 = false;
	};

	LogMakeDebugHdl make_debug_scope(bool ಆರಂಭ_on = false)
	{
		ಹಿಂತಿರುಗಿ LogMakeDebugHdl(*this, ಆರಂಭ_on);
	}

	class ForceDebug
	{
	ಸಾರ್ವಜನಿಕ:
		explicit ForceDebug(LogManager &manager, bool ಆರಂಭ_on = false)
			: manager(manager)
		{
			ಆದರೆ (ಆರಂಭ_on)
				on();
		}

		~ForceDebug()
		{
			off();
		}

		ಶೂನ್ಯ on()
		{
			ಆದರೆ (ಸಕ್ರಿಯ)
				ಹಿಂತಿರುಗಿ;

			ಸಕ್ರಿಯ = true;
			manager.force_debug++;
		}

		ಶೂನ್ಯ off()
		{
			ಆದರೆ (!ಸಕ್ರಿಯ)
				ಹಿಂತಿರುಗಿ;

			ಸಕ್ರಿಯ = false;
			manager.force_debug--;
		}
	ಖಾಸಗಿ:
		LogManager &manager;
		bool ಸಕ್ರಿಯ = false;
	};

	ForceDebug force_debug_scope(bool ಆರಂಭ_on = false)
	{
		ಹಿಂತಿರುಗಿ ForceDebug(*this, ಆರಂಭ_on);
	}

	ಶೂನ್ಯ force_debug_on() { force_debug++; }
	ಶೂನ್ಯ force_debug_off() { ಆದರೆ (force_debug > 0) force_debug--; }
	ಶೂನ್ಯ ಗಣ_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	ಶೂನ್ಯ report_unexpected_error();

	ಶೂನ್ಯ ಕೂಡಿಸು_experimental_ignore(std::string name) { experimental_ignored.ಸೇರಿಸು(name); }
	ಶೂನ್ಯ ಕೂಡಿಸು_warn(std::string ನಮೂನೆ) { warn_regexes.ತಳ್ಳು_ಹಿಂಭಾಗ(YS_REGEX_COMPILE(ನಮೂನೆ)); }
	ಶೂನ್ಯ ಕೂಡಿಸು_nowarn(std::string ನಮೂನೆ) { nowarn_regexes.ತಳ್ಳು_ಹಿಂಭಾಗ(YS_REGEX_COMPILE(ನಮೂನೆ)); }
	ಶೂನ್ಯ ಕೂಡಿಸು_werror(std::string ನಮೂನೆ) { werror_regexes.ತಳ್ಳು_ಹಿಂಭಾಗ(YS_REGEX_COMPILE(ನಮೂನೆ)); }
	ಶೂನ್ಯ ಕೂಡಿಸು_expect(std::string type, std::string ನಮೂನೆ, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);

	ಶೂನ್ಯ ಗಣ_verbose_level(ಪೂರ್ಣಾಂಕ level) { verbose_level = level; }
	ಶೂನ್ಯ ಕೂಡಿಸು_verbose_level(ಪೂರ್ಣಾಂಕ level) { verbose_level += level; }
	ಶೂನ್ಯ ಗಣ_expect_no_warnings(bool ಮೌಲ್ಯ) { expect_no_warnings = ಮೌಲ್ಯ; }
	ಶೂನ್ಯ ಗಣ_log_ಸಮಯ(bool ಮೌಲ್ಯ) { log_ಸಮಯ = ಮೌಲ್ಯ; }
	ಶೂನ್ಯ ಗಣ_cmd_error_throw(bool ಮೌಲ್ಯ) { cmd_error_throw = ಮೌಲ್ಯ; }
	ಶೂನ್ಯ ಗಣ_hdump_all(bool ಮೌಲ್ಯ) { hdump_all = ಮೌಲ್ಯ; }
	ಪೂರ್ಣಾಂಕ get_verbose_level() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ verbose_level; }
	bool get_log_ಸಮಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ log_ಸಮಯ; }
	ಪೂರ್ಣಾಂಕ get_warnings_unique() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ Get_ಗಾತ್ರ(warnings); }
	ಪೂರ್ಣಾಂಕ get_warnings_ಒಟ್ಟು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ warnings_ಎಣಿಕೆ; }
	ಪೂರ್ಣಾಂಕ get_errors_ಒಟ್ಟು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ errors_ಎಣಿಕೆ; }
	ಸ್ಥಿರಾಂಕ std::ಗಣ<std::string> &get_experimental() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ experimental; }
	ಸ್ಥಿರಾಂಕ std::map<std::string, std::string> &get_deprecated() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ deprecated; }
	std::chrono::steady_clock::ಸಮಯ_ಬಿಂದು get_initial_ಸಮಯ() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಕೂಡಿಸು_hdump(std::string name, std::string ಮೌಲ್ಯ) { hdump[name].ಸೇರಿಸು(ಮೌಲ್ಯ); }

	ಶೂನ್ಯ formatted_string(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string str);
	ಶೂನ್ಯ formatted_ಶೀರ್ಷಿಕೆ(RTLIL::Design *design, std::string_view format, std::string str);
	ಶೂನ್ಯ formatted_warning(Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2);
	[[noreturn]] ಶೂನ್ಯ formatted_error(Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2);
	ಶೂನ್ಯ formatted_nonfatal_error(Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2);
	[[noreturn]] ಶೂನ್ಯ formatted_cmd_error(std::string_view format, std::string ಸಂದೇಶ_2);
	ಶೂನ್ಯ suppressed();
	ಶೂನ್ಯ ಕೂಡಿಸು_experimental(ಸ್ಥಿರಾಂಕ std::string &str);
	ಶೂನ್ಯ ಕೂಡಿಸು_deprecated(ಸ್ಥಿರಾಂಕ std::string &feature, ಸ್ಥಿರಾಂಕ std::string &earliest_removal);
	ಶೂನ್ಯ spacer();
	ಶೂನ್ಯ ತಳ್ಳು();
	ಶೂನ್ಯ ತೆಗೆ_2();

	ಶೂನ್ಯ reset_ರಾಶಿ();

	ಶೂನ್ಯ check_expected();
	bool expects_error() { ಹಿಂತಿರುಗಿ (expect_error.ಗಾತ್ರ_2() + expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error.ಗಾತ್ರ_2())>0; }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
	bool is_debug(ಪೂರ್ಣಾಂಕ n = 0) { ಆದರೆ (force_debug) ಹಿಂತಿರುಗಿ true; debug_suppressed += n; ಹಿಂತಿರುಗಿ false; }
#ಇಲ್ಲದಿದ್ದರೆ
	bool is_debug(ಪೂರ್ಣಾಂಕ = 0) { ಹಿಂತಿರುಗಿ false; }
#ಆದರೆ_ಅಂತ್ಯ
	ಶೂನ್ಯ ಆರಂಭ_hasher();
	std::string ಮುಕ್ತಾಯ_hasher();

ಖಾಸಗಿ:
	ಶೂನ್ಯ logv_string(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string str_ಒಳ);
	ಶೂನ್ಯ error_with_ಪೂರ್ವಪ್ರತ್ಯಯ(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2, std::string_view format, std::string ಸಂದೇಶ_2);

	std::ಸದಿಶ<std::unique_ptr<LogSink>> sinks;
	ಪೂರ್ಣಾಂಕ verbose_level = 0;
	ಪೂರ್ಣಾಂಕ newline_ಎಣಿಕೆ = 0;
	ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ errors_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಂಕ warnings_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಂಕ warnings_ಎಣಿಕೆ_noexpect = 0;
	std::ಗಣ<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::ಸದಿಶ<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, Log_Expected_ಅಂಶ> expect_log, expect_warning, expect_error;
	dict<std::string, Log_Expected_ಅಂಶ> expect_ಪೂರ್ವಪ್ರತ್ಯಯ_log, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_warning, expect_ಪೂರ್ವಪ್ರತ್ಯಯ_error;
	bool expect_no_warnings = false;
	bool log_ಸಮಯ = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::ಗಣ<std::string>> hdump;
	bool hdump_all = false;

	ಪೂರ್ಣಾಂಕ debug_suppressed = 0;
	ಪೂರ್ಣಾಂಕ make_debug = 0;
	ಪೂರ್ಣಾಂಕ force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

ಬಾಹ್ಯ ಶೂನ್ಯ (*log_error_atexit)();

ಶೂನ್ಯ ಗಣ_verific_logging(ಶೂನ್ಯ (*cb)(ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_id, Log_ಮೂಲ_Location src, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ));
ಬಾಹ್ಯ ಶೂನ್ಯ (*log_verific_callback)(ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_id, Log_ಮೂಲ_Location src, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ys_debug(ಪೂರ್ಣಾಂಕ n = 0) { ಹಿಂತಿರುಗಿ logger().is_debug(n); }
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ys_debug(ಪೂರ್ಣಾಂಕ = 0) { ಹಿಂತಿರುಗಿ false; }
#ಆದರೆ_ಅಂತ್ಯ

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಲಘುಗಣಕ(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_string(LogSeverity::Info, Log_ಮೂಲ_Location{}, {}, fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_comment(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_string(LogSeverity::Comment, Log_ಮೂಲ_Location{}, {}, fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_highlight(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_string(LogSeverity::Highlight, Log_ಮೂಲ_Location{}, {}, fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_formatted_string(LogSeverity severity, Log_ಮೂಲ_Location src, std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ_2,
		FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_string(severity, src, ಪೂರ್ವಪ್ರತ್ಯಯ_2, fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

#ವ್ಯಾಖ್ಯಾನಿಸು log_debug(...) ಮಾಡು { ಆದರೆ (ys_debug(1)) YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ log_formatted_string(YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ LogSeverity::Debug, {}, {}, __VA_ARGS__); } ಆಗ (0)

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_ಶೀರ್ಷಿಕೆ(RTLIL::Design *design, FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_ಶೀರ್ಷಿಕೆ(design, fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_warning(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_warning(Log_ಮೂಲ_Location{}, "Warning: ", fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_warning_noprefix(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_warning(Log_ಮೂಲ_Location{}, {}, fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_experimental(ಸ್ಥಿರಾಂಕ std::string &str)
{
	logger().ಕೂಡಿಸು_experimental(str);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_deprecated(ಸ್ಥಿರಾಂಕ std::string &feature, ಸ್ಥಿರಾಂಕ std::string &earliest_removal = "")
{
	logger().ಕೂಡಿಸು_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಶೂನ್ಯ log_file_warning(Log_ಮೂಲ_Location src, FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಶೂನ್ಯ log_file_info(Log_ಮೂಲ_Location src, FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
[[noreturn]] ಶೂನ್ಯ log_error(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_error(Log_ಮೂಲ_Location{}, "ERROR: ", fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
[[noreturn]] ಶೂನ್ಯ log_file_error(Log_ಮೂಲ_Location src, FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
ಶೂನ್ಯ log_file_nonfatal_error(Log_ಮೂಲ_Location src, FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಮಾದರಿ <typename... ವಾದಗಳು_3>
[[noreturn]] ಶೂನ್ಯ log_cmd_error(FmtString<Type_ತಟಸ್ಥ<ವಾದಗಳು_3>...> fmt, ಸ್ಥಿರಾಂಕ ವಾದಗಳು_3 &... ವಾದಗಳು_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(ವಾದಗಳು_2...));
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_suppressed()
{
	logger().suppressed();
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_spacer() { logger().spacer(); }
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_ತಳ್ಳು() { logger().ತಳ್ಳು(); }
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_ತೆಗೆ() { logger().ತೆಗೆ_2(); }

ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_reset_ರಾಶಿ() { logger().reset_ರಾಶಿ(); }
ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_flush() { logger().flush(); }

ಶೂನ್ಯ log_backtrace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ_2, ಪೂರ್ಣಾಂಕ ಮಟ್ಟಗಳು);


std::string log_signal(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ಸ್ಥಿರಾಂಕ RTLIL::Const &ಮೌಲ್ಯ, bool autoint = true);

ಶೂನ್ಯ log_module(RTLIL::Module *module, std::string indent = "");
ಶೂನ್ಯ log_cell(RTLIL::Cell *cell, std::string indent = "");
ಶೂನ್ಯ log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
ಶೂನ್ಯ log_abort_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು);
#ವ್ಯಾಖ್ಯಾನಿಸು log_abort() YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ log_abort_ಆಂತರಿಕ(__FILE__, __LINE__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ping() YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ ಲಘುಗಣಕ("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
ರಚನೆ PerformanceTimer
{
#ಆದರೆ 1
	int64_t ಒಟ್ಟು_ns;

	PerformanceTimer() {
		ಒಟ್ಟು_ns = 0;
	}

	ಸ್ಥಿರ int64_t query() {
#  ifdef _WIN32
		ಹಿಂತಿರುಗಿ 0;
#  elif defined(RUSAGE_SELF)
		ರಚನೆ rusage rusage;
		int64_t t = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ who : {RUSAGE_SELF, RUSAGE_CHILDREN}) {
			ಆದರೆ (getrusage(who, &rusage) == -1) {
				log_cmd_error("getrusage failed!\n");
				log_abort();
			}
			t += 1000000000ULL * (int64_t) rusage.ru_utime.tv_sec + (int64_t) rusage.ru_utime.tv_usec * 1000ULL;
			t += 1000000000ULL * (int64_t) rusage.ru_stime.tv_sec + (int64_t) rusage.ru_stime.tv_usec * 1000ULL;
		}
		ಹಿಂತಿರುಗಿ t;
#  ಇಲ್ಲದಿದ್ದರೆ
#    error "Don't know how to measure per-process CPU time. Need alternative method (times()/clocks()/gettimeofday()?)."
#  endif
	}

	ಶೂನ್ಯ reset() {
		ಒಟ್ಟು_ns = 0;
	}

	ಶೂನ್ಯ begin() {
		ಒಟ್ಟು_ns -= query();
	}

	ಶೂನ್ಯ ಕೊನೆ() {
		ಒಟ್ಟು_ns += query();
	}

	ದಶಮಾಂಶ sec() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಒಟ್ಟು_ns * 1e-9f;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿರ int64_t query() { ಹಿಂತಿರುಗಿ 0; }
	ಶೂನ್ಯ reset() { }
	ಶೂನ್ಯ begin() { }
	ಶೂನ್ಯ ಕೊನೆ() { }
	ದಶಮಾಂಶ sec() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ 0; }
#ಆದರೆ_ಅಂತ್ಯ
};

// simple API for quickly dumping values when debugging

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಗಿಡ್ಡ v) { ಲಘುಗಣಕ("%d", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ v) { ಲಘುಗಣಕ("%u", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಪೂರ್ಣಾಂಕ v) { ಲಘುಗಣಕ("%d", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ v) { ಲಘುಗಣಕ("%u", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ದೀರ್ಘ ಪೂರ್ಣಾಂಕ v) { ಲಘುಗಣಕ("%ld", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ v) { ಲಘುಗಣಕ("%lu", v); }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _WIN32
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ದೀರ್ಘ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ v) { ಲಘುಗಣಕ("%lld", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ v) { ಲಘುಗಣಕ("%lld", v); }
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಅಕ್ಷರ c)
{
	ಆದರೆ (c >= 32 && c < 127) {
		ಲಘುಗಣಕ("'%c'", c);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("'\\x%02x'", c);
	}
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
	ಆದರೆ (c >= 32 && c < 127) {
		ಲಘುಗಣಕ("'%c'", c);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಲಘುಗಣಕ("'\\x%02x'", c);
	}
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(bool v) { ಲಘುಗಣಕ("%s", v ? "true" : "false"); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ದ್ವಿಗುಣ v) { ಲಘುಗಣಕ("%f", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಅಕ್ಷರ *v) { ಲಘುಗಣಕ("%s", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v) { ಲಘುಗಣಕ("%s", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(std::string v) { ಲಘುಗಣಕ("%s", v); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(PerformanceTimer p) { ಲಘುಗಣಕ("%f seconds", p.sec()); }
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_ವಾದಗಳು_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) { log_assert(*p == 0); }
ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(IdString v);
ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(RTLIL::SigSpec v);
ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(RTLIL::State v);

ಮಾದರಿ<typename K, typename T> ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(dict<K, T> &v);
ಮಾದರಿ<typename K> ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(pool<K> &v);
ಮಾದರಿ<typename K> ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(std::ಸದಿಶ<K> &v);
ಮಾದರಿ<typename T> ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(T *ptr);

ಮಾದರಿ<typename K, typename T>
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(dict<K, T> &v) {
	ಲಘುಗಣಕ("{");
	bool ಮೊದಲ = true;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : v) {
		ಲಘುಗಣಕ("%s ", ಮೊದಲ ? "" : ",");
		log_dump_val_ಕೆಲಸಗಾರ(it.ಮೊದಲ);
		ಲಘುಗಣಕ(": ");
		log_dump_val_ಕೆಲಸಗಾರ(it.ಎರಡನೇ);
		ಮೊದಲ = false;
	}
	ಲಘುಗಣಕ(" }");
}

ಮಾದರಿ<typename K>
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(pool<K> &v) {
	ಲಘುಗಣಕ("{");
	bool ಮೊದಲ = true;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : v) {
		ಲಘುಗಣಕ("%s ", ಮೊದಲ ? "" : ",");
		log_dump_val_ಕೆಲಸಗಾರ(it);
		ಮೊದಲ = false;
	}
	ಲಘುಗಣಕ(" }");
}

ಮಾದರಿ<typename K>
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(std::ಸದಿಶ<K> &v) {
	ಲಘುಗಣಕ("{");
	bool ಮೊದಲ = true;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : v) {
		ಲಘುಗಣಕ("%s ", ಮೊದಲ ? "" : ",");
		log_dump_val_ಕೆಲಸಗಾರ(it);
		ಮೊದಲ = false;
	}
	ಲಘುಗಣಕ(" }");
}

ಮಾದರಿ<typename T>
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ log_dump_val_ಕೆಲಸಗಾರ(T *ptr) { ಲಘುಗಣಕ("%p", ptr); }

ಮಾದರಿ<typename T, typename ... ವಾದಗಳು_3>
ಶೂನ್ಯ log_dump_ವಾದಗಳು_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, T ಮೊದಲ, ವಾದಗಳು_3 ... ವಾದಗಳು_2)
{
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_p_state = 0;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮುಂದಿನ_p = p;
	ಆಗ (*ಮುಂದಿನ_p && (ಮುಂದಿನ_p_state != 0 || *ಮುಂದಿನ_p != ',')) {
		ಆದರೆ (*ಮುಂದಿನ_p == '"')
			ಮಾಡು {
				ಮುಂದಿನ_p++;
				ಆಗ (*ಮುಂದಿನ_p == '\\' && *(ಮುಂದಿನ_p + 1))
					ಮುಂದಿನ_p += 2;
			} ಆಗ (*ಮುಂದಿನ_p && *ಮುಂದಿನ_p != '"');
		ಆದರೆ (*ಮುಂದಿನ_p == '\'') {
			ಮುಂದಿನ_p++;
			ಆದರೆ (*ಮುಂದಿನ_p == '\\')
				ಮುಂದಿನ_p++;
			ಆದರೆ (*ಮುಂದಿನ_p)
				ಮುಂದಿನ_p++;
		}
		ಆದರೆ (*ಮುಂದಿನ_p == '(' || *ಮುಂದಿನ_p == '[' || *ಮುಂದಿನ_p == '{')
			ಮುಂದಿನ_p_state++;
		ಆದರೆ ((*ಮುಂದಿನ_p == ')' || *ಮುಂದಿನ_p == ']' || *ಮುಂದಿನ_p == '}') && ಮುಂದಿನ_p_state > 0)
			ಮುಂದಿನ_p_state--;
		ಮುಂದಿನ_p++;
	}
	ಲಘುಗಣಕ("\n\t%.*s => ", ಪೂರ್ಣಾಂಕ(ಮುಂದಿನ_p - p), p);
	ಆದರೆ (*ಮುಂದಿನ_p == ',')
		ಮುಂದಿನ_p++;
	ಆಗ (*ಮುಂದಿನ_p == ' ' || *ಮುಂದಿನ_p == '\t' || *ಮುಂದಿನ_p == '\r' || *ಮುಂದಿನ_p == '\n')
		ಮುಂದಿನ_p++;
	log_dump_val_ಕೆಲಸಗಾರ(ಮೊದಲ);
	log_dump_ವಾದಗಳು_ಕೆಲಸಗಾರ(ಮುಂದಿನ_p, ವಾದಗಳು_2 ...);
}

#ವ್ಯಾಖ್ಯಾನಿಸು log_dump(...) ಮಾಡು { \
	ಲಘುಗಣಕ("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_ವಾದಗಳು_ಕೆಲಸಗಾರ(#__VA_ARGS__, __VA_ARGS__); \
	ಲಘುಗಣಕ("\n"); \
} ಆಗ (0)

YOSYS_NAMESPACE_ಕೊನೆ

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

#ಆದರೆ_ಅಂತ್ಯ

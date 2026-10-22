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

#यदि_अपरिभाषितम् LOG_H
#परिभाषय LOG_H

#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव <chrono>
#अन्तर्भाव <time.h>

#अन्तर्भाव <regex>
#परिभाषय YS_REGEX_COMPILE(param) std::regex(param, \
				std::regex_constants::nosubs | \
				std::regex_constants::optimize | \
				std::regex_constants::egrep)
#परिभाषय YS_REGEX_COMPILE_WITH_SUBS(param) std::regex(param, \
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
#यदि_अन्तः

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
#यदि_अन्तः

// YS_DEBUGTRAP_IF_DEBUGGING is a macro that is functionally equivalent to a breakpoint
// if a debugger is attached, and does nothing otherwise.
#यदि defined(_WIN32)
# define YS_DEBUGTRAP_IF_DEBUGGING कुरु { यदि (IsDebuggerPresent()) DebugBreak(); } यावत्(0)
# elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// There is no reliable (or portable) *nix equivalent of IsDebuggerPresent(). However,
// debuggers will stop when SIGTRAP is raised, even if the action is set to ignore.
# define YS_DEBUGTRAP_IF_DEBUGGING कुरु { \
		स्वचालित old = सङ्केत(SIGTRAP, SIG_IGN); उत्थापय(SIGTRAP); सङ्केत(SIGTRAP, old); \
	} यावत्(0)
#अन्यथा
# define YS_DEBUGTRAP_IF_DEBUGGING कुरु {} यावत्(0)
#यदि_अन्तः

संरचना log_cmd_error_exception { };

गणना class LogSeverity {
	Debug,
	Comment,
	Info,
	Highlight,
	शीर्षकम्,
	Warning,
	NonFatalError,
	Error
};

संरचना LogSourceLocation {
	LogSourceLocation() : filename({}),start_पङ्क्तिः(-1),start_col(-1),अन्तः_पङ्क्तिः(-1),अन्तः_col(-1) { }
	LogSourceLocation(std::string filename) : filename(filename),start_पङ्क्तिः(-1),start_col(-1),अन्तः_पङ्क्तिः(-1),अन्तः_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णाङ्क start_पङ्क्तिः) : filename(filename),start_पङ्क्तिः(start_पङ्क्तिः),start_col(-1),अन्तः_पङ्क्तिः(-1),अन्तः_col(-1) { }
	LogSourceLocation(std::string filename,पूर्णाङ्क start_पङ्क्तिः,पूर्णाङ्क start_col,पूर्णाङ्क अन्तः_पङ्क्तिः,पूर्णाङ्क अन्तः_col) :
		filename(filename),start_पङ्क्तिः(start_पङ्क्तिः),start_col(start_col),अन्तः_पङ्क्तिः(अन्तः_पङ्क्तिः),अन्तः_col(अन्तः_col) { }
	std::string filename;
	पूर्णाङ्क start_पङ्क्तिः;
	पूर्णाङ्क start_col;
	पूर्णाङ्क अन्तः_पङ्क्तिः;
	पूर्णाङ्क अन्तः_col;
};

संरचना Log_सन्देशः {
	Log_सन्देशः(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string_view सन्देशः_2);
	LogSeverity severity;
	std::string उपसर्गः_2;
	std::string format;
	std::string सन्देशः_2;
	std::chrono::steady_clock::time_point timestamp;
	LogSourceLocation src;

	std::string cached_सन्देशः;
};

class LogSink
{
सार्वजनिक:
	आभास ~LogSink() = स्वभाव;

	आभास bool should_log(ध्रुव Log_सन्देशः &) ध्रुव { प्रत्यागम true; }
	आभास शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) = 0;
	आभास शून्य flush() {}
	// TODO: Remove when AST/read_verilog removed
	आभास FILE *file_handle() { प्रत्यागम nullptr; }
};

class LogSinkRef : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit LogSinkRef(LogSink *sink) : sink(sink) {}
	bool should_log(ध्रुव Log_सन्देशः &सन्देशः) ध्रुव override { प्रत्यागम sink->should_log(सन्देशः); }
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override { sink->लघुगणकः(सन्देशः); }
	शून्य flush() override { sink->flush(); }
	FILE *file_handle() override { प्रत्यागम sink->file_handle(); }
गुप्त:
	LogSink *sink;
};

class FileLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit FileLogSink(ध्रुव std::string &filename, bool पङ्क्तिः_buffered, bool append);
	~FileLogSink() override;
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override;
	शून्य flush() override;
	FILE *file_handle() override { प्रत्यागम file; }

गुप्त:
	FILE *file;
};

class ConsoleLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override;
	शून्य flush() override;
	FILE *file_handle() override { प्रत्यागम stdout; }
};

class StderrLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit StderrLogSink(bool quiet) : quiet_warnings(quiet) {}
	bool should_log(ध्रुव Log_सन्देशः &सन्देशः) ध्रुव override;
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override;
	शून्य flush() override;
गुप्त:
	bool quiet_warnings;
};

class प्रवाहः_Log_Sink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit प्रवाहः_Log_Sink(std::ostream &प्रवाहः) : प्रवाहः(प्रवाहः) {}
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override { प्रवाहः << सन्देशः.cached_सन्देशः; }
	शून्य flush() override { प्रवाहः.flush(); }

गुप्त:
	std::ostream &प्रवाहः;
};

class ScratchPadLogSink : सार्वजनिक LogSink
{
सार्वजनिक:
	explicit ScratchPadLogSink(std::string scratchpad);
	शून्य लघुगणकः(ध्रुव Log_सन्देशः &सन्देशः) override;

गुप्त:
	std::string scratchpad;
};

class LogManager
{
गुप्त:
	संरचना LogExpectedItem
	{
		LogExpectedItem(ध्रुव std::regex &pat, पूर्णाङ्क expected) :
				आदर्शः(pat), expected_गणनम्(expected), current_गणनम्(0) {}
		LogExpectedItem() : expected_गणनम्(0), current_गणनम्(0) {}

		std::regex आदर्शः;
		पूर्णाङ्क expected_गणनम्;
		पूर्णाङ्क current_गणनम्;
	};

सार्वजनिक:
	LogManager() = स्वभाव;

	प्रतिरूपम्<typename T, typename... तर्काः_3>
	T &सङ्कलय_sink(तर्काः_3&&... तर्काः_2)
	{
		स्वचालित sink = std::make_unique<T>(std::forward<तर्काः_3>(तर्काः_2)...);
		T &ref = *sink;
		sinks.प्रक्षिप_पृष्ठभागः(std::move(sink));
		प्रत्यागम ref;
	}

	प्रतिरूपम्<typename F>
	शून्य for_each_sink(F &&func)
	{
		प्रति (स्वचालित &sink : sinks)
			func(*sink);
	}

	bool रिक्तानि() { प्रत्यागम sinks.रिक्तानि(); }
	शून्य clear() { sinks.clear(); }
	शून्य clear_original();
	शून्य flush() { प्रति (स्वचालित &sink : sinks) sink->flush(); }

	class Scoped
	{
	सार्वजनिक:
		explicit Scoped(LogManager &manager) :
			manager(manager),
			backup_sinks(std::move(manager.sinks)),
			backup_verbose_level(manager.verbose_level)
		{
			manager.sinks.reserve(backup_sinks.आकार());

			प्रति (ध्रुव स्वचालित &sink : backup_sinks)
				manager.sinks.प्रक्षिप_पृष्ठभागः(std::make_unique<LogSinkRef>(sink.get()));
		}

		~Scoped()
		{
			manager.sinks.clear();
			manager.sinks = std::move(backup_sinks);
			manager.verbose_level = backup_verbose_level;
		}

		Scoped(ध्रुव Scoped &) = delete;
		Scoped &संक्रियकः=(ध्रुव Scoped &) = delete;

	गुप्त:
		LogManager &manager;
		std::सदिशम्<std::unique_ptr<LogSink>> backup_sinks;
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
			यदि (स्थितिः)
				प्रत्यागम;
			स्थितिः = true;
			manager.make_debug++;
		}

		शून्य off_silent()
		{
			यदि (!स्थितिः)
				प्रत्यागम;
			स्थितिः = false;
			manager.make_debug--;
		}

		शून्य off()
		{
			off_silent();
		}
	गुप्त:
		LogManager &manager;
		bool स्थितिः = false;
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
			यदि (सक्रिय)
				प्रत्यागम;

			सक्रिय = true;
			manager.force_debug++;
		}

		शून्य off()
		{
			यदि (!सक्रिय)
				प्रत्यागम;

			सक्रिय = false;
			manager.force_debug--;
		}
	गुप्त:
		LogManager &manager;
		bool सक्रिय = false;
	};

	ForceDebug force_debug_scope(bool start_on = false)
	{
		प्रत्यागम ForceDebug(*this, start_on);
	}

	शून्य force_debug_on() { force_debug++; }
	शून्य force_debug_off() { यदि (force_debug > 0) force_debug--; }
	शून्य समुच्चयः_force_debug(bool enabled) { force_debug = enabled ? 1 : 0;	}

	शून्य report_unexpected_error();

	शून्य सङ्कलय_experimental_ignore(std::string name) { experimental_ignored.निवेशय(name); }
	शून्य सङ्कलय_warn(std::string आदर्शः) { warn_regexes.प्रक्षिप_पृष्ठभागः(YS_REGEX_COMPILE(आदर्शः)); }
	शून्य सङ्कलय_nowarn(std::string आदर्शः) { nowarn_regexes.प्रक्षिप_पृष्ठभागः(YS_REGEX_COMPILE(आदर्शः)); }
	शून्य सङ्कलय_werror(std::string आदर्शः) { werror_regexes.प्रक्षिप_पृष्ठभागः(YS_REGEX_COMPILE(आदर्शः)); }
	शून्य सङ्कलय_expect(std::string type, std::string आदर्शः, पूर्णाङ्क गणनम्);

	शून्य समुच्चयः_verbose_level(पूर्णाङ्क level) { verbose_level = level; }
	शून्य सङ्कलय_verbose_level(पूर्णाङ्क level) { verbose_level += level; }
	शून्य समुच्चयः_expect_no_warnings(bool मानम्) { expect_no_warnings = मानम्; }
	शून्य समुच्चयः_log_time(bool मानम्) { log_time = मानम्; }
	शून्य समुच्चयः_cmd_error_throw(bool मानम्) { cmd_error_throw = मानम्; }
	शून्य समुच्चयः_hdump_all(bool मानम्) { hdump_all = मानम्; }
	पूर्णाङ्क get_verbose_level() ध्रुव { प्रत्यागम verbose_level; }
	bool get_log_time() ध्रुव { प्रत्यागम log_time; }
	पूर्णाङ्क get_warnings_unique() ध्रुव { प्रत्यागम Get_आकार(warnings); }
	पूर्णाङ्क get_warnings_योग() ध्रुव { प्रत्यागम warnings_गणनम्; }
	पूर्णाङ्क get_errors_योग() ध्रुव { प्रत्यागम errors_गणनम्; }
	ध्रुव std::समुच्चयः<std::string> &get_experimental() ध्रुव { प्रत्यागम experimental; }
	ध्रुव std::map<std::string, std::string> &get_deprecated() ध्रुव { प्रत्यागम deprecated; }
	std::chrono::steady_clock::time_point get_initial_time() ध्रुव;

	शून्य सङ्कलय_hdump(std::string name, std::string मानम्) { hdump[name].निवेशय(मानम्); }

	शून्य formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string str);
	शून्य formatted_शीर्षकम्(RTLIL::Design *design, std::string_view format, std::string str);
	शून्य formatted_warning(LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2);
	[[noreturn]] शून्य formatted_error(LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2);
	शून्य formatted_nonfatal_error(LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2);
	[[noreturn]] शून्य formatted_cmd_error(std::string_view format, std::string सन्देशः_2);
	शून्य suppressed();
	शून्य सङ्कलय_experimental(ध्रुव std::string &str);
	शून्य सङ्कलय_deprecated(ध्रुव std::string &feature, ध्रुव std::string &earliest_removal);
	शून्य spacer();
	शून्य प्रक्षिप();
	शून्य निष्कासय();

	शून्य reset_राशिः();

	शून्य check_expected();
	bool expects_error() { प्रत्यागम (expect_error.आकार() + expect_उपसर्गः_error.आकार())>0; }
#यदि_अपरिभाषितम् NDEBUG
	bool is_debug(पूर्णाङ्क n = 0) { यदि (force_debug) प्रत्यागम true; debug_suppressed += n; प्रत्यागम false; }
#अन्यथा
	bool is_debug(पूर्णाङ्क = 0) { प्रत्यागम false; }
#यदि_अन्तः
	शून्य start_hasher();
	std::string finish_hasher();

गुप्त:
	शून्य logv_string(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string str_अन्तर्);
	शून्य error_with_उपसर्गः(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2, std::string_view format, std::string सन्देशः_2);

	std::सदिशम्<std::unique_ptr<LogSink>> sinks;
	पूर्णाङ्क verbose_level = 0;
	पूर्णाङ्क newline_गणनम् = 0;
	सदिशम्<पूर्णाङ्क> शीर्षकम्_गणनम्;
	पूर्णाङ्क errors_गणनम् = 0;
	पूर्णाङ्क warnings_गणनम् = 0;
	पूर्णाङ्क warnings_गणनम्_noexpect = 0;
	std::समुच्चयः<std::string> warnings, experimental, experimental_ignored;
	std::map<std::string, std::string> deprecated;

	std::सदिशम्<std::regex> warn_regexes, nowarn_regexes, werror_regexes;
	dict<std::string, LogExpectedItem> expect_log, expect_warning, expect_error;
	dict<std::string, LogExpectedItem> expect_उपसर्गः_log, expect_उपसर्गः_warning, expect_उपसर्गः_error;
	bool expect_no_warnings = false;
	bool log_time = false;
	bool cmd_error_throw = false;
	std::map<std::string, std::समुच्चयः<std::string>> hdump;
	bool hdump_all = false;

	पूर्णाङ्क debug_suppressed = 0;
	पूर्णाङ्क make_debug = 0;
	पूर्णाङ्क force_debug = 0;
	std::unique_ptr<SHA1> hasher;
};

LogManager &logger();

बाह्य शून्य (*log_error_atexit)();

शून्य समुच्चयः_verific_logging(शून्य (*cb)(पूर्णाङ्क सन्देशः_type, ध्रुव वर्ण *सन्देशः_id, LogSourceLocation src, ध्रुव वर्ण *सन्देशः));
बाह्य शून्य (*log_verific_callback)(पूर्णाङ्क सन्देशः_type, ध्रुव वर्ण *सन्देशः_id, LogSourceLocation src, ध्रुव वर्ण *सन्देशः);

#यदि_अपरिभाषितम् NDEBUG
स्थिर अन्तःस्थ bool ys_debug(पूर्णाङ्क n = 0) { प्रत्यागम logger().is_debug(n); }
#अन्यथा
स्थिर अन्तःस्थ bool ys_debug(पूर्णाङ्क = 0) { प्रत्यागम false; }
#यदि_अन्तः

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य लघुगणकः(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_string(LogSeverity::Info, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य log_comment(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_string(LogSeverity::Comment, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य log_highlight(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_string(LogSeverity::Highlight, LogSourceLocation{}, {}, fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य log_formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view उपसर्गः_2,
		FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_string(severity, src, उपसर्गः_2, fmt.format_string(), fmt.format(तर्काः_2...));
}

#परिभाषय log_debug(...) कुरु { यदि (ys_debug(1)) YOSYS_NAMESPACE_उपसर्गः log_formatted_string(YOSYS_NAMESPACE_उपसर्गः LogSeverity::Debug, {}, {}, __VA_ARGS__); } यावत् (0)

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य log_शीर्षकम्(RTLIL::Design *design, FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_शीर्षकम्(design, fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य log_warning(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_warning(LogSourceLocation{}, "Warning: ", fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
अन्तःस्थ शून्य log_warning_noprefix(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_warning(LogSourceLocation{}, {}, fmt.format_string(), fmt.format(तर्काः_2...));
}

अन्तःस्थ शून्य log_experimental(ध्रुव std::string &str)
{
	logger().सङ्कलय_experimental(str);
}

अन्तःस्थ शून्य log_deprecated(ध्रुव std::string &feature, ध्रुव std::string &earliest_removal = "")
{
	logger().सङ्कलय_deprecated(feature, earliest_removal);
}

// Log with filename to report a problem in a source file.
प्रतिरूपम् <typename... तर्काः_3>
शून्य log_file_warning(LogSourceLocation src, FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_warning(src, "Warning: ", fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
शून्य log_file_info(LogSourceLocation src, FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_string(LogSeverity::Info, src, "NOTE: ", fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
[[noreturn]] शून्य log_error(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_error(LogSourceLocation{}, "ERROR: ", fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
[[noreturn]] शून्य log_file_error(LogSourceLocation src, FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_error(src, "ERROR: ", fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
शून्य log_file_nonfatal_error(LogSourceLocation src, FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_nonfatal_error(src, "ERROR: ", fmt.format_string(), fmt.format(तर्काः_2...));
}

प्रतिरूपम् <typename... तर्काः_3>
[[noreturn]] शून्य log_cmd_error(FmtString<Type_तटस्थः<तर्काः_3>...> fmt, ध्रुव तर्काः_3 &... तर्काः_2)
{
	logger().formatted_cmd_error(fmt.format_string(), fmt.format(तर्काः_2...));
}

अन्तःस्थ शून्य log_suppressed()
{
	logger().suppressed();
}

अन्तःस्थ शून्य log_spacer() { logger().spacer(); }
अन्तःस्थ शून्य log_प्रक्षिप() { logger().प्रक्षिप(); }
अन्तःस्थ शून्य log_निष्कासय() { logger().निष्कासय(); }

अन्तःस्थ शून्य log_reset_राशिः() { logger().reset_राशिः(); }
अन्तःस्थ शून्य log_flush() { logger().flush(); }

शून्य log_backtrace(ध्रुव वर्ण *उपसर्गः_2, पूर्णाङ्क स्तराः);


std::string log_signal(ध्रुव RTLIL::SigSpec &sig, bool autoint = true);
std::string log_const(ध्रुव RTLIL::Const &मानम्, bool autoint = true);

शून्य log_module(RTLIL::Module *module, std::string indent = "");
शून्य log_cell(RTLIL::Cell *cell, std::string indent = "");
शून्य log_wire(RTLIL::Wire *wire, std::string indent = "");

[[noreturn]]
शून्य log_abort_आन्तरिकम्(ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः);
#परिभाषय log_abort() YOSYS_NAMESPACE_उपसर्गः log_abort_आन्तरिकम्(__FILE__, __LINE__)
#परिभाषय log_ping() YOSYS_NAMESPACE_उपसर्गः लघुगणकः("-- %s:%d %s --\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)


// ------------------------------------------------------------
// everything below this line are utilities for troubleshooting
// ------------------------------------------------------------

// simple timer for performance measurements
// toggle the '#if 1' to get a baseline for the performance penalty added by the measurement
संरचना PerformanceTimer
{
#यदि 1
	int64_t योग_ns;

	PerformanceTimer() {
		योग_ns = 0;
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
		योग_ns = 0;
	}

	शून्य begin() {
		योग_ns -= query();
	}

	शून्य अन्तः() {
		योग_ns += query();
	}

	भिन्न sec() ध्रुव {
		प्रत्यागम योग_ns * 1e-9f;
	}
#अन्यथा
	स्थिर int64_t query() { प्रत्यागम 0; }
	शून्य reset() { }
	शून्य begin() { }
	शून्य अन्तः() { }
	भिन्न sec() ध्रुव { प्रत्यागम 0; }
#यदि_अन्तः
};

// simple API for quickly dumping values when debugging

स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(ह्रस्व v) { लघुगणकः("%d", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(अचिह्नित ह्रस्व v) { लघुगणकः("%u", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(पूर्णाङ्क v) { लघुगणकः("%d", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(अचिह्नित पूर्णाङ्क v) { लघुगणकः("%u", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(दीर्घ पूर्णाङ्क v) { लघुगणकः("%ld", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(अचिह्नित दीर्घ पूर्णाङ्क v) { लघुगणकः("%lu", v); }
#यदि_अपरिभाषितम् _WIN32
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(दीर्घ दीर्घ पूर्णाङ्क v) { लघुगणकः("%lld", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(अचिह्नित दीर्घ दीर्घ पूर्णाङ्क v) { लघुगणकः("%lld", v); }
#यदि_अन्तः
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(वर्ण c)
{
	यदि (c >= 32 && c < 127) {
		लघुगणकः("'%c'", c);
	} अन्यथा {
		लघुगणकः("'\\x%02x'", c);
	}
}
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(अचिह्नित वर्ण c)
{
	यदि (c >= 32 && c < 127) {
		लघुगणकः("'%c'", c);
	} अन्यथा {
		लघुगणकः("'\\x%02x'", c);
	}
}
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(bool v) { लघुगणकः("%s", v ? "true" : "false"); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(द्विगुण v) { लघुगणकः("%f", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(वर्ण *v) { लघुगणकः("%s", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(ध्रुव वर्ण *v) { लघुगणकः("%s", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(std::string v) { लघुगणकः("%s", v); }
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(PerformanceTimer p) { लघुगणकः("%f seconds", p.sec()); }
स्थिर अन्तःस्थ शून्य log_dump_तर्काः_कार्यकर्ता(ध्रुव वर्ण *p) { log_assert(*p == 0); }
शून्य log_dump_val_कार्यकर्ता(IdString v);
शून्य log_dump_val_कार्यकर्ता(RTLIL::SigSpec v);
शून्य log_dump_val_कार्यकर्ता(RTLIL::State v);

प्रतिरूपम्<typename K, typename T> स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(dict<K, T> &v);
प्रतिरूपम्<typename K> स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(pool<K> &v);
प्रतिरूपम्<typename K> स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(std::सदिशम्<K> &v);
प्रतिरूपम्<typename T> स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(T *ptr);

प्रतिरूपम्<typename K, typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(dict<K, T> &v) {
	लघुगणकः("{");
	bool first = true;
	प्रति (स्वचालित &it : v) {
		लघुगणकः("%s ", first ? "" : ",");
		log_dump_val_कार्यकर्ता(it.first);
		लघुगणकः(": ");
		log_dump_val_कार्यकर्ता(it.second);
		first = false;
	}
	लघुगणकः(" }");
}

प्रतिरूपम्<typename K>
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(pool<K> &v) {
	लघुगणकः("{");
	bool first = true;
	प्रति (स्वचालित &it : v) {
		लघुगणकः("%s ", first ? "" : ",");
		log_dump_val_कार्यकर्ता(it);
		first = false;
	}
	लघुगणकः(" }");
}

प्रतिरूपम्<typename K>
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(std::सदिशम्<K> &v) {
	लघुगणकः("{");
	bool first = true;
	प्रति (स्वचालित &it : v) {
		लघुगणकः("%s ", first ? "" : ",");
		log_dump_val_कार्यकर्ता(it);
		first = false;
	}
	लघुगणकः(" }");
}

प्रतिरूपम्<typename T>
स्थिर अन्तःस्थ शून्य log_dump_val_कार्यकर्ता(T *ptr) { लघुगणकः("%p", ptr); }

प्रतिरूपम्<typename T, typename ... तर्काः_3>
शून्य log_dump_तर्काः_कार्यकर्ता(ध्रुव वर्ण *p, T first, तर्काः_3 ... तर्काः_2)
{
	पूर्णाङ्क अग्र_p_state = 0;
	ध्रुव वर्ण *अग्र_p = p;
	यावत् (*अग्र_p && (अग्र_p_state != 0 || *अग्र_p != ',')) {
		यदि (*अग्र_p == '"')
			कुरु {
				अग्र_p++;
				यावत् (*अग्र_p == '\\' && *(अग्र_p + 1))
					अग्र_p += 2;
			} यावत् (*अग्र_p && *अग्र_p != '"');
		यदि (*अग्र_p == '\'') {
			अग्र_p++;
			यदि (*अग्र_p == '\\')
				अग्र_p++;
			यदि (*अग्र_p)
				अग्र_p++;
		}
		यदि (*अग्र_p == '(' || *अग्र_p == '[' || *अग्र_p == '{')
			अग्र_p_state++;
		यदि ((*अग्र_p == ')' || *अग्र_p == ']' || *अग्र_p == '}') && अग्र_p_state > 0)
			अग्र_p_state--;
		अग्र_p++;
	}
	लघुगणकः("\n\t%.*s => ", पूर्णाङ्क(अग्र_p - p), p);
	यदि (*अग्र_p == ',')
		अग्र_p++;
	यावत् (*अग्र_p == ' ' || *अग्र_p == '\t' || *अग्र_p == '\r' || *अग्र_p == '\n')
		अग्र_p++;
	log_dump_val_कार्यकर्ता(first);
	log_dump_तर्काः_कार्यकर्ता(अग्र_p, तर्काः_2 ...);
}

#परिभाषय log_dump(...) कुरु { \
	लघुगणकः("DEBUG DUMP IN %s AT %s:%d:", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	log_dump_तर्काः_कार्यकर्ता(#__VA_ARGS__, __VA_ARGS__); \
	लघुगणकः("\n"); \
} यावत् (0)

YOSYS_NAMESPACE_अन्तः

#अन्तर्भाव "kernel/yosys.h"

#यदि_अन्तः

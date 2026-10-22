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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "libs/sha1/sha1.h"
#അന്തര്ഭാവ "backends/rtlil/rtlil_backend.h"

#എങ്കിൽ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#എങ്കിൽ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <vector>
#അന്തര്ഭാവ <list>

YOSYS_NAMESPACE_BEGIN

തർക്കം log_stderr_sink_forced = അസത്യം;

LogManager &logger()
{
	സ്ഥിതം LogManager instance;
	തിരികെ instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() സ്ഥിരം
{
	സ്ഥിതം സ്ഥിരം auto initial_time = std::chrono::steady_clock::now();
	തിരികെ initial_time;
}

ശൂന്യം (*log_error_atexit)() = nullptr;
ശൂന്യം (*log_verific_callback)(പൂർണ്ണസംഖ്യ സന്ദേശം_type, സ്ഥിരം അക്ഷരം *message_id, LogSourceLocation src, സ്ഥിരം അക്ഷരം *സന്ദേശം) = nullptr;

സ്ഥിതം തർക്കം next_print_log = സത്യം;

FileLogSink::FileLogSink(സ്ഥിരം std::string &filename, തർക്കം വരി_buffered, തർക്കം append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	എങ്കിൽ (!file)
		എറിയുക std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	എങ്കിൽ (വരി_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	എങ്കിൽ (file)
		fclose(file);
}

ശൂന്യം FileLogSink::log(സ്ഥിരം LogMessage &സന്ദേശം)
{
	fputs(സന്ദേശം.cached_സന്ദേശം.c_str(), file);
}

ശൂന്യം FileLogSink::flush()
{
	fflush(file);
}

ശൂന്യം ConsoleLogSink::log(സ്ഥിരം LogMessage &സന്ദേശം)
{
	FILE *file = (സന്ദേശം.severity == LogSeverity::Error || സന്ദേശം.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(സന്ദേശം.cached_സന്ദേശം.c_str(), file);
}

ശൂന്യം ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

തർക്കം StderrLogSink::should_log(സ്ഥിരം LogMessage &സന്ദേശം) സ്ഥിരം
{
	തിരികെ സന്ദേശം.severity == LogSeverity::Error || സന്ദേശം.severity == LogSeverity::NonFatalError ||
			(സന്ദേശം.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

ശൂന്യം StderrLogSink::log(സ്ഥിരം LogMessage &സന്ദേശം)
{
	fputs(സന്ദേശം.cached_സന്ദേശം.c_str(), stderr);
}

ശൂന്യം StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

ശൂന്യം ScratchPadLogSink::log(സ്ഥിരം LogMessage &സന്ദേശം)
{
	RTLIL::Design *design = yosys_get_design();
	എങ്കിൽ (!design)
		തിരികെ;

	design->scratchpad[scratchpad].append(സന്ദേശം.cached_സന്ദേശം);
}

LogMessage::LogMessage(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string_view message) :
	severity(severity),
	prefix(prefix),
	format(format),
	message(message),
	timestamp(std::chrono::steady_clock::now()),
	src(src)
{
	std::string time_str;
	എങ്കിൽ (logger().get_log_time())
	{
		എങ്കിൽ (next_print_log) {
			next_print_log = അസത്യം;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", പൂർണ്ണസംഖ്യ(us.എണ്ണം() / 1'000'000),പൂർണ്ണസംഖ്യ(us.എണ്ണം() % 1'000'000));
		}

		എങ്കിൽ (!format.empty() && format.back() == '\n')
			next_print_log = സത്യം;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		എങ്കിൽ (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = സത്യം;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_വരി) : "";
	cached_സന്ദേശം = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

ശൂന്യം LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	ആകുമ്പോൾ (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	എങ്കിൽ (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_വരി) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	എങ്കിൽ (str.empty())
		തിരികെ;

	size_t nnl_pos = str.find_last_not_of('\n');
	എങ്കിൽ (nnl_pos == std::string::npos)
		newline_എണ്ണം += GetSize(str);
	അല്ലെങ്കിൽ
		newline_എണ്ണം = GetSize(str) - nnl_pos - 1;

	എങ്കിൽ (hasher)
		hasher->update(str);

	auto സന്ദേശം = LogMessage(severity, src, prefix, format, str_in);
	വേണ്ടി (auto &sink : sinks) {
		എങ്കിൽ (sink->should_log(സന്ദേശം))
			sink->log(സന്ദേശം);
	}

	എങ്കിൽ (severity == LogSeverity::Header)
		str = str_in;

	സ്ഥിതം std::string linebuffer;
	സ്ഥിതം തർക്കം warn_regex_recusion_guard = അസത്യം;

	എങ്കിൽ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = സത്യം;

		എങ്കിൽ (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		അല്ലെങ്കിൽ
		{
			linebuffer += str;

			എങ്കിൽ (!linebuffer.empty() && linebuffer.back() == '\n') {
				വേണ്ടി (auto &re : warn_regexes)
					എങ്കിൽ (std::regex_തിരയൽ(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				വേണ്ടി (auto &[_, item] : expect_log)
					എങ്കിൽ (std::regex_തിരയൽ(linebuffer, item.pattern))
						item.current_എണ്ണം++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = അസത്യം;
	}
}

ശൂന്യം LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	എങ്കിൽ (make_debug && !is_debug(1))
		തിരികെ;
	logv_string(severity, src, prefix, format, std::move(str));
}

ശൂന്യം LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	എങ്കിൽ (header_എണ്ണം.size() > 0)
		header_എണ്ണം.back()++;

	എങ്കിൽ (പൂർണ്ണസംഖ്യ(header_എണ്ണം.size()) <= verbose_level) {
		log_stderr_sink_forced = സത്യം;
	}

	std::string header_id;

	വേണ്ടി (പൂർണ്ണസംഖ്യ c : header_എണ്ണം)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	എങ്കിൽ (hdump_all)
		hdump[header_id].തിരുകുക("yosys_dump_" + header_id + ".il");

	എങ്കിൽ (hdump.എണ്ണം(header_id) && design != nullptr)
		വേണ്ടി (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = അസത്യം;
}

ശൂന്യം LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	തർക്കം suppressed = അസത്യം;

	വേണ്ടി (auto &re : nowarn_regexes)
		എങ്കിൽ (std::regex_തിരയൽ(message, re))
			suppressed = സത്യം;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_വരി) : "";
	എങ്കിൽ (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	അല്ലെങ്കിൽ
	{
		പൂർണ്ണസംഖ്യ bak_make_debug = make_debug;
		make_debug = 0;

		വേണ്ടി (auto &re : werror_regexes)
			എങ്കിൽ (std::regex_തിരയൽ(message, re))
				formatted_error(src, "ERROR: ", format, message);

		തർക്കം warning_match = അസത്യം;
		വേണ്ടി (auto &[_, item] : expect_warning)
			എങ്കിൽ (std::regex_തിരയൽ(message, item.pattern)) {
				item.current_എണ്ണം++;
				warning_match = സത്യം;
			}

		std::string pattern = loc + string(prefix) + message;
		വേണ്ടി (auto &[_, item] : expect_prefix_warning)
			എങ്കിൽ (std::regex_തിരയൽ(pattern, item.pattern)) {
				item.current_എണ്ണം++;
				warning_match = സത്യം;
			}

		എങ്കിൽ (warnings.എണ്ണം(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		അല്ലെങ്കിൽ
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.തിരുകുക(message);
		}

		എങ്കിൽ (!warning_match)
			warnings_എണ്ണം_noexpect++;
		warnings_എണ്ണം++;
		make_debug = bak_make_debug;
	}
}

ശൂന്യം LogManager::suppressed() {
	എങ്കിൽ (debug_suppressed && !make_debug) {
		constexpr സ്ഥിരം അക്ഷരം* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

ശൂന്യം LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	പൂർണ്ണസംഖ്യ bak_make_debug = make_debug;
	make_debug = 0;
	എങ്കിൽ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	വേണ്ടി (auto &[_, item] : expect_error)
		എങ്കിൽ (std::regex_തിരയൽ(message, item.pattern))
			item.current_എണ്ണം++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_വരി) : "";
	std::string pattern = loc + string(prefix) + message;
	വേണ്ടി (auto &[_, item] : expect_prefix_error)
		എങ്കിൽ (std::regex_തിരയൽ(pattern, item.pattern))
			item.current_എണ്ണം++;

	errors_എണ്ണം++;
}

[[noreturn]]
ശൂന്യം LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	എങ്കിൽ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	സ്ഥിരം അക്ഷരം *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	എങ്കിൽ (e && atoi(e))
		abort();

#എങ്കിൽ defined(_MSC_VER)
	_exit(1);
#അല്ലെങ്കിൽ
	_Exit(1);
#endif
}

ശൂന്യം LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

ശൂന്യം LogManager::add_experimental(സ്ഥിരം std::string &str)
{
	എങ്കിൽ (experimental_ignored.എണ്ണം(str) == 0 && experimental.എണ്ണം(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.തിരുകുക(str);
	}
}

ശൂന്യം LogManager::add_deprecated(സ്ഥിരം std::string &feature, സ്ഥിരം std::string &earliest_removal)
{
	എങ്കിൽ (deprecated.എണ്ണം(feature) == 0) {
		എങ്കിൽ (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		അല്ലെങ്കിൽ
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

ശൂന്യം log_assert_failure(സ്ഥിരം അക്ഷരം *വ്യഞ്ജകം, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", വ്യഞ്ജകം, location.file_name(), location.വരി());
}

ശൂന്യം log_abort_internal(സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി)
{
	log_error("Abort in %s:%d.\n", file, വരി);
}

ശൂന്യം log_yosys_abort_message(std::string_view file, പൂർണ്ണസംഖ്യ വരി, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, വരി, func, message);
}

ശൂന്യം LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	എങ്കിൽ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		എറിയുക log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

ശൂന്യം LogManager::spacer()
{
	എങ്കിൽ (newline_എണ്ണം < 2) log("\n");
	എങ്കിൽ (newline_എണ്ണം < 2) log("\n");
}

ശൂന്യം LogManager::clear_original()
{
	std::erase_if(sinks, [](സ്ഥിരം auto &sink) { തിരികെ dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

ശൂന്യം LogManager::തള്ളുക()
{
	header_എണ്ണം.തള്ളുക_back(0);
}

ശൂന്യം LogManager::എടുക്കുക()
{
	header_എണ്ണം.എടുക്കുക_back();
	flush();
}

#എങ്കിൽ defined(YOSYS_ENABLE_DLOPEN)
ശൂന്യം log_backtrace(സ്ഥിരം അക്ഷരം *prefix, പൂർണ്ണസംഖ്യ levels)
{
	എങ്കിൽ (levels <= 0) തിരികെ;

	Dl_info dli;
	ശൂന്യം *p;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #1: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 1) തിരികെ;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#അല്ലെങ്കിൽ
	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #2: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 2) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #3: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 3) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #4: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 4) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #5: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 5) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #6: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 6) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #7: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 7) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #8: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 8) തിരികെ;

	എങ്കിൽ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} അല്ലെങ്കിൽ {
		log("%sframe #9: ---\n", prefix);
		തിരികെ;
	}

	എങ്കിൽ (levels <= 9) തിരികെ;
#endif
}
#അല്ലെങ്കിൽ
ശൂന്യം log_backtrace(സ്ഥിരം അക്ഷരം*, പൂർണ്ണസംഖ്യ) { }
#endif

ശൂന്യം LogManager::reset_അട്ടി()
{
	ആകുമ്പോൾ (header_എണ്ണം.size() > 1)
		header_എണ്ണം.എടുക്കുക_back();
	flush();
}

ശൂന്യം log_dump_val_worker(RTLIL::IdString v) {
	എങ്കിൽ (v == IdString::Null)
		log("(null)");
	അല്ലെങ്കിൽ എങ്കിൽ (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	അല്ലെങ്കിൽ എങ്കിൽ (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	അല്ലെങ്കിൽ
		log("%s", v.handle_token());
}

ശൂന്യം log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

ശൂന്യം log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(സ്ഥിരം RTLIL::SigSpec &sig, തർക്കം autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ buf.str();
}

std::string log_const(സ്ഥിരം RTLIL::Const &മൂല്യം, തർക്കം autoint)
{
	എങ്കിൽ ((മൂല്യം.flags & RTLIL::CONST_FLAG_STRING) == 0)
		തിരികെ log_signal(മൂല്യം, autoint);

	തിരികെ "\"" + മൂല്യം.decode_string() + "\"";
}

ശൂന്യം log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, അസത്യം, സത്യം, അസത്യം, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ശൂന്യം log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ശൂന്യം log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ശൂന്യം LogManager::check_expected()
{
	// copy out all of the expected logs so that they cannot be re-checked
	// or match against themselves
	dict<std::string, LogExpectedItem> expect_log_copy, expect_warning_copy, expect_error_copy;
	dict<std::string, LogExpectedItem> expect_prefix_log_copy, expect_prefix_warning_copy, expect_prefix_error_copy;
	std::swap(expect_warning_copy, expect_warning);
	std::swap(expect_log_copy, expect_log);
	std::swap(expect_error_copy, expect_error);
	std::swap(expect_prefix_warning_copy, expect_prefix_warning);
	std::swap(expect_prefix_log_copy, expect_prefix_log);
	std::swap(expect_prefix_error_copy, expect_prefix_error);

	auto check = [&](സ്ഥിരം std::string kind, std::string pattern, LogExpectedItem item) {
		എങ്കിൽ (item.current_എണ്ണം == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		എങ്കിൽ (item.current_എണ്ണം != item.expected_എണ്ണം) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_എണ്ണം, item.expected_എണ്ണം);
		}
	};

	വേണ്ടി (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	വേണ്ടി (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	വേണ്ടി (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	വേണ്ടി (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](സ്ഥിരം std::string kind, std::string pattern, LogExpectedItem item) {
		എങ്കിൽ (item.current_എണ്ണം == item.expected_എണ്ണം) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#എങ്കിൽ defined(_MSC_VER)
				_exit(0);
			#അല്ലെങ്കിൽ
				_Exit(0);
			#endif
		} അല്ലെങ്കിൽ {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	വേണ്ടി (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	വേണ്ടി (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

ശൂന്യം LogManager::report_unexpected_error()
{
	എങ്കിൽ (expect_no_warnings && warnings_എണ്ണം_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_എണ്ണം, warnings_എണ്ണം - warnings_എണ്ണം_noexpect);
}

ശൂന്യം LogManager::add_expect(std::string type, std::string pattern, പൂർണ്ണസംഖ്യ എണ്ണം)
{
	എങ്കിൽ (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), എണ്ണം);
	അല്ലെങ്കിൽ എങ്കിൽ (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), എണ്ണം);
	അല്ലെങ്കിൽ log_abort();
}

ശൂന്യം LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	എങ്കിൽ (!hasher)
		തിരികെ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	തിരികെ hash;
}

YOSYS_NAMESPACE_END

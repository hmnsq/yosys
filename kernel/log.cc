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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/sha1/sha1.h"
#ಅನ್ತರ್ಭಾವ "backends/rtlil/rtlil_backend.h"

#ಯದಿ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#ಯದಿ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <list>

YOSYS_NAMESPACE_BEGIN

ತಾರ್ಕಿಕ log_stderr_sink_forced = ಅಸತ್ಯ;

LogManager &logger()
{
	ಸ್ಥಿರ LogManager instance;
	ಪ್ರತ್ಯಾಗಮ instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ಧ್ರುವ
{
	ಸ್ಥಿರ ಧ್ರುವ auto initial_time = std::chrono::steady_clock::now();
	ಪ್ರತ್ಯಾಗಮ initial_time;
}

ಶೂನ್ಯ (*log_error_atexit)() = nullptr;
ಶೂನ್ಯ (*log_verific_callback)(ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_type, ಧ್ರುವ ವರ್ಣ *message_id, LogSourceLocation src, ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ) = nullptr;

ಸ್ಥಿರ ತಾರ್ಕಿಕ next_print_log = ಸತ್ಯ;

FileLogSink::FileLogSink(ಧ್ರುವ std::string &filename, ತಾರ್ಕಿಕ ಸಾಲು_buffered, ತಾರ್ಕಿಕ append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	ಯದಿ (!file)
		ಕ್ಷಿಪ std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	ಯದಿ (ಸಾಲು_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	ಯದಿ (file)
		fclose(file);
}

ಶೂನ್ಯ FileLogSink::log(ಧ್ರುವ LogMessage &ಸಂದೇಶ)
{
	fputs(ಸಂದೇಶ.cached_ಸಂದೇಶ.c_str(), file);
}

ಶೂನ್ಯ FileLogSink::flush()
{
	fflush(file);
}

ಶೂನ್ಯ ConsoleLogSink::log(ಧ್ರುವ LogMessage &ಸಂದೇಶ)
{
	FILE *file = (ಸಂದೇಶ.severity == LogSeverity::Error || ಸಂದೇಶ.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(ಸಂದೇಶ.cached_ಸಂದೇಶ.c_str(), file);
}

ಶೂನ್ಯ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ತಾರ್ಕಿಕ StderrLogSink::should_log(ಧ್ರುವ LogMessage &ಸಂದೇಶ) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ.severity == LogSeverity::Error || ಸಂದೇಶ.severity == LogSeverity::NonFatalError ||
			(ಸಂದೇಶ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

ಶೂನ್ಯ StderrLogSink::log(ಧ್ರುವ LogMessage &ಸಂದೇಶ)
{
	fputs(ಸಂದೇಶ.cached_ಸಂದೇಶ.c_str(), stderr);
}

ಶೂನ್ಯ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

ಶೂನ್ಯ ScratchPadLogSink::log(ಧ್ರುವ LogMessage &ಸಂದೇಶ)
{
	RTLIL::Design *design = yosys_get_design();
	ಯದಿ (!design)
		ಪ್ರತ್ಯಾಗಮ;

	design->scratchpad[scratchpad].append(ಸಂದೇಶ.cached_ಸಂದೇಶ);
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
	ಯದಿ (logger().get_log_time())
	{
		ಯದಿ (next_print_log) {
			next_print_log = ಅಸತ್ಯ;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", ಪೂರ್ಣಾಙ್ಕ(us.ಎಣಿಕೆ() / 1'000'000),ಪೂರ್ಣಾಙ್ಕ(us.ಎಣಿಕೆ() % 1'000'000));
		}

		ಯದಿ (!format.empty() && format.back() == '\n')
			next_print_log = ಸತ್ಯ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		ಯದಿ (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = ಸತ್ಯ;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ಸಾಲು) : "";
	cached_ಸಂದೇಶ = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

ಶೂನ್ಯ LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	ಯಾವತ್ (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	ಯದಿ (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ಸಾಲು) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	ಯದಿ (str.empty())
		ಪ್ರತ್ಯಾಗಮ;

	size_t nnl_pos = str.find_last_not_of('\n');
	ಯದಿ (nnl_pos == std::string::npos)
		newline_ಎಣಿಕೆ += GetSize(str);
	ಅನ್ಯಥಾ
		newline_ಎಣಿಕೆ = GetSize(str) - nnl_pos - 1;

	ಯದಿ (hasher)
		hasher->update(str);

	auto ಸಂದೇಶ = LogMessage(severity, src, prefix, format, str_in);
	ಪ್ರತಿ (auto &sink : sinks) {
		ಯದಿ (sink->should_log(ಸಂದೇಶ))
			sink->log(ಸಂದೇಶ);
	}

	ಯದಿ (severity == LogSeverity::Header)
		str = str_in;

	ಸ್ಥಿರ std::string linebuffer;
	ಸ್ಥಿರ ತಾರ್ಕಿಕ warn_regex_recusion_guard = ಅಸತ್ಯ;

	ಯದಿ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = ಸತ್ಯ;

		ಯದಿ (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		ಅನ್ಯಥಾ
		{
			linebuffer += str;

			ಯದಿ (!linebuffer.empty() && linebuffer.back() == '\n') {
				ಪ್ರತಿ (auto &re : warn_regexes)
					ಯದಿ (std::regex_ಹುಡುಕು(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				ಪ್ರತಿ (auto &[_, item] : expect_log)
					ಯದಿ (std::regex_ಹುಡುಕು(linebuffer, item.pattern))
						item.current_ಎಣಿಕೆ++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = ಅಸತ್ಯ;
	}
}

ಶೂನ್ಯ LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	ಯದಿ (make_debug && !is_debug(1))
		ಪ್ರತ್ಯಾಗಮ;
	logv_string(severity, src, prefix, format, std::move(str));
}

ಶೂನ್ಯ LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	ಯದಿ (header_ಎಣಿಕೆ.size() > 0)
		header_ಎಣಿಕೆ.back()++;

	ಯದಿ (ಪೂರ್ಣಾಙ್ಕ(header_ಎಣಿಕೆ.size()) <= verbose_level) {
		log_stderr_sink_forced = ಸತ್ಯ;
	}

	std::string header_id;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ c : header_ಎಣಿಕೆ)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	ಯದಿ (hdump_all)
		hdump[header_id].ಸೇರಿಸು("yosys_dump_" + header_id + ".il");

	ಯದಿ (hdump.ಎಣಿಕೆ(header_id) && design != nullptr)
		ಪ್ರತಿ (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = ಅಸತ್ಯ;
}

ಶೂನ್ಯ LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	ತಾರ್ಕಿಕ suppressed = ಅಸತ್ಯ;

	ಪ್ರತಿ (auto &re : nowarn_regexes)
		ಯದಿ (std::regex_ಹುಡುಕು(message, re))
			suppressed = ಸತ್ಯ;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ಸಾಲು) : "";
	ಯದಿ (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	ಅನ್ಯಥಾ
	{
		ಪೂರ್ಣಾಙ್ಕ bak_make_debug = make_debug;
		make_debug = 0;

		ಪ್ರತಿ (auto &re : werror_regexes)
			ಯದಿ (std::regex_ಹುಡುಕು(message, re))
				formatted_error(src, "ERROR: ", format, message);

		ತಾರ್ಕಿಕ warning_match = ಅಸತ್ಯ;
		ಪ್ರತಿ (auto &[_, item] : expect_warning)
			ಯದಿ (std::regex_ಹುಡುಕು(message, item.pattern)) {
				item.current_ಎಣಿಕೆ++;
				warning_match = ಸತ್ಯ;
			}

		std::string pattern = loc + string(prefix) + message;
		ಪ್ರತಿ (auto &[_, item] : expect_prefix_warning)
			ಯದಿ (std::regex_ಹುಡುಕು(pattern, item.pattern)) {
				item.current_ಎಣಿಕೆ++;
				warning_match = ಸತ್ಯ;
			}

		ಯದಿ (warnings.ಎಣಿಕೆ(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		ಅನ್ಯಥಾ
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.ಸೇರಿಸು(message);
		}

		ಯದಿ (!warning_match)
			warnings_ಎಣಿಕೆ_noexpect++;
		warnings_ಎಣಿಕೆ++;
		make_debug = bak_make_debug;
	}
}

ಶೂನ್ಯ LogManager::suppressed() {
	ಯದಿ (debug_suppressed && !make_debug) {
		constexpr ಧ್ರುವ ವರ್ಣ* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

ಶೂನ್ಯ LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	ಪೂರ್ಣಾಙ್ಕ bak_make_debug = make_debug;
	make_debug = 0;
	ಯದಿ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	ಪ್ರತಿ (auto &[_, item] : expect_error)
		ಯದಿ (std::regex_ಹುಡುಕು(message, item.pattern))
			item.current_ಎಣಿಕೆ++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ಸಾಲು) : "";
	std::string pattern = loc + string(prefix) + message;
	ಪ್ರತಿ (auto &[_, item] : expect_prefix_error)
		ಯದಿ (std::regex_ಹುಡುಕು(pattern, item.pattern))
			item.current_ಎಣಿಕೆ++;

	errors_ಎಣಿಕೆ++;
}

[[noreturn]]
ಶೂನ್ಯ LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	ಯದಿ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ಧ್ರುವ ವರ್ಣ *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	ಯದಿ (e && atoi(e))
		abort();

#ಯದಿ defined(_MSC_VER)
	_exit(1);
#ಅನ್ಯಥಾ
	_Exit(1);
#endif
}

ಶೂನ್ಯ LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

ಶೂನ್ಯ LogManager::add_experimental(ಧ್ರುವ std::string &str)
{
	ಯದಿ (experimental_ignored.ಎಣಿಕೆ(str) == 0 && experimental.ಎಣಿಕೆ(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ಸೇರಿಸು(str);
	}
}

ಶೂನ್ಯ LogManager::add_deprecated(ಧ್ರುವ std::string &feature, ಧ್ರುವ std::string &earliest_removal)
{
	ಯದಿ (deprecated.ಎಣಿಕೆ(feature) == 0) {
		ಯದಿ (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		ಅನ್ಯಥಾ
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

ಶೂನ್ಯ log_assert_failure(ಧ್ರುವ ವರ್ಣ *ಅಭಿವ್ಯಕ್ತಿ, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", ಅಭಿವ್ಯಕ್ತಿ, location.file_name(), location.ಸಾಲು());
}

ಶೂನ್ಯ log_abort_internal(ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು)
{
	log_error("Abort in %s:%d.\n", file, ಸಾಲು);
}

ಶೂನ್ಯ log_yosys_abort_message(std::string_view file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, ಸಾಲು, func, message);
}

ಶೂನ್ಯ LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	ಯದಿ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		ಕ್ಷಿಪ log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

ಶೂನ್ಯ LogManager::spacer()
{
	ಯದಿ (newline_ಎಣಿಕೆ < 2) log("\n");
	ಯದಿ (newline_ಎಣಿಕೆ < 2) log("\n");
}

ಶೂನ್ಯ LogManager::clear_original()
{
	std::erase_if(sinks, [](ಧ್ರುವ auto &sink) { ಪ್ರತ್ಯಾಗಮ dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

ಶೂನ್ಯ LogManager::ತಳ್ಳು()
{
	header_ಎಣಿಕೆ.ತಳ್ಳು_back(0);
}

ಶೂನ್ಯ LogManager::ತೆಗೆ()
{
	header_ಎಣಿಕೆ.ತೆಗೆ_back();
	flush();
}

#ಯದಿ defined(YOSYS_ENABLE_DLOPEN)
ಶೂನ್ಯ log_backtrace(ಧ್ರುವ ವರ್ಣ *prefix, ಪೂರ್ಣಾಙ್ಕ levels)
{
	ಯದಿ (levels <= 0) ಪ್ರತ್ಯಾಗಮ;

	Dl_info dli;
	ಶೂನ್ಯ *p;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #1: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 1) ಪ್ರತ್ಯಾಗಮ;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#ಅನ್ಯಥಾ
	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #2: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 2) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #3: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 3) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #4: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 4) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #5: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 5) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #6: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 6) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #7: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 7) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #8: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 8) ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ಅನ್ಯಥಾ {
		log("%sframe #9: ---\n", prefix);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (levels <= 9) ಪ್ರತ್ಯಾಗಮ;
#endif
}
#ಅನ್ಯಥಾ
ಶೂನ್ಯ log_backtrace(ಧ್ರುವ ವರ್ಣ*, ಪೂರ್ಣಾಙ್ಕ) { }
#endif

ಶೂನ್ಯ LogManager::reset_ರಾಶಿ()
{
	ಯಾವತ್ (header_ಎಣಿಕೆ.size() > 1)
		header_ಎಣಿಕೆ.ತೆಗೆ_back();
	flush();
}

ಶೂನ್ಯ log_dump_val_worker(RTLIL::IdString v) {
	ಯದಿ (v == IdString::Null)
		log("(null)");
	ಅನ್ಯಥಾ ಯದಿ (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	ಅನ್ಯಥಾ ಯದಿ (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	ಅನ್ಯಥಾ
		log("%s", v.handle_token());
}

ಶೂನ್ಯ log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

ಶೂನ್ಯ log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ಧ್ರುವ RTLIL::SigSpec &sig, ತಾರ್ಕಿಕ autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ buf.str();
}

std::string log_const(ಧ್ರುವ RTLIL::Const &ಮೌಲ್ಯ, ತಾರ್ಕಿಕ autoint)
{
	ಯದಿ ((ಮೌಲ್ಯ.flags & RTLIL::CONST_FLAG_STRING) == 0)
		ಪ್ರತ್ಯಾಗಮ log_signal(ಮೌಲ್ಯ, autoint);

	ಪ್ರತ್ಯಾಗಮ "\"" + ಮೌಲ್ಯ.decode_string() + "\"";
}

ಶೂನ್ಯ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, ಅಸತ್ಯ, ಸತ್ಯ, ಅಸತ್ಯ, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ಶೂನ್ಯ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ಶೂನ್ಯ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ಶೂನ್ಯ LogManager::check_expected()
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

	auto check = [&](ಧ್ರುವ std::string kind, std::string pattern, LogExpectedItem item) {
		ಯದಿ (item.current_ಎಣಿಕೆ == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		ಯದಿ (item.current_ಎಣಿಕೆ != item.expected_ಎಣಿಕೆ) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_ಎಣಿಕೆ, item.expected_ಎಣಿಕೆ);
		}
	};

	ಪ್ರತಿ (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	ಪ್ರತಿ (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	ಪ್ರತಿ (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	ಪ್ರತಿ (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ಧ್ರುವ std::string kind, std::string pattern, LogExpectedItem item) {
		ಯದಿ (item.current_ಎಣಿಕೆ == item.expected_ಎಣಿಕೆ) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#ಯದಿ defined(_MSC_VER)
				_exit(0);
			#ಅನ್ಯಥಾ
				_Exit(0);
			#endif
		} ಅನ್ಯಥಾ {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	ಪ್ರತಿ (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	ಪ್ರತಿ (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

ಶೂನ್ಯ LogManager::report_unexpected_error()
{
	ಯದಿ (expect_no_warnings && warnings_ಎಣಿಕೆ_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_ಎಣಿಕೆ, warnings_ಎಣಿಕೆ - warnings_ಎಣಿಕೆ_noexpect);
}

ಶೂನ್ಯ LogManager::add_expect(std::string type, std::string pattern, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
	ಯದಿ (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ಎಣಿಕೆ);
	ಅನ್ಯಥಾ ಯದಿ (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ಎಣಿಕೆ);
	ಅನ್ಯಥಾ log_abort();
}

ಶೂನ್ಯ LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	ಯದಿ (!hasher)
		ಪ್ರತ್ಯಾಗಮ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	ಪ್ರತ್ಯಾಗಮ hash;
}

YOSYS_NAMESPACE_END

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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "libs/sha1/sha1.h"
#ଅନ୍ତର୍ଭାଵ "backends/rtlil/rtlil_backend.h"

#ଯଦି !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#ଯଦି defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <vector>
#ଅନ୍ତର୍ଭାଵ <list>

YOSYS_NAMESPACE_BEGIN

ତାର୍କିକ log_stderr_sink_forced = ମିଥ୍ୟା;

LogManager &logger()
{
	ସ୍ଥିର LogManager instance;
	ଫେରନ୍ତୁ instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ଅଚଳ
{
	ସ୍ଥିର ଅଚଳ auto initial_time = std::chrono::steady_clock::now();
	ଫେରନ୍ତୁ initial_time;
}

ଶୂନ୍ୟ (*log_error_atexit)() = nullptr;
ଶୂନ୍ୟ (*log_verific_callback)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_type, ଅଚଳ ଅକ୍ଷର *message_id, LogSourceLocation src, ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ) = nullptr;

ସ୍ଥିର ତାର୍କିକ next_print_log = ସତ୍ୟ;

FileLogSink::FileLogSink(ଅଚଳ std::string &filename, ତାର୍କିକ ଧାଡ଼ି_buffered, ତାର୍କିକ append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	ଯଦି (!file)
		ଫିଙ୍ଗ std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	ଯଦି (ଧାଡ଼ି_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	ଯଦି (file)
		fclose(file);
}

ଶୂନ୍ୟ FileLogSink::log(ଅଚଳ LogMessage &ସନ୍ଦେଶ)
{
	fputs(ସନ୍ଦେଶ.cached_ସନ୍ଦେଶ.c_str(), file);
}

ଶୂନ୍ୟ FileLogSink::flush()
{
	fflush(file);
}

ଶୂନ୍ୟ ConsoleLogSink::log(ଅଚଳ LogMessage &ସନ୍ଦେଶ)
{
	FILE *file = (ସନ୍ଦେଶ.severity == LogSeverity::Error || ସନ୍ଦେଶ.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(ସନ୍ଦେଶ.cached_ସନ୍ଦେଶ.c_str(), file);
}

ଶୂନ୍ୟ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

ତାର୍କିକ StderrLogSink::should_log(ଅଚଳ LogMessage &ସନ୍ଦେଶ) ଅଚଳ
{
	ଫେରନ୍ତୁ ସନ୍ଦେଶ.severity == LogSeverity::Error || ସନ୍ଦେଶ.severity == LogSeverity::NonFatalError ||
			(ସନ୍ଦେଶ.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

ଶୂନ୍ୟ StderrLogSink::log(ଅଚଳ LogMessage &ସନ୍ଦେଶ)
{
	fputs(ସନ୍ଦେଶ.cached_ସନ୍ଦେଶ.c_str(), stderr);
}

ଶୂନ୍ୟ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

ଶୂନ୍ୟ ScratchPadLogSink::log(ଅଚଳ LogMessage &ସନ୍ଦେଶ)
{
	RTLIL::Design *design = yosys_get_design();
	ଯଦି (!design)
		ଫେରନ୍ତୁ;

	design->scratchpad[scratchpad].append(ସନ୍ଦେଶ.cached_ସନ୍ଦେଶ);
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
	ଯଦି (logger().get_log_time())
	{
		ଯଦି (next_print_log) {
			next_print_log = ମିଥ୍ୟା;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", ପୂର୍ଣ୍ଣସଂଖ୍ୟା(us.ଗଣନା_2() / 1'000'000),ପୂର୍ଣ୍ଣସଂଖ୍ୟା(us.ଗଣନା_2() % 1'000'000));
		}

		ଯଦି (!format.empty() && format.back() == '\n')
			next_print_log = ସତ୍ୟ;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		ଯଦି (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = ସତ୍ୟ;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ଧାଡ଼ି) : "";
	cached_ସନ୍ଦେଶ = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

ଶୂନ୍ୟ LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	ଯେତେବେଳେ (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	ଯଦି (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ଧାଡ଼ି) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	ଯଦି (str.empty())
		ଫେରନ୍ତୁ;

	size_t nnl_pos = str.find_last_not_of('\n');
	ଯଦି (nnl_pos == std::string::npos)
		newline_ଗଣନା += GetSize(str);
	ଅନ୍ୟଥା
		newline_ଗଣନା = GetSize(str) - nnl_pos - 1;

	ଯଦି (hasher)
		hasher->update(str);

	auto ସନ୍ଦେଶ = LogMessage(severity, src, prefix, format, str_in);
	ପାଇଁ (auto &sink : sinks) {
		ଯଦି (sink->should_log(ସନ୍ଦେଶ))
			sink->log(ସନ୍ଦେଶ);
	}

	ଯଦି (severity == LogSeverity::Header)
		str = str_in;

	ସ୍ଥିର std::string linebuffer;
	ସ୍ଥିର ତାର୍କିକ warn_regex_recusion_guard = ମିଥ୍ୟା;

	ଯଦି (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = ସତ୍ୟ;

		ଯଦି (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		ଅନ୍ୟଥା
		{
			linebuffer += str;

			ଯଦି (!linebuffer.empty() && linebuffer.back() == '\n') {
				ପାଇଁ (auto &re : warn_regexes)
					ଯଦି (std::regex_ସନ୍ଧାନ(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				ପାଇଁ (auto &[_, item] : expect_log)
					ଯଦି (std::regex_ସନ୍ଧାନ(linebuffer, item.pattern))
						item.current_ଗଣନା++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = ମିଥ୍ୟା;
	}
}

ଶୂନ୍ୟ LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	ଯଦି (make_debug && !is_debug(1))
		ଫେରନ୍ତୁ;
	logv_string(severity, src, prefix, format, std::move(str));
}

ଶୂନ୍ୟ LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	ଯଦି (header_ଗଣନା.size() > 0)
		header_ଗଣନା.back()++;

	ଯଦି (ପୂର୍ଣ୍ଣସଂଖ୍ୟା(header_ଗଣନା.size()) <= verbose_level) {
		log_stderr_sink_forced = ସତ୍ୟ;
	}

	std::string header_id;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା c : header_ଗଣନା)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	ଯଦି (hdump_all)
		hdump[header_id].ଭର୍ତ୍ତି("yosys_dump_" + header_id + ".il");

	ଯଦି (hdump.ଗଣନା_2(header_id) && design != nullptr)
		ପାଇଁ (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	ତାର୍କିକ suppressed = ମିଥ୍ୟା;

	ପାଇଁ (auto &re : nowarn_regexes)
		ଯଦି (std::regex_ସନ୍ଧାନ(message, re))
			suppressed = ସତ୍ୟ;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ଧାଡ଼ି) : "";
	ଯଦି (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	ଅନ୍ୟଥା
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bak_make_debug = make_debug;
		make_debug = 0;

		ପାଇଁ (auto &re : werror_regexes)
			ଯଦି (std::regex_ସନ୍ଧାନ(message, re))
				formatted_error(src, "ERROR: ", format, message);

		ତାର୍କିକ warning_match = ମିଥ୍ୟା;
		ପାଇଁ (auto &[_, item] : expect_warning)
			ଯଦି (std::regex_ସନ୍ଧାନ(message, item.pattern)) {
				item.current_ଗଣନା++;
				warning_match = ସତ୍ୟ;
			}

		std::string pattern = loc + string(prefix) + message;
		ପାଇଁ (auto &[_, item] : expect_prefix_warning)
			ଯଦି (std::regex_ସନ୍ଧାନ(pattern, item.pattern)) {
				item.current_ଗଣନା++;
				warning_match = ସତ୍ୟ;
			}

		ଯଦି (warnings.ଗଣନା_2(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		ଅନ୍ୟଥା
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.ଭର୍ତ୍ତି(message);
		}

		ଯଦି (!warning_match)
			warnings_ଗଣନା_noexpect++;
		warnings_ଗଣନା++;
		make_debug = bak_make_debug;
	}
}

ଶୂନ୍ୟ LogManager::suppressed() {
	ଯଦି (debug_suppressed && !make_debug) {
		constexpr ଅଚଳ ଅକ୍ଷର* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

ଶୂନ୍ୟ LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bak_make_debug = make_debug;
	make_debug = 0;
	ଯଦି (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	ପାଇଁ (auto &[_, item] : expect_error)
		ଯଦି (std::regex_ସନ୍ଧାନ(message, item.pattern))
			item.current_ଗଣନା++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_ଧାଡ଼ି) : "";
	std::string pattern = loc + string(prefix) + message;
	ପାଇଁ (auto &[_, item] : expect_prefix_error)
		ଯଦି (std::regex_ସନ୍ଧାନ(pattern, item.pattern))
			item.current_ଗଣନା++;

	errors_ଗଣନା++;
}

[[noreturn]]
ଶୂନ୍ୟ LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	ଯଦି (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ଅଚଳ ଅକ୍ଷର *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	ଯଦି (e && atoi(e))
		abort();

#ଯଦି defined(_MSC_VER)
	_exit(1);
#ଅନ୍ୟଥା
	_Exit(1);
#endif
}

ଶୂନ୍ୟ LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

ଶୂନ୍ୟ LogManager::add_experimental(ଅଚଳ std::string &str)
{
	ଯଦି (experimental_ignored.ଗଣନା_2(str) == 0 && experimental.ଗଣନା_2(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ଭର୍ତ୍ତି(str);
	}
}

ଶୂନ୍ୟ LogManager::add_deprecated(ଅଚଳ std::string &feature, ଅଚଳ std::string &earliest_removal)
{
	ଯଦି (deprecated.ଗଣନା_2(feature) == 0) {
		ଯଦି (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		ଅନ୍ୟଥା
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

ଶୂନ୍ୟ log_assert_failure(ଅଚଳ ଅକ୍ଷର *ପଦସମଷ୍ଟି, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", ପଦସମଷ୍ଟି, location.file_name(), location.ଧାଡ଼ି());
}

ଶୂନ୍ୟ log_abort_internal(ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି)
{
	log_error("Abort in %s:%d.\n", file, ଧାଡ଼ି);
}

ଶୂନ୍ୟ log_yosys_abort_message(std::string_view file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, ଧାଡ଼ି, func, message);
}

ଶୂନ୍ୟ LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	ଯଦି (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		ଫିଙ୍ଗ log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

ଶୂନ୍ୟ LogManager::spacer()
{
	ଯଦି (newline_ଗଣନା < 2) log("\n");
	ଯଦି (newline_ଗଣନା < 2) log("\n");
}

ଶୂନ୍ୟ LogManager::clear_original()
{
	std::erase_if(sinks, [](ଅଚଳ auto &sink) { ଫେରନ୍ତୁ dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

ଶୂନ୍ୟ LogManager::ଠେଲ()
{
	header_ଗଣନା.ଠେଲ_back(0);
}

ଶୂନ୍ୟ LogManager::କାଢ଼()
{
	header_ଗଣନା.କାଢ଼_back();
	flush();
}

#ଯଦି defined(YOSYS_ENABLE_DLOPEN)
ଶୂନ୍ୟ log_backtrace(ଅଚଳ ଅକ୍ଷର *prefix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା levels)
{
	ଯଦି (levels <= 0) ଫେରନ୍ତୁ;

	Dl_info dli;
	ଶୂନ୍ୟ *p;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #1: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 1) ଫେରନ୍ତୁ;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#ଅନ୍ୟଥା
	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #2: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 2) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #3: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 3) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #4: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 4) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #5: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 5) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #6: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 6) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #7: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 7) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #8: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 8) ଫେରନ୍ତୁ;

	ଯଦି ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ଅନ୍ୟଥା {
		log("%sframe #9: ---\n", prefix);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (levels <= 9) ଫେରନ୍ତୁ;
#endif
}
#ଅନ୍ୟଥା
ଶୂନ୍ୟ log_backtrace(ଅଚଳ ଅକ୍ଷର*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { }
#endif

ଶୂନ୍ୟ LogManager::reset_ଗଦା()
{
	ଯେତେବେଳେ (header_ଗଣନା.size() > 1)
		header_ଗଣନା.କାଢ଼_back();
	flush();
}

ଶୂନ୍ୟ log_dump_val_worker(RTLIL::IdString v) {
	ଯଦି (v == IdString::Null)
		log("(null)");
	ଅନ୍ୟଥା ଯଦି (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	ଅନ୍ୟଥା ଯଦି (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	ଅନ୍ୟଥା
		log("%s", v.handle_token());
}

ଶୂନ୍ୟ log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

ଶୂନ୍ୟ log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ଅଚଳ RTLIL::SigSpec &sig, ତାର୍କିକ autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ buf.str();
}

std::string log_const(ଅଚଳ RTLIL::Const &ମୂଲ୍ୟ, ତାର୍କିକ autoint)
{
	ଯଦି ((ମୂଲ୍ୟ.flags & RTLIL::CONST_FLAG_STRING) == 0)
		ଫେରନ୍ତୁ log_signal(ମୂଲ୍ୟ, autoint);

	ଫେରନ୍ତୁ "\"" + ମୂଲ୍ୟ.decode_string() + "\"";
}

ଶୂନ୍ୟ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, ମିଥ୍ୟା, ସତ୍ୟ, ମିଥ୍ୟା, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ଶୂନ୍ୟ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ଶୂନ୍ୟ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

ଶୂନ୍ୟ LogManager::check_expected()
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

	auto check = [&](ଅଚଳ std::string kind, std::string pattern, LogExpectedItem item) {
		ଯଦି (item.current_ଗଣନା == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		ଯଦି (item.current_ଗଣନା != item.expected_ଗଣନା) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_ଗଣନା, item.expected_ଗଣନା);
		}
	};

	ପାଇଁ (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	ପାଇଁ (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	ପାଇଁ (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	ପାଇଁ (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ଅଚଳ std::string kind, std::string pattern, LogExpectedItem item) {
		ଯଦି (item.current_ଗଣନା == item.expected_ଗଣନା) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#ଯଦି defined(_MSC_VER)
				_exit(0);
			#ଅନ୍ୟଥା
				_Exit(0);
			#endif
		} ଅନ୍ୟଥା {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	ପାଇଁ (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	ପାଇଁ (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

ଶୂନ୍ୟ LogManager::report_unexpected_error()
{
	ଯଦି (expect_no_warnings && warnings_ଗଣନା_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_ଗଣନା, warnings_ଗଣନା - warnings_ଗଣନା_noexpect);
}

ଶୂନ୍ୟ LogManager::add_expect(std::string type, std::string pattern, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
	ଯଦି (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ଗଣନା_2);
	ଅନ୍ୟଥା ଯଦି (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), ଗଣନା_2);
	ଅନ୍ୟଥା log_abort();
}

ଶୂନ୍ୟ LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	ଯଦି (!hasher)
		ଫେରନ୍ତୁ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	ଫେରନ୍ତୁ hash;
}

YOSYS_NAMESPACE_END

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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "libs/sha1/sha1.h"
#অন্তর্ভাব "backends/rtlil/rtlil_backend.h"

#যদি !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#যদি defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <vector>
#অন্তর্ভাব <list>

YOSYS_NAMESPACE_BEGIN

যৌক্তিক log_stderr_sink_forced = মিথ্যা;

LogManager &logger()
{
	স্থির LogManager instance;
	ফেরত instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ধ্রুবক
{
	স্থির ধ্রুবক auto initial_time = std::chrono::steady_clock::now();
	ফেরত initial_time;
}

শূন্য (*log_error_atexit)() = nullptr;
শূন্য (*log_verific_callback)(পূর্ণসংখ্যা বার্তা_type, ধ্রুবক অক্ষর *message_id, LogSourceLocation src, ধ্রুবক অক্ষর *বার্তা) = nullptr;

স্থির যৌক্তিক পরবর্তী_print_log = সত্য;

FileLogSink::FileLogSink(ধ্রুবক std::string &filename, যৌক্তিক সারি_buffered, যৌক্তিক append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	যদি (!file)
		ছোড়ো std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	যদি (সারি_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	যদি (file)
		fclose(file);
}

শূন্য FileLogSink::log(ধ্রুবক LogMessage &বার্তা)
{
	fputs(বার্তা.cached_বার্তা.c_str(), file);
}

শূন্য FileLogSink::flush()
{
	fflush(file);
}

শূন্য ConsoleLogSink::log(ধ্রুবক LogMessage &বার্তা)
{
	FILE *file = (বার্তা.severity == LogSeverity::Error || বার্তা.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(বার্তা.cached_বার্তা.c_str(), file);
}

শূন্য ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

যৌক্তিক StderrLogSink::should_log(ধ্রুবক LogMessage &বার্তা) ধ্রুবক
{
	ফেরত বার্তা.severity == LogSeverity::Error || বার্তা.severity == LogSeverity::NonFatalError ||
			(বার্তা.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

শূন্য StderrLogSink::log(ধ্রুবক LogMessage &বার্তা)
{
	fputs(বার্তা.cached_বার্তা.c_str(), stderr);
}

শূন্য StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

শূন্য ScratchPadLogSink::log(ধ্রুবক LogMessage &বার্তা)
{
	RTLIL::Design *design = yosys_get_design();
	যদি (!design)
		ফেরত;

	design->scratchpad[scratchpad].append(বার্তা.cached_বার্তা);
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
	যদি (logger().get_log_time())
	{
		যদি (পরবর্তী_print_log) {
			পরবর্তী_print_log = মিথ্যা;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", পূর্ণসংখ্যা(us.গুনতি() / 1'000'000),পূর্ণসংখ্যা(us.গুনতি() % 1'000'000));
		}

		যদি (!format.empty() && format.back() == '\n')
			পরবর্তী_print_log = সত্য;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		যদি (format == "%s" && !message.empty() && message.back() == '\n')
			পরবর্তী_print_log = সত্য;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_সারি) : "";
	cached_বার্তা = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

শূন্য LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	আকার_t remove_leading = 0;
	যখন (format.আকার_2() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	যদি (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_সারি) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	যদি (str.empty())
		ফেরত;

	আকার_t nnl_pos = str.find_last_not_of('\n');
	যদি (nnl_pos == std::string::npos)
		newline_গুনতি += Get_আকার(str);
	অন্যথা
		newline_গুনতি = Get_আকার(str) - nnl_pos - 1;

	যদি (hasher)
		hasher->update(str);

	auto বার্তা = LogMessage(severity, src, prefix, format, str_in);
	জন্য (auto &sink : sinks) {
		যদি (sink->should_log(বার্তা))
			sink->log(বার্তা);
	}

	যদি (severity == LogSeverity::Header)
		str = str_in;

	স্থির std::string linebuffer;
	স্থির যৌক্তিক warn_regex_recusion_guard = মিথ্যা;

	যদি (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = সত্য;

		যদি (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		অন্যথা
		{
			linebuffer += str;

			যদি (!linebuffer.empty() && linebuffer.back() == '\n') {
				জন্য (auto &re : warn_regexes)
					যদি (std::regex_অনুসন্ধান(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				জন্য (auto &[_, item] : expect_log)
					যদি (std::regex_অনুসন্ধান(linebuffer, item.pattern))
						item.current_গুনতি++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = মিথ্যা;
	}
}

শূন্য LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	যদি (make_debug && !is_debug(1))
		ফেরত;
	logv_string(severity, src, prefix, format, std::move(str));
}

শূন্য LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	যদি (header_গুনতি.আকার_2() > 0)
		header_গুনতি.back()++;

	যদি (পূর্ণসংখ্যা(header_গুনতি.আকার_2()) <= verbose_level) {
		log_stderr_sink_forced = সত্য;
	}

	std::string header_id;

	জন্য (পূর্ণসংখ্যা c : header_গুনতি)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	যদি (hdump_all)
		hdump[header_id].ঢোকাও("yosys_dump_" + header_id + ".il");

	যদি (hdump.গুনতি(header_id) && design != nullptr)
		জন্য (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = মিথ্যা;
}

শূন্য LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	যৌক্তিক suppressed = মিথ্যা;

	জন্য (auto &re : nowarn_regexes)
		যদি (std::regex_অনুসন্ধান(message, re))
			suppressed = সত্য;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_সারি) : "";
	যদি (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	অন্যথা
	{
		পূর্ণসংখ্যা bak_make_debug = make_debug;
		make_debug = 0;

		জন্য (auto &re : werror_regexes)
			যদি (std::regex_অনুসন্ধান(message, re))
				formatted_error(src, "ERROR: ", format, message);

		যৌক্তিক warning_match = মিথ্যা;
		জন্য (auto &[_, item] : expect_warning)
			যদি (std::regex_অনুসন্ধান(message, item.pattern)) {
				item.current_গুনতি++;
				warning_match = সত্য;
			}

		std::string pattern = loc + string(prefix) + message;
		জন্য (auto &[_, item] : expect_prefix_warning)
			যদি (std::regex_অনুসন্ধান(pattern, item.pattern)) {
				item.current_গুনতি++;
				warning_match = সত্য;
			}

		যদি (warnings.গুনতি(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		অন্যথা
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.ঢোকাও(message);
		}

		যদি (!warning_match)
			warnings_গুনতি_noexpect++;
		warnings_গুনতি++;
		make_debug = bak_make_debug;
	}
}

শূন্য LogManager::suppressed() {
	যদি (debug_suppressed && !make_debug) {
		constexpr ধ্রুবক অক্ষর* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

শূন্য LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	পূর্ণসংখ্যা bak_make_debug = make_debug;
	make_debug = 0;
	যদি (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	জন্য (auto &[_, item] : expect_error)
		যদি (std::regex_অনুসন্ধান(message, item.pattern))
			item.current_গুনতি++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_সারি) : "";
	std::string pattern = loc + string(prefix) + message;
	জন্য (auto &[_, item] : expect_prefix_error)
		যদি (std::regex_অনুসন্ধান(pattern, item.pattern))
			item.current_গুনতি++;

	errors_গুনতি++;
}

[[noreturn]]
শূন্য LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	যদি (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ধ্রুবক অক্ষর *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	যদি (e && atoi(e))
		abort();

#যদি defined(_MSC_VER)
	_exit(1);
#অন্যথা
	_Exit(1);
#endif
}

শূন্য LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

শূন্য LogManager::add_experimental(ধ্রুবক std::string &str)
{
	যদি (experimental_ignored.গুনতি(str) == 0 && experimental.গুনতি(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.ঢোকাও(str);
	}
}

শূন্য LogManager::add_deprecated(ধ্রুবক std::string &feature, ধ্রুবক std::string &earliest_removal)
{
	যদি (deprecated.গুনতি(feature) == 0) {
		যদি (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		অন্যথা
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

শূন্য log_assert_failure(ধ্রুবক অক্ষর *রাশিমালা, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", রাশিমালা, location.file_name(), location.সারি());
}

শূন্য log_abort_internal(ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি)
{
	log_error("Abort in %s:%d.\n", file, সারি);
}

শূন্য log_yosys_abort_message(std::string_view file, পূর্ণসংখ্যা সারি, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, সারি, func, message);
}

শূন্য LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	যদি (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		ছোড়ো log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

শূন্য LogManager::spacer()
{
	যদি (newline_গুনতি < 2) log("\n");
	যদি (newline_গুনতি < 2) log("\n");
}

শূন্য LogManager::clear_original()
{
	std::erase_if(sinks, [](ধ্রুবক auto &sink) { ফেরত dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

শূন্য LogManager::ঠেলো()
{
	header_গুনতি.ঠেলো_back(0);
}

শূন্য LogManager::বের_করো()
{
	header_গুনতি.বের_করো_back();
	flush();
}

#যদি defined(YOSYS_ENABLE_DLOPEN)
শূন্য log_backtrace(ধ্রুবক অক্ষর *prefix, পূর্ণসংখ্যা levels)
{
	যদি (levels <= 0) ফেরত;

	Dl_info dli;
	শূন্য *p;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #1: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 1) ফেরত;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#অন্যথা
	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #2: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 2) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #3: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 3) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #4: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 4) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #5: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 5) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #6: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 6) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #7: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 7) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #8: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 8) ফেরত;

	যদি ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} অন্যথা {
		log("%sframe #9: ---\n", prefix);
		ফেরত;
	}

	যদি (levels <= 9) ফেরত;
#endif
}
#অন্যথা
শূন্য log_backtrace(ধ্রুবক অক্ষর*, পূর্ণসংখ্যা) { }
#endif

শূন্য LogManager::reset_স্তূপ()
{
	যখন (header_গুনতি.আকার_2() > 1)
		header_গুনতি.বের_করো_back();
	flush();
}

শূন্য log_dump_val_worker(RTLIL::IdString v) {
	যদি (v == IdString::Null)
		log("(null)");
	অন্যথা যদি (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	অন্যথা যদি (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	অন্যথা
		log("%s", v.handle_token());
}

শূন্য log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

শূন্য log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ধ্রুবক RTLIL::SigSpec &sig, যৌক্তিক autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	ফেরত buf.str();
}

std::string log_const(ধ্রুবক RTLIL::Const &মান, যৌক্তিক autoint)
{
	যদি ((মান.flags & RTLIL::CONST_FLAG_STRING) == 0)
		ফেরত log_signal(মান, autoint);

	ফেরত "\"" + মান.decode_string() + "\"";
}

শূন্য log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, মিথ্যা, সত্য, মিথ্যা, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

শূন্য log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

শূন্য log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

শূন্য LogManager::check_expected()
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

	auto check = [&](ধ্রুবক std::string kind, std::string pattern, LogExpectedItem item) {
		যদি (item.current_গুনতি == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		যদি (item.current_গুনতি != item.expected_গুনতি) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_গুনতি, item.expected_গুনতি);
		}
	};

	জন্য (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	জন্য (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	জন্য (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	জন্য (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ধ্রুবক std::string kind, std::string pattern, LogExpectedItem item) {
		যদি (item.current_গুনতি == item.expected_গুনতি) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#যদি defined(_MSC_VER)
				_exit(0);
			#অন্যথা
				_Exit(0);
			#endif
		} অন্যথা {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	জন্য (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	জন্য (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

শূন্য LogManager::report_unexpected_error()
{
	যদি (expect_no_warnings && warnings_গুনতি_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", Get_আকার(warnings),
					warnings_গুনতি, warnings_গুনতি - warnings_গুনতি_noexpect);
}

শূন্য LogManager::add_expect(std::string type, std::string pattern, পূর্ণসংখ্যা গুনতি)
{
	যদি (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), গুনতি);
	অন্যথা যদি (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), গুনতি);
	অন্যথা যদি (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), গুনতি);
	অন্যথা যদি (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), গুনতি);
	অন্যথা যদি (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), গুনতি);
	অন্যথা যদি (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), গুনতি);
	অন্যথা log_abort();
}

শূন্য LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	যদি (!hasher)
		ফেরত {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	ফেরত hash;
}

YOSYS_NAMESPACE_END

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

#include "kernel/yosys.h"
#include "libs/sha1/sha1.h"
#include "backends/rtlil/rtlil_backend.h"

#ᰀᰧ !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#ᰀᰧ defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <vector>
#include <list>

YOSYS_NAMESPACE_BEGIN

bool log_stderr_sink_forced = false;

LogManager &logger()
{
	static LogManager instance;
	ᰀᰧᰌ instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() const
{
	static const auto initial_time = std::chrono::steady_clock::now();
	ᰀᰧᰌ initial_time;
}

void (*log_error_atexit)() = nullptr;
void (*log_verific_callback)(puur_naa_nka msg_type, const var_na *message_id, LogSourceLocation src, const var_na *msg) = nullptr;

static bool next_print_log = true;

FileLogSink::FileLogSink(const std::string &filename, bool line_buffered, bool append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	ᰀᰧ (!file)
		k_sipa std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	ᰀᰧ (line_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	ᰀᰧ (file)
		fclose(file);
}

void FileLogSink::log(const LogMessage &msg)
{
	fputs(msg.cached_msg.c_str(), file);
}

void FileLogSink::flush()
{
	fflush(file);
}

void ConsoleLogSink::log(const LogMessage &msg)
{
	FILE *file = (msg.severity == LogSeverity::Error || msg.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(msg.cached_msg.c_str(), file);
}

void ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

bool StderrLogSink::should_log(const LogMessage &msg) const
{
	ᰀᰧᰌ msg.severity == LogSeverity::Error || msg.severity == LogSeverity::NonFatalError ||
			(msg.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

void StderrLogSink::log(const LogMessage &msg)
{
	fputs(msg.cached_msg.c_str(), stderr);
}

void StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

void ScratchPadLogSink::log(const LogMessage &msg)
{
	RTLIL::Design *design = yosys_get_design();
	ᰀᰧ (!design)
		ᰀᰧᰌ;

	design->scratchpad[scratchpad].append(msg.cached_msg);
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
	ᰀᰧ (logger().get_log_time())
	{
		ᰀᰧ (next_print_log) {
			next_print_log = false;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", puur_naa_nka(us.count() / 1'000'000),puur_naa_nka(us.count() % 1'000'000));
		}

		ᰀᰧ (!format.empty() && format.back() == '\n')
			next_print_log = true;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		ᰀᰧ (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = true;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	cached_msg = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

void LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	ᰀᰬ (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	ᰀᰧ (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	ᰀᰧ (str.empty())
		ᰀᰧᰌ;

	size_t nnl_pos = str.find_last_not_of('\n');
	ᰀᰧ (nnl_pos == std::string::npos)
		newline_count += GetSize(str);
	ᰀᰧᰀ
		newline_count = GetSize(str) - nnl_pos - 1;

	ᰀᰧ (hasher)
		hasher->update(str);

	auto msg = LogMessage(severity, src, prefix, format, str_in);
	for (auto &sink : sinks) {
		ᰀᰧ (sink->should_log(msg))
			sink->log(msg);
	}

	ᰀᰧ (severity == LogSeverity::Header)
		str = str_in;

	static std::string linebuffer;
	static bool warn_regex_recusion_guard = false;

	ᰀᰧ (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = true;

		ᰀᰧ (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		ᰀᰧᰀ
		{
			linebuffer += str;

			ᰀᰧ (!linebuffer.empty() && linebuffer.back() == '\n') {
				for (auto &re : warn_regexes)
					ᰀᰧ (std::regex_search(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				for (auto &[_, item] : expect_log)
					ᰀᰧ (std::regex_search(linebuffer, item.pattern))
						item.current_count++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = false;
	}
}

void LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	ᰀᰧ (make_debug && !is_debug(1))
		ᰀᰧᰌ;
	logv_string(severity, src, prefix, format, std::move(str));
}

void LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	ᰀᰧ (header_count.size() > 0)
		header_count.back()++;

	ᰀᰧ (puur_naa_nka(header_count.size()) <= verbose_level) {
		log_stderr_sink_forced = true;
	}

	std::string header_id;

	for (puur_naa_nka c : header_count)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	ᰀᰧ (hdump_all)
		hdump[header_id].insert("yosys_dump_" + header_id + ".il");

	ᰀᰧ (hdump.count(header_id) && design != nullptr)
		for (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = false;
}

void LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	bool suppressed = false;

	for (auto &re : nowarn_regexes)
		ᰀᰧ (std::regex_search(message, re))
			suppressed = true;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	ᰀᰧ (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	ᰀᰧᰀ
	{
		puur_naa_nka bak_make_debug = make_debug;
		make_debug = 0;

		for (auto &re : werror_regexes)
			ᰀᰧ (std::regex_search(message, re))
				formatted_error(src, "ERROR: ", format, message);

		bool warning_match = false;
		for (auto &[_, item] : expect_warning)
			ᰀᰧ (std::regex_search(message, item.pattern)) {
				item.current_count++;
				warning_match = true;
			}

		std::string pattern = loc + string(prefix) + message;
		for (auto &[_, item] : expect_prefix_warning)
			ᰀᰧ (std::regex_search(pattern, item.pattern)) {
				item.current_count++;
				warning_match = true;
			}

		ᰀᰧ (warnings.count(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		ᰀᰧᰀ
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.insert(message);
		}

		ᰀᰧ (!warning_match)
			warnings_count_noexpect++;
		warnings_count++;
		make_debug = bak_make_debug;
	}
}

void LogManager::suppressed() {
	ᰀᰧ (debug_suppressed && !make_debug) {
		constexpr const var_na* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

void LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	puur_naa_nka bak_make_debug = make_debug;
	make_debug = 0;
	ᰀᰧ (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	for (auto &[_, item] : expect_error)
		ᰀᰧ (std::regex_search(message, item.pattern))
			item.current_count++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_line) : "";
	std::string pattern = loc + string(prefix) + message;
	for (auto &[_, item] : expect_prefix_error)
		ᰀᰧ (std::regex_search(pattern, item.pattern))
			item.current_count++;

	errors_count++;
}

[[noreturn]]
void LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	ᰀᰧ (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	const var_na *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	ᰀᰧ (e && atoi(e))
		abort();

#ᰀᰧ defined(_MSC_VER)
	_exit(1);
#ᰀᰧᰀ
	_Exit(1);
#endif
}

void LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

void LogManager::add_experimental(const std::string &str)
{
	ᰀᰧ (experimental_ignored.count(str) == 0 && experimental.count(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.insert(str);
	}
}

void LogManager::add_deprecated(const std::string &feature, const std::string &earliest_removal)
{
	ᰀᰧ (deprecated.count(feature) == 0) {
		ᰀᰧ (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		ᰀᰧᰀ
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

void log_assert_failure(const var_na *expr, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", expr, location.file_name(), location.line());
}

void log_abort_internal(const var_na *file, puur_naa_nka line)
{
	log_error("Abort in %s:%d.\n", file, line);
}

void log_yosys_abort_message(std::string_view file, puur_naa_nka line, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, line, func, message);
}

void LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	ᰀᰧ (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		k_sipa log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

void LogManager::spacer()
{
	ᰀᰧ (newline_count < 2) log("\n");
	ᰀᰧ (newline_count < 2) log("\n");
}

void LogManager::clear_original()
{
	std::erase_if(sinks, [](const auto &sink) { ᰀᰧᰌ dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

void LogManager::push()
{
	header_count.push_back(0);
}

void LogManager::pop()
{
	header_count.pop_back();
	flush();
}

#ᰀᰧ defined(YOSYS_ENABLE_DLOPEN)
void log_backtrace(const var_na *prefix, puur_naa_nka levels)
{
	ᰀᰧ (levels <= 0) ᰀᰧᰌ;

	Dl_info dli;
	void *p;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #1: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 1) ᰀᰧᰌ;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#ᰀᰧᰀ
	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #2: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 2) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #3: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 3) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #4: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 4) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #5: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 5) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #6: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 6) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #7: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 7) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #8: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 8) ᰀᰧᰌ;

	ᰀᰧ ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} ᰀᰧᰀ {
		log("%sframe #9: ---\n", prefix);
		ᰀᰧᰌ;
	}

	ᰀᰧ (levels <= 9) ᰀᰧᰌ;
#endif
}
#ᰀᰧᰀ
void log_backtrace(const var_na*, puur_naa_nka) { }
#endif

void LogManager::reset_stack()
{
	ᰀᰬ (header_count.size() > 1)
		header_count.pop_back();
	flush();
}

void log_dump_val_worker(RTLIL::IdString v) {
	ᰀᰧ (v == IdString::Null)
		log("(null)");
	ᰀᰧᰀ ᰀᰧ (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	ᰀᰧᰀ ᰀᰧ (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	ᰀᰧᰀ
		log("%s", v.handle_token());
}

void log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

void log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(const RTLIL::SigSpec &sig, bool autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	ᰀᰧᰌ buf.str();
}

std::string log_const(const RTLIL::Const &value, bool autoint)
{
	ᰀᰧ ((value.flags & RTLIL::CONST_FLAG_STRING) == 0)
		ᰀᰧᰌ log_signal(value, autoint);

	ᰀᰧᰌ "\"" + value.decode_string() + "\"";
}

void log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, false, true, false, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

void log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

void log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

void LogManager::check_expected()
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

	auto check = [&](const std::string kind, std::string pattern, LogExpectedItem item) {
		ᰀᰧ (item.current_count == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		ᰀᰧ (item.current_count != item.expected_count) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_count, item.expected_count);
		}
	};

	for (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	for (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	for (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	for (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](const std::string kind, std::string pattern, LogExpectedItem item) {
		ᰀᰧ (item.current_count == item.expected_count) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#ᰀᰧ defined(_MSC_VER)
				_exit(0);
			#ᰀᰧᰀ
				_Exit(0);
			#endif
		} ᰀᰧᰀ {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	for (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	for (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

void LogManager::report_unexpected_error()
{
	ᰀᰧ (expect_no_warnings && warnings_count_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_count, warnings_count - warnings_count_noexpect);
}

void LogManager::add_expect(std::string type, std::string pattern, puur_naa_nka count)
{
	ᰀᰧ (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	ᰀᰧᰀ ᰀᰧ (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	ᰀᰧᰀ ᰀᰧ (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	ᰀᰧᰀ ᰀᰧ (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	ᰀᰧᰀ ᰀᰧ (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	ᰀᰧᰀ ᰀᰧ (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), count);
	ᰀᰧᰀ log_abort();
}

void LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	ᰀᰧ (!hasher)
		ᰀᰧᰌ {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	ᰀᰧᰌ hash;
}

YOSYS_NAMESPACE_END

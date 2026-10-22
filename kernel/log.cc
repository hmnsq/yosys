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

#شامل "kernel/yosys.h"
#شامل "libs/sha1/sha1.h"
#شامل "backends/rtlil/rtlil_backend.h"

#اگر !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#اگر defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <stdarg.h>
#شامل <vector>
#شامل <list>

YOSYS_NAMESPACE_BEGIN

منطقی log_stderr_sink_forced = نادرست;

LogManager &logger()
{
	ایستا LogManager instance;
	بازگشت instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ثابت
{
	ایستا ثابت auto initial_time = std::chrono::steady_clock::now();
	بازگشت initial_time;
}

تهی (*log_error_atexit)() = nullptr;
تهی (*log_verific_callback)(صحیح پیام_type, ثابت نویسه *message_id, LogSourceLocation src, ثابت نویسه *پیام) = nullptr;

ایستا منطقی next_print_log = درست;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقی سطر_buffered, منطقی append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	اگر (!file)
		پرتابکن std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	اگر (سطر_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	اگر (file)
		fclose(file);
}

تهی FileLogSink::log(ثابت LogMessage &پیام)
{
	fputs(پیام.cached_پیام.c_str(), file);
}

تهی FileLogSink::flush()
{
	fflush(file);
}

تهی ConsoleLogSink::log(ثابت LogMessage &پیام)
{
	FILE *file = (پیام.severity == LogSeverity::Error || پیام.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(پیام.cached_پیام.c_str(), file);
}

تهی ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقی StderrLogSink::should_log(ثابت LogMessage &پیام) ثابت
{
	بازگشت پیام.severity == LogSeverity::Error || پیام.severity == LogSeverity::NonFatalError ||
			(پیام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

تهی StderrLogSink::log(ثابت LogMessage &پیام)
{
	fputs(پیام.cached_پیام.c_str(), stderr);
}

تهی StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

تهی ScratchPadLogSink::log(ثابت LogMessage &پیام)
{
	RTLIL::Design *design = yosys_get_design();
	اگر (!design)
		بازگشت;

	design->scratchpad[scratchpad].append(پیام.cached_پیام);
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
	اگر (logger().get_log_time())
	{
		اگر (next_print_log) {
			next_print_log = نادرست;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", صحیح(us.شمار() / 1'000'000),صحیح(us.شمار() % 1'000'000));
		}

		اگر (!format.empty() && format.back() == '\n')
			next_print_log = درست;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		اگر (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = درست;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	cached_پیام = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

تهی LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	وقتی (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	اگر (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	اگر (str.empty())
		بازگشت;

	size_t nnl_pos = str.find_last_not_of('\n');
	اگر (nnl_pos == std::string::npos)
		newline_شمار += GetSize(str);
	وگرنه
		newline_شمار = GetSize(str) - nnl_pos - 1;

	اگر (hasher)
		hasher->update(str);

	auto پیام = LogMessage(severity, src, prefix, format, str_in);
	برای (auto &sink : sinks) {
		اگر (sink->should_log(پیام))
			sink->log(پیام);
	}

	اگر (severity == LogSeverity::Header)
		str = str_in;

	ایستا std::string linebuffer;
	ایستا منطقی warn_regex_recusion_guard = نادرست;

	اگر (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = درست;

		اگر (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		وگرنه
		{
			linebuffer += str;

			اگر (!linebuffer.empty() && linebuffer.back() == '\n') {
				برای (auto &re : warn_regexes)
					اگر (std::regex_جستجو(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				برای (auto &[_, item] : expect_log)
					اگر (std::regex_جستجو(linebuffer, item.pattern))
						item.current_شمار++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = نادرست;
	}
}

تهی LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	اگر (make_debug && !is_debug(1))
		بازگشت;
	logv_string(severity, src, prefix, format, std::move(str));
}

تهی LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	اگر (header_شمار.size() > 0)
		header_شمار.back()++;

	اگر (صحیح(header_شمار.size()) <= verbose_level) {
		log_stderr_sink_forced = درست;
	}

	std::string header_id;

	برای (صحیح c : header_شمار)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	اگر (hdump_all)
		hdump[header_id].درج("yosys_dump_" + header_id + ".il");

	اگر (hdump.شمار(header_id) && design != nullptr)
		برای (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = نادرست;
}

تهی LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	منطقی suppressed = نادرست;

	برای (auto &re : nowarn_regexes)
		اگر (std::regex_جستجو(message, re))
			suppressed = درست;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	اگر (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	وگرنه
	{
		صحیح bak_make_debug = make_debug;
		make_debug = 0;

		برای (auto &re : werror_regexes)
			اگر (std::regex_جستجو(message, re))
				formatted_error(src, "ERROR: ", format, message);

		منطقی warning_match = نادرست;
		برای (auto &[_, item] : expect_warning)
			اگر (std::regex_جستجو(message, item.pattern)) {
				item.current_شمار++;
				warning_match = درست;
			}

		std::string pattern = loc + string(prefix) + message;
		برای (auto &[_, item] : expect_prefix_warning)
			اگر (std::regex_جستجو(pattern, item.pattern)) {
				item.current_شمار++;
				warning_match = درست;
			}

		اگر (warnings.شمار(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		وگرنه
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.درج(message);
		}

		اگر (!warning_match)
			warnings_شمار_noexpect++;
		warnings_شمار++;
		make_debug = bak_make_debug;
	}
}

تهی LogManager::suppressed() {
	اگر (debug_suppressed && !make_debug) {
		constexpr ثابت نویسه* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

تهی LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	صحیح bak_make_debug = make_debug;
	make_debug = 0;
	اگر (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	برای (auto &[_, item] : expect_error)
		اگر (std::regex_جستجو(message, item.pattern))
			item.current_شمار++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	std::string pattern = loc + string(prefix) + message;
	برای (auto &[_, item] : expect_prefix_error)
		اگر (std::regex_جستجو(pattern, item.pattern))
			item.current_شمار++;

	errors_شمار++;
}

[[noreturn]]
تهی LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	اگر (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ثابت نویسه *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	اگر (e && atoi(e))
		abort();

#اگر defined(_MSC_VER)
	_exit(1);
#وگرنه
	_Exit(1);
#endif
}

تهی LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

تهی LogManager::add_experimental(ثابت std::string &str)
{
	اگر (experimental_ignored.شمار(str) == 0 && experimental.شمار(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.درج(str);
	}
}

تهی LogManager::add_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	اگر (deprecated.شمار(feature) == 0) {
		اگر (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وگرنه
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

تهی log_assert_failure(ثابت نویسه *عبارت, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", عبارت, location.file_name(), location.سطر());
}

تهی log_abort_internal(ثابت نویسه *file, صحیح سطر)
{
	log_error("Abort in %s:%d.\n", file, سطر);
}

تهی log_yosys_abort_message(std::string_view file, صحیح سطر, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, سطر, func, message);
}

تهی LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	اگر (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		پرتابکن log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

تهی LogManager::spacer()
{
	اگر (newline_شمار < 2) log("\n");
	اگر (newline_شمار < 2) log("\n");
}

تهی LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت auto &sink) { بازگشت dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

تهی LogManager::افزودن()
{
	header_شمار.افزودن_back(0);
}

تهی LogManager::برداشت()
{
	header_شمار.برداشت_back();
	flush();
}

#اگر defined(YOSYS_ENABLE_DLOPEN)
تهی log_backtrace(ثابت نویسه *prefix, صحیح levels)
{
	اگر (levels <= 0) بازگشت;

	Dl_info dli;
	تهی *p;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #1: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 1) بازگشت;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#وگرنه
	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #2: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 2) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #3: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 3) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #4: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 4) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #5: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 5) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #6: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 6) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #7: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 7) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #8: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 8) بازگشت;

	اگر ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وگرنه {
		log("%sframe #9: ---\n", prefix);
		بازگشت;
	}

	اگر (levels <= 9) بازگشت;
#endif
}
#وگرنه
تهی log_backtrace(ثابت نویسه*, صحیح) { }
#endif

تهی LogManager::reset_پشته()
{
	وقتی (header_شمار.size() > 1)
		header_شمار.برداشت_back();
	flush();
}

تهی log_dump_val_worker(RTLIL::IdString v) {
	اگر (v == IdString::Null)
		log("(null)");
	وگرنه اگر (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	وگرنه اگر (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	وگرنه
		log("%s", v.handle_token());
}

تهی log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

تهی log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقی autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	بازگشت buf.str();
}

std::string log_const(ثابت RTLIL::Const &مقدار, منطقی autoint)
{
	اگر ((مقدار.flags & RTLIL::CONST_FLAG_STRING) == 0)
		بازگشت log_signal(مقدار, autoint);

	بازگشت "\"" + مقدار.decode_string() + "\"";
}

تهی log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, نادرست, درست, نادرست, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

تهی log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

تهی log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

تهی LogManager::check_expected()
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

	auto check = [&](ثابت std::string kind, std::string pattern, LogExpectedItem item) {
		اگر (item.current_شمار == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		اگر (item.current_شمار != item.expected_شمار) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_شمار, item.expected_شمار);
		}
	};

	برای (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	برای (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	برای (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	برای (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ثابت std::string kind, std::string pattern, LogExpectedItem item) {
		اگر (item.current_شمار == item.expected_شمار) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#اگر defined(_MSC_VER)
				_exit(0);
			#وگرنه
				_Exit(0);
			#endif
		} وگرنه {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	برای (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	برای (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

تهی LogManager::report_unexpected_error()
{
	اگر (expect_no_warnings && warnings_شمار_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_شمار, warnings_شمار - warnings_شمار_noexpect);
}

تهی LogManager::add_expect(std::string type, std::string pattern, صحیح شمار)
{
	اگر (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), شمار);
	وگرنه اگر (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), شمار);
	وگرنه اگر (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), شمار);
	وگرنه اگر (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), شمار);
	وگرنه اگر (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), شمار);
	وگرنه اگر (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), شمار);
	وگرنه log_abort();
}

تهی LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	اگر (!hasher)
		بازگشت {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	بازگشت hash;
}

YOSYS_NAMESPACE_END

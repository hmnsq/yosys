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

#ضمن "kernel/yosys.h"
#ضمن "libs/sha1/sha1.h"
#ضمن "backends/rtlil/rtlil_backend.h"

#إذا !defined(_WIN32) || defined(__MINGW32__)
#  include <sys/time.h>
#endif

#إذا defined(YOSYS_ENABLE_DLOPEN)
#  include <dlfcn.h>
#endif

#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <stdarg.h>
#ضمن <vector>
#ضمن <list>

YOSYS_NAMESPACE_BEGIN

منطقي log_stderr_sink_forced = خطأ;

LogManager &logger()
{
	ساكن LogManager instance;
	أرجع instance;
}

std::chrono::steady_clock::time_point LogManager::get_initial_time() ثابت
{
	ساكن ثابت auto initial_time = std::chrono::steady_clock::now();
	أرجع initial_time;
}

فراغ (*log_error_atexit)() = nullptr;
فراغ (*log_verific_callback)(صحيح پیغام_type, ثابت حرف *message_id, LogSourceLocation src, ثابت حرف *پیغام) = nullptr;

ساكن منطقي next_print_log = true;

FileLogSink::FileLogSink(ثابت std::string &filename, منطقي سطر_buffered, منطقي append)
	: file(fopen(filename.c_str(), append ? "at" : "wt"))
{
	إذا (!file)
		ارم std::runtime_error("Can't open log file `" + filename + "' for writing!\n");

	إذا (سطر_buffered)
		setvbuf(file, nullptr, _IOLBF, 0);
}

FileLogSink::~FileLogSink()
{
	flush();
	إذا (file)
		fclose(file);
}

فراغ FileLogSink::log(ثابت LogMessage &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), file);
}

فراغ FileLogSink::flush()
{
	fflush(file);
}

فراغ ConsoleLogSink::log(ثابت LogMessage &پیغام)
{
	FILE *file = (پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError) ? stderr : stdout;
	fputs(پیغام.cached_پیغام.c_str(), file);
}

فراغ ConsoleLogSink::flush()
{
	fflush(stdout);
	fflush(stderr);
}

منطقي StderrLogSink::should_log(ثابت LogMessage &پیغام) ثابت
{
	أرجع پیغام.severity == LogSeverity::Error || پیغام.severity == LogSeverity::NonFatalError ||
			(پیغام.severity == LogSeverity::Warning && !quiet_warnings) ||
			log_stderr_sink_forced;
}

فراغ StderrLogSink::log(ثابت LogMessage &پیغام)
{
	fputs(پیغام.cached_پیغام.c_str(), stderr);
}

فراغ StderrLogSink::flush()
{
	fflush(stderr);
}

ScratchPadLogSink::ScratchPadLogSink(std::string scratchpad)
	: scratchpad(std::move(scratchpad))
{
}

فراغ ScratchPadLogSink::log(ثابت LogMessage &پیغام)
{
	RTLIL::Design *design = yosys_get_design();
	إذا (!design)
		أرجع;

	design->scratchpad[scratchpad].append(پیغام.cached_پیغام);
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
	إذا (logger().get_log_time())
	{
		إذا (next_print_log) {
			next_print_log = خطأ;
			auto elapsed = std::chrono::steady_clock::now() - logger().get_initial_time();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
			time_str += stringf("[%05d.%06d] ", صحيح(us.گنتی() / 1'000'000),صحيح(us.گنتی() % 1'000'000));
		}

		إذا (!format.empty() && format.back() == '\n')
			next_print_log = true;

		// Special case to detect newlines in Python log output, since
		// the binding always calls `log("%s", payload)` and the newline
		// is then in the first formatted argument
		إذا (format == "%s" && !message.empty() && message.back() == '\n')
			next_print_log = true;
	}
	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	cached_پیغام = stringf("%s%s%s%s", time_str, loc, prefix, message);
}

فراغ LogManager::logv_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str_in) {
	size_t remove_leading = 0;
	طالما (format.size() > 1 && format[0] == '\n') {
		logv_string(severity, src, prefix, "\n", "\n");
		format = format.substr(1);
		++remove_leading;
	}
	إذا (remove_leading > 0) {
		str_in = str_in.substr(remove_leading);
	}

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	std::string str = stringf("%s%s%s",loc,prefix,str_in);

	إذا (str.empty())
		أرجع;

	size_t nnl_pos = str.find_last_not_of('\n');
	إذا (nnl_pos == std::string::npos)
		newline_گنتی += GetSize(str);
	وإلا
		newline_گنتی = GetSize(str) - nnl_pos - 1;

	إذا (hasher)
		hasher->update(str);

	auto پیغام = LogMessage(severity, src, prefix, format, str_in);
	لكل (auto &sink : sinks) {
		إذا (sink->should_log(پیغام))
			sink->log(پیغام);
	}

	إذا (severity == LogSeverity::Header)
		str = str_in;

	ساكن std::string linebuffer;
	ساكن منطقي warn_regex_recusion_guard = خطأ;

	إذا (!warn_regex_recusion_guard)
	{
		warn_regex_recusion_guard = true;

		إذا (warn_regexes.empty() && expect_log.empty() && expect_prefix_log.empty())
		{
			linebuffer.clear();
		}
		وإلا
		{
			linebuffer += str;

			إذا (!linebuffer.empty() && linebuffer.back() == '\n') {
				لكل (auto &re : warn_regexes)
					إذا (std::regex_تلاش(linebuffer, re))
						log_warning("Found log message matching -W regex:\n%s", str);

				لكل (auto &[_, item] : expect_log)
					إذا (std::regex_تلاش(linebuffer, item.pattern))
						item.current_گنتی++;

				linebuffer.clear();
			}
		}

		warn_regex_recusion_guard = خطأ;
	}
}

فراغ LogManager::formatted_string(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	إذا (make_debug && !is_debug(1))
		أرجع;
	logv_string(severity, src, prefix, format, std::move(str));
}

فراغ LogManager::formatted_header(RTLIL::Design *design, std::string_view format, std::string str)
{
	log_assert(!Multithreading::active());

	spacer();
	إذا (header_گنتی.size() > 0)
		header_گنتی.back()++;

	إذا (صحيح(header_گنتی.size()) <= verbose_level) {
		log_stderr_sink_forced = true;
	}

	std::string header_id;

	لكل (صحيح c : header_گنتی)
		header_id += stringf("%s%d", header_id.empty() ? "" : ".", c);

	formatted_string(LogSeverity::Header, LogSourceLocation{}, stringf("%s. ", header_id), format, std::move(str));
	flush();

	إذا (hdump_all)
		hdump[header_id].داخل_کرو("yosys_dump_" + header_id + ".il");

	إذا (hdump.گنتی(header_id) && design != nullptr)
		لكل (auto &filename : hdump.at(header_id)) {
			log("Dumping current design to '%s'.\n", filename);
			Pass::call(design, {"dump", "-o", filename});
		}
	log_stderr_sink_forced = خطأ;
}

فراغ LogManager::formatted_warning(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	log_assert(!Multithreading::active());

	منطقي suppressed = خطأ;

	لكل (auto &re : nowarn_regexes)
		إذا (std::regex_تلاش(message, re))
			suppressed = true;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	إذا (suppressed)
	{
		log("Suppressed %s%s%s", loc, prefix, message);
	}
	وإلا
	{
		صحيح bak_make_debug = make_debug;
		make_debug = 0;

		لكل (auto &re : werror_regexes)
			إذا (std::regex_تلاش(message, re))
				formatted_error(src, "ERROR: ", format, message);

		منطقي warning_match = خطأ;
		لكل (auto &[_, item] : expect_warning)
			إذا (std::regex_تلاش(message, item.pattern)) {
				item.current_گنتی++;
				warning_match = true;
			}

		std::string pattern = loc + string(prefix) + message;
		لكل (auto &[_, item] : expect_prefix_warning)
			إذا (std::regex_تلاش(pattern, item.pattern)) {
				item.current_گنتی++;
				warning_match = true;
			}

		إذا (warnings.گنتی(message))
		{
			formatted_string(LogSeverity::Info, src, prefix, format, message);
			flush();
		}
		وإلا
		{
			formatted_string(LogSeverity::Warning, src, prefix, format, message);
			flush();
			warnings.داخل_کرو(message);
		}

		إذا (!warning_match)
			warnings_گنتی_noexpect++;
		warnings_گنتی++;
		make_debug = bak_make_debug;
	}
}

فراغ LogManager::suppressed() {
	إذا (debug_suppressed && !make_debug) {
		constexpr ثابت حرف* format = "<suppressed ~%d debug messages>\n";
		logv_string(LogSeverity::Info, LogSourceLocation{}, {}, format, stringf(format, debug_suppressed));
		debug_suppressed = 0;
	}
}

فراغ LogManager::error_with_prefix(LogSeverity severity, LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	صحيح bak_make_debug = make_debug;
	make_debug = 0;
	إذا (severity == LogSeverity::Error)
		suppressed();

	logv_string(severity, src, prefix, format, message);
	flush();

	make_debug = bak_make_debug;

	لكل (auto &[_, item] : expect_error)
		إذا (std::regex_تلاش(message, item.pattern))
			item.current_گنتی++;

	std::string loc = !src.filename.empty() ? stringf("%s:%d: ", src.filename, src.start_سطر) : "";
	std::string pattern = loc + string(prefix) + message;
	لكل (auto &[_, item] : expect_prefix_error)
		إذا (std::regex_تلاش(pattern, item.pattern))
			item.current_گنتی++;

	errors_گنتی++;
}

[[noreturn]]
فراغ LogManager::formatted_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::Error, src, prefix, format, message);

	check_expected();

	إذا (log_error_atexit)
		log_error_atexit();

	YS_DEBUGTRAP_IF_DEBUGGING;
	ثابت حرف *e = getenv("YOSYS_ABORT_ON_LOG_ERROR");
	إذا (e && atoi(e))
		abort();

#إذا defined(_MSC_VER)
	_exit(1);
#وإلا
	_Exit(1);
#endif
}

فراغ LogManager::formatted_nonfatal_error(LogSourceLocation src, std::string_view prefix, std::string_view format, std::string message)
{
	error_with_prefix(LogSeverity::NonFatalError, src, prefix, format, message);
}

فراغ LogManager::add_experimental(ثابت std::string &str)
{
	إذا (experimental_ignored.گنتی(str) == 0 && experimental.گنتی(str) == 0) {
		log_warning("Feature '%s' is experimental.\n", str);
		experimental.داخل_کرو(str);
	}
}

فراغ LogManager::add_deprecated(ثابت std::string &feature, ثابت std::string &earliest_removal)
{
	إذا (deprecated.گنتی(feature) == 0) {
		إذا (earliest_removal.empty())
			log_warning("Feature '%s' is deprecated.\n", feature);
		وإلا
			log_warning("Feature '%s' is deprecated and may be removed as soon as Yosys %s.\n", feature, earliest_removal);
		deprecated[feature] = earliest_removal;
	}
}

فراغ log_assert_failure(ثابت حرف *اظہاریہ, source_location location)
{
	log_error("Assert `%s' failed in %s:%d.\n", اظہاریہ, location.file_name(), location.سطر());
}

فراغ log_abort_internal(ثابت حرف *file, صحيح سطر)
{
	log_error("Abort in %s:%d.\n", file, سطر);
}

فراغ log_yosys_abort_message(std::string_view file, صحيح سطر, std::string_view func, std::string_view message)
{
	log_error("Abort in %s:%d (%s): %s\n", file, سطر, func, message);
}

فراغ LogManager::formatted_cmd_error(std::string_view format, std::string message)
{
	إذا (cmd_error_throw) {
		formatted_string(LogSeverity::Error, LogSourceLocation{}, "ERROR: ", format, message);
		flush();

		ارم log_cmd_error_exception();
	}

	formatted_error(LogSourceLocation{}, "ERROR: ", format, message);
}

فراغ LogManager::spacer()
{
	إذا (newline_گنتی < 2) log("\n");
	إذا (newline_گنتی < 2) log("\n");
}

فراغ LogManager::clear_original()
{
	std::erase_if(sinks, [](ثابت auto &sink) { أرجع dynamic_cast<LogSinkRef *>(sink.get()) != nullptr; });
}

فراغ LogManager::ڈالو()
{
	header_گنتی.ڈالو_back(0);
}

فراغ LogManager::نکالو()
{
	header_گنتی.نکالو_back();
	flush();
}

#إذا defined(YOSYS_ENABLE_DLOPEN)
فراغ log_backtrace(ثابت حرف *prefix, صحيح levels)
{
	إذا (levels <= 0) أرجع;

	Dl_info dli;
	فراغ *p;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(0))) && dladdr(p, &dli)) {
		log("%sframe #1: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #1: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 1) أرجع;

#ifndef DEBUG
	log("%sframe #2: [build Yosys with ENABLE_DEBUG for deeper backtraces]\n", prefix);
#وإلا
	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(1))) && dladdr(p, &dli)) {
		log("%sframe #2: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #2: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 2) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(2))) && dladdr(p, &dli)) {
		log("%sframe #3: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #3: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 3) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(3))) && dladdr(p, &dli)) {
		log("%sframe #4: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #4: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 4) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(4))) && dladdr(p, &dli)) {
		log("%sframe #5: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #5: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 5) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(5))) && dladdr(p, &dli)) {
		log("%sframe #6: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #6: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 6) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(6))) && dladdr(p, &dli)) {
		log("%sframe #7: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #7: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 7) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(7))) && dladdr(p, &dli)) {
		log("%sframe #8: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #8: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 8) أرجع;

	إذا ((p = __builtin_extract_return_addr(__builtin_return_address(8))) && dladdr(p, &dli)) {
		log("%sframe #9: %p %s(%p) %s(%p)\n", prefix, p, dli.dli_fname, dli.dli_fbase, dli.dli_sname, dli.dli_saddr);
	} وإلا {
		log("%sframe #9: ---\n", prefix);
		أرجع;
	}

	إذا (levels <= 9) أرجع;
#endif
}
#وإلا
فراغ log_backtrace(ثابت حرف*, صحيح) { }
#endif

فراغ LogManager::reset_ڈھیر()
{
	طالما (header_گنتی.size() > 1)
		header_گنتی.نکالو_back();
	flush();
}

فراغ log_dump_val_worker(RTLIL::IdString v) {
	إذا (v == IdString::Null)
		log("(null)");
	وإلا إذا (ID::is_static(v))
		log("%s", ID::unescaped_str(v));
	وإلا إذا (yosys_design && yosys_design->twines().is_live(v))
		log("%s", yosys_design->twines().unescaped_str(v));
	وإلا
		log("%s", v.handle_token());
}

فراغ log_dump_val_worker(RTLIL::SigSpec v) {
	log("%s", log_signal(v));
}

فراغ log_dump_val_worker(RTLIL::State v) {
	log("%s", log_signal(v));
}

std::string log_signal(ثابت RTLIL::SigSpec &sig, منطقي autoint)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_sigspec(buf, sig, autoint, RTLIL_BACKEND::DumpMode::Readable);
	أرجع buf.str();
}

std::string log_const(ثابت RTLIL::Const &قدر, منطقي autoint)
{
	إذا ((قدر.flags & RTLIL::CONST_FLAG_STRING) == 0)
		أرجع log_signal(قدر, autoint);

	أرجع "\"" + قدر.decode_string() + "\"";
}

فراغ log_module(RTLIL::Module *module, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_module(buf, indent, module, module->design, خطأ, true, خطأ, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

فراغ log_cell(RTLIL::Cell *cell, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_cell(buf, indent, cell, cell->module ? cell->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

فراغ log_wire(RTLIL::Wire *wire, std::string indent)
{
	std::stringstream buf;
	RTLIL_BACKEND::dump_wire(buf, indent, wire, wire->module ? wire->module->design : nullptr, RTLIL_BACKEND::DumpMode::Readable);
	log("%s", buf.str());
}

فراغ LogManager::check_expected()
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
		إذا (item.current_گنتی == 0) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
		إذا (item.current_گنتی != item.expected_گنتی) {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' found %d time(s), instead of %d time(s) !\n",
				kind.c_str(), pattern.c_str(), item.current_گنتی, item.expected_گنتی);
		}
	};

	لكل (auto &[pattern, item] : expect_warning_copy)
		check("warning", pattern, item);
	لكل (auto &[pattern, item] : expect_prefix_warning_copy)
		check("prefixed warning", pattern, item);
	لكل (auto &[pattern, item] : expect_log_copy)
		check("log", pattern, item);
	لكل (auto &[pattern, item] : expect_prefix_log_copy)
		check("prefixed log", pattern, item);

	auto check_err = [&](ثابت std::string kind, std::string pattern, LogExpectedItem item) {
		إذا (item.current_گنتی == item.expected_گنتی) {
			warn_regexes.clear();
			log("Expected %s pattern '%s' found !!!\n", kind, pattern);
			yosys_shutdown();
			#إذا defined(_MSC_VER)
				_exit(0);
			#وإلا
				_Exit(0);
			#endif
		} وإلا {
			warn_regexes.clear();
			log_error("Expected %s pattern '%s' not found !\n", kind, pattern);
		}
	};
	لكل (auto &[pattern, item] : expect_error_copy)
		check_err("error", pattern, item);
	لكل (auto &[pattern, item] : expect_prefix_error_copy)
		check_err("prefixed error", pattern, item);
}

فراغ LogManager::report_unexpected_error()
{
	إذا (expect_no_warnings && warnings_گنتی_noexpect)
		log_error("Unexpected warnings found: %d unique messages, %d total, %d expected\n", GetSize(warnings),
					warnings_گنتی, warnings_گنتی - warnings_گنتی_noexpect);
}

فراغ LogManager::add_expect(std::string type, std::string pattern, صحيح گنتی)
{
	إذا (type == "error")
		expect_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), گنتی);
	وإلا إذا (type == "prefix-error")
		expect_prefix_error[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), گنتی);
	وإلا إذا (type == "warning")
		expect_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), گنتی);
	وإلا إذا (type == "prefix-warning")
		expect_prefix_warning[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), گنتی);
	وإلا إذا (type == "log")
		expect_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), گنتی);
	وإلا إذا (type == "prefix-log")
		expect_prefix_log[pattern] = LogExpectedItem(YS_REGEX_COMPILE(pattern), گنتی);
	وإلا log_abort();
}

فراغ LogManager::start_hasher()
{
	hasher = std::make_unique<SHA1>();
}

std::string LogManager::finish_hasher()
{
	إذا (!hasher)
		أرجع {};

	std::string hash = hasher->final().substr(0, 10);
	hasher.reset();
	أرجع hash;
}

YOSYS_NAMESPACE_END
